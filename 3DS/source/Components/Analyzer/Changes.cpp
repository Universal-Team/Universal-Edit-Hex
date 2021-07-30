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
#include "Changes.hpp"

#define CHANGES_PER_LIST 6

void Changes::Draw() {
	Gui::Draw_Rect(49, 0, 271, 20, UniversalEdit::UE->TData->BarColor());
	Gui::Draw_Rect(49, 20, 271, 1, UniversalEdit::UE->TData->BarOutline());
	UniversalEdit::UE->GData->SpriteBlend(sprites_arrow_idx, 50, 0, UniversalEdit::UE->TData->BackArrowColor(), 1.0f);

	if (FileHandler::Loaded && UniversalEdit::UE->CurrentFile) {
		Gui::DrawStringCentered(24, 2, 0.5f, UniversalEdit::UE->TData->TextColor(), Common::GetStr("CHANGE_LIST") + " " + std::to_string(UniversalEdit::UE->CurrentFile->GetChanges().size()), 310);

		auto IT = UniversalEdit::UE->CurrentFile->GetChanges().begin();
		if (this->SPos < UniversalEdit::UE->CurrentFile->GetChanges().size()) std::advance(IT, this->SPos); // Go to the proper idx.

		for (size_t Idx = 0; Idx < CHANGES_PER_LIST && Idx < UniversalEdit::UE->CurrentFile->GetChanges().size(); Idx++) {
			if (this->SPos + Idx == this->Selection) Gui::Draw_Rect(this->Menu[Idx + 1].x - 2, this->Menu[Idx + 1].y - 2, this->Menu[Idx + 1].w + 4, this->Menu[Idx + 1].h + 4, UniversalEdit::UE->TData->ButtonSelected());
			Gui::Draw_Rect(this->Menu[Idx + 1].x, this->Menu[Idx + 1].y, this->Menu[Idx + 1].w, this->Menu[Idx + 1].h, UniversalEdit::UE->TData->ButtonColor());
		
			Gui::DrawStringCentered(24, this->Menu[Idx + 1].y + 7, 0.5f, UniversalEdit::UE->TData->TextColor(), "0x" + Common::ToHex<uint32_t>(IT->first) + " -> " + "0x" + Common::ToHex<uint8_t>(IT->second), 240);
			IT++;
		};
	};
};

void Changes::Handler() {
	if (UniversalEdit::UE->Down & KEY_TOUCH) {
		if (Common::Touching(UniversalEdit::UE->T, this->Menu[0])) this->Back();
	};

	if (FileHandler::Loaded && UniversalEdit::UE->CurrentFile) {
		if (UniversalEdit::UE->Repeat & KEY_DOWN) {
			if (this->Selection < UniversalEdit::UE->CurrentFile->GetChanges().size() - 1) this->Selection++;
			else this->Selection = 0;
		};

		if (UniversalEdit::UE->Repeat & KEY_UP) {
			if (this->Selection > 0) this->Selection--;
			else this->Selection = UniversalEdit::UE->CurrentFile->GetChanges().size() - 1;
		};
	};

	/* Scroll. */
	if (this->Selection < this->SPos) this->SPos = this->Selection;
	else if (this->Selection > this->SPos + CHANGES_PER_LIST - 1) this->SPos = this->Selection - CHANGES_PER_LIST + 1;
};

void Changes::Back() {
	this->SPos = 0, this->Selection = 0;
	Analyzer::Mode = Analyzer::SubMode::Main;
};