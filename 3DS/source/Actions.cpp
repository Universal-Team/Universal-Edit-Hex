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

#include "Actions.hpp"
#include "Common.hpp"
#include "DirSelector.hpp"
#include "PromptMessage.hpp"
#include "StatusMessage.hpp"

#define BUFFER_SIZE 0x80000 // TODO: Find the best size or so..


/*
	Insert bytes to the current file. SD Card and size affect the speed too.

	const uint32_t Offs: The offset from where to start the insert.
	const uint32_t Size: The size to insert.
	const uint8_t ToInsert: The uint8_t value to insert. (0x0 - 0xFF)

	This basically creates a temp file at sdmc:/3ds/Universal-Edit/Hex-Editor/Temp.bin;
	Action 1: Write the data until the start offset to the file.
	Action 2: Append the ToInsert value with the given amount of size to the TEMP file.
	Action 3: Copy the rest of the file to the TEMP file.
	Action 4: Remove current file, move temp file to current file and reload.
*/
void Actions::Insert(const uint32_t Offs, const uint32_t Size, const uint8_t ToInsert) {
	Actions::Backup();

	if (Common::GetFreeSpace() >= (UniversalEdit::UE->CurrentFile->GetSize() * 2) + Size) {
		std::unique_ptr<PromptMessage> PM = std::make_unique<PromptMessage>();
		const bool ShouldDo = PM->Handler(Common::GetStr("INSERT_WARNING"));

		if (ShouldDo) {
			FILE *TEMPFILE = fopen("sdmc:/3ds/Universal-Edit/Hex-Editor/Temp.bin", "wb");
			uint32_t SizeToInsert = Offs; // Up until the offset.
			std::vector<uint8_t> DataToCopy;

			Common::ProgressMessage(Common::GetStr("INSERT_STEP_1") + "\n" + Common::ToHex<uint32_t>(SizeToInsert));
			aptSetSleepAllowed(false); // DON'T WHILE WRITTING!
			aptSetHomeAllowed(false);

			/* Action 1: COPY until offset to TEMPFILE. */
			while(SizeToInsert > 0 && aptMainLoop()) {
				if (SizeToInsert > BUFFER_SIZE) DataToCopy.resize(BUFFER_SIZE);
				else DataToCopy.resize(SizeToInsert);

				fseek(UniversalEdit::UE->CurrentFile->GetFileHandler(), Offs - SizeToInsert, SEEK_SET);
				fread(DataToCopy.data(), DataToCopy.size(), 1, UniversalEdit::UE->CurrentFile->GetFileHandler()); // Read.
				fwrite(DataToCopy.data(), DataToCopy.size(), 1, TEMPFILE); // Write.

				SizeToInsert -= DataToCopy.size();
				DataToCopy.clear();

				Common::ProgressMessage(Common::GetStr("INSERT_STEP_1") + "\n" + Common::ToHex<uint32_t>(SizeToInsert));
			};

			fclose(TEMPFILE);

			/* Action 2: Append the new data AND then the rest of the FILE. */
			TEMPFILE = fopen("sdmc:/3ds/Universal-Edit/Hex-Editor/Temp.bin", "a"); // APPEND.
			SizeToInsert = Size;
			Common::ProgressMessage(Common::GetStr("INSERT_STEP_2") + "\n" + Common::ToHex<uint32_t>(SizeToInsert));

			while(SizeToInsert > 0 && aptMainLoop()) {
				if (SizeToInsert > BUFFER_SIZE) DataToCopy.resize(BUFFER_SIZE);
				else DataToCopy.resize(SizeToInsert);

				std::fill_n(DataToCopy.begin(), DataToCopy.size(), ToInsert); // Fill vector with the value to insert.
				fwrite(DataToCopy.data(), DataToCopy.size(), 1, TEMPFILE);
				SizeToInsert -= DataToCopy.size();
				DataToCopy.clear();

				Common::ProgressMessage(Common::GetStr("INSERT_STEP_2") + "\n" + Common::ToHex<uint32_t>(SizeToInsert));
			};

			SizeToInsert = UniversalEdit::UE->CurrentFile->GetSize() - Offs; // Is that right?
			Common::ProgressMessage(Common::GetStr("INSERT_STEP_3") + "\n" + Common::ToHex<uint32_t>(SizeToInsert));
			fseek(UniversalEdit::UE->CurrentFile->GetFileHandler(), UniversalEdit::UE->CurrentFile->GetSize() - SizeToInsert , SEEK_SET);

			while(SizeToInsert > 0 && aptMainLoop()) {
				if (SizeToInsert > BUFFER_SIZE) DataToCopy.resize(BUFFER_SIZE);
				else DataToCopy.resize(SizeToInsert);

				fread(DataToCopy.data(), DataToCopy.size(), 1, UniversalEdit::UE->CurrentFile->GetFileHandler()); // Read.
				fwrite(DataToCopy.data(), DataToCopy.size(), 1, TEMPFILE); // Write.

				SizeToInsert -= DataToCopy.size();
				DataToCopy.clear();

				Common::ProgressMessage(Common::GetStr("INSERT_STEP_3") + "\n" + Common::ToHex<uint32_t>(SizeToInsert));
			};

			fclose(TEMPFILE);
			aptSetSleepAllowed(true); // Re-Allow Sleep and HOME, since we are done.
			aptSetHomeAllowed(true);

			/* Action 3: Set to CURRENT File. */
			UniversalEdit::UE->CurrentFile->End(); // Close.
			std::remove(UniversalEdit::UE->CurrentFile->EditFile().c_str());
			std::rename("sdmc:/3ds/Universal-Edit/Hex-Editor/Temp.bin", UniversalEdit::UE->CurrentFile->EditFile().c_str()); // Move.
			UniversalEdit::UE->CurrentFile->Load(UniversalEdit::UE->CurrentFile->EditFile().c_str(), 0xD, 0x20000); // RELOAD.
		};

	} else {
		std::unique_ptr<StatusMessage> Msg = std::make_unique<StatusMessage>();
		Msg->Handler(Common::GetStr("NOT_ENOUGH_SPACE_INSERT"), -1);
	};
};


/*
	Remove bytes from the current file. SD Card and size affect the speed too.

	const uint32_t Offs: The offset from where to start the remove.
	const uint32_t Size: The size to remove.

	This basically creates a temp file at sdmc:/3ds/Universal-Edit/Hex-Editor/Temp.bin;
	Action 1: Write the data until the start offset to the file.
	Action 2: Skip the Offset + Size and append the data after it to the TEMP file.
	Action 3: Remove current file, move temp file to current file and reload.
*/
void Actions::Remove(const uint32_t Offs, const uint32_t Size) {
	Actions::Backup();
	
	if (Common::GetFreeSpace() >= (UniversalEdit::UE->CurrentFile->GetSize() * 2) - Size) {
		std::unique_ptr<PromptMessage> PM = std::make_unique<PromptMessage>();
		const bool ShouldDo = PM->Handler(Common::GetStr("REMOVE_WARNING"));

		if (ShouldDo) {
			FILE *TEMPFILE = fopen("sdmc:/3ds/Universal-Edit/Hex-Editor/Temp.bin", "wb");

			uint32_t SizeToInsert = Offs; // Up until the offset.
			std::vector<uint8_t> DataToCopy;
			Common::ProgressMessage(Common::GetStr("REMOVE_STEP_1") + "\n" + Common::ToHex<uint32_t>(SizeToInsert));
			aptSetSleepAllowed(false); // DON'T WHILE WRITTING!
			aptSetHomeAllowed(false);

			/* Action 1: COPY until offset to TEMPFILE. */
			while(SizeToInsert > 0 && aptMainLoop()) {
				if (SizeToInsert > BUFFER_SIZE) DataToCopy.resize(BUFFER_SIZE);
				else DataToCopy.resize(SizeToInsert);

				fseek(UniversalEdit::UE->CurrentFile->GetFileHandler(), Offs - SizeToInsert, SEEK_SET);
				fread(DataToCopy.data(), DataToCopy.size(), 1, UniversalEdit::UE->CurrentFile->GetFileHandler()); // Read.
				fwrite(DataToCopy.data(), DataToCopy.size(), 1, TEMPFILE); // Write.

				SizeToInsert -= DataToCopy.size();
				DataToCopy.clear();

				Common::ProgressMessage(Common::GetStr("REMOVE_STEP_1") + "\n" + Common::ToHex<uint32_t>(SizeToInsert));
			};

			fclose(TEMPFILE);

			/* Action 2: SKIP the offset + size data and copy the rest. */
			TEMPFILE = fopen("sdmc:/3ds/Universal-Edit/Hex-Editor/Temp.bin", "a"); // APPEND.
			SizeToInsert = UniversalEdit::UE->CurrentFile->GetSize() - (Offs + Size);

			Common::ProgressMessage(Common::GetStr("REMOVE_STEP_2") + "\n" + Common::ToHex<uint32_t>(SizeToInsert));
			fseek(UniversalEdit::UE->CurrentFile->GetFileHandler(), UniversalEdit::UE->CurrentFile->GetSize() - SizeToInsert , SEEK_SET);

			while(SizeToInsert > 0 && aptMainLoop()) {
				if (SizeToInsert > BUFFER_SIZE) DataToCopy.resize(BUFFER_SIZE);
				else DataToCopy.resize(SizeToInsert);

				fread(DataToCopy.data(), DataToCopy.size(), 1, UniversalEdit::UE->CurrentFile->GetFileHandler()); // Read.
				fwrite(DataToCopy.data(), DataToCopy.size(), 1, TEMPFILE); // Write.

				SizeToInsert -= DataToCopy.size();
				DataToCopy.clear();

				/* Update progress display. */
				Common::ProgressMessage(Common::GetStr("REMOVE_STEP_2") + "\n" + Common::ToHex<uint32_t>(SizeToInsert));
			};

			fclose(TEMPFILE);
			aptSetSleepAllowed(true); // Re-Allow Sleep and HOME, since we are done.
			aptSetHomeAllowed(true);

			/* Action 3: Set to CURRENT File. */
			UniversalEdit::UE->CurrentFile->End(); // Close.
			std::remove(UniversalEdit::UE->CurrentFile->EditFile().c_str());
			std::rename("sdmc:/3ds/Universal-Edit/Hex-Editor/Temp.bin", UniversalEdit::UE->CurrentFile->EditFile().c_str()); // Move.
			UniversalEdit::UE->CurrentFile->Load(UniversalEdit::UE->CurrentFile->EditFile().c_str(), 0xD, 0x20000); // RELOAD.
		};

	} else {
		std::unique_ptr<StatusMessage> Msg = std::make_unique<StatusMessage>();
		Msg->Handler(Common::GetStr("NOT_ENOUGH_SPACE_REMOVE"), -1);
	};
};


/*
	Create a backup of the current file.

	Returns false if size of space is not enough and cancelled action on second prompt, else true.
*/
bool Actions::Backup() {
	if (!UniversalEdit::UE->CurrentFile->FirstWrite()) {
		bool Cancelled = false;

		switch(UniversalEdit::UE->CData->Backup()) {
			case 0: // Directly do backups.
				break;

			case 1: // Don't even bother making one.
				Cancelled = true;
				break;

			default:
			case 2: { // Ask first.
				std::unique_ptr<PromptMessage> PM = std::make_unique<PromptMessage>();
				Cancelled = !PM->Handler(Common::GetStr("CREATE_BACKUP"));
			};
			break;
		};

		if (Cancelled) {
			UniversalEdit::UE->CurrentFile->SetWrite(true); // First write done.
			return true;
		};

		if (!UniversalEdit::UE->CurrentFile->FirstWrite()) {
			if (Common::GetFreeSpace() >= (UniversalEdit::UE->CurrentFile->GetSize() * 2)) {
				std::string Path = UniversalEdit::UE->CData->BackupPath();

				if (UniversalEdit::UE->CData->BackupPath()[UniversalEdit::UE->CData->BackupPath().size() - 1] != '/') {
					Path += "/"; // Add the trailing slash there.
				};

				const std::string FName = Common::Keyboard(Common::GetStr("ENTER_BACKUP_FILENAME"), "", 100);

				if (FName != "") {
					Path += FName; // Add filename to PATH.

					uint32_t ToWrite = UniversalEdit::UE->CurrentFile->GetSize();
					std::vector<uint8_t> DataToCopy;

					FILE *Temp = fopen(Path.c_str(), "wb");
					fseek(UniversalEdit::UE->CurrentFile->GetFileHandler(), 0, SEEK_SET); // Seek to start.

					aptSetSleepAllowed(false); // DON'T WHILE WRITTING!
					aptSetHomeAllowed(false);

					Common::ProgressMessage(Common::GetStr("BACKING_UP_FILE") + "\n" + Common::ToHex<uint32_t>(ToWrite));
					while(ToWrite > 0 && aptMainLoop()) {
						if (ToWrite > BUFFER_SIZE) DataToCopy.resize(BUFFER_SIZE);
						else DataToCopy.resize(ToWrite);

						fread(DataToCopy.data(), 0x1, DataToCopy.size(), UniversalEdit::UE->CurrentFile->GetFileHandler());
						fwrite(DataToCopy.data(), 1, DataToCopy.size(), Temp);
						ToWrite -= DataToCopy.size();

						/* Update. */
						Common::ProgressMessage(Common::GetStr("BACKING_UP_FILE") + "\n" + Common::ToHex<uint32_t>(ToWrite));
					};

					fclose(Temp);
					aptSetSleepAllowed(true); // Re-Allow Sleep and HOME, since we are done.
					aptSetHomeAllowed(true);
				};

			} else {
				UniversalEdit::UE->CurrentFile->SetWrite(true); // First write done.

				std::unique_ptr<PromptMessage> Msg = std::make_unique<PromptMessage>();
				if (!Msg->Handler(Common::GetStr("NOT_ENOUGH_SPACE_BACKUP"))) return false;
			};
		};

		UniversalEdit::UE->CurrentFile->SetWrite(true); // First write done.
	};

	return true;
};


/*
	Save the current file as a different file.

	It will have the new file as the active file and returns true, if succeeded.
*/
bool Actions::SaveFileAs() {
	if (FileHandler::Loaded) {
		std::unique_ptr<DirSelector> DS = std::make_unique<DirSelector>();
		const std::string Dest = DS->Handler("sdmc:/", Common::GetStr("SELECT_DEST"));

		if (Dest != "") { // Ensure a directory got selected.
			const std::string FName = Common::Keyboard(Common::GetStr("ENTER_FILE_NAME"), "", 100);

			if (FName != "") { // Ensure a filename got entered.
				Common::ProgressMessage(Common::GetStr("SAVING_TO_FILE"));

				if (Common::GetFreeSpace() >= (UniversalEdit::UE->CurrentFile->GetSize() * 2)) {
					std::string Path = Dest + FName;

					uint32_t ToWrite = UniversalEdit::UE->CurrentFile->GetSize();
					std::vector<uint8_t> DataToCopy;

					FILE *Temp = fopen(Path.c_str(), "wb");
					fseek(UniversalEdit::UE->CurrentFile->GetFileHandler(), 0, SEEK_SET); // Seek to start.

					aptSetSleepAllowed(false); // DON'T WHILE WRITTING!
					aptSetHomeAllowed(false);

					Common::ProgressMessage(Common::GetStr("SAVING_FILE") + "\n" + Common::ToHex<uint32_t>(ToWrite));
					while(ToWrite > 0 && aptMainLoop()) {
						if (ToWrite > BUFFER_SIZE) DataToCopy.resize(BUFFER_SIZE);
						else DataToCopy.resize(ToWrite);

						fread(DataToCopy.data(), 0x1, DataToCopy.size(), UniversalEdit::UE->CurrentFile->GetFileHandler());
						fwrite(DataToCopy.data(), 1, DataToCopy.size(), Temp);
						ToWrite -= DataToCopy.size();

						/* Update. */
						Common::ProgressMessage(Common::GetStr("SAVING_FILE") + "\n" + Common::ToHex<uint32_t>(ToWrite));
					};

					fclose(Temp);
					aptSetSleepAllowed(true); // Re-Allow Sleep and HOME, since we are done.
					aptSetHomeAllowed(true);

					UniversalEdit::UE->CurrentFile->Load(Path, 0xD, 0x20000); // Load the new file.
					return true;

				} else {
					std::unique_ptr<StatusMessage> Msg = std::make_unique<StatusMessage>();
					Msg->Handler(Common::GetStr("NOT_ENOUGH_SPACE_SAVE"), -1);
				};
			};
		};
	};

	return false;
};


/*
	Creates a new Temp file at "sdmc:/3ds/Universal-Edit/Temp.bin" with 1 byte.

	Returns true if action succeeded.
*/
bool Actions::NewFile() {
	if (Common::GetFreeSpace() >= 0x1) {
		FILE *Out = fopen("sdmc:/3ds/Universal-Edit/Temp.bin", "w");

		if (Out) {
			const uint8_t V = 0x0;
			fwrite(&V, 1, 1, Out);
			fclose(Out);
			UniversalEdit::UE->CurrentFile->Load("sdmc:/3ds/Universal-Edit/Temp.bin", 0xD, 0x20000); // Load.

			return true;
		};

	} else {
		std::unique_ptr<StatusMessage> Msg = std::make_unique<StatusMessage>();
		Msg->Handler(Common::GetStr("NOT_ENOUGH_SPACE_NEW"), -1);
	};

	return false;
};


/*
	Searches the current file for sequences.

	const uint32_t StartOffs: The offset where to start the search.
	const uint32_t Size: The size to search.
	const std::vector<uint8_t> &Seq: The Sequences to search.

	NOTE: Depending on the current edit mode, this searches the EditData buffer OR the FILE itself.
*/
std::vector<uint32_t> Actions::Search(const uint32_t StartOffs, const uint32_t Size, const std::vector<uint8_t> &Seq) {
	std::vector<uint32_t> Res;
	bool Matched = true;

	for (size_t Idx = 0; Idx < Size && Idx < UniversalEdit::UE->CurrentFile->GetCurModeSize(); Idx++) {
		Common::ProgressMessage(Common::GetStr("SEARCH_MATCHES") + "\n\n" + Common::GetStr("CURRENT_INDEX") + Common::ToHex<uint32_t>(StartOffs + Idx) + " | " + Common::ToHex<uint32_t>(StartOffs + Size) + "\n\n" + Common::GetStr("FOUND_RESULTS") + std::to_string(Res.size()));
		Matched = true; // Always set to true first.

		hidScanInput();
		if (hidKeysDown() & KEY_B) break; // Let B cancel the search.

		for (size_t Idx2 = 0; Idx2 < Seq.size(); Idx2++) {
			if (Idx + Idx2 < UniversalEdit::UE->CurrentFile->GetCurModeSize()) {
				if (UniversalEdit::UE->CurrentFile->GetCurMode() == HexData::EditMode::Scroll) { // Scroll Mode.
					if (UniversalEdit::UE->CurrentFile->Read<uint8_t>(StartOffs + Idx + Idx2, false) != Seq[Idx2]) {
						Matched = false;
						break;
					};

				} else { // Edit / Change Mode.
					if (UniversalEdit::UE->CurrentFile->EditData()[(StartOffs - UniversalEdit::UE->CurrentFile->EditStart()) + Idx + Idx2] != Seq[Idx2]) {
						Matched = false;
						break;
					};
				};

			} else { // Index too large.
				Matched = false;
				break;
			};
		};

		if (Matched) {
			try { // If can push, then do it, else we break it here.
				Res.push_back(StartOffs + Idx);

			} catch(...) {
				break;
			};
		};
	};

	return Res;
};