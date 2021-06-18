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
#include "EncodingCharSelector.hpp"

uint8_t EncodingCharSelector::Handler(const uint8_t Old) {
	this->Selection = Old;

	while(aptMainLoop()) {
		C2D_TargetClear(Top, C2D_Color32(0, 0, 0, 0));
		C2D_TargetClear(Bottom, C2D_Color32(0, 0, 0, 0));
		Gui::clearTextBufs();
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

		UniversalEdit::UE->DrawTop(); // Keep the top screen for sure.
		UniversalEdit::UE->GData->DrawBottom();
		Gui::Draw_Rect(0, 0, 320, 20, UniversalEdit::UE->TData->BarColor());
		Gui::Draw_Rect(0, 20, 320, 1, UniversalEdit::UE->TData->BarOutline());
		UniversalEdit::UE->GData->SpriteBlend(sprites_arrow_idx, 0, 0, UniversalEdit::UE->TData->BackArrowColor(), 1.0f);
		Gui::DrawStringCentered(0, 2, 0.5f, UniversalEdit::UE->TData->TextColor(), Common::GetStr("SELECT_ENCODING_CHAR"), 310);


		for (size_t Idx = 0; Idx < 256; Idx++) { // Encoding characters.
			if (Idx == this->Selection) Gui::Draw_Rect(this->XPos[Idx % 0x10], this->YPos[Idx / 0x10], 18, 12, UniversalEdit::UE->TData->SelectedByte());
			else Gui::Draw_Rect(this->XPos[Idx % 0x10], this->YPos[Idx / 0x10], 18, 12, UniversalEdit::UE->TData->BarColor());

			Gui::DrawString(this->XPos[Idx % 0x10] + 4, this->YPos[Idx / 0x10], 0.4f, UniversalEdit::UE->TData->TextColor(), UniversalEdit::UE->CurrentFile->GetEncodingChar(Idx));
		};

		C3D_FrameEnd(0);

		uint32_t Down = 0, Repeat = 0;
		touchPosition T;
		hidScanInput();
		Down = hidKeysDown();
		Repeat = hidKeysDownRepeat();
		hidTouchRead(&T);

		if (Repeat & KEY_UP) {
			if (this->Selection > 0xF) this->Selection -= 0x10;
		};

		if (Repeat & KEY_DOWN) {
			if (this->Selection < 0xF0) this->Selection += 0x10;
		};

		if (Repeat & KEY_LEFT) {
			if (this->Selection > 0) this->Selection--;
		};

		if (Repeat & KEY_RIGHT) {
			if (this->Selection < 255) this->Selection++;
		};

		if (Down & KEY_TOUCH) {
			if (Common::Touching(T, this->Back)) return Old;
		};

		if (Down & KEY_A) return this->Selection;
		if (Down & KEY_B) return Old;
	};

	return Old;
};