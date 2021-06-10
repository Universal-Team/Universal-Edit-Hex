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

#ifndef _UNIVERSAL_EDIT_NAVIGATOR_NAVIGATION_HPP
#define _UNIVERSAL_EDIT_NAVIGATOR_NAVIGATION_HPP

#include "structs.hpp"
#include <memory>
#include <string>
#include <vector>

#include "Reminsert.hpp"
#include "Search.hpp"

class Navigation {
public:
	enum class SubMode : uint8_t { Main = 0, Reminsert = 1, Search = 2 };
	Navigation() {
		this->RemInsert = std::make_unique<Reminsert>();
		this->_Search = std::make_unique<Search>();
	};
	void Draw();
	void Handler();

	static SubMode Mode;
private:
	void AccessSearch();
	void JumpTo();
	void AccessRemInsert();

	const std::vector<Structs::ButtonPos> Menu = {
		{ 70, 40, 100, 30 }, // Search.
		{ 200, 40, 100, 30 }, // Jump to.
		{ 70, 80, 100, 30 }, // Remove / Insert.
	};

	const std::vector<std::string> MenuOptions = { "SEARCH", "JUMP_TO", "REMINSERT" };
	const std::vector<std::function<void()>> Funcs = {
		{ [this]() { this->AccessSearch(); } },
		{ [this]() { this->JumpTo(); } },
		{ [this]() { this->AccessRemInsert(); } }
	};

	std::unique_ptr<Reminsert> RemInsert = nullptr;
	std::unique_ptr<Search> _Search = nullptr;
};

#endif