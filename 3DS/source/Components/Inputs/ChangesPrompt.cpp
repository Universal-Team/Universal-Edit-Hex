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
#include "ChangesPrompt.hpp"

#define CHANGES_PER_LIST 4

void ChangesPrompt::Handler() {
	while(aptMainLoop() && !this->Confirmed) {
		C2D_TargetClear(Top, C2D_Color32(0, 0, 0, 0));
		C2D_TargetClear(Bottom, C2D_Color32(0, 0, 0, 0));
		Gui::clearTextBufs();
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

		UniversalEdit::UE->DrawTop(); // Keep the top screen for sure.
		UniversalEdit::UE->GData->DrawBottom();

		Gui::Draw_Rect(0, 0, 320, 20, UniversalEdit::UE->TData->BarColor());
		Gui::DrawStringCentered(0, 1, 0.6f, UniversalEdit::UE->TData->TextColor(), Common::GetStr("CHANGES_MADE") + std::to_string(UniversalEdit::UE->CurrentFile->GetChanges().size()), 310);

		auto IT = UniversalEdit::UE->CurrentFile->GetChanges().begin();
		if (this->SPos < UniversalEdit::UE->CurrentFile->GetChanges().size()) std::advance(IT, this->SPos); // Go to the proper idx.

		for (uint8_t Idx = 0; Idx < 2; Idx++) {
			if (Idx == this->Res) Gui::Draw_Rect(this->Buttons[Idx].x - 2, this->Buttons[Idx].y - 2, this->Buttons[Idx].w + 4, this->Buttons[Idx].h + 4, UniversalEdit::UE->TData->ButtonSelected());
			Gui::Draw_Rect(this->Buttons[Idx].x, this->Buttons[Idx].y, this->Buttons[Idx].w, this->Buttons[Idx].h, UniversalEdit::UE->TData->ButtonColor());

			Gui::DrawStringCentered((Idx ? 60 : -60), this->Buttons[Idx].y + 3, 0.6f, UniversalEdit::UE->TData->TextColor(), (Idx ? Common::GetStr("CONFIRM") : Common::GetStr("CANCEL")));
		};

		for (size_t Idx = 0; Idx < CHANGES_PER_LIST && Idx < UniversalEdit::UE->CurrentFile->GetChanges().size(); Idx++) {
			if (this->SPos + Idx == this->Selection) Gui::Draw_Rect(this->Menu[Idx].x - 2, this->Menu[Idx].y - 2, this->Menu[Idx].w + 4, this->Menu[Idx].h + 4, UniversalEdit::UE->TData->ButtonSelected());
			Gui::Draw_Rect(this->Menu[Idx].x, this->Menu[Idx].y, this->Menu[Idx].w, this->Menu[Idx].h, UniversalEdit::UE->TData->ButtonColor());
		
			Gui::DrawStringCentered(0, this->Menu[Idx].y + 7, 0.5f, UniversalEdit::UE->TData->TextColor(), "0x" + Common::ToHex<uint32_t>(IT->first) + " -> " + "0x" + Common::ToHex<uint8_t>(IT->second), 240);
			IT++;
		};

		C3D_FrameEnd(0);
		uint32_t Down = 0, Repeat = 0;
		touchPosition T;
		
		do {
			gspWaitForVBlank();
			hidScanInput();
			Down = hidKeysDown();
			Repeat = hidKeysDownRepeat();
			hidTouchRead(&T);
		} while(!Down || !Repeat);

		if (Down & KEY_LEFT || Down & KEY_RIGHT) this->Res = !this->Res;
		if (Down & KEY_A) {
			this->Confirmed = true;
		};
		if (Repeat & KEY_DOWN) {
			if (this->Selection < UniversalEdit::UE->CurrentFile->GetChanges().size() - 1) this->Selection++;
		};

		if (Repeat & KEY_UP) {
			if (this->Selection > 0) this->Selection--;
		};

		if (Down & KEY_TOUCH) {
			for (uint8_t Idx = 0; Idx < 2; Idx++) {
				if (Common::Touching(T, this->Buttons[Idx])) {
					this->Res = Idx;
					this->Confirmed = true;
					break;
				};
			};
		};

		/* Scroll. */
		if (this->Selection < this->SPos) this->SPos = this->Selection;
		else if (this->Selection > this->SPos + CHANGES_PER_LIST - 1) this->SPos = this->Selection - CHANGES_PER_LIST + 1;
	};

	UniversalEdit::UE->CurrentFile->WriteChanges(this->Res);
};