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

#ifndef _UNIVERSAL_EDIT_ANALYZER_HPP
#define _UNIVERSAL_EDIT_ANALYZER_HPP

#include "structs.hpp"
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "Analyze.hpp"
#include "Changes.hpp"
#include "EditBytes.hpp"

class Analyzer {
public:
	enum class SubMode : uint8_t { Main = 0, Analyze = 1, Edit = 2, Changes = 3 };
	Analyzer() {
		this->_Analyze = std::make_unique<Analyze>();
		this->CH = std::make_unique<Changes>();
		this->EB = std::make_unique<EditBytes>();
	};
	void Draw();
	void Handler();

	static bool Endian; // Little Endian.
	static SubMode Mode;
private:
	void AccessAnalyze();
	void AccessEdit();
	void AccessChanges();

	const std::vector<Structs::ButtonPos> Menu = {
		{ 114, 40, 140, 30 }, // Analyze.
		{ 114, 90, 140, 30 }, // Edit Bytes.
		{ 114, 140, 140, 30 } // Changes.
	};

	const std::vector<std::string> MenuOptions = { "ANALYZE", "EDIT_BYTES", "CHANGE_LIST" };
	const std::vector<std::function<void()>> Funcs = {
		{ [this]() { this->AccessAnalyze(); } },
		{ [this]() { this->AccessEdit(); } },
		{ [this]() { this->AccessChanges(); } }
	};

	std::unique_ptr<Analyze> _Analyze = nullptr;
	std::unique_ptr<Changes> CH = nullptr;
	std::unique_ptr<EditBytes> EB = nullptr;
};

#endif