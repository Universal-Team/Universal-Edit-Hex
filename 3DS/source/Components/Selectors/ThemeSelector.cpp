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
#include "ThemeSelector.hpp"
#include <unistd.h>

#define THEMES_ON_LIST 4

void ThemeSelector::Draw() {
	C2D_TargetClear(Top, C2D_Color32(0, 0, 0, 0));
	C2D_TargetClear(Bottom, C2D_Color32(0, 0, 0, 0));
	Gui::clearTextBufs();
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

	UniversalEdit::UE->DrawTop(); // Keep the top screen for sure.
	UniversalEdit::UE->GData->DrawBottom();
	Gui::Draw_Rect(0, 0, 320, 20, UniversalEdit::UE->TData->BarColor());
	Gui::Draw_Rect(0, 20, 320, 1, UniversalEdit::UE->TData->BarOutline());
	UniversalEdit::UE->GData->SpriteBlend(sprites_arrow_idx, 0, 0, UniversalEdit::UE->TData->BackArrowColor(), 1.0f);
	Gui::DrawStringCentered(0, 2, 0.5f, UniversalEdit::UE->TData->TextColor(), Common::GetStr("SELECT_THEME"), 310);

	/* Now begin to draw the Theme List. */
	for (uint8_t Idx = 0; Idx < THEMES_ON_LIST && Idx < UniversalEdit::UE->ThemeNames.size(); Idx++) {
		if (this->SPos + Idx == this->Selection) Gui::Draw_Rect(this->TPos[Idx + 1].x - 2, this->TPos[Idx + 1].y - 2, this->TPos[Idx + 1].w + 4, this->TPos[Idx + 1].h + 4, UniversalEdit::UE->TData->ButtonSelected());
		Gui::Draw_Rect(this->TPos[Idx + 1].x, this->TPos[Idx + 1].y, this->TPos[Idx + 1].w, this->TPos[Idx + 1].h, UniversalEdit::UE->TData->ButtonColor());

		Gui::DrawStringCentered(0, this->TPos[Idx + 1].y + 4, 0.5f, UniversalEdit::UE->TData->TextColor(), UniversalEdit::UE->ThemeNames[this->SPos + Idx].first, 240);
		Gui::DrawStringCentered(0, this->TPos[Idx + 1].y + 20, 0.4f, UniversalEdit::UE->TData->TextColor(), UniversalEdit::UE->ThemeNames[this->SPos + Idx].second, 240, 25, nullptr, C2D_WordWrap);
	};

	C3D_FrameEnd(0);
};


void ThemeSelector::Handler() {
	this->SelectMode = true;

	if (!UniversalEdit::UE->ThemeNames.empty()) { // Only do action if not empty.
		while(aptMainLoop() && this->SelectMode) {
			this->Draw();

			touchPosition T;
			hidScanInput();
			const uint32_t Down = hidKeysDown();
			const uint32_t Repeat = hidKeysDownRepeat();
			hidTouchRead(&T);

			if (Repeat & KEY_DOWN) {
				if (this->Selection < (int)UniversalEdit::UE->ThemeNames.size() - 1) this->Selection++;
				else this->Selection = 0;
			};

			if (Repeat & KEY_UP) {
				if (this->Selection > 0) this->Selection--;
				else this->Selection = UniversalEdit::UE->ThemeNames.size() - 1;
			};

			if (Repeat & KEY_LEFT) {
				if (this->Selection - THEMES_ON_LIST >= 0) this->Selection -= THEMES_ON_LIST;
				else this->Selection = 0;
			};

			if (Repeat & KEY_RIGHT) {
				if (this->Selection + THEMES_ON_LIST < (int)UniversalEdit::UE->ThemeNames.size()) this->Selection += THEMES_ON_LIST;
				else this->Selection = UniversalEdit::UE->ThemeNames.size() - 1;
			};

			if (Down & KEY_A) {
				UniversalEdit::UE->CData->Theme(UniversalEdit::UE->ThemeNames[this->Selection].first);
				UniversalEdit::UE->TData->LoadTheme(UniversalEdit::UE->CData->Theme());
				this->SelectMode = false;
			};

			if (Down & KEY_TOUCH) {
				if (Common::Touching(T, this->TPos[0])) {
					this->SelectMode = false;
					return;
				};

				for (uint8_t Idx = 0; Idx < THEMES_ON_LIST; Idx++) {
					if (this->SPos + Idx < (int)UniversalEdit::UE->ThemeNames.size()) {
						if (Common::Touching(T, this->TPos[Idx + 1])) {
							UniversalEdit::UE->CData->Theme(UniversalEdit::UE->ThemeNames[this->SPos + Idx].first);
							UniversalEdit::UE->TData->LoadTheme(UniversalEdit::UE->CData->Theme());
							this->SelectMode = false;
							break;
						};
					};
				};
			};

			if (Down & KEY_B) this->SelectMode = false;

			if (this->Selection < this->SPos) this->SPos = this->Selection;
			else if (this->Selection > this->SPos + THEMES_ON_LIST - 1) this->SPos = this->Selection - THEMES_ON_LIST + 1;
		};
	};
};