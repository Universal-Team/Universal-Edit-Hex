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

#ifndef _UNIVERSAL_EDIT_ANALYZER_EDIT_BYTES_HPP
#define _UNIVERSAL_EDIT_ANALYZER_EDIT_BYTES_HPP

#include "structs.hpp"
#include <functional>
#include <string>
#include <vector>

class EditBytes {
public:
	void Draw();
	void Handler();
private:
	void Back();

	void SetU8();
	void SetU16();
	void SetU32();
	void ToggleBit(const uint8_t Idx);

	const std::vector<Structs::ButtonPos> Menu = {
		{ 50, 0, 20, 20 }, // Back.

		/* uint8_t. */
		{ 134, 40, 100, 20 },

		/* uint16_t. */
		{ 134, 80, 100, 20 },

		/* uint32_t. */
		{ 134, 120, 100, 20 },

		/* Bits. */
		{ 69, 190, 20, 20 },
		{ 99, 190, 20, 20 },
		{ 129, 190, 20, 20 },
		{ 159, 190, 20, 20 },
		{ 189, 190, 20, 20 },
		{ 219, 190, 20, 20 },
		{ 249, 190, 20, 20 },
		{ 279, 190, 20, 20 }
	};

	const std::vector<std::function<void()>> Funcs = {
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