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
#include "FileBrowser.hpp"
#include "FileCompare.hpp"

#define ENTRIES_PER_LIST 6


void FileCompare::Handler() {
	/* The Compare File Selector here. */
	std::unique_ptr<FileBrowser> FB = std::make_unique<FileBrowser>();
	bool Good = true;

	const std::string F1 = FB->Handler("sdmc:/", false, Common::GetStr("SELECT_FIRST_FILE_COMPARE"), { });

	if (F1 != "") {
		const std::string F2 = FB->Handler("sdmc:/", false, Common::GetStr("SELECT_SECOND_FILE_COMPARE"), { });

		if (F2 != "") {
			this->Changes = Actions::CompareFiles(F1, F2);

		} else {
			Good = false;
		}

	} else {
		Good = false;
	}
	
	gspWaitForVBlank();
	hidScanInput();

	/* In case both files are good, do this. */
	if (Good) {
		while(aptMainLoop() && !this->Confirmed) {
			C2D_TargetClear(Top, C2D_Color32(0, 0, 0, 0));
			C2D_TargetClear(Bottom, C2D_Color32(0, 0, 0, 0));
			Gui::clearTextBufs();
			C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

			UniversalEdit::UE->DrawTop(); // Keep the top screen for sure.
			UniversalEdit::UE->GData->DrawBottom();

			Gui::Draw_Rect(0, 0, 320, 20, UniversalEdit::UE->TData->BarColor());
			UniversalEdit::UE->GData->SpriteBlend(sprites_arrow_idx, 0, 0, UniversalEdit::UE->TData->BackArrowColor(), 1.0f);
			Gui::DrawStringCentered(0, 2, 0.5f, UniversalEdit::UE->TData->TextColor(), Common::GetStr("FOUND_CHANGES") + std::to_string(this->Changes.size()), 310);

			for (size_t Idx = 0; Idx < ENTRIES_PER_LIST && Idx < this->Changes.size(); Idx++) {
				if (this->SPos + Idx == this->Selection) {
					Gui::Draw_Rect(this->XPos[0] - 2, this->YPos[Idx] - 2, 40 + 4, 30 + 4, UniversalEdit::UE->TData->ButtonSelected());
					Gui::Draw_Rect(this->XPos[1] - 2, this->YPos[Idx] - 2, 120 + 4, 30 + 4, UniversalEdit::UE->TData->ButtonSelected());
					Gui::Draw_Rect(this->XPos[2] - 2, this->YPos[Idx] - 2, 40 + 4, 30 + 4, UniversalEdit::UE->TData->ButtonSelected());
				}

				Gui::Draw_Rect(this->XPos[0], this->YPos[Idx], 40, 30, UniversalEdit::UE->TData->ButtonColor());
				Gui::Draw_Rect(this->XPos[1], this->YPos[Idx], 120, 30, UniversalEdit::UE->TData->ButtonColor());
				Gui::Draw_Rect(this->XPos[2], this->YPos[Idx], 40, 30, UniversalEdit::UE->TData->ButtonColor());

				/* Now draw Text. */
				Gui::DrawStringCentered(-105, this->YPos[Idx] + 7, 0.5f, UniversalEdit::UE->TData->TextColor(), "0x" + Common::ToHex<uint8_t>(this->Changes[this->SPos + Idx].File1), 40);
				Gui::DrawStringCentered(0, this->YPos[Idx] + 7, 0.5f, UniversalEdit::UE->TData->TextColor(), "0x" + Common::ToHex<uint32_t>(this->Changes[this->SPos + Idx].Offs), 120);
				Gui::DrawStringCentered(105, this->YPos[Idx] + 7, 0.5f, UniversalEdit::UE->TData->TextColor(), "0x" + Common::ToHex<uint8_t>(this->Changes[this->SPos + Idx].File2), 40);
			}

			C3D_FrameEnd(0);
			uint32_t Down = 0, Repeat = 0;
			touchPosition T;
		
			hidScanInput();
			Down = hidKeysDown();
			Repeat = hidKeysDownRepeat();
			hidTouchRead(&T);

			if (Down & KEY_TOUCH) {
				if (Common::Touching(T, this->Back)) this->Confirmed = true;
			}

			if (Down & KEY_B) this->Confirmed = true;

			if (this->Changes.size() > 0) {
				if (Repeat & KEY_DOWN) {
					if (this->Selection < this->Changes.size() - 1) this->Selection++;
				}

				if (Repeat & KEY_UP) {
					if (this->Selection > 0) this->Selection--;
				}

				if (Repeat & KEY_LEFT) {
					if (this->Selection > ENTRIES_PER_LIST) this->Selection -= ENTRIES_PER_LIST;
					else this->Selection = 0;
				}

				if (Repeat & KEY_RIGHT) {
					if (this->Selection + ENTRIES_PER_LIST < this->Changes.size() - 1) this->Selection += ENTRIES_PER_LIST;
					else this->Selection = this->Changes.size() - 1;
				}

				/* Scroll. */
				if (this->Selection < this->SPos) this->SPos = this->Selection;
				else if (this->Selection > this->SPos + ENTRIES_PER_LIST - 1) this->SPos = this->Selection - ENTRIES_PER_LIST + 1;
			}
		}
	}
};