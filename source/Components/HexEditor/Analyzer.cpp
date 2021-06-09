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

#include "Analyzer.hpp"
#include "Common.hpp"

#include "Analyzer.hpp"
#include "Common.hpp"

void Analyzer::Draw() {
	Gui::Draw_Rect(49, 0, 271, 20, UniversalEdit::UE->TData->BarColor());
	Gui::Draw_Rect(49, 20, 271, 1, UniversalEdit::UE->TData->BarOutline());
	UniversalEdit::UE->GData->SpriteBlend(sprites_arrow_idx, 50, 0, UniversalEdit::UE->TData->BackArrowColor(), 1.0f);
	Gui::DrawStringCentered(24, 2, 0.5f, UniversalEdit::UE->TData->TextColor(), Utils::GetStr("ANALYZER"), 310);

	if (FileHandler::Loaded) {
		if (this->IsOnAnalyzer) { // Main Analyzer.
			/* Draw Selection Size. */
			Gui::DrawString(60, this->Menu[1].y + 3, 0.45f, UniversalEdit::UE->TData->TextColor(), Utils::GetStr("SELECTION_SIZE"));
			Gui::DrawString(this->Menu[3].x + 30, this->Menu[1].y + 3, 0.45f, UniversalEdit::UE->TData->TextColor(), Utils::GetStr("BYTES"));
			for (uint8_t Idx = 0; Idx < 3; Idx++) {
				if (HexEditor::SelectionSize == 1 << Idx) Gui::Draw_Rect(this->Menu[Idx + 1].x - 2, this->Menu[Idx + 1].y - 2, this->Menu[Idx + 1].w + 4, this->Menu[Idx + 1].h + 4, UniversalEdit::UE->TData->ButtonSelected());
		
				Gui::Draw_Rect(this->Menu[Idx + 1].x, this->Menu[Idx + 1].y, this->Menu[Idx + 1].w, this->Menu[Idx + 1].h, UniversalEdit::UE->TData->ButtonColor());
				Gui::DrawString(this->Menu[Idx + 1].x + 1, this->Menu[Idx + 1].y + 1, 0.4f, UniversalEdit::UE->TData->TextColor(), std::to_string(1 << Idx));
			};

			/* Draw Endian and Hex/Dec buttons. */
			Gui::Draw_Rect(this->Menu[4].x, this->Menu[4].y, this->Menu[4].w, this->Menu[4].h, UniversalEdit::UE->TData->ButtonColor()); // LE / BE.
			Gui::DrawString(this->Menu[4].x + 1, this->Menu[4].y + 1, 0.4f, UniversalEdit::UE->TData->TextColor(), (this->Endian ? "BE" : "LE"));

			Gui::Draw_Rect(this->Menu[5].x, this->Menu[5].y, this->Menu[5].w, this->Menu[5].h, UniversalEdit::UE->TData->ButtonColor()); // Hex / Dec.
			Gui::DrawString(this->Menu[5].x + 1, this->Menu[5].y + 1, 0.4f, UniversalEdit::UE->TData->TextColor(), (this->Hex ? "Hex" : "Dec"));


			union {
				uint32_t U32;
				int8_t S8;
				int16_t S16;
				int32_t S32;
				float F;
			} Val;

			if (this->Endian) { // Big Endian.
				for (int Idx = 0; Idx < HexEditor::SelectionSize && HexEditor::OffsIdx * 0x10 + HexEditor::CursorIdx + Idx < UniversalEdit::UE->CurrentFile->GetSize(); Idx++) {
					Val.U32 |= *(UniversalEdit::UE->CurrentFile->GetData() + HexEditor::OffsIdx * 0x10 + HexEditor::CursorIdx + Idx) << (HexEditor::SelectionSize - 1 - Idx) * 8;
				};

			} else { // Little Endian.
				for (int Idx = 0; Idx < HexEditor::SelectionSize && HexEditor::OffsIdx * 0x10 + HexEditor::CursorIdx + Idx < UniversalEdit::UE->CurrentFile->GetSize(); Idx++) {
					Val.U32 |= *(UniversalEdit::UE->CurrentFile->GetData() + HexEditor::OffsIdx * 0x10 + HexEditor::CursorIdx + Idx) << Idx * 8;
				};
			};

			char Str[32] = { 0 };

			/* Draw Unsigned Integer. */
			snprintf(Str, sizeof(Str), this->Hex ? "0x%lX" : "%lu", Val.U32);
			Gui::DrawString(60, this->Menu[6].y + 3, 0.45f, UniversalEdit::UE->TData->TextColor(), Utils::GetStr("UNSIGNED_INT") + Str);

			/* Draw Signed Integer. */
			snprintf(Str, sizeof(Str), "%ld", HexEditor::SelectionSize == 1 ? Val.S8 : (HexEditor::SelectionSize == 2 ? Val.S16 : Val.S32));
			Gui::DrawString(60, this->Menu[7].y + 3, 0.45f, UniversalEdit::UE->TData->TextColor(), Utils::GetStr("SIGNED_INT") + Str);

			/* Draw Float. */
			snprintf(Str, sizeof(Str), "%e", Val.F);
			Gui::DrawString(60, this->Menu[8].y + 3, 0.45f, UniversalEdit::UE->TData->TextColor(), Utils::GetStr("FLOAT") + Str);

			/* Draw Binary. */
			for (uint8_t Idx = 0; Idx < HexEditor::SelectionSize * 9; Idx++) {
				if (Idx % 9 == 8) Str[Idx] = ' ';
				else Str[Idx] = ((Val.U32 & (1 << Idx)) >> Idx) ? '1' : '0';
			};

			Str[HexEditor::SelectionSize * 9] = 0;
			Gui::DrawString(60, this->Menu[9].y + 3, 0.45f, UniversalEdit::UE->TData->TextColor(), Utils::GetStr("BINARY") + Str, 260);

			/* Draw UTF-8. */
			memcpy(Str, UniversalEdit::UE->CurrentFile->GetData() + HexEditor::OffsIdx * 0x10 + HexEditor::CursorIdx, std::min((uint32_t)HexEditor::SelectionSize, UniversalEdit::UE->CurrentFile->GetSize() - HexEditor::OffsIdx * 0x10 - HexEditor::CursorIdx));
			for (uint8_t Idx = 0; Idx < HexEditor::SelectionSize; Idx++) {
				if (Str[Idx] == 0) Str[Idx] = '.';
			};

			Str[std::min((uint32_t)HexEditor::SelectionSize, UniversalEdit::UE->CurrentFile->GetSize() - HexEditor::OffsIdx * 0x10 - HexEditor::CursorIdx)] = 0;
			Gui::DrawString(60, this->Menu[10].y + 3, 0.45f, UniversalEdit::UE->TData->TextColor(), Utils::GetStr("UTF_8") + Str);

		} else { // Edit Bytes menu.
			/* Display u8, u16 and u32 buttons. */
			for (uint8_t Idx = 0; Idx < 3; Idx++) {
				Gui::Draw_Rect(this->EditMenu[Idx + 1].x - 2, this->EditMenu[Idx + 1].y - 2, this->EditMenu[Idx + 1].w + 4, this->EditMenu[Idx + 1].h + 4, UniversalEdit::UE->TData->ButtonSelected());
				Gui::Draw_Rect(this->EditMenu[Idx + 1].x, this->EditMenu[Idx + 1].y, this->EditMenu[Idx + 1].w, this->EditMenu[Idx + 1].h, UniversalEdit::UE->TData->ButtonColor());
			};

			Gui::DrawString(this->EditMenu[1].x + 3, this->EditMenu[1].y + 3, 0.45f, UniversalEdit::UE->TData->TextColor(), "uint8_t");
			Gui::DrawString(this->EditMenu[2].x + 3, this->EditMenu[2].y + 3, 0.45f, UniversalEdit::UE->TData->TextColor(), "uint16_t");
			Gui::DrawString(this->EditMenu[3].x + 3, this->EditMenu[3].y + 3, 0.45f, UniversalEdit::UE->TData->TextColor(), "uint32_t");

			/* Display Bits. */
			for (uint8_t Idx = 0; Idx < 8; Idx++) {
				Gui::Draw_Rect(this->EditMenu[Idx + 4].x - 2, this->EditMenu[Idx + 4].y - 2, this->EditMenu[Idx + 4].w + 4, this->EditMenu[Idx + 4].h + 4, UniversalEdit::UE->TData->ButtonSelected());
				Gui::Draw_Rect(this->EditMenu[Idx + 4].x, this->EditMenu[Idx + 4].y, this->EditMenu[Idx + 4].w, this->EditMenu[Idx + 4].h, UniversalEdit::UE->TData->ButtonColor());

				Gui::DrawString(this->EditMenu[4 + Idx].x + 3, this->EditMenu[4 + Idx].y + 3, 0.45f, UniversalEdit::UE->TData->TextColor(), (UniversalEdit::UE->CurrentFile->ReadBit(HexEditor::OffsIdx * 0x10 + HexEditor::CursorIdx, Idx) == 0 ? "0" : "1"));
			};
		};
	};
};

void Analyzer::SwitchByteSize(const uint8_t Size) {
	if (FileHandler::Loaded) {
		/* Ensure size is within range. */
		if (((HexEditor::OffsIdx * 0x10) + HexEditor::CursorIdx) + Size - 1 < UniversalEdit::UE->CurrentFile->GetSize()) {
			HexEditor::SelectionSize = Size;
		};
	};
};

void Analyzer::ToggleEndian() { this->Endian = !this->Endian; };
void Analyzer::ToggleHex() { this->Hex = !this->Hex; };

void Analyzer::Handler() {
	if (FileHandler::Loaded) {
		if (this->IsOnAnalyzer) {
			if (UniversalEdit::UE->Down & KEY_TOUCH) {
				for (uint8_t Idx = 0; Idx < 6; Idx++) {
					if (Utils::Touching(UniversalEdit::UE->T, this->Menu[Idx])) {
						this->MainFuncs[Idx]();
						break;
					};
				};
			};

		} else {
			if (UniversalEdit::UE->Down & KEY_TOUCH) {
				for (uint8_t Idx = 0; Idx < 12; Idx++) {
					if (Utils::Touching(UniversalEdit::UE->T, this->EditMenu[Idx])) {
						this->EditFuncs[Idx]();
						break;
					};
				};
			};
		};

		if (UniversalEdit::UE->Down & KEY_Y) { // For now `Y` switches between Analyze and Edit Mode.
			this->IsOnAnalyzer = !this->IsOnAnalyzer;
		};
	};
};


void Analyzer::SetU8() {
	if (FileHandler::Loaded && UniversalEdit::UE->CurrentFile->GetSize() > 0) {
		UniversalEdit::UE->CurrentFile->GetData()[((HexEditor::OffsIdx * 0x10) + HexEditor::CursorIdx)] = Utils::HexPad(Utils::GetStr("ENTER_VALUE_IN_HEX"), UniversalEdit::UE->CurrentFile->GetData()[((HexEditor::OffsIdx * 0x10) + HexEditor::CursorIdx)], 0x0, 0xFF, 4);
		UniversalEdit::UE->CurrentFile->SetChanges(true);
	};
};

void Analyzer::SetU16() {
	if (FileHandler::Loaded) {
		if (HexEditor::OffsIdx * 0x10 + HexEditor::CursorIdx + 1 <= UniversalEdit::UE->CurrentFile->GetSize()) {
			const uint16_t Val = Utils::HexPad(Utils::GetStr("ENTER_VALUE_IN_HEX"), UniversalEdit::UE->CurrentFile->Read<uint16_t>(HexEditor::OffsIdx * 0x10 + HexEditor::CursorIdx, this->Endian), 0x0, 0xFFFF, 6);
			UniversalEdit::UE->CurrentFile->Write<uint16_t>(HexEditor::OffsIdx * 0x10 + HexEditor::CursorIdx, Val, this->Endian);
			UniversalEdit::UE->CurrentFile->SetChanges(true);
		};
	};
};

void Analyzer::SetU32() {
	if (FileHandler::Loaded) {
		if (HexEditor::OffsIdx * 0x10 + HexEditor::CursorIdx + 3 <= UniversalEdit::UE->CurrentFile->GetSize()) {
			const uint32_t Val = Utils::HexPad(Utils::GetStr("ENTER_VALUE_IN_HEX"), UniversalEdit::UE->CurrentFile->Read<uint32_t>(HexEditor::OffsIdx * 0x10 + HexEditor::CursorIdx, this->Endian), 0x0, 0xFFFFFFFF, 10);
			UniversalEdit::UE->CurrentFile->Write<uint32_t>(HexEditor::OffsIdx * 0x10 + HexEditor::CursorIdx, Val, this->Endian);
			UniversalEdit::UE->CurrentFile->SetChanges(true);
		};
	};
};

void Analyzer::ToggleBit(const uint8_t Idx) {
	if (FileHandler::Loaded) {
		UniversalEdit::UE->CurrentFile->WriteBit(HexEditor::OffsIdx * 0x10 + HexEditor::CursorIdx, Idx, !UniversalEdit::UE->CurrentFile->ReadBit(HexEditor::OffsIdx * 0x10 + HexEditor::CursorIdx, Idx));
		UniversalEdit::UE->CurrentFile->SetChanges(true);
	};
};


void Analyzer::Back() { HexEditor::Mode = HexEditor::SubMode::Sub; };