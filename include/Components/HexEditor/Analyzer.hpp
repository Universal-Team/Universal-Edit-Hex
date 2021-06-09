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

#ifndef _UNIVERSAL_EDIT_HEX_EDITOR_ANALYZER_HPP
#define _UNIVERSAL_EDIT_HEX_EDITOR_ANALYZER_HPP

#include "structs.hpp"
#include <functional>
#include <string>
#include <vector>

class Analyzer {
public:
	Analyzer() { };
	void Draw();
	void Handler();
private:
	bool IsOnAnalyzer = true; // True: Analyze main sub mode, False: Second page.

	void Back();
	void Fetch();

	void SwitchByteSize(const uint8_t Size);
	void ToggleEndian();
	void ToggleHex();

	void SetU8();
	void SetU16();
	void SetU32();
	void ToggleBit(const uint8_t Idx);

	bool Endian = false; // Little Endian.
	bool Hex = true; // Hexadecimal.

	const std::vector<Structs::ButtonPos> EditMenu = {
		{ 50, 0, 20, 20 }, // Back.

		/* uint8_t. */
		{ 80, 40, 100, 20 },

		/* uint16_t. */
		{ 80, 80, 100, 20 },

		/* uint32_t. */
		{ 80, 120, 100, 20 },

		/* Bits. */
		{ 80, 160, 20, 20 },
		{ 110, 160, 20, 20 },
		{ 140, 160, 20, 20 },
		{ 170, 160, 20, 20 },
		{ 200, 160, 20, 20 },
		{ 230, 160, 20, 20 },
		{ 260, 160, 20, 20 },
		{ 290, 160, 20, 20 }
	};

	const std::vector<Structs::ButtonPos> Menu = {
		{ 50, 0, 20, 20 }, // Back.

		/* Byte size selection. */
		{ 150, 40, 20, 20 },
		{ 180, 40, 20, 20 },
		{ 210, 40, 20, 20 },

		/* Endian/Hex buttons */
		{ 280, 40, 30, 20 },
		{ 280, 80, 30, 20 },

		/* Unsigned Byte. */
		{ 210, 80, 30, 20 },

		/* Signed Byte. */
		{ 210, 110, 30, 20 },

		/* Float. */
		{ 260, 140, 30, 20 },

		/* Binary. */
		{ 260, 170, 30, 20 },

		/* UTF-8. */
		{ 260, 200, 30, 20 }
	};

	const std::vector<std::function<void()>> MainFuncs = {
		{ [this]() { this->Back(); } },

		{ [this]() { this->SwitchByteSize(1); } },
		{ [this]() { this->SwitchByteSize(2); } },
		{ [this]() { this->SwitchByteSize(4); } },

		{ [this]() { this->ToggleEndian(); } },

		{ [this]() { this->ToggleHex(); } }
	};

	const std::vector<std::function<void()>> EditFuncs = {
		{ [this]() { this->Back(); } },

		{ [this]() { this->SetU8(); } },
		{ [this]() { this->SetU16(); } },
		{ [this]() { this->SetU32(); } },

		{ [this]() { this->ToggleBit(0); } },
		{ [this]() { this->ToggleBit(1); } },
		{ [this]() { this->ToggleBit(2); } },
		{ [this]() { this->ToggleBit(3); } },
		{ [this]() { this->ToggleBit(4); } },
		{ [this]() { this->ToggleBit(5); } },
		{ [this]() { this->ToggleBit(6); } },
		{ [this]() { this->ToggleBit(7); } }
	};
};

#endif