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
#include "Navigation.hpp"

Navigation::SubMode Navigation::Mode = Navigation::SubMode::Main;

void Navigation::Draw() {
	switch(Navigation::Mode) {
		case Navigation::SubMode::Main: // Draw the main menu.
			Gui::Draw_Rect(49, 0, 271, 20, UniversalEdit::UE->TData->BarColor());
			Gui::Draw_Rect(49, 20, 271, 1, UniversalEdit::UE->TData->BarOutline());
			Gui::DrawStringCentered(24, 2, 0.5f, UniversalEdit::UE->TData->TextColor(), Common::GetStr("NAVIGATOR_MENU"), 310);

			if (FileHandler::Loaded) {
				for (uint8_t Idx = 0; Idx < 3; Idx++) {
					Gui::Draw_Rect(this->Menu[Idx].x - 2, this->Menu[Idx].y - 2, this->Menu[Idx].w + 4, this->Menu[Idx].h + 4, UniversalEdit::UE->TData->ButtonSelected());
					Gui::Draw_Rect(this->Menu[Idx].x, this->Menu[Idx].y, this->Menu[Idx].w, this->Menu[Idx].h, UniversalEdit::UE->TData->ButtonColor());

					Gui::DrawStringCentered(24, this->Menu[Idx].y + 9, 0.4f, UniversalEdit::UE->TData->TextColor(), Common::GetStr(this->MenuOptions[Idx]));
				};
			};
			break;

		case Navigation::SubMode::Search: // Search Mode.
			this->_Search->Draw();
			break;

		case Navigation::SubMode::Reminsert: // Remove Insert.
			this->RemInsert->Draw();
			break;
	};
};

void Navigation::Handler() {
	switch(Navigation::Mode) {
		case Navigation::SubMode::Main: // Sub Main.
			if (FileHandler::Loaded) {
				if (UniversalEdit::UE->Down & KEY_TOUCH) {
					for (uint8_t Idx = 0; Idx < 3; Idx++) {
						if (Common::Touching(UniversalEdit::UE->T, this->Menu[Idx])) {
							this->Funcs[Idx]();
							break;
						};
					};
				};
			};
			break;

		case Navigation::SubMode::Search: // Search Mode.
			this->_Search->Handler();
			break;

		case Navigation::SubMode::Reminsert: // Remove Insert.
			this->RemInsert->Handler();
			break;
	};
};


void Navigation::AccessSearch() { Navigation::Mode = Navigation::SubMode::Search; };
void Navigation::AccessRemInsert() { Navigation::Mode = Navigation::SubMode::Reminsert; };

void Navigation::JumpTo() {
	if (FileHandler::Loaded && UniversalEdit::UE->CurrentFile->GetSize() > 0) {
		const uint32_t Offs = Common::HexPad(Common::GetStr("ENTER_OFFSET_IN_HEX"), (HexEditor::OffsIdx * 0x10) + HexEditor::CursorIdx, 0, UniversalEdit::UE->CurrentFile->GetSize() - 1, 10);

		if (Offs != (HexEditor::OffsIdx * 0x10) + HexEditor::CursorIdx) {
			if (Offs < 0xD0) {
				HexEditor::OffsIdx = 0;
				HexEditor::CursorIdx = Offs;

			} else {
				HexEditor::OffsIdx = 1 + ((Offs - 0xD0) / 0x10);
				HexEditor::CursorIdx = (0xD0 - 0x10) + (Offs % 0x10);
			};
		};
	};
};