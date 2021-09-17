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
#include "EditBytes.hpp"


void EditBytes::Draw() {
	Gui::Draw_Rect(49, 0, 271, 20, UniversalEdit::UE->TData->BarColor());
	Gui::Draw_Rect(49, 20, 271, 1, UniversalEdit::UE->TData->BarOutline());
	UniversalEdit::UE->GData->SpriteBlend(sprites_arrow_idx, 50, 0, UniversalEdit::UE->TData->BackArrowColor(), 1.0f);
	Gui::DrawStringCentered(24, 2, 0.5f, UniversalEdit::UE->TData->TextColor(), Common::GetStr("EDIT_BYTES"), 310);

	if (FileHandler::Loaded) {
		/* Display Hexadecimal | Decimal, Endian, u8, u16 and u32 buttons. */
		for (uint8_t Idx = 0; Idx < 5; Idx++) {
			Gui::Draw_Rect(this->Menu[Idx + 1].x - 2, this->Menu[Idx + 1].y - 2, this->Menu[Idx + 1].w + 4, this->Menu[Idx + 1].h + 4, UniversalEdit::UE->TData->ButtonSelected());
			Gui::Draw_Rect(this->Menu[Idx + 1].x, this->Menu[Idx + 1].y, this->Menu[Idx + 1].w, this->Menu[Idx + 1].h, UniversalEdit::UE->TData->ButtonColor());
		}

		Gui::DrawStringCentered(-40, this->Menu[1].y + 3, 0.45f, UniversalEdit::UE->TData->TextColor(), (this->HexMode ? Common::GetStr("HEX") : Common::GetStr("DECIMAL")));
		Gui::DrawStringCentered(89, this->Menu[2].y + 3, 0.45f, UniversalEdit::UE->TData->TextColor(), (this->Endian ? Common::GetStr("BIG_ENDIAN") : Common::GetStr("LITTLE_ENDIAN")));

		Gui::DrawStringCentered(24, this->Menu[3].y + 3, 0.45f, UniversalEdit::UE->TData->TextColor(), "uint8_t");
		Gui::DrawStringCentered(24, this->Menu[4].y + 3, 0.45f, UniversalEdit::UE->TData->TextColor(), "uint16_t");
		Gui::DrawStringCentered(24, this->Menu[5].y + 3, 0.45f, UniversalEdit::UE->TData->TextColor(), "uint32_t");

		/* Display Bits. */
		for (uint8_t Idx = 0; Idx < 8; Idx++) {
			Gui::Draw_Rect(this->Menu[Idx + 6].x - 2, this->Menu[Idx + 6].y - 2, this->Menu[Idx + 6].w + 4, this->Menu[Idx + 6].h + 4, UniversalEdit::UE->TData->ButtonSelected());
			Gui::Draw_Rect(this->Menu[Idx + 6].x, this->Menu[Idx + 6].y, this->Menu[Idx + 6].w, this->Menu[Idx + 6].h, UniversalEdit::UE->TData->ButtonColor());

			Gui::DrawString(this->Menu[Idx + 6].x + 6, this->Menu[Idx + 6].y + 3, 0.45f, UniversalEdit::UE->TData->TextColor(), (UniversalEdit::UE->CurrentFile->ReadBitFromEditBuffer(UniversalEdit::UE->CurrentFile->GetOffs() * 0x10 + UniversalEdit::UE->CurrentFile->GetCursor() - UniversalEdit::UE->CurrentFile->EditStart(), Idx) == 0 ? "0" : "1"));
		}
	}
};


void EditBytes::Handler() {
	if (UniversalEdit::UE->Down & KEY_TOUCH) {
		for (uint8_t Idx = 0; Idx < 14; Idx++) {
			if (Common::Touching(UniversalEdit::UE->T, this->Menu[Idx])) {
				this->Funcs[Idx]();
				break;
			}
		}
	}

	if (UniversalEdit::UE->CurrentFile->GetCurMode() == HexData::EditMode::Scroll) this->Back();
};


/* Hex / Decimal and Endian. */
void EditBytes::ChangeInputMethod() { this->HexMode = !this->HexMode; };
void EditBytes::ChangeEndian() { this->Endian = !this->Endian; };


void EditBytes::SetU8() {
	if (FileHandler::Loaded && UniversalEdit::UE->CurrentFile->GetEditSize() > 0) {
		uint8_t V = UniversalEdit::UE->CurrentFile->EditData()[UniversalEdit::UE->CurrentFile->GetOffs() * 0x10 + UniversalEdit::UE->CurrentFile->GetCursor() - UniversalEdit::UE->CurrentFile->EditStart()];

		if (this->HexMode) V = Common::HexPad(Common::GetStr("ENTER_VALUE_IN_HEX"), V, 0x0, 0xFF, 4);
		else V = Common::Numpad(Common::GetStr("ENTER_VALUE_IN_DEC"), V, 0, 255, 3);

		UniversalEdit::UE->CurrentFile->EditData()[UniversalEdit::UE->CurrentFile->GetOffs() * 0x10 + UniversalEdit::UE->CurrentFile->GetCursor() - UniversalEdit::UE->CurrentFile->EditStart()] = V;
		UniversalEdit::UE->CurrentFile->GetChanges()[UniversalEdit::UE->CurrentFile->GetOffs() * 0x10 + UniversalEdit::UE->CurrentFile->GetCursor() - UniversalEdit::UE->CurrentFile->EditStart()] = V;
		UniversalEdit::UE->CurrentFile->UpdateDisplay();
	}
};


void EditBytes::SetU16() {
	if (FileHandler::Loaded && UniversalEdit::UE->CurrentFile->GetEditSize() > 0) {
		if (UniversalEdit::UE->CurrentFile->GetOffs() * 0x10 + UniversalEdit::UE->CurrentFile->GetCursor() - UniversalEdit::UE->CurrentFile->EditStart() + 1 <= UniversalEdit::UE->CurrentFile->GetEditSize()) {
			uint16_t V = UniversalEdit::UE->CurrentFile->ReadFromEditBuffer<uint16_t>(UniversalEdit::UE->CurrentFile->GetOffs() * 0x10 + UniversalEdit::UE->CurrentFile->GetCursor() - UniversalEdit::UE->CurrentFile->EditStart(), this->Endian);

			if (this->HexMode) V = Common::HexPad(Common::GetStr("ENTER_VALUE_IN_HEX"), V, 0x0, 0xFFFF, 6);
			else V = Common::Numpad(Common::GetStr("ENTER_VALUE_IN_DEC"), V, 0, 0xFFFF, 5);

			UniversalEdit::UE->CurrentFile->WriteToEditBuffer<uint16_t>(UniversalEdit::UE->CurrentFile->GetOffs() * 0x10 + UniversalEdit::UE->CurrentFile->GetCursor() - UniversalEdit::UE->CurrentFile->EditStart(), V, this->Endian);
			UniversalEdit::UE->CurrentFile->UpdateDisplay();
		}
	}
};


void EditBytes::SetU32() {
	if (FileHandler::Loaded) {
		if (UniversalEdit::UE->CurrentFile->GetOffs() * 0x10 + UniversalEdit::UE->CurrentFile->GetCursor() - UniversalEdit::UE->CurrentFile->EditStart() + 3 <= UniversalEdit::UE->CurrentFile->GetEditSize()) {
			uint32_t V = UniversalEdit::UE->CurrentFile->ReadFromEditBuffer<uint32_t>(UniversalEdit::UE->CurrentFile->GetOffs() * 0x10 + UniversalEdit::UE->CurrentFile->GetCursor() - UniversalEdit::UE->CurrentFile->EditStart(), this->Endian);

			if (this->HexMode) V = Common::HexPad(Common::GetStr("ENTER_VALUE_IN_HEX"), V, 0x0, 0xFFFFFFFF, 10);
			else Common::Numpad(Common::GetStr("ENTER_VALUE_IN_DEC"), V, 0, 0xFFFFFFFF, 10);

			UniversalEdit::UE->CurrentFile->WriteToEditBuffer<uint32_t>(UniversalEdit::UE->CurrentFile->GetOffs() * 0x10 + UniversalEdit::UE->CurrentFile->GetCursor() - UniversalEdit::UE->CurrentFile->EditStart(), V, this->Endian);
			UniversalEdit::UE->CurrentFile->UpdateDisplay();
		}
	}
};


void EditBytes::ToggleBit(const uint8_t Idx) {
	if (FileHandler::Loaded) {
		UniversalEdit::UE->CurrentFile->WriteBitToEditBuffer(UniversalEdit::UE->CurrentFile->GetOffs() * 0x10 + UniversalEdit::UE->CurrentFile->GetCursor() - UniversalEdit::UE->CurrentFile->EditStart(), Idx, !UniversalEdit::UE->CurrentFile->ReadBitFromEditBuffer(UniversalEdit::UE->CurrentFile->GetOffs() * 0x10 + UniversalEdit::UE->CurrentFile->GetCursor() - UniversalEdit::UE->CurrentFile->EditStart(), Idx));
		UniversalEdit::UE->CurrentFile->UpdateDisplay();
	}
};


void EditBytes::Back() { Analyzer::Mode = Analyzer::SubMode::Main; };