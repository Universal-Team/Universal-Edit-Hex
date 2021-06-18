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

#ifndef _UNIVERSAL_EDIT_ENCODING_CHAR_SELECTOR_HPP
#define _UNIVERSAL_EDIT_ENCODING_CHAR_SELECTOR_HPP

#include "structs.hpp"
#include <string>

class EncodingCharSelector {
public:
	uint8_t Handler(const uint8_t Old = 0);
private:
	uint8_t Selection = 0;

	/* X: 16 start += 18. Y: 35 start += 12. */
	static constexpr int XPos[0x10] = { 16, 34, 52, 70, 88, 106, 124, 142, 160, 178, 196, 214, 232, 250, 268, 286 };
	static constexpr int YPos[0x10] = { 35, 47, 59, 71, 83, 95, 107, 119, 131, 143, 155, 167, 179, 191, 203, 215 };

	const Structs::ButtonPos Back = { 0, 0, 20, 20 };
};

#endif