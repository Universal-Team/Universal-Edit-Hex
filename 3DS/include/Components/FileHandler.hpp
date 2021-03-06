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

#ifndef _UNIVERSAL_EDIT_FILE_HANDLER_HPP
#define _UNIVERSAL_EDIT_FILE_HANDLER_HPP

#include "structs.hpp"
#include <functional>
#include <string>
#include <vector>

class FileHandler {
public:
	void Draw();
	void Handler();

	static bool Loaded;
private:
	/* Actions. */
	void LoadFile();
	void NewFile();
	void SaveFile();
	void SaveFileAs();

	const std::vector<Structs::ButtonPos> Menu = {
		{ 114, 40, 140, 30 }, // Load File.
		{ 114, 90, 140, 30 }, // New File.
		{ 114, 140, 140, 30 }, // Save File.
		{ 114, 190, 140, 30 } // Save as....
	};

	const std::vector<std::string> MenuOptions = { "LOAD_FILE", "NEW_FILE", "SAVE_FILE", "SAVE_FILE_AS" };
	const std::vector<std::function<void()>> Funcs = {
		{ [this]() { this->LoadFile(); } },
		{ [this]() { this->NewFile(); } },
		{ [this]() { this->SaveFile(); } },
		{ [this]() { this->SaveFileAs(); } }
	};
};

#endif