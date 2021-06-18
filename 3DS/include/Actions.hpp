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

#ifndef _UNIVERSAL_EDIT_ACTIONS_HPP
#define _UNIVERSAL_EDIT_ACTIONS_HPP

#include <string>
#include <vector>

namespace Actions {
	struct CompareStruct {
		uint32_t Offs = 0x0;
		uint8_t File1 = 0x0;
		uint8_t File2 = 0x0;
	};

	void Insert(const uint32_t Offs, const uint32_t Size, const uint8_t ToInsert);
	void Remove(const uint32_t Offs, const uint32_t Size);

	bool Backup();
	bool SaveFileAs();
	bool NewFile();

	std::vector<uint32_t> Search(const uint32_t StartOffs, const uint32_t EndOffs, const std::vector<uint8_t> &Seq);
	std::vector<CompareStruct> CompareFiles(const std::string &File1, const std::string &File2);
};

#endif