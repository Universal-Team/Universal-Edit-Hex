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

#include <cstring> // memcpy.
#include <string>
#include <vector>

class HexData {
public:
	HexData();
	HexData(const std::string &File) {
		this->Load(File);
		this->LoadEncoding("romfs:/encodings/ascii.json");
	};

	void SetNewPath(const std::string &P) { this->File = P; };
	int Load(const std::string &File);
	bool Changes() const { return this->ChangesMade; };
	void SetChanges(const bool V) { this->ChangesMade = V; };
	bool IsGood() const { return this->FileGood; };
	uint32_t GetSize() const { return this->FileData.size(); };
	uint8_t *GetData() { return this->FileData.data(); };
	
	std::string GetChar(const uint32_t Offs) {
		if (Offs >= this->GetSize()) return ".";
		return this->Encoding[this->FileData[Offs]];
	};


	template<class T> T Read(const uint32_t Offs, const bool BigEndian = false) {
		if (!this->IsGood() || !this->GetData() || (Offs + (sizeof(T)) - 1) >= this->GetSize()) return 0;
		T Val = 0;

		if (BigEndian) { // Big Endian.
			for (size_t Idx = 0; Idx < sizeof(T) && Offs + Idx < this->GetSize(); Idx++) {
				Val |= *(this->GetData() + Offs + Idx) << (sizeof(T) - 1 - Idx) * 8;
			};

		} else { // Little Endian.
			for (size_t Idx = 0; Idx < sizeof(T) && Offs + Idx < this->GetSize(); Idx++) {
				Val |= *(this->GetData() + Offs + Idx) << Idx * 8;
			};
		};

		return Val;
	};

	/* Write from uint8_t, uint16_t, uint32_t and so on, or better said: Any type that has the '>>=' operator. */
	template<class T> void Write(const uint32_t Offs, T Data, const bool BigEndian = false) {
		if (!this->IsGood() || !this->GetData() || (Offs + (sizeof(T)) - 1) >= this->GetSize()) return; // Do nothing.

		if (BigEndian) { // Big Endian.
			for (int Idx = (int)sizeof(T) - 1; Idx >= 0; Idx--) { // Write backwards.
				this->GetData()[Offs + Idx] = (uint8_t)Data;
				Data >>= 8; // Go to the last byte.
			};

		} else { // Little Endian.
			for (size_t Idx = 0; Idx < sizeof(T); Idx++) { // Write forwards.
				this->GetData()[Offs + Idx] = (uint8_t)Data;
				Data >>= 8; // Go to the next byte.
			};
		};

		this->SetChanges(true);
	};

	/* Bit Operations. */
	bool ReadBit(const uint32_t Offs, const uint8_t BitIndex);
	void WriteBit(const uint32_t Offs, const uint8_t BitIndex, const bool IsSet);
	
	/* Bits Operations. */
	uint8_t ReadBits(const uint32_t Offs, const bool First);
	void WriteBits(const uint32_t Offs, const bool First, const uint8_t Data);

	/* Insert bytes to the HexData. */
	int InsertBytes(const uint32_t Offs, const std::vector<uint8_t> &ToInsert);
	int EraseBytes(const uint32_t Offs, const uint32_t Size);

	bool WriteBack(const std::string &File);

	std::string ByteToString(const uint32_t Offs);
	std::string EditFile() const { return this->File; };
	void LoadEncoding(const std::string &ENCFile);
private:
	std::string File = "";
	std::vector<uint8_t> FileData;
	bool FileGood = false, ChangesMade = false;

	std::string Encoding[256];
};

#endif