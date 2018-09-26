#pragma once
#ifndef  H_IMGUI_TOOLS
#define H_IMGUI_TOOLS

//GUI Headers
#include <imgui\imgui.h>
#include <imgui/imguitabwindow.h>
#include <TextEditor.h>
#include <filesystem>
#include <experimental\filesystem>
#include <vector>
#include <Texture.h>

//File Headers
#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <stdexcept>
#include <array>


//Definitions
#define IT_TEXT_SIZE_X 1024
#define IT_TEXT_SIZE_Y 1024
#define IT_STATE_FILE_PATH "data\\state"
#define IT_COMMAND_LENGTH_MAX 256
#define IT_CONSOLE_TEXT_LENGTH 1024*1024

namespace fs = std::experimental::filesystem::v1;
using namespace std;
using namespace fs;

//Text View




namespace itTextView {
	char text[IT_TEXT_SIZE_X * IT_TEXT_SIZE_Y] = "";
	TextEditor codeEditor;
	bool textLoaded = false;
	bool fileSaved = true;
	//0 : normal
	//1 : Code
	//2 : small
	int textViewFormat = 1;


	//to switch text views
	void switchTextView(unsigned int view) {
		textViewFormat = view;
		switch (textViewFormat) {
		case 0: strncpy(text, codeEditor.GetText().c_str(), IT_TEXT_SIZE_X * IT_TEXT_SIZE_Y - 1);
			break;
		case 1: codeEditor.SetText(text);
			break;
		}
	}

	void CodeEditorTextView(ImVec2 size, ImVec2 position = ImVec2(0, 0)) {
		if (textViewFormat == 1) {
			ImGui::SetNextWindowSize(size);
			ImGui::SetNextWindowPos(position);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			ImGui::Begin("Code Editor", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
			if (codeEditor.IsTextChanged()) {
				
				fileSaved = false;
			}
			
			codeEditor.Render("Main", ImVec2(size.x- 17,size.y - 17), false);
			ImGui::PopStyleVar(2);
			ImGui::End();
		}
	}

	void NoteTextView(ImVec2 size,ImVec2 position = ImVec2(0,0)) {
		if (textViewFormat == 0) {
			ImGui::SetNextWindowSize(size);
			ImGui::SetNextWindowPos(position);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			ImGui::Begin("Text Viewer", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
			if (ImGui::InputTextMultiline("", text, IM_ARRAYSIZE(text), ImVec2(size.x - 17, size.y - 17), ImGuiInputTextFlags_AllowTabInput)) {
				fileSaved = false;
			}
			ImGui::PopStyleVar(2);
			ImGui::End();
		}
	}

	void LoadTextFromFile(path file) {
		if (is_regular_file(file) && textLoaded == false) {
			ifstream fileStream;
			fileStream.open(file);
			if(fileStream.is_open()) {
				fileStream.open(file);
				string fileContent((istreambuf_iterator<char>(fileStream)), istreambuf_iterator<char>());
				strncpy(text, fileContent.c_str(), IT_TEXT_SIZE_X * IT_TEXT_SIZE_Y - 1);
				codeEditor.SetText(text);
				textLoaded = true;
			}
			fileStream.close();
		}
	}

	void SaveTextToFile(path file) {
		if (fileSaved == false) {
			if (is_regular_file(file)) {
				ofstream fileStream;
				fileStream.open(file);
				if (textViewFormat == 1) {
					fileStream << codeEditor.GetText().c_str();
				}
				else {
					fileStream << text;
				}
				fileStream.close();
				fileSaved = true;
			}
		}
	}

}





//File Dialog View
namespace itFileDialogView{
	vector<string> pathList;
	vector<string> pathListNames;
	int selected = -1;
	bool functionRun = false;
	path current = current_path().root_path();
	path file;
	char pathName[MAX_PATH] = "";
	bool enableFDV = true;

	bool deleteFile(path FilePath) {
		if (exists(FilePath)) {
			if (is_regular_file(FilePath)) {
				if (path(FilePath) == itFileDialogView::file) {
					remove(FilePath);
					itFileDialogView::file = path();
					strncpy(itTextView::text, "", IT_TEXT_SIZE_X * IT_TEXT_SIZE_Y - 1);
					itTextView::codeEditor.SetText("");
					itFileDialogView::functionRun = false;
					return true;
				}
				else {
					remove(FilePath);
					itFileDialogView::functionRun = false;
					return true;
				}
			}
			else if (is_directory(FilePath)) {
				remove_all(FilePath);
				itFileDialogView::functionRun = false;
				return true;
			}
		}
		return false;
	}

	void FileDialogView(ImVec2 size, ImVec2 position = ImVec2(0, 0)) {
		if (enableFDV == true) {
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);

			ImGui::Begin("File Chooser", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);
			ImGui::SetWindowSize(size);
			ImGui::SetWindowPos(position);

			if (ImGui::BeginPopupContextItem())
			{
				if (ImGui::MenuItem("Close"))
					enableFDV = false;
				ImGui::EndPopup();
			}

			if (ImGui::ArrowButton("##left", ImGuiDir_Left) && current != current_path().root_path()) {
				current = current.parent_path();
				functionRun = false;
			}ImGui::SameLine();

			if (ImGui::ArrowButton("##up", ImGuiDir_Up)) {
				current = current_path().root_path();
				functionRun = false;
			}ImGui::SameLine();
			if (ImGui::Button("Refresh")) {
				functionRun = false;
			}


			//Path View
			ImGui::PushItemWidth(size.x - 15);
			if (ImGui::InputText(" ", pathName, 64, ImGuiInputTextFlags_EnterReturnsTrue)) {
				path checkInput(pathName);
				if (is_directory(checkInput)) {
					current = checkInput;
					functionRun = false;
				}
				else if (is_regular_file(checkInput)) {
					functionRun = false;
					enableFDV = false;
				}
			}
			ImGui::PopItemWidth();

			//Filter
			static ImGuiTextFilter filter;
			filter.Draw("", size.x - 15);
			if (functionRun == false) {
				selected = -1;
				pathList = vector<string>();
				for (auto &p : directory_iterator(current))
				{
					if (is_directory(p) || is_regular_file(p))
						pathList.push_back(path(p).string().c_str());
				}

				strncpy(pathName, current.string().c_str(), MAX_PATH);
				functionRun = true;
			}

			ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 10.0f);
			ImGui::BeginChild("Child2", ImVec2(0, size.y - 150), true);
			int listNumber = 0;
			for (vector<string>::iterator it = pathList.begin(); it != pathList.end(); it++) {
				path temp(*it);
				if (filter.PassFilter(temp.string().c_str())) {
					if (ImGui::Selectable(temp.filename().string().c_str(), selected == listNumber, ImGuiSelectableFlags_AllowDoubleClick)) {
						selected = listNumber;
						if (ImGui::IsMouseDoubleClicked(0) && selected != -1) {
							if (is_directory(pathList[selected])) {
								current = pathList[selected];
								functionRun = false;
							}
							if (is_regular_file(pathList[selected])) {
								file = pathList[selected];
								itTextView::textLoaded = false;
							}
						}

					}
					listNumber++;
				}
			}
			ImGui::EndChild();
			ImGui::PopStyleVar();
			ImGui::Separator();
			if(ImGui::Button("Delete", ImVec2(size.x - 15, 20))){
				deleteFile(pathList[selected]);
			}
			string buttonOption = "Open";
			if (selected > -1) {
				path selectedPath = path(pathList[selected]);
				if (is_directory(selectedPath)) {
					buttonOption = "Open Folder";
				}
				else if (is_regular_file(selectedPath)) {
					buttonOption = "Load File";
				}
			}


			if (ImGui::Button(buttonOption.c_str(),ImVec2(size.x - 15,20)) && selected > -1) {
				path selectedPath = path(pathList[selected]);
				if (is_directory(selectedPath)) {
					current = selectedPath;
					functionRun = false;
				}
				if (is_regular_file(selectedPath)) {
					file = selectedPath;
					itTextView::textLoaded = false;
				}
			}
			ImGui::SameLine();
			ImGui::PopStyleVar(2);
			ImGui::End();
		}
	}



}

//System File Modifers
namespace itSystemFileModifer {
	bool newFile(string file) {
		string newFilePath = itFileDialogView::current.string() + "/" + file;
		if (!exists(newFilePath)) {
			ofstream newFile;
			newFile.open(newFilePath.c_str(), ios::out);
			newFile.close();
			return true;
		}
		return false;
	}

	bool newDir(string dirName) {
		string newDirPath = itFileDialogView::current.string() + "/" + dirName;
		if (!is_directory(newDirPath) || !exists(newDirPath)) {
			create_directory(newDirPath);
			return true;
		}
		return false;
	}

	bool deleteFile(path FilePath) {
		if (exists(FilePath)) {
			if (is_regular_file(FilePath)) {
				if (path(FilePath) == itFileDialogView::file) {
					remove(FilePath);
					itFileDialogView::file = path();
					strncpy(itTextView::text, "", IT_TEXT_SIZE_X * IT_TEXT_SIZE_Y - 1);
					itTextView::codeEditor.SetText("");
					itFileDialogView::functionRun = false;
					return true;
				}
				else {
					remove(FilePath);
					itFileDialogView::functionRun = false;
					return true;
				}
			}
			else if (is_directory(FilePath)) {
				remove_all(FilePath);
				itFileDialogView::functionRun = false;
				return true;
			}
		}
		return false;
	}

}
//FILE DIALOG
namespace itFileDialog {
	vector<string> pathList;
	vector<string> pathListNames;
	int selected = -1;
	bool functionRun = false;
	path current = current_path().root_path();
	path file;
	char pathName[MAX_PATH] = "";
	bool enableFD = false;
	char newFileDirName[256] = "";



	void FileDialog() {
		if (enableFD == true) {
			ImGui::OpenPopup("Choose Directory");
		}
		if (ImGui::BeginPopupModal("Choose Directory", NULL, ImGuiWindowFlags_AlwaysAutoResize)){

			if (ImGui::ArrowButton("##left", ImGuiDir_Left) && current != current_path().root_path()) {
				current = current.parent_path();
				functionRun = false;
			}ImGui::SameLine();

			if (ImGui::ArrowButton("##up", ImGuiDir_Up)) {
				current = current_path().root_path();
				functionRun = false;
			}ImGui::SameLine();

			if (ImGui::InputText("Path", pathName, 64, ImGuiInputTextFlags_EnterReturnsTrue)) {
				path checkInput(pathName);
				if (is_directory(checkInput)) {
					current = checkInput;
					functionRun = false;
				}
				else if (is_regular_file(checkInput)) {
					functionRun = false;
					enableFD = false;
				}
			}
			ImGui::SameLine();

			static ImGuiTextFilter filter;
			filter.Draw("Search", 100.0f);
			ImGui::SameLine();
			if (ImGui::Button("Refresh")) {
				functionRun = false;
			}

			ImGui::Separator();
			if (functionRun == false) {
				selected = -1;
				pathList = vector<string>();
				for (auto &p : directory_iterator(current))
				{
					if (is_directory(p) || is_regular_file(p))
						pathList.push_back(path(p).string().c_str());
				}

				strncpy(pathName, current.string().c_str(), MAX_PATH);
				functionRun = true;
			}

			ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 10.0f);
			ImGui::BeginChild("Child2", ImVec2(0, 300), true);
			int listNumber = 0;
			for (vector<string>::iterator it = pathList.begin(); it != pathList.end(); it++) {
				path temp(*it);
				if (filter.PassFilter(temp.string().c_str())) {
					if (ImGui::Selectable(temp.filename().string().c_str(), selected == listNumber, ImGuiSelectableFlags_AllowDoubleClick)) {
						selected = listNumber;
						if (ImGui::IsMouseDoubleClicked(0) && selected != -1) {
							if (is_directory(pathList[selected])) {
								current = pathList[selected];
								functionRun = false;
							}
						}
					}
					listNumber++;
				}
			}
			ImGui::EndChild();
			ImGui::PopStyleVar();
			ImGui::Separator();

			string buttonOption = "Open";
			if (selected > -1) {
				path selectedPath = path(pathList[selected]);
				if (is_directory(selectedPath)) {
					buttonOption = "Open Folder";
				}
			}


			if (ImGui::Button(buttonOption.c_str()) && selected > -1) {
				path selectedPath = path(pathList[selected]);
				if (is_directory(selectedPath)) {
					current = selectedPath;
					functionRun = false;
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel")) {
				pathList = vector<string>();
				enableFD = false;
				functionRun = false;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Create File")) {
				if (itSystemFileModifer::newFile(newFileDirName)) {
					pathList = vector<string>();
					enableFD = false;
					itFileDialogView::functionRun = false;
					functionRun = false;
					strcpy(newFileDirName, "");
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Create Dir")) {
				if (itSystemFileModifer::newDir(newFileDirName)) {
					pathList = vector<string>();
					enableFD = false;
					itFileDialogView::functionRun = false;
					functionRun = false;
					strcpy(newFileDirName, "");
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::SameLine();
			ImGui::InputText("Name", newFileDirName, 256);
			ImGui::EndPopup();
		}
	}


}

//Save Sate
namespace itStateModifier {
	void saveState() {
		ofstream fileState;
		fileState.open(IT_STATE_FILE_PATH);
		if (fileState.is_open()) {
			fileState << itFileDialogView::current << endl;
			fileState << itFileDialogView::file;
		}
		fileState.close();
	}

	void loadState() {
		ifstream fileState;
		string dirPath;
		string filePath;

		fileState.open(IT_STATE_FILE_PATH);
		//First read
		getline(fileState, dirPath);
		getline(fileState, filePath);
		path dir(dirPath);
		path file(filePath);

		if (is_directory(dir)) {
			itFileDialogView::current = dir;
			itFileDialog::current = dir;
		}

		if (is_regular_file(file)) {
			itFileDialogView::file = file;
		}

		fileState.close();
	}
}


namespace itConsole{
	bool enableConsole = true;
	char command[IT_COMMAND_LENGTH_MAX] = "";
	char ConsoleViewText[IT_CONSOLE_TEXT_LENGTH] = "";

	string exec(const char* cmd) {
		std::array<char, 128> buffer;
		std::string result;
		std::shared_ptr<FILE> pipe(_popen(cmd, "r"), _pclose);
		if (!pipe) throw std::runtime_error("popen() failed!");
		while (!feof(pipe.get())) {
			if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
				result += buffer.data();
		}
		return result;
	}
	

	struct Console
	{
		char                  InputBuf[256];
		ImVector<char*>       Items;
		bool                  ScrollToBottom;
		ImVector<char*>       History;
		int                   HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
		ImVector<const char*> Commands;

		Console()
		{
			ClearLog();
			memset(InputBuf, 0, sizeof(InputBuf));
			HistoryPos = -1;
		}
		~Console()
		{
			ClearLog();
			for (int i = 0; i < History.Size; i++)
				free(History[i]);
		}

		// Portable helpers
		static int   Stricmp(const char* str1, const char* str2) { int d; while ((d = toupper(*str2) - toupper(*str1)) == 0 && *str1) { str1++; str2++; } return d; }
		static int   Strnicmp(const char* str1, const char* str2, int n) { int d = 0; while (n > 0 && (d = toupper(*str2) - toupper(*str1)) == 0 && *str1) { str1++; str2++; n--; } return d; }
		static char* Strdup(const char *str) { size_t len = strlen(str) + 1; void* buff = malloc(len); return (char*)memcpy(buff, (const void*)str, len); }
		static void  Strtrim(char* str) { char* str_end = str + strlen(str); while (str_end > str && str_end[-1] == ' ') str_end--; *str_end = 0; }

		void    ClearLog()
		{
			for (int i = 0; i < Items.Size; i++)
				free(Items[i]);
			Items.clear();
			ScrollToBottom = true;
		}

		void    AddLog(const char* fmt, ...) IM_FMTARGS(2)
		{
			// FIXME-OPT
			char buf[1024];
			va_list args;
			va_start(args, fmt);
			vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
			buf[IM_ARRAYSIZE(buf) - 1] = 0;
			va_end(args);
			Items.push_back(Strdup(buf));
			ScrollToBottom = true;
		}

		void    Draw(const char* title,ImVec2 size, ImVec2 position, bool* p_open)
		{
			ImGui::SetNextWindowSize(size);
			ImGui::SetNextWindowPos(position);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			if (!ImGui::Begin(title, p_open, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove))
			{
				ImGui::End();
				return;
			}

			// As a specific feature guaranteed by the library, after calling Begin() the last Item represent the title bar. So e.g. IsItemHovered() will return true when hovering the title bar.
			// Here we create a context menu only available from the title bar.
			if (ImGui::BeginPopupContextItem())
			{
				if (ImGui::MenuItem("Close"))
					*p_open = false;
				ImGui::EndPopup();
			}

			if (ImGui::SmallButton("Clear")) { ClearLog(); } ImGui::SameLine();
			bool copy_to_clipboard = ImGui::SmallButton("Copy"); ImGui::SameLine();
			if (ImGui::SmallButton("Scroll to bottom")) ScrollToBottom = true;
			//static float t = 0.0f; if (ImGui::GetTime() - t > 0.02f) { t = ImGui::GetTime(); AddLog("Spam %f", t); }

			ImGui::Separator();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
			static ImGuiTextFilter filter;
			filter.Draw("Filter", 180);
			ImGui::PopStyleVar();
			ImGui::Separator();

			const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing(); // 1 separator, 1 input text
			ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar); // Leave room for 1 separator + 1 InputText
			if (ImGui::BeginPopupContextWindow())
			{
				if (ImGui::Selectable("Clear")) ClearLog();
				ImGui::EndPopup();
			}

			// Display every line as a separate entry so we can change their color or add custom widgets. If you only want raw text you can use ImGui::TextUnformatted(log.begin(), log.end());
			// NB- if you have thousands of entries this approach may be too inefficient and may require user-side clipping to only process visible items.
			// You can seek and display only the lines that are visible using the ImGuiListClipper helper, if your elements are evenly spaced and you have cheap random access to the elements.
			// To use the clipper we could replace the 'for (int i = 0; i < Items.Size; i++)' loop with:
			//     ImGuiListClipper clipper(Items.Size);
			//     while (clipper.Step())
			//         for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
			// However, note that you can not use this code as is if a filter is active because it breaks the 'cheap random-access' property. We would need random-access on the post-filtered list.
			// A typical application wanting coarse clipping and filtering may want to pre-compute an array of indices that passed the filtering test, recomputing this array when user changes the filter,
			// and appending newly elements as they are inserted. This is left as a task to the user until we can manage to improve this example code!
			// If your items are of variable size you may want to implement code similar to what ImGuiListClipper does. Or split your data into fixed height items to allow random-seeking into your list.
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
			if (copy_to_clipboard)
				ImGui::LogToClipboard();
			ImVec4 col_default_text = ImGui::GetStyleColorVec4(ImGuiCol_Text);
			for (int i = 0; i < Items.Size; i++)
			{
				const char* item = Items[i];
				if (!filter.PassFilter(item))
					continue;
				ImVec4 col = col_default_text;
				if (strstr(item, "[error]")) col = ImColor(1.0f, 0.4f, 0.4f, 1.0f);
				else if (strncmp(item, "# ", 2) == 0) col = ImColor(1.0f, 0.78f, 0.58f, 1.0f);
				ImGui::PushStyleColor(ImGuiCol_Text, col);
				ImGui::TextUnformatted(item);
				ImGui::PopStyleColor();
			}
			if (copy_to_clipboard)
				ImGui::LogFinish();
			if (ScrollToBottom)
				ImGui::SetScrollHere(1.0f);
			ScrollToBottom = false;
			ImGui::PopStyleVar();
			ImGui::EndChild();
			ImGui::Separator();

			// Command-line
			bool reclaim_focus = false;
			ImGui::PushItemWidth(-1);
			if (ImGui::InputText("", InputBuf, IM_ARRAYSIZE(InputBuf), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory, &TextEditCallbackStub, (void*)this))
			{
				if (InputBuf[0])
					AddLog(exec(InputBuf).c_str());
					AddLog("\n");
				strcpy(InputBuf, "");
				reclaim_focus = true;
			}
			ImGui::PopItemWidth();

			// Demonstrate keeping focus on the input box
			ImGui::SetItemDefaultFocus();
			if (reclaim_focus)
				ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

			ImGui::PopStyleVar();
			ImGui::End();
		}


		static int TextEditCallbackStub(ImGuiTextEditCallbackData* data) // In C++11 you are better off using lambdas for this sort of forwarding callbacks
		{
			Console* console = (Console*)data->UserData;
			return console->TextEditCallback(data);
		}

		int     TextEditCallback(ImGuiTextEditCallbackData* data)
		{
			//AddLog("cursor: %d, selection: %d-%d", data->CursorPos, data->SelectionStart, data->SelectionEnd);
			switch (data->EventFlag)
			{
			case ImGuiInputTextFlags_CallbackCompletion:
			{
				// Example of TEXT COMPLETION

				// Locate beginning of current word
				const char* word_end = data->Buf + data->CursorPos;
				const char* word_start = word_end;
				while (word_start > data->Buf)
				{
					const char c = word_start[-1];
					if (c == ' ' || c == '\t' || c == ',' || c == ';')
						break;
					word_start--;
				}

				// Build a list of candidates
				ImVector<const char*> candidates;
				for (int i = 0; i < Commands.Size; i++)
					if (Strnicmp(Commands[i], word_start, (int)(word_end - word_start)) == 0)
						candidates.push_back(Commands[i]);

				if (candidates.Size == 0)
				{
					// No match
					AddLog("No match for \"%.*s\"!\n", (int)(word_end - word_start), word_start);
				}
				else if (candidates.Size == 1)
				{
					// Single match. Delete the beginning of the word and replace it entirely so we've got nice casing
					data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
					data->InsertChars(data->CursorPos, candidates[0]);
					data->InsertChars(data->CursorPos, " ");
				}
				else
				{
					// Multiple matches. Complete as much as we can, so inputing "C" will complete to "CL" and display "CLEAR" and "CLASSIFY"
					int match_len = (int)(word_end - word_start);
					for (;;)
					{
						int c = 0;
						bool all_candidates_matches = true;
						for (int i = 0; i < candidates.Size && all_candidates_matches; i++)
							if (i == 0)
								c = toupper(candidates[i][match_len]);
							else if (c == 0 || c != toupper(candidates[i][match_len]))
								all_candidates_matches = false;
						if (!all_candidates_matches)
							break;
						match_len++;
					}

					if (match_len > 0)
					{
						data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
						data->InsertChars(data->CursorPos, candidates[0], candidates[0] + match_len);
					}

					// List matches
					AddLog("Possible matches:\n");
					for (int i = 0; i < candidates.Size; i++)
						AddLog("- %s\n", candidates[i]);
				}

				break;
			}
			case ImGuiInputTextFlags_CallbackHistory:
			{
				// Example of HISTORY
				const int prev_history_pos = HistoryPos;
				if (data->EventKey == ImGuiKey_UpArrow)
				{
					if (HistoryPos == -1)
						HistoryPos = History.Size - 1;
					else if (HistoryPos > 0)
						HistoryPos--;
				}
				else if (data->EventKey == ImGuiKey_DownArrow)
				{
					if (HistoryPos != -1)
						if (++HistoryPos >= History.Size)
							HistoryPos = -1;
				}

				// A better implementation would preserve the data on the current input line along with cursor position.
				if (prev_history_pos != HistoryPos)
				{
					data->CursorPos = data->SelectionStart = data->SelectionEnd = data->BufTextLen = (int)snprintf(data->Buf, (size_t)data->BufSize, "%s", (HistoryPos >= 0) ? History[HistoryPos] : "");
					data->BufDirty = true;
				}
			}
			}
			return 0;
		}
	};

	static Console console;
	static void consoleView(ImVec2 size = ImVec2(400,400), ImVec2 position = ImVec2(0,0))
	{
		if (itConsole::enableConsole) {
			console.Draw("Console", size, position, &enableConsole);
		}
	}

	void compile(path filePath) {
		if (is_regular_file(filePath) && filePath.extension() == ".py") {
			string command = string("py ") + string("\"") + filePath.string() + string("\"");
			console.AddLog(exec(command.c_str()).c_str());
		}
	}

}
#endif
