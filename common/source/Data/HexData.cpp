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

/*
	Initializes new data.

	This one has 1 byte set as 0x0 and it's save file set to Temp.bin.
*/
HexData::HexData() {
	this->FileData.resize(1);
	this->FileData[0] = { 0x0 }; // Init with 0x0.
	this->FileGood = true;

	#ifdef _3DS // 3DS -> sdmc and romfs.
		this->File = "sdmc:/3ds/Universal-Edit/Temp.bin";
		this->LoadEncoding("romfs:/encodings/ascii.json");
	#elif ARM9 // DS(i) -> sd and nitro.
		this->File = "sd:/_nds/Universal-Edit/Temp.bin";
		this->LoadEncoding("nitro:/encodings/ascii.json");
	#endif
};

/*
	Load a file.

	const std::string &File: The file to load.

	Returns -2 for file not existing, -1 for allocate related errors and 0 for good.
*/
int HexData::Load(const std::string &File) {
	this->File = File;
	this->FileGood = false;

	if (access(this->File.c_str(), F_OK) == 0) {
		FILE *In = fopen(this->File.c_str(), "r");

		if (In) {
			fseek(In, 0, SEEK_END);
			const uint32_t Size = ftell(In);
			fseek(In, 0, SEEK_SET);

			/* Do this to ensure to not cause crashes. */
			try {
				this->FileData.resize(Size);
				fread(this->FileData.data(), 1, Size, In);

			} catch(...) {
				fclose(In);
				this->FileGood = false;
				return -1; // "The file load caused an exception. File might be too big.".
			};

			fclose(In);
			this->FileGood = true;
		};

	} else {
		this->FileGood = false;
		return -2; // File doesn't exist.
	};

	return 0;
};


/*
	Insert bytes to a specific offset.

	const uint32_t Offs: The offset to which to insert.
	const std::vector<uint8_t> &ToInsert: The vector of data to insert.

	Returns -2 for out of bounds access, -1 for allocate related errors and 0 for good.
*/
int HexData::InsertBytes(const uint32_t Offs, const std::vector<uint8_t> &ToInsert) {
	if (Offs > this->GetSize()) return - 2; // Out of bounds.

	try {
		this->FileData.insert(this->FileData.begin() + Offs, ToInsert.begin(), ToInsert.end());

	} catch(...) {
		return - 1; // "The insert caused an exception. Issue might be caused by bad allocation through too large data.".
	};

	this->SetChanges(true);
	return 0;
};

/*
	Erase bytes from a specific offset for a specific size.

	const uint32_t Offs: The offset from which to remove.
	const uint32_t Size: The size which to remove.

	Returns -2 for out of bounds access, -1 for erase error, 0 for good.
*/
int HexData::EraseBytes(const uint32_t Offs, const uint32_t Size) {
	if (Offs >= this->GetSize() || Offs + Size > this->GetSize()) return -2; // Out of bounds.
	try {
		this->FileData.erase(this->FileData.begin() + Offs, this->FileData.begin() + Offs + Size);

	} catch(...) {
		return -1; // "The erase caused an exception.".
	};

	this->SetChanges(true);
	return 0;
};

/*
	Write the changes back to the file.

	const std::string &File: The file to write back.
*/
bool HexData::WriteBack(const std::string &File) {
	if (this->IsGood() && this->GetData()) {
		FILE *Out = fopen(File.c_str(), "w");
		fwrite(this->GetData(), 1, this->GetSize(), Out);
		fclose(Out);
		return true;
	};

	return false;
};


/*
	Return a byte to an uint8_t hex string like this: 00, 0F, 20 etc.

	const uint32_t Offs: The offset from which to return the byte from as hex.
*/
std::string HexData::ByteToString(const uint32_t Offs) {
	if (this->IsGood() && this->GetData() && Offs < this->GetSize()) return Common::ToHex<uint8_t>(this->GetData()[Offs]);
	return "";
};


/*
	Return a bit from the data.

	const uint32_t Offs: The Offset to read from.
	const uint8_t BitIndex: The Bit index ( 0 - 7 ).
*/
bool HexData::ReadBit(const uint32_t Offs, const uint8_t BitIndex) {
	if (!this->IsGood() || !this->GetData() || BitIndex > 7 || Offs >= this->GetSize()) return false;

	return (this->GetData()[Offs] >> BitIndex & 1) != 0;
};

/*
	Write a bit to the data.

	const uint32_t Offs: The Offset to write to.
	const uint8_t BitIndex: The Bit index ( 0 - 7 ).
	const bool IsSet: If it's set (1) or not (0).
*/
void HexData::WriteBit(const uint32_t Offs, const uint8_t BitIndex, const bool IsSet) {
	if (!this->IsGood() || !this->GetData() || BitIndex > 7 || Offs >= this->GetSize()) return;

	this->GetData()[Offs] &= ~(1 << BitIndex);
	this->GetData()[Offs] |= (IsSet ? 1 : 0) << BitIndex;

	this->SetChanges(true);
};



/*
	Read Lower / Upper Bits from the data.

	const uint32_t Offs: The offset where to read from.
	const bool First: If Reading from the first four bits, or second.
*/
uint8_t HexData::ReadBits(const uint32_t Offs, const bool First) {
	if (!this->IsGood() || !this->GetData() || Offs >= this->GetSize()) return 0x0;

	if (First) return (this->GetData()[Offs] & 0xF); // Bit 0 - 3.
	else return (this->GetData()[Offs] >> 4); // Bit 4 - 7.
};

/*
	Write Lower / Upper Bits to the data.

	const uint32_t Offs: The offset where to write to.
	const bool First: If Writing on the first four bits, or second.
	const uint8_t Data: The Data to write.
*/
void HexData::WriteBits(const uint32_t Offs, const bool First, const uint8_t Data) {
	if (!this->IsGood() || !this->GetData() || Data > 0xF || Offs >= this->GetSize()) return;

	if (First) this->GetData()[Offs] = (this->GetData()[Offs] & 0xF0) | (Data & 0xF); // Bit 0 - 3.
	else this->GetData()[Offs] = (this->GetData()[Offs] & 0x0F) | (Data << 4); // Bit 4 - 7.
	this->SetChanges(true);
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