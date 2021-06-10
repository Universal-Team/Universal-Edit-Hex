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
	Gui::DrawStringCentered(24, 2, 0.5f, UniversalEdit::UE->TData->TextColor(), Common::GetStr("ANALYZER"), 310);

	if (FileHandler::Loaded) {
		/* Display u8, u16 and u32 buttons. */
		for (uint8_t Idx = 0; Idx < 3; Idx++) {
			Gui::Draw_Rect(this->Menu[Idx + 1].x - 2, this->Menu[Idx + 1].y - 2, this->Menu[Idx + 1].w + 4, this->Menu[Idx + 1].h + 4, UniversalEdit::UE->TData->ButtonSelected());
			Gui::Draw_Rect(this->Menu[Idx + 1].x, this->Menu[Idx + 1].y, this->Menu[Idx + 1].w, this->Menu[Idx + 1].h, UniversalEdit::UE->TData->ButtonColor());
		};

		Gui::DrawString(this->Menu[1].x + 3, this->Menu[1].y + 3, 0.45f, UniversalEdit::UE->TData->TextColor(), "uint8_t");
		Gui::DrawString(this->Menu[2].x + 3, this->Menu[2].y + 3, 0.45f, UniversalEdit::UE->TData->TextColor(), "uint16_t");
		Gui::DrawString(this->Menu[3].x + 3, this->Menu[3].y + 3, 0.45f, UniversalEdit::UE->TData->TextColor(), "uint32_t");

		/* Display Bits. */
		for (uint8_t Idx = 0; Idx < 8; Idx++) {
			Gui::Draw_Rect(this->Menu[Idx + 4].x - 2, this->Menu[Idx + 4].y - 2, this->Menu[Idx + 4].w + 4, this->Menu[Idx + 4].h + 4, UniversalEdit::UE->TData->ButtonSelected());
			Gui::Draw_Rect(this->Menu[Idx + 4].x, this->Menu[Idx + 4].y, this->Menu[Idx + 4].w, this->Menu[Idx + 4].h, UniversalEdit::UE->TData->ButtonColor());

			Gui::DrawString(this->Menu[4 + Idx].x + 3, this->Menu[4 + Idx].y + 3, 0.45f, UniversalEdit::UE->TData->TextColor(), (UniversalEdit::UE->CurrentFile->ReadBit(HexEditor::OffsIdx * 0x10 + HexEditor::CursorIdx, Idx) == 0 ? "0" : "1"));
		};
	};
};

void EditBytes::Handler() {
	if (UniversalEdit::UE->Down & KEY_TOUCH) {
		for (uint8_t Idx = 0; Idx < 12; Idx++) {
			if (Common::Touching(UniversalEdit::UE->T, this->Menu[Idx])) {
				this->Funcs[Idx]();
				break;
			};
		};
	};
};


void EditBytes::SetU8() {
	if (FileHandler::Loaded && UniversalEdit::UE->CurrentFile->GetSize() > 0) {
		UniversalEdit::UE->CurrentFile->GetData()[((HexEditor::OffsIdx * 0x10) + HexEditor::CursorIdx)] = Common::HexPad(Common::GetStr("ENTER_VALUE_IN_HEX"), UniversalEdit::UE->CurrentFile->GetData()[((HexEditor::OffsIdx * 0x10) + HexEditor::CursorIdx)], 0x0, 0xFF, 4);
		UniversalEdit::UE->CurrentFile->SetChanges(true);
	};
};

void EditBytes::SetU16() {
	if (FileHandler::Loaded) {
		if (HexEditor::OffsIdx * 0x10 + HexEditor::CursorIdx + 1 <= UniversalEdit::UE->CurrentFile->GetSize()) {
			const uint16_t Val = Common::HexPad(Common::GetStr("ENTER_VALUE_IN_HEX"), UniversalEdit::UE->CurrentFile->Read<uint16_t>(HexEditor::OffsIdx * 0x10 + HexEditor::CursorIdx, Analyzer::Endian), 0x0, 0xFFFF, 6);
			UniversalEdit::UE->CurrentFile->Write<uint16_t>(HexEditor::OffsIdx * 0x10 + HexEditor::CursorIdx, Val, Analyzer::Endian);
			UniversalEdit::UE->CurrentFile->SetChanges(true);
		};
	};
};

void EditBytes::SetU32() {
	if (FileHandler::Loaded) {
		if (HexEditor::OffsIdx * 0x10 + HexEditor::CursorIdx + 3 <= UniversalEdit::UE->CurrentFile->GetSize()) {
			const uint32_t Val = Common::HexPad(Common::GetStr("ENTER_VALUE_IN_HEX"), UniversalEdit::UE->CurrentFile->Read<uint32_t>(HexEditor::OffsIdx * 0x10 + HexEditor::CursorIdx, Analyzer::Endian), 0x0, 0xFFFFFFFF, 10);
			UniversalEdit::UE->CurrentFile->Write<uint32_t>(HexEditor::OffsIdx * 0x10 + HexEditor::CursorIdx, Val, Analyzer::Endian);
			UniversalEdit::UE->CurrentFile->SetChanges(true);
		};
	};
};

void EditBytes::ToggleBit(const uint8_t Idx) {
	if (FileHandler::Loaded) {
		UniversalEdit::UE->CurrentFile->WriteBit(HexEditor::OffsIdx * 0x10 + HexEditor::CursorIdx, Idx, !UniversalEdit::UE->CurrentFile->ReadBit(HexEditor::OffsIdx * 0x10 + HexEditor::CursorIdx, Idx));
		UniversalEdit::UE->CurrentFile->SetChanges(true);
	};
};


void EditBytes::Back() { Analyzer::Mode = Analyzer::SubMode::Main; };