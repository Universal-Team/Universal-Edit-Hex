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

#include "Common.hpp"
#include "HexData.hpp"
#include "JSON.hpp"
#include <unistd.h>

#define BYTES_PER_OFFS 0x10 // That's always the case.


HexData::~HexData() { this->End(); };

/*
	Load a file and specify the amount of lines and the edit buffer size.

	const std::string &File: The file to open.
	const uint8_t MaxLines: The amount of lines to display.
	const uint32_t EditModeBufferLen: The size of the Edit Buffer.
*/
void HexData::Load(const std::string &File, const uint8_t MaxLines, const uint32_t EditModeBufferLen) {
	this->File = File;
	this->FileSize = 0;
	this->FileGood = false;
	this->EditLen = EditModeBufferLen;
	this->End(); // Close FILE handle, if not already closed.

	this->MaxLines = MaxLines;
	this->BytesPerList = (this->MaxLines * BYTES_PER_OFFS);
	this->CursorPos = 0;
	this->OffsIdx = 0;

	if (access(this->File.c_str(), F_OK) == 0) {
		this->FileHandler = fopen(this->File.c_str(), "rb+");

		if (this->FileHandler) {
			fseek(this->FileHandler, 0, SEEK_END);
			this->FileSize = ftell(this->FileHandler);
			fseek(this->FileHandler, 0, SEEK_SET);
			this->FileGood = true;
			this->UpdateDisplay();
		};
	};

	#ifdef _3DS // 3DS -> sdmc and romfs.
		this->LoadEncoding("romfs:/encodings/ascii.json");
	#elif ARM9 // DS(i) -> sd and nitro.
		this->LoadEncoding("nitro:/encodings/ascii.json");
	#endif
};

/* Close the FILE stream. */
void HexData::End() {
	if (this->FileHandler) fclose(this->FileHandler);
};

/*
	IS THIS A GOOD WAY?!
	
	Write made changes to file.
*/
void HexData::WriteChanges(const bool DoChanges) {
	if (this->Changes.empty()) return; // Don't do ANYTHING if empty.

	if (DoChanges) {
		for (const auto &[Offs, Val] : this->Changes) {
			fseek(this->FileHandler, Offs, SEEK_SET);
			fwrite(&Val, 1, 1, this->FileHandler);
		};
	};

	this->Changes.clear();
};


/*
	Return a bit from the data.

	const uint32_t Offs: The Offset to read from.
	const uint8_t BitIndex: The Bit index ( 0 - 7 ).
*/
bool HexData::ReadBit(const uint32_t Offs, const uint8_t BitIndex) {
	if (!this->FileHandler || BitIndex > 7 || Offs >= this->GetSize()) return false;

	fseek(this->FileHandler, Offs, SEEK_SET);
	uint8_t V = 0; fread(&V, 1, 1, this->FileHandler);

	return (V >> BitIndex & 1) != 0;
};

/* ^ Same as above, but for the Editing Buffer instead of FILE stream. */
bool HexData::ReadBitFromEditBuffer(const uint32_t Offs, const uint8_t BitIndex) {
	if (!this->_EditData.data() || BitIndex > 7 || Offs >= this->GetEditSize()) return false;

	const uint8_t V = this->_EditData.data()[Offs];
	return (V >> BitIndex & 1) != 0;
};

/*
	Write a bit to the data.

	const uint32_t Offs: The Offset to write to.
	const uint8_t BitIndex: The Bit index ( 0 - 7 ).
	const bool IsSet: If it's set (1) or not (0).
*/
void HexData::WriteBit(const uint32_t Offs, const uint8_t BitIndex, const bool IsSet) {
	if (!this->FileHandler || BitIndex > 7 || Offs >= this->GetSize()) return;

	fseek(this->FileHandler, Offs, SEEK_SET);
	uint8_t V = 0; fread(&V, 1, 1, this->FileHandler);

	V &= ~(1 << BitIndex);
	V |= (IsSet ? 1 : 0) << BitIndex;

	this->Changes[Offs] = V;
};

/* ^ Same as above, but for the Editing Buffer instead of FILE stream. */
void HexData::WriteBitToEditBuffer(const uint32_t Offs, const uint8_t BitIndex, const bool IsSet) {
	if (!this->_EditData.data() || BitIndex > 7 || Offs >= this->GetEditSize()) return;
	
	this->_EditData.data()[Offs] &= ~(1 << BitIndex);
	this->_EditData.data()[Offs] |= (IsSet ? 1 : 0) << BitIndex;

	this->Changes[this->EditStartOffs + Offs] = this->_EditData.data()[Offs];
	this->UpdateDisplay();
};


/*
	Read Lower / Upper Bits from the data.

	const uint32_t Offs: The offset where to read from.
	const bool First: If Reading from the first four bits, or second.
*/
uint8_t HexData::ReadBits(const uint32_t Offs, const bool First) {
	if (!this->FileHandler || Offs >= this->GetSize()) return 0x0;

	fseek(this->FileHandler, Offs, SEEK_SET);
	uint8_t V = 0; fread(&V, 1, 1, this->FileHandler);

	if (First) return (V & 0xF); // Bit 0 - 3.
	else return (V >> 4); // Bit 4 - 7.
};

/*
	Write Lower / Upper Bits to the data.

	const uint32_t Offs: The offset where to write to.
	const bool First: If Writing on the first four bits, or second.
	const uint8_t Data: The Data to write.
*/
void HexData::WriteBits(const uint32_t Offs, const bool First, const uint8_t Data) {
	if (!this->FileHandler || Data > 0xF || Offs >= this->GetSize()) return;

	fseek(this->FileHandler, Offs, SEEK_SET);
	uint8_t V = 0; fread(&V, 1, 1, this->FileHandler);

	if (First) V = (V & 0xF0) | (Data & 0xF); // Bit 0 - 3.
	else V = (V & 0x0F) | (Data << 4); // Bit 4 - 7.
	
	this->Changes[Offs] = V;
};

/*
	Return a byte to an uint8_t hex string like this: 00, 0F, 20 etc.

	const uint32_t Cursor: The cursor position.
*/
std::string HexData::ByteToString(const uint32_t Cursor) {
	if (Cursor >= this->_DisplayData.size()) return "";
	return Common::ToHex<uint8_t>(this->_DisplayData[Cursor]);
};

/*
	Load an Encoding.

	const std::string &ENCFile: The Encoding JSON file to load.
*/
void HexData::LoadEncoding(const std::string &ENCFile) {
	if (access(ENCFile.c_str(), F_OK) != 0) return;

	/* Open Handle. */
	nlohmann::json ENC;
	FILE *File = fopen(ENCFile.c_str(), "r");

	if (File) {
		ENC = nlohmann::json::parse(File, nullptr, false);
		fclose(File);

	} else {
		return;
	};

	if (ENC.is_discarded()) return; // Bad Encoding data.
	for (size_t Idx = 0; Idx < 256; Idx++) this->Encoding[Idx] = "."; // Reset all to ".".

	if (ENC.contains("map") && ENC["map"].is_object()) {
		for (size_t Idx = 0; Idx < 256; Idx++) {
			const std::string Str = Common::ToHex<uint8_t>(Idx);

			if (ENC["map"].contains(Str) && ENC["map"][Str].is_string()) this->Encoding[Idx] = ENC["map"][Str].get<std::string>();
		};
	};
};

/* DOWN Action. */
void HexData::DOWN() {
	bool DoUpdate = false;

	switch(this->Mode) {
		case HexData::EditMode::Scroll:
			if (this->GetSize() > BYTES_PER_OFFS) { // ONLY if larger as 0x10.
				if (((this->OffsIdx * BYTES_PER_OFFS) + this->CursorPos) < this->GetSize() - BYTES_PER_OFFS) {
					if (this->CursorPos >= this->BytesPerList - BYTES_PER_OFFS) { // When row changed, update display buffer.
						this->OffsIdx++;
						DoUpdate = true;

					} else {
						this->CursorPos += BYTES_PER_OFFS;
					};

					if (((this->OffsIdx * BYTES_PER_OFFS) + this->CursorPos) >= this->GetSize()) {
						if (this->GetSize() < this->BytesPerList) {
							this->CursorPos = this->GetSize() - 1;

						} else {
							this->CursorPos = this->BytesPerList - BYTES_PER_OFFS; // TODO: Set to max.
						};
					};

					if (DoUpdate) this->UpdateDisplay();
				};
			};
			break;

		case HexData::EditMode::Edit:
			if (this->GetEditSize() > BYTES_PER_OFFS) {
				if (((this->OffsIdx * BYTES_PER_OFFS) + this->CursorPos) < (this->EditStart() + this->GetEditSize() - BYTES_PER_OFFS)) {
					if (this->CursorPos >= this->BytesPerList - BYTES_PER_OFFS) { // When row changed, update display buffer.
						this->OffsIdx++;
						DoUpdate = true;

					} else {
						this->CursorPos += BYTES_PER_OFFS;
					};

					if (((this->OffsIdx * BYTES_PER_OFFS) + this->CursorPos) >= (this->EditStart() + this->GetEditSize())) {
						if (this->GetEditSize() < this->BytesPerList) {
							this->CursorPos = this->GetEditSize() - 1;

						} else {
							this->CursorPos = this->BytesPerList - BYTES_PER_OFFS; // TODO: Set to max.
						};
					};

					if (DoUpdate) this->UpdateDisplay();
				};
			};
			break;

		case HexData::EditMode::Change:
			/* We DON'T want to have unexpected behavior, so check buffer first! */
			if (((this->OffsIdx * BYTES_PER_OFFS) + this->CursorPos) - this->EditStart() <= this->GetEditSize()) {
				if (this->_EditData[((this->OffsIdx * BYTES_PER_OFFS) + this->CursorPos) - this->EditStart()] > 0x0) {
					this->_EditData[((this->OffsIdx * BYTES_PER_OFFS) + this->CursorPos) - this->EditStart()]--;

					/* Also set to the changes map so you see what you edited. */
					this->Changes[((this->OffsIdx * BYTES_PER_OFFS) + this->CursorPos)] = this->_EditData[((this->OffsIdx * BYTES_PER_OFFS) + this->CursorPos) - this->EditStart()];
					this->_DisplayData[this->CursorPos] = this->_EditData[((this->OffsIdx * BYTES_PER_OFFS) + this->CursorPos) - this->EditStart()]; // Refresh display.
				};
			};
			break;
	};
};

/* UP Action. */
void HexData::UP() {
	switch(this->Mode) {
		case HexData::EditMode::Scroll:
			if (((this->OffsIdx * BYTES_PER_OFFS) + this->CursorPos) > BYTES_PER_OFFS - 1) {
				if (this->CursorPos > 0xF) this->CursorPos -= BYTES_PER_OFFS;
				else {
					this->OffsIdx--; // One offset row up.
					this->UpdateDisplay();
				};
			};
			break;

		case HexData::EditMode::Edit:
			if (((this->OffsIdx * BYTES_PER_OFFS) + this->CursorPos) > (this->EditStart() + BYTES_PER_OFFS - 1)) {
				if (this->CursorPos > 0xF) this->CursorPos -= BYTES_PER_OFFS;
				else {
					this->OffsIdx--; // One offset row up.
					this->UpdateDisplay();
				};
			};
			break;

		case HexData::EditMode::Change:
			/* We DON'T want to have unexpected behavior, so check buffer first! */
			if (((this->OffsIdx * BYTES_PER_OFFS) + this->CursorPos) - this->EditStart() <= this->GetEditSize()) {
				if (this->_EditData[((this->OffsIdx * BYTES_PER_OFFS) + this->CursorPos) - this->EditStart()] < 0xFF) {
					this->_EditData[((this->OffsIdx * BYTES_PER_OFFS) + this->CursorPos) - this->EditStart()]++;

					/* Also set to the changes map so you see what you edited. */
					this->Changes[((this->OffsIdx * BYTES_PER_OFFS) + this->CursorPos)] = this->_EditData[((this->OffsIdx * BYTES_PER_OFFS) + this->CursorPos) - this->EditStart()];
					this->_DisplayData[this->CursorPos] = this->_EditData[((this->OffsIdx * BYTES_PER_OFFS) + this->CursorPos) - this->EditStart()]; // Refresh display.
				};
			};
			break;
	};
};

/* LEFT Action. */
void HexData::LEFT() {
	switch(this->Mode) {
		case HexData::EditMode::Scroll:
			if (((this->OffsIdx * BYTES_PER_OFFS) + this->CursorPos) > 0) {
				if (this->CursorPos > 0) this->CursorPos--;
				else {
					this->CursorPos = 0xF; // 0xF.
					this->OffsIdx--; // One offset row down.
					this->UpdateDisplay();
				};
			};
			break;

		case HexData::EditMode::Edit:
			if (((this->OffsIdx * BYTES_PER_OFFS) + this->CursorPos) > this->EditStart()) {
				if (this->CursorPos > 0) this->CursorPos--;
				else {
					this->CursorPos = 0xF; // 0xF.
					this->OffsIdx--; // One offset row down.
					this->UpdateDisplay();
				};
			};
			break;

		case HexData::EditMode::Change:
			/* We DON'T want to have unexpected behavior, so check buffer first! */
			if (((this->OffsIdx * BYTES_PER_OFFS) + this->CursorPos) - this->EditStart() <= this->GetEditSize()) {
				if (this->_EditData[((this->OffsIdx * BYTES_PER_OFFS) + this->CursorPos) - this->EditStart()] > 0xF) {
					this->_EditData[((this->OffsIdx * BYTES_PER_OFFS) + this->CursorPos) - this->EditStart()] -= BYTES_PER_OFFS;

					/* Also set to the changes map so you see what you edited. */
					this->Changes[((this->OffsIdx * BYTES_PER_OFFS) + this->CursorPos)] = this->_EditData[((this->OffsIdx * BYTES_PER_OFFS) + this->CursorPos) - this->EditStart()];
					this->_DisplayData[this->CursorPos] = this->_EditData[((this->OffsIdx * BYTES_PER_OFFS) + this->CursorPos) - this->EditStart()]; // Refresh display.
				};
			};
			break;
	};
};

/* RIGHT Action. */
void HexData::RIGHT() {
	switch(this->Mode) {
		case HexData::EditMode::Scroll:
			if (((this->OffsIdx * BYTES_PER_OFFS) + this->CursorPos) < this->GetSize() - 1) {
				if (this->CursorPos < this->BytesPerList - 1) this->CursorPos++;
				else {
					this->CursorPos = this->BytesPerList - BYTES_PER_OFFS;
					this->OffsIdx++; // One offset row down.
					this->UpdateDisplay();
				};
			};
			break;

		case HexData::EditMode::Edit:
			if (((this->OffsIdx * BYTES_PER_OFFS) + this->CursorPos) < (this->EditStart() + this->GetEditSize() - 1)) {
				if (this->CursorPos < this->BytesPerList - 1) this->CursorPos++;
				else {
					this->CursorPos = this->BytesPerList - BYTES_PER_OFFS;
					this->OffsIdx++; // One offset row down.
					this->UpdateDisplay();
				};
			};
			break;

		case HexData::EditMode::Change:
			/* We DON'T want to have unexpected behavior, so check buffer first! */
			if (((this->OffsIdx * BYTES_PER_OFFS) + this->CursorPos) - this->EditStart() <= this->GetEditSize()) {
				if (this->_EditData[((this->OffsIdx * BYTES_PER_OFFS) + this->CursorPos) - this->EditStart()] < 0xF0) {
					this->_EditData[((this->OffsIdx * BYTES_PER_OFFS) + this->CursorPos) - this->EditStart()] += BYTES_PER_OFFS;

					/* Also set to the changes map so you see what you edited. */
					this->Changes[((this->OffsIdx * BYTES_PER_OFFS) + this->CursorPos)] = this->_EditData[((this->OffsIdx * BYTES_PER_OFFS) + this->CursorPos) - this->EditStart()];
					this->_DisplayData[this->CursorPos] = this->_EditData[((this->OffsIdx * BYTES_PER_OFFS) + this->CursorPos) - this->EditStart()]; // Refresh display.
				};
			};
			break;
	};
};


/*
	Update the Display data with the FILE data, or the Editing buffer, depending on the Mode.
*/
void HexData::UpdateDisplay() {
	this->_DisplayData.clear();

	if (this->Mode == HexData::EditMode::Scroll) { // Get Data from FILE.
		/* When it's smaller than this, only load data for the size. */
		if (this->GetSize() < this->BytesPerList) {
			this->_DisplayData.resize(this->GetSize());
			fseek(this->FileHandler, 0, SEEK_SET);
			fread(this->_DisplayData.data(), this->GetSize(), 1, this->FileHandler);
			return;
		};

		this->_DisplayData.resize(this->BytesPerList);
		fseek(this->FileHandler, (this->OffsIdx * BYTES_PER_OFFS), SEEK_SET);
		fread(this->_DisplayData.data(), this->BytesPerList, 1, this->FileHandler);

	} else { // Get Data from _Edit Buffer.
		if (this->GetEditSize() < this->BytesPerList) {
			this->_DisplayData.resize(this->GetEditSize()); // Properly resize before.
			memcpy(this->_DisplayData.data(), this->_EditData.data() + (this->OffsIdx * BYTES_PER_OFFS) - this->EditStartOffs, this->GetEditSize());
			return;
		};

		this->_DisplayData.resize(this->BytesPerList); // Properly resize before.
		memcpy(this->_DisplayData.data(), this->_EditData.data() + (this->OffsIdx * BYTES_PER_OFFS) - this->EditStartOffs, this->BytesPerList);
	};
};


/*
	Fetch the Editing buffer with the FILE data.
*/
void HexData::FetchBuffer() {
	this->_EditData.clear(); // Clear first.
	this->_EditData.resize(this->EditLen);

	/* When it's smaller than this, only load data for the size. */
	if (this->GetSize() < this->EditStart() + this->EditLen) {
		this->_EditData.resize(this->GetSize() - this->EditStart());

	} else { // Else use full LEN.
		this->_EditData.resize(this->EditLen);
	};

	/* Now read the data from the FILE in. */
	fseek(this->FileHandler, this->EditStartOffs, SEEK_SET);
	fread(this->_EditData.data(), this->_EditData.size(), 1, this->FileHandler);
};


/* Switch to the scroll mode. */
void HexData::SwitchToScrollMode() {
	this->_EditData.clear();
	this->Mode = HexData::EditMode::Scroll;

	this->UpdateDisplay();
};

/*
	Switch to the editing mode.
	
	const bool FromScroll: If switching FROM the scroll mode to it, or not.
*/
void HexData::SwitchToEditMode(const bool FromScroll) {
	if (FromScroll) {
		this->_DisplayData.clear(); // Clear, and at a later point refresh it with the edit buffer.
		// this->CursorPos = 0; // Set to top.
		this->EditStartOffs = (this->OffsIdx * BYTES_PER_OFFS);
		this->FetchBuffer();
		this->UpdateDisplay();
	};


	this->Mode = HexData::EditMode::Edit;
};

/* Switch to the value change mode. */
void HexData::SwitchToChangeMode() {
	this->Mode = HexData::EditMode::Change;
};

/* Handle the proper A presses. */
void HexData::APress() {
	switch(this->Mode) {
		case HexData::EditMode::Scroll:
			this->SwitchToEditMode(true);
			break;

		case HexData::EditMode::Edit:
			this->SwitchToChangeMode();
			break;

		case HexData::EditMode::Change:
			break; // Nothing.
	};
};

/* Handle the proper B presses. */
void HexData::BPress() {
	switch(this->Mode) {
		case HexData::EditMode::Scroll:
			break; // Nothing.

		case HexData::EditMode::Edit:
			this->SwitchToScrollMode();
			break;

		case HexData::EditMode::Change:
			this->SwitchToEditMode(false);
			break;
	};
};

/*
	Jump to the specified offset.

	const uint32_t Offs: The offset to jump to.
*/
void HexData::JumpOffs(const uint32_t Offs) {
	if (this->Mode == HexData::EditMode::Scroll) { // Only allow in scroll mode.
		if (Offs < this->GetSize()) {
			if (Offs < this->BytesPerList) {
				this->OffsIdx = 0;
				this->CursorPos = Offs;
				this->UpdateDisplay();

			} else {
				this->OffsIdx = 1 + (Offs - this->BytesPerList) / BYTES_PER_OFFS;
				this->CursorPos = (this->BytesPerList - BYTES_PER_OFFS) + (Offs % BYTES_PER_OFFS);
				this->UpdateDisplay();
			};
		};

	} else { // Edit.
		if (Offs >= this->EditStart() && Offs < this->EditStart() + this->GetEditSize()) {
			if (Offs < this->BytesPerList) {
				this->OffsIdx = 0;
				this->CursorPos = Offs;
				this->UpdateDisplay();

			} else {
				this->OffsIdx = 1 + (Offs - this->BytesPerList) / BYTES_PER_OFFS;
				this->CursorPos = (this->BytesPerList - BYTES_PER_OFFS) + (Offs % BYTES_PER_OFFS);
				this->UpdateDisplay();
			};
		};
	}
};