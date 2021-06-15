/*
*   This file is part of Universal-Edit
*   Copyright (C) 2019-2021 Universal-Team
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*   Additional Terms 7.b and 7.c of GPLv3 apply to this file:
*       * Requiring preservation of specified reasonable legal notices or
*         author attributions in that material or in the Appropriate Legal
*         Notices displayed by works containing it.
*       * Prohibiting misrepresentation of the origin of that material,
*         or requiring that modified versions of such material be marked in
*         reasonable ways as different from the original version.
*/

#ifndef _UNIVERSAL_EDIT_HEX_DATA_HPP
#define _UNIVERSAL_EDIT_HEX_DATA_HPP

#include <map>
#include <string>
#include <vector>

class HexData {
public:
	enum class EditMode : uint8_t { Scroll = 0, Edit = 1, Change = 2 };
	~HexData(); // Literally calls End().

	void Load(const std::string &File, const uint8_t MaxLines = 0xD, const uint32_t EditModeBufferLen = 0x5000);
	void End(); // Used to CLOSE the FILE stream.
	uint32_t GetSize() const { return this->FileSize; }; // Get the filesize.
	void WriteChanges(const bool DoChanges); // Write the changes you made to the file through FWRITE.

	uint8_t *DisplayData() { return this->_DisplayData.data(); }; // Get the display data.
	uint8_t *EditData() { return this->_EditData.data(); }; // Get the Editing Data.
	uint32_t GetDisplaySize() const { return this->_DisplayData.size(); }; // Get the display size.
	uint32_t GetEditSize() const { return this->_EditData.size(); }; // Get the editing size.
	uint8_t GetSelectionSize() const { return this->SelectionSize; }; // Get the Selection size.
	uint32_t GetBytePerList() const { return this->BytesPerList; }; // Get the bytes per list amount.
	uint32_t GetLines() const { return this->MaxLines; }; // Get the amount of lines to draw.
	uint32_t GetCursor() const { return this->CursorPos; }; // Get the cursor position.
	uint32_t GetOffs() const { return this->OffsIdx; }; // Get the line / row whatever you call it offset.
	EditMode GetCurMode() const { return this->Mode; }; // Get the current mode, such as scroll or editing or changing.
	std::string EditFile() const { return this->File; }; // Get the name of the current editing file.

	uint32_t GetCurModeSize() const { // Get the size of the FILE or the buffer, depending on current mode.
		if (this->Mode == HexData::EditMode::Scroll) return this->GetSize();
		else return this->GetEditSize();
	};

	std::map<uint32_t, uint8_t> &GetChanges() { return this->Changes; }; // Return a reference to the changes.
	bool IsGood() const { return this->FileGood; }; // Return if the file is good.
	void SetSelectionSize(const uint8_t V) { this->SelectionSize = V; }; // Set the Selection size.
	uint32_t EditStart() const { return this->EditStartOffs; }; // Return the Editing start offset.

	std::string GetChar(const uint32_t Cursor) {
		if (Cursor >= this->GetDisplaySize()) return ".";
		return this->Encoding[this->_DisplayData[Cursor]];
	};

	std::string GetEncChar(const uint32_t Offs, const bool FromFile = true) {
		if (FromFile) {
			if (Offs < this->GetSize()) return this->Encoding[this->Read<uint8_t>(Offs, false)];
			else return ".";

		} else {
			if (Offs < this->GetEditSize()) return this->Encoding[this->ReadFromEditBuffer<uint8_t>(Offs, false)];
			else return ".";
		};

		return ".";
	};

	std::string GetEncodingChar(const uint8_t Idx) { return this->Encoding[Idx]; };


	/* Only use this if you KNOW WHAT YOU DO!!! */
	template <class T> T ReadFromEditBuffer(const uint32_t Offs, const bool BigEndian = false) {
		if (!this->EditData() || !this->IsGood() || (Offs + sizeof(T) >= this->GetEditSize())) return 0;
		T Val = 0;

		if (BigEndian) {
			for (size_t Idx = 0; Idx < sizeof(T) && Offs + Idx < this->GetEditSize(); Idx++) {
				Val |= *(this->EditData() + Offs + Idx) << (sizeof(T) - 1 - Idx) * 8;
			};
		};

		return Val;
	};

	/* ^ Same as above, ONLY USE IT IF YOU KNOW WHAT YOU DO. */
	template <class T> void WriteToEditBuffer(const uint32_t Offs, T Data, const bool BigEndian = false) {
		if (!this->EditData() || !this->IsGood() || (Offs + sizeof(T) >= this->GetEditSize())) return;

		if (BigEndian) { // Big Endian.
			for (int Idx = (int)sizeof(T) - 1; Idx >= 0; Idx--) { // Backwards.
				this->_EditData[Offs + Idx] = (uint8_t)Data;
				this->Changes[this->EditStart() + Offs + Idx] = (uint8_t)Data;
				Data >>= 8; // Go to the last byte.
			};

			this->UpdateDisplay();

		} else { // Little Endian.
			for (size_t Idx = 0; Idx < sizeof(T); Idx++) { // Forwards.
				this->_EditData[Offs + Idx] = (uint8_t)Data;
				this->Changes[this->EditStart() + Offs + Idx] = (uint8_t)Data;
				Data >>= 8; // Go to the next byte.
			};

			this->UpdateDisplay();
		};
	};

	template <class T> std::vector<T> ReadScript(const uint32_t Offs, const uint32_t Count = 0x1, const bool BigEndian = false) {
		if (!this->FileHandler || !this->IsGood() || ((Offs + (sizeof(T) * Count) >= this->GetSize()))) return { 0 };
		std::vector<T> Read;

		try {
			Read.resize(Count);
			fseek(this->FileHandler, Offs, SEEK_SET); // Seek to position.
			fread(Read.data(), sizeof(T), Count, this->FileHandler); // Read.

		} catch(...) {
			return { 0 }; // Likely out of space, so just return 0.
		};

		if (BigEndian) { // TODO: Correct that like before, for now hardcode the ways for u16 and u32, cause that's all you really need actually.
			for (size_t Idx = 0; Idx < Count; Idx++) { // Get through the whole vector.
				if (sizeof(T) == 2) { // uint16_t.
					Read[Idx] =  (Read[Idx] >> 8) | (Read[Idx] << 8);

				} else if (sizeof(T) == 4) { // uint32_t.
					Read[Idx] = (Read[Idx] >> 24) | ((Read[Idx] << 8) & 0x00FF0000) | ((Read[Idx] >> 8) & 0x0000FF00) | (Read[Idx] << 24);
				};
			};

		};

		return Read;
	};

	template<class T> void WriteScript(const uint32_t Offs, const std::vector<T> &Data) {
		if (!this->FileHandler || !this->IsGood() || ((Offs + (sizeof(T) * Data.size()) >= this->GetSize()))) return;

		fseek(this->FileHandler, Offs, SEEK_SET);
		fwrite(Data.data(), sizeof(T), Data.size(), this->FileHandler);
	};

	/* Read things from the FILE. */
	template <class T> T Read(const uint32_t Offs, const bool BigEndian = false) {
		if (!this->FileHandler || !this->IsGood() || (Offs + sizeof(T) >= this->GetSize())) return 0;
		T Read = 0;

		fseek(this->FileHandler, Offs, SEEK_SET); // Seek to position.
		fread(&Read, sizeof(T), 1, this->FileHandler); // Read.

		if (BigEndian) { // TODO: Correct that like before, for now hardcode the ways for u16 and u32, cause that's all you really need actually.

			if (sizeof(T) == 2) { // uint16_t.
				Read = (Read >> 8) | (Read << 8);

			} else if (sizeof(T) == 4) { // uint32_t.
				Read = (Read >> 24) | ((Read << 8) & 0x00FF0000) | ((Read >> 8) & 0x0000FF00) | (Read << 24);
			};
		};

		return Read;
	};


	/* Write from uint8_t, uint16_t, uint32_t and so on, or better said: Any type that has the '>>=' operator. */
	template<class T> void Write(const uint32_t Offs, T Data, const bool BigEndian = false) {
		if (!this->FileHandler || !this->IsGood() || (Offs + sizeof(T) >= this->GetSize())) return;

		if (BigEndian) { // Big Endian.
			for (int Idx = (int)sizeof(T) - 1; Idx >= 0; Idx--) { // Backwards.
				this->Changes[Offs + Idx] = (uint8_t)Data; // Set data to our changes map.
				Data >>= 8; // Go to the last byte.
			};

		} else { // Little Endian.
			for (size_t Idx = 0; Idx < sizeof(T); Idx++) { // Forwards.
				this->Changes[Offs + Idx] = (uint8_t)Data; // Set data to our changes map.
				Data >>= 8; // Go to the next byte.
			};
		};
	};


	/* Bit Operations. */
	bool ReadBit(const uint32_t Offs, const uint8_t BitIndex);
	bool ReadBitFromEditBuffer(const uint32_t Offs, const uint8_t BitIndex); // Same as above, but instead of FILE, load from EDIT buffer.
	void WriteBit(const uint32_t Offs, const uint8_t BitIndex, const bool IsSet, const bool ToChanges = true);
	void WriteBitToEditBuffer(const uint32_t Offs, const uint8_t BitIndex, const bool IsSet); // Same as above, but instead of FILE, load from EDIT buffer.

	/* Bits Operations. */
	uint8_t ReadBits(const uint32_t Offs, const bool First);
	void WriteBits(const uint32_t Offs, const bool First, const uint8_t Data, const bool ToChanges = true);

	std::string ByteToString(const uint32_t Cursor); // Return the cursor's position byte to a hex string such as `00` or `FF`.

	/* Actions! */
	void DOWN();
	void UP();
	void LEFT();
	void RIGHT();
	void APress();
	void BPress();

	/* Switch to modes, but ehh just use the calls above for proper handling. */
	void SwitchToEditMode(const bool FromScroll);
	void SwitchToScrollMode();
	void SwitchToChangeMode();

	/* Some misc features, such as fetching the buffer, updating the display loading encoding AND jumping to an offset. */
	void FetchBuffer();
	void UpdateDisplay();
	void LoadEncoding(const std::string &ENCFile);
	void JumpOffs(const uint32_t Offs);
	FILE *GetFileHandler() { return this->FileHandler; };
private:
	FILE *FileHandler = nullptr; // The Handler of the currently OPEN file.
	bool FileGood = false; // If the file was a success.
	uint32_t FileSize = 0; // Update THIS when insert or removes happen.
	std::string File = ""; // The path to the currently open FILE.
	EditMode Mode = EditMode::Scroll; // The current Hex Editor Mode.

	/* Edit, Scroll Mode related. */
	std::map<uint32_t, uint8_t> Changes; // All changes made while edit mode listed HERE.
	uint8_t MaxLines = 0x1, SelectionSize = 1; // Max amount of lines.
	uint32_t EditStartOffs = 0, EditLen = 0x1000; // Start offset of the EDIT Mode Buffer, and the LENGTH of it.
	uint32_t OffsIdx = 0, CursorPos = 0, BytesPerList = 0; // Offset scroll index, cursor position and the bytes per list.

	/* Buffers and such. */
	std::vector<uint8_t> _DisplayData, _EditData; // The display buffer, for what's on screen AND the Edit-Mode buffer.
	std::string Encoding[256]; // Encoding; Being 256 signs.
};

#endif