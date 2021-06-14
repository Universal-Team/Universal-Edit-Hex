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

#ifndef _UNIVERSAL_EDIT_HEX_EDITOR_HPP
#define _UNIVERSAL_EDIT_HEX_EDITOR_HPP

#include "structs.hpp"
#include <string>
#include <vector>

class HexEditor {
public:
	void DrawTop();
	void Handler();
private:
	std::string OffsToStr(const int Offs);
	void DrawHexOnly();
	void DrawTextOnly();
	void DrawTextAndHex();

	uint8_t GetNums(const uint8_t Group) {
		switch(Group) {
			default:
			case 0: return 16;
			case 1: return 8;
			case 2: return 4;
			case 3: return 2;
			case 4: return 1;
		};
	};

	uint8_t BytesPerGroup(const uint8_t Group) {
		switch(Group) {
			default:
			case 0: return 1;
			case 1: return 2;
			case 2: return 4;
			case 3: return 8;
			case 4: return 16;
		};
	};

	uint8_t GetTopRow(const uint8_t Group, const uint8_t Idx) {
		switch(Group) {
			default:
			case 0: return Idx; // It's as simple as that lol.
			case 1: return Idx * 2;
			case 2: return Idx * 4;
			case 3: return Idx * 8;
			case 4: return Idx * 16;
		};
	};

	static constexpr int XPositions[0x5][0x10] = {
		{ // 1 byte per group.
			70,  90,  110,  130,  150,  170,  190,  210,  230,  250,  270,  290,  310,  330,  350,  370
		},
		{ // 2 byte per group.
			70, 85,  105, 120,  140, 155,  175, 190,  210, 225,  245, 260,  280, 295,  315, 330
		},
		{ // 4 byte per group.
			70, 85, 100, 115,  135, 150, 165, 180,  200, 215, 230, 245,  265, 280, 295, 310
		},
		{ // 8 byte per group.
			70, 85, 100, 115, 130, 145, 160, 175,  195, 210, 225, 240, 255, 270, 285, 300
		},
		{ // 16 byte per group.
			70, 85, 100, 115, 130, 145, 160, 175, 190, 205, 220, 235, 250, 265, 280, 295
		}
	};

	/* Used for Hex / Text only Mode. */
	static constexpr int YPositions[0xD] = { 42, 57, 72, 87, 102, 117, 132, 147, 162, 177, 192, 207, 222 };

	/* Used for Text + Hex Mode. */
	static constexpr int XPositionsAlt[0x10] = {
		65, 77, 89, 101,
		119, 131, 143, 155,
		174, 186, 198, 210,
		229, 241, 253, 265
	};

	/* The decoded text position. */
	static constexpr int DecodedPos[0x10] = {
		290, 297, 304, 311,
		318, 325, 332, 339,
		346, 352, 359, 366,
		373, 379, 386, 393
	};
};

#endif