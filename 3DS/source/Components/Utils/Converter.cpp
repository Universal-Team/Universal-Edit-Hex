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
#include "Converter.hpp"

void Converter::Draw() {
	Gui::Draw_Rect(49, 0, 271, 20, UniversalEdit::UE->TData->BarColor());
	Gui::Draw_Rect(49, 20, 271, 1, UniversalEdit::UE->TData->BarOutline());
	UniversalEdit::UE->GData->SpriteBlend(sprites_arrow_idx, 50, 0, UniversalEdit::UE->TData->BackArrowColor(), 1.0f);
	Gui::DrawStringCentered(24, 2, 0.5f, UniversalEdit::UE->TData->TextColor(), Common::GetStr("CONVERTER"), 310);

	/* Hex. */
	Gui::Draw_Rect(this->Menu[1].x, this->Menu[1].y, this->Menu[1].w, this->Menu[1].h, UniversalEdit::UE->TData->ButtonColor());
	Gui::DrawString(this->Menu[1].x + 5, this->Menu[1].y - 20, 0.5f, UniversalEdit::UE->TData->TextColor(), Common::GetStr("HEX"));

	if (this->Val < 0x100) { // uint8_t.
		Gui::DrawString(this->Menu[1].x + 5, this->Menu[1].y + 4, 0.4f, UniversalEdit::UE->TData->TextColor(), "0x" + Common::ToHex<uint8_t>(this->Val));

	} else if (this->Val > 0xFF && this->Val < 0x10000) { // uint16_t.
		Gui::DrawString(this->Menu[1].x + 5, this->Menu[1].y + 4, 0.4f, UniversalEdit::UE->TData->TextColor(), "0x" + Common::ToHex<uint16_t>(this->Val));

	} else { // all over uint16_t.
		Gui::DrawString(this->Menu[1].x + 5, this->Menu[1].y + 4, 0.4f, UniversalEdit::UE->TData->TextColor(), "0x" + Common::ToHex<uint32_t>(this->Val));
	};

	/* Decimal. */
	Gui::Draw_Rect(this->Menu[2].x, this->Menu[2].y, this->Menu[2].w, this->Menu[2].h, UniversalEdit::UE->TData->ButtonColor());
	Gui::DrawString(this->Menu[2].x + 5, this->Menu[2].y - 20, 0.5f, UniversalEdit::UE->TData->TextColor(), Common::GetStr("DECIMAL"));
	Gui::DrawString(this->Menu[2].x + 5, this->Menu[2].y + 4, 0.4f, UniversalEdit::UE->TData->TextColor(), std::to_string(this->Val));
};

void Converter::Handler() {
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
};

void Converter::Back() { Utils::Mode = Utils::SubMode::Main; };

void Converter::EnterHex() {
	if (FileHandler::Loaded) {
		this->Val = Common::HexPad(Common::GetStr("ENTER_VALUE_IN_HEX"), this->Val, 0, 0xFFFFFFFF, 10);
	};
};

void Converter::EnterDecimal() {
	if (FileHandler::Loaded) {
		this->Val = Common::Numpad(Common::GetStr("ENTER_VALUE_IN_DEC"), this->Val, 0, 0xFFFFFFFF, 10);
	};
};