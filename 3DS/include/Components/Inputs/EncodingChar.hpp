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

#ifndef _UNIVERSAL_EDIT_ENCODING_CHAR_HPP
#define _UNIVERSAL_EDIT_ENCODING_CHAR_HPP

#include <string>

class EncodingChar {
public:
	uint8_t Handler(const uint8_t Old = 0);
private:
	uint8_t Selection = 0;

	/* X: 25 start += 18. Y: 25 start += 13. */
	static constexpr int XPos[0x10] = { 25, 43, 61, 79, 97, 115, 133, 151, 169, 187, 205, 223, 241, 259, 277, 295 };
	static constexpr int YPos[0x10] = { 25, 38, 51, 64, 77, 90, 103, 116, 129, 142, 155, 168, 181, 194, 207, 220 };
};

#endif