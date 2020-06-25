#include <direct.h>
#include <string> //string related statements
#include <windows.h>
#include <iomanip>
#include <conio.h>
#include <cctype>
#include <algorithm>
#include <vector>
#include <fstream> //opening and closing files
#include <filesystem>
#include <iostream>



using namespace std;

HANDLE hOut;
CONSOLE_SCREEN_BUFFER_INFO SBInfo;
COORD NewSBSize;
int Status;

bool exitApp = false;

void gotoXY(int x, int y) {
	COORD coord;
	coord.X = x;
	coord.Y = y;

	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void ShowConsoleCursor(bool showFlag)
{
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_CURSOR_INFO     cursorInfo;

	GetConsoleCursorInfo(out, &cursorInfo);
	cursorInfo.bVisible = showFlag; // set the cursor visibility
	SetConsoleCursorInfo(out, &cursorInfo);
}

void removeScrollBar() {
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);

	GetConsoleScreenBufferInfo(hOut, &SBInfo);
	NewSBSize.X = SBInfo.dwSize.X;
	NewSBSize.Y = SBInfo.dwMaximumWindowSize.Y; //37

	Status = SetConsoleScreenBufferSize(hOut, NewSBSize);
	if (Status == 0)
	{
		Status = GetLastError();
		std::cout << "SetConsoleScreenBufferSize() failed! Reason : " << Status << endl;
		exit(Status);
	}
}

void showScrollBar() {
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);

	GetConsoleScreenBufferInfo(hOut, &SBInfo);
	NewSBSize.X = SBInfo.dwSize.X;
	NewSBSize.Y = SBInfo.dwMaximumWindowSize.Y + 100; //37

	Status = SetConsoleScreenBufferSize(hOut, NewSBSize);
	if (Status == 0)
	{
		Status = GetLastError();
		std::cout << "SetConsoleScreenBufferSize() failed! Reason : " << Status << endl;
		exit(Status);
	}
}

void title() {
	gotoXY(25, 3); std::cout << "*---------------------*";
	gotoXY(25, 4); std::cout << "|                     |";
	gotoXY(25, 5); std::cout << "|      AlmaPedia      |";
	gotoXY(25, 6); std::cout << "|                     |";
	gotoXY(25, 7); std::cout << "*---------------------*";
}

void categories(string category);

void adminAccess();

void entry(string directory, string entry);

void encyclopedia();

bool loginCheck(string username, string password) {
	ifstream user;
	username = "users\\" + username + ".txt";

	user.open(username);
	
	//checks user
	if (user.is_open()) {
		string pass;
		getline(user, pass);

		//if user success
		if (password == pass) {
			return true;
		}
	}
	// if user doesn't exist or fails
	else {
		return false;
		user.close();
	}
}

void loadingScreen() {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
	
	gotoXY(25, 10); std::cout << "Welcome to AlmaPedia";
	gotoXY(25, 11); std::cout << "Loading  ";
	for (int load = 0; load < 5; load++) {
		Sleep(50);
		std::cout << "\b\\";
		Sleep(50);
		std::cout << "\b|";
		Sleep(50);
		std::cout << "\b/";
		Sleep(50);
		std::cout << "\b-";
	}
	std::system("cls");
}

void startScreen();

void loginScreen() {
	std::system("cls");
	string username, password;

	ShowConsoleCursor(true);

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	gotoXY(20, 5); std::cout << "Login Screen\n\n";
	//User must input their assigned username
	gotoXY(20, 7); std::cout << "User: ";
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
	getline(cin, username);
	//Following the username, they must also input the appropriate password
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	gotoXY(20, 8); std::cout << "Pass: ";
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0);
	getline(cin, password);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);

	ShowConsoleCursor(false);

	loginCheck(username, password);

	if (loginCheck(username, password)) {
		std::system("cls");
		adminAccess();
	}
	else if (!loginCheck(username, password)) {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
		std::system("cls");
		gotoXY(20, 13); cout << "Invalid login credentials";
		startScreen();
	}

}

void modifyCategory(int menu_item);

void modifyContent(string category);

void modifyEntry(int menu_item, string entry);

void deleteEntry(string directory, string thing, int menu_item) {
	int confirm = 0;
	int x = 7;
	bool running = true;

	gotoXY(20, 5); std::cout << "Are you sure you want to delete " << thing << "?";
	gotoXY(18, x); std::cout << "->";

	gotoXY(20, 7); std::cout << "Yes";
	gotoXY(20, 8); std::cout << "No";

	_getch();

	while (running) {
			if (GetAsyncKeyState(VK_UP) && x != 7) //up button pressed
			{
				gotoXY(18, x); std::cout << "  ";
				x--;
				gotoXY(18, x); std::cout << "->";
				confirm--;
				continue;
			}
			else if (GetAsyncKeyState(VK_DOWN) && x != 8) //down button pressed
			{
				gotoXY(18, x); std::cout << "  ";
				x++;
				gotoXY(18, x); std::cout << "->";
				confirm++;
				continue;
			}


			if (confirm == 0) {
				thing.erase(remove_if(thing.begin(), thing.end(), isspace), thing.end());
				filesystem::remove_all(directory + thing + ".txt");
				Sleep(10);
				// open file in read mode or in mode 
				ifstream is(directory + "dir.txt");

				// open file in write mode or out mode 
				ofstream ofs;
				ofs.open("temp.txt", ofstream::out);

				// loop getting single characters 
				char c;
				int line_no = 1;
				while (is.get(c))
				{
					// if a newline character 
					if (c == '\n')
						line_no++;

					// file content not to be deleted 
					if (line_no != menu_item + 1)
						ofs << c;
				}

				// closing output file 
				ofs.close();

				// closing input file 
				is.close();

				// remove the original file 
				filesystem::remove(directory + "dir.txt");

				// rename the file 
				filesystem::rename("temp.txt", "dir.txt");
				filesystem::copy("dir.txt", directory + "dir.txt");
				filesystem::remove("dir.txt");
				adminAccess();
			}
			else if (confirm == 1) {
				modifyCategory(menu_item);
			}
		
	}
}

void deleteCategory(string thing, int menu_item) {
	int confirm = 0;
	int x = 7;
	bool running = true;

	gotoXY(20, 5); std::cout << "Are you sure you want to delete " << thing << "?";
	gotoXY(18, x); std::cout << "->";

	gotoXY(20, 7); std::cout << "Yes";
	gotoXY(20, 8); std::cout << "No";

	_getch();

	while (running) {
		if (GetAsyncKeyState(VK_UP) && x != 7) //up button pressed
		{
			gotoXY(18, x); std::cout << "  ";
			x--;
			gotoXY(18, x); std::cout << "->";
			confirm--;
			continue;
		}
		else if (GetAsyncKeyState(VK_DOWN) && x != 8) //down button pressed
		{
			gotoXY(18, x); std::cout << "  ";
			x++;
			gotoXY(18, x); std::cout << "->";
			confirm++;
			continue;
		}

		if (GetAsyncKeyState(VK_RETURN)) {
			running = false;

			if (confirm == 0) {
				thing.erase(remove_if(thing.begin(), thing.end(), isspace), thing.end());
				Sleep(50);
				filesystem::remove_all(".\\entries\\" + thing);
				Sleep(50);
				// open file in read mode or in mode 
				ifstream is(".\\entries\\dir.txt");

				// open file in write mode or out mode 
				ofstream ofs;
				ofs.open("temp.txt", ofstream::out);

				// loop getting single characters 
				char c;
				int line_no = 1;
				while (is.get(c))
				{
					// if a newline character 
					if (c == '\n')
						line_no++;

					// file content not to be deleted 
					if (line_no != menu_item + 1)
						ofs << c;
				}

				// closing output file 
				ofs.close();

				// closing input file 
				is.close();

				// remove the original file 
				filesystem::remove(".\\entries\\dir.txt");
				//

				// rename the file 
				filesystem::rename("temp.txt", "dir.txt");
				filesystem::copy("dir.txt", ".\\entries\\dir.txt");
				filesystem::remove("dir.txt");
				running = false;
				adminAccess();
			}
			else if (confirm == 1) {
				running = false;
				adminAccess();
			}
		}
	}
}

void textEdit(string directory, string textFile) {
	std::system("cls");
	int menu_item = 0, x = 11;
	bool running = true;
	
	gotoXY(18, x); std::cout << "->";

	while (running)
	{
		gotoXY(20, 9); cout << textFile;
		title();
		gotoXY(20, 11);  std::cout << "View Entry";
		gotoXY(20, 12); std::cout << "Write New Line";
		//gotoXY(20, 11); std::cout << "Redo a Line";
		gotoXY(20, 13); std::cout << "Delete a Line";
		gotoXY(20, 14); std::cout << "Finish Editing";

		std::system("pause>nul"); // the >nul bit causes it the print no message

		if (GetAsyncKeyState(VK_UP) && x != 11) //up button pressed
		{
			gotoXY(18, x); std::cout << "  ";
			x--;
			gotoXY(18, x); std::cout << "->";
			menu_item--;
			continue;
		}
		else if (GetAsyncKeyState(VK_DOWN) && x != 14) //down button pressed
		{
			gotoXY(18, x); std::cout << "  ";
			x++;
			gotoXY(18, x); std::cout << "->";
			menu_item++;
			continue;
		}

		if (GetAsyncKeyState(VK_RETURN)) { // Enter key pressed

			if (menu_item == 0) {
				std::system("cls");
				showScrollBar();

				ifstream preview(directory + textFile);
				string output;
				int i = 1;
				 {
					while (!preview.eof()) {
						getline(preview, output);
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
						std::cout << i++ << " | ";
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
						std::cout << output << endl;
					}
					preview.close();
				}

				std::cout << "\n\n\n...Press any key to go back...";
				std::system("pause>nul");
				removeScrollBar();
				std::system("cls");
			}

			else if (menu_item == 1) {
				std::system("cls");
				string line;
				fstream write(directory + textFile, ios::app);
				ShowConsoleCursor(true);
				gotoXY(20, 9);	std::cout << "Enter - write line";
				gotoXY(20, 10); std::cout << "Line: ";
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
				gotoXY(20, 11); getline(cin, line);
				write << endl << line;
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
				write.close();
				ShowConsoleCursor(false);
				std::system("cls");
			}

			/*else if (menu_item == 2) {
				string line;
				int remove;

				gotoXY(20, 9); std::cout << "Which line to redo? (input 0 to go back)";
				gotoXY(20, 10); std::cout << "Line: "; cin >> remove;

				gotoXY(20, 12); std::cout << "Enter new line:";
				gotoXY(20, 13); cin >> line;
				// open file in read mode or in mode 
				ifstream is(directory + textFile);

				// open file in write mode or out mode 
				ofstream ofs;
				ofs.open("temp.txt", ofstream::out);

				// loop getting single characters 
				char c;
				int line_no = 1;
				while (is.get(c))
				{
					// if a newline character 
					if (c == '\n')
						line_no++;

					// file content not to be deleted 
					if (line_no != remove){
						ofs << c;
						}
					else if (line_no == remove){
						ofs << line;
						}
				}

				// closing output file 
				ofs.close();

				// closing input file 
				is.close();

				// remove the original file 
				filesystem::remove(directory + textFile);

				// rename the file 
				filesystem::rename("temp.txt", textFile);
				filesystem::copy(textFile, directory + textFile);
				filesystem::remove(textFile);
				std::system("cls");
			}*/
			

			else if (menu_item == 2) {
				std::system("cls");
				int remove = 0;	
				ShowConsoleCursor(true);
				gotoXY(20, 9); std::cout << "Which line to remove? (input 0 to go back)";
				gotoXY(20, 10); std::cout << "Line: "; cin >> remove;

				if (remove < 2 && remove != 0) {
					while (std::cin.fail())
					{
						std::cin.clear();
						gotoXY(20, 7); std::cout << "Input is invalid";
						std::cin >> remove;
					}
				}


				ShowConsoleCursor(false);
				// open file in read mode or in mode 
				ifstream is(directory + textFile);

				// open file in write mode or out mode 
				ofstream ofs;
				ofs.open("temp.txt", ofstream::out);

				// loop getting single characters 
				char c;
				int line_no = 1;
				while (is.get(c))
				{
					// if a newline character 
					if (c == '\n')
						line_no++;

					// file content not to be deleted 
					if (line_no != remove)
						ofs << c;
				}

				// closing output file 
				ofs.close();

				// closing input file 
				is.close();

				// remove the original file 
				filesystem::remove(directory + textFile);

				// rename the file 
				filesystem::rename("temp.txt", textFile);
				filesystem::copy(textFile, directory + textFile);
				filesystem::remove(textFile);
				std::system("cls");
			}

			else if (menu_item == 3) {
				adminAccess();
				break;
			}

		}

	}
}

/* MAIN */
int main() {
	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(cfi);
	cfi.nFont = 0;
	cfi.dwFontSize.X = 0;                   // Width of each character in the font
	cfi.dwFontSize.Y = 24;                  // Height
	cfi.FontFamily = FF_MODERN;
	cfi.FontWeight = FW_NORMAL;
	SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);

	SendMessage(GetConsoleWindow(), WM_SYSKEYDOWN, VK_RETURN, 0x20000000);

	hOut = GetStdHandle(STD_OUTPUT_HANDLE);

	GetConsoleScreenBufferInfo(hOut, &SBInfo);
	NewSBSize.X = SBInfo.dwSize.X;
	NewSBSize.Y = SBInfo.dwMaximumWindowSize.Y; //37

	Status = SetConsoleScreenBufferSize(hOut, NewSBSize);
	if (Status == 0)
	{
		Status = GetLastError();
		std::cout << "SetConsoleScreenBufferSize() failed! Reason : " << Status << endl;
		exit(Status);
	}

	ShowConsoleCursor(false);

	std::system("cls");
	loadingScreen();
	std::system("cls");
	startScreen();
}
/* MAIN */


void modifyEntry(int menu_item, string entry) {
	string thing;
	string directory = ".\\entries\\" + entry + "\\";
	fstream selection(directory + "dir.txt");

	hOut = GetStdHandle(STD_OUTPUT_HANDLE);

	GetConsoleScreenBufferInfo(hOut, &SBInfo);
	NewSBSize.X = SBInfo.dwSize.X;
	NewSBSize.Y = SBInfo.dwMaximumWindowSize.Y; //37

	Status = SetConsoleScreenBufferSize(hOut, NewSBSize);
	if (Status == 0)
	{
		Status = GetLastError();
		std::cout << "SetConsoleScreenBufferSize() failed! Reason : " << Status << endl;
		exit(Status);
	}

	for (int i = 0; i < menu_item + 1; i++)
	{
		getline(selection, thing);
	}
	selection.close();

	int option = 0, x = 7;
	bool running = true;

	gotoXY(20, 5); std::cout << "What do you want to do with " << thing;
	gotoXY(18, x); std::cout << "->";

	gotoXY(20, 7); std::cout << "Modify Content";
	gotoXY(20, 8); std::cout << "Delete Content";
	gotoXY(20, 9); std::cout << "Back";

	while (running)
	{

		std::system("pause>nul"); // the >nul bit causes it the print no message
		if (GetAsyncKeyState(VK_UP) && x != 7) //up button pressed
		{
			gotoXY(18, x); std::cout << "  ";
			x--;
			gotoXY(18, x); std::cout << "->";
			option--;
			continue;
		}
		else if (GetAsyncKeyState(VK_DOWN) && x != 9) //down button pressed
		{
			gotoXY(18, x); std::cout << "  ";
			x++;
			gotoXY(18, x); std::cout << "->";
			option++;
			continue;

		}

		if (GetAsyncKeyState(VK_RETURN)) { // Enter key pressed
			bool running = false;
			if (option == 0) {
				thing.erase(remove_if(thing.begin(), thing.end(), isspace), thing.end());
				textEdit(directory, thing + ".txt");
			}
			else if (option == 1) {
				std::system("cls");
				deleteEntry(directory, thing, menu_item);
			}
			else if (option == 2) {
				modifyContent(entry);
			}
		}

	}

	adminAccess();
}

void modifyContent(string category) {
	std::system("cls");
	int menu_item = 1, x = 11, cat = 10;
	bool running = true;
	int number_of_lines = 0;

	hOut = GetStdHandle(STD_OUTPUT_HANDLE);

	GetConsoleScreenBufferInfo(hOut, &SBInfo);
	NewSBSize.X = SBInfo.dwSize.X;

	if (Status == 0)
	{
		Status = GetLastError();
		std::cout << "SetConsoleScreenBufferSize() failed! Reason : " << Status << endl;
		exit(Status);
	}

	string output; // text to be displayed
	ifstream directory; // the file to be opened

	directory.open(".\\entries\\" + category + "\\dir.txt");

	while (running)
	{
		gotoXY(20, 9); cout << category;
		title();
		if (directory.is_open()) {
			while (!directory.eof()) {
				getline(directory, output);
				gotoXY(20, cat++); std::cout << output;
				++number_of_lines;
				NewSBSize.Y = SBInfo.dwMaximumWindowSize.Y + (number_of_lines / 3); //37
				Status = SetConsoleScreenBufferSize(hOut, NewSBSize);
			}
			gotoXY(20, cat); std::cout << "Create New Entry";
			gotoXY(20, ++cat); std::cout << "Back";
			directory.close();
		}
		gotoXY(18, x); std::cout << "->";

		std::system("pause>nul"); // the >nul bit causes it the print no message

		if (GetAsyncKeyState(VK_UP) && x != 11) //up button pressed
		{
			gotoXY(18, x); std::cout << "  ";
			x--;
			gotoXY(18, x); std::cout << "->";
			menu_item--;
			continue;
		}
		else if (GetAsyncKeyState(VK_DOWN) && x != cat) //down button pressed
		{
			gotoXY(18, x); std::cout << "  ";
			x++;
			gotoXY(18, x); std::cout << "->";
			menu_item++;
			continue;

		}

		if (GetAsyncKeyState(VK_RETURN)) { // Enter key pressed
			running = false;
			std::system("cls");

			if (x == cat) {
				adminAccess();
			}
			else if (x == cat - 1) {
				std::system("cls");
				ShowConsoleCursor(true);

				string newCat;
				fstream newEntry;
				string directory = ".\\entries\\" + category + "\\";
				newEntry.open(directory + "dir.txt", ios::app);

				gotoXY(20, 5); std::cout << "WARNING: You can't rename entries after making one";
				gotoXY(20, 7); std::cout << "Name of entry: "; getline(cin, newCat);

				string lineOne = newCat;

				newCat.erase(remove_if(newCat.begin(), newCat.end(), isspace), newCat.end());

				newEntry << endl << newCat;

				newEntry.close();

				fstream newEnt;
				
				newCat = newCat + ".txt";
				string createFile = directory + newCat;
				
				newEnt.open(createFile, ofstream::out);

				newEnt << lineOne;

				newEnt.close();

				ShowConsoleCursor(false);
				textEdit(directory, newCat);
			}
			else {
				std::system("cls");
				fstream selection(".\\entries\\" + category + "\\dir.txt");
				string thing;
				for (int i = 0; i < menu_item + 1; i++)
				{
					getline(selection, thing);
				}
				selection.close();
				thing.erase(remove_if(thing.begin(), thing.end(), isspace), thing.end());
				//textEdit(".\\entries\\" + category + "\\", thing + ".txt");
				modifyEntry(menu_item, category);
			}

		}

	}
}

void modifyCategory(int menu_item) {
	string thing;
	fstream selection(".\\entries\\dir.txt");

	hOut = GetStdHandle(STD_OUTPUT_HANDLE);

	GetConsoleScreenBufferInfo(hOut, &SBInfo);
	NewSBSize.X = SBInfo.dwSize.X;
	NewSBSize.Y = SBInfo.dwMaximumWindowSize.Y; //37

	Status = SetConsoleScreenBufferSize(hOut, NewSBSize);
	if (Status == 0)
	{
		Status = GetLastError();
		std::cout << "SetConsoleScreenBufferSize() failed! Reason : " << Status << endl;
		exit(Status);
	}

	for (int i = 0; i < menu_item + 1; i++)
	{
		getline(selection, thing);
	}
	selection.close();

	OPTIONS:
	int option = 0, x = 7;
	bool running = true;

	while (running)
	{
		gotoXY(20, 5); std::cout << "What do you want to do with " << thing << "?";
		gotoXY(18, x); std::cout << "->";

		gotoXY(20, 7); std::cout << "Modify Contents";
		gotoXY(20, 8); std::cout << "Delete Category";
		gotoXY(20, 9); std::cout << "Back";

		std::system("pause>nul"); // the >nul bit causes it the print no message

		if (GetAsyncKeyState(VK_UP) && x != 7) //up button pressed
		{
			gotoXY(18, x); std::cout << "  ";
			x--;
			gotoXY(18, x); std::cout << "->";
			option--;
			continue;
		}
		else if (GetAsyncKeyState(VK_DOWN) && x != 9) //down button pressed
		{
			gotoXY(18, x); std::cout << "  ";
			x++;
			gotoXY(18, x); std::cout << "->";
			option++;
			continue;
		}

		else if (GetAsyncKeyState(VK_RETURN)) { // Enter key pressed
			bool running = false;
			if (option == 0) {
				thing.erase(remove_if(thing.begin(), thing.end(), isspace), thing.end());
				modifyContent(thing);
			}
			else if (option == 1) {
				std::system("cls");
				deleteCategory(thing, menu_item);
			}
			else if (option == 2) {
				adminAccess();
			}
		}
	}
}

void adminAccess() {
	std::system("cls");
	int menu_item = 1, x = 10, cat = 9;
	bool running = true;

	int number_of_lines = 0;

	hOut = GetStdHandle(STD_OUTPUT_HANDLE);

	GetConsoleScreenBufferInfo(hOut, &SBInfo);
	NewSBSize.X = SBInfo.dwSize.X;

	if (Status == 0)
	{
		Status = GetLastError();
		std::cout << "SetConsoleScreenBufferSize() failed! Reason : " << Status << endl;
		exit(Status);
	}

	ShowConsoleCursor(false);

	string output; // text to be displayed
	ifstream directory; // the file to be opened

	directory.open(".\\entries\\dir.txt");

	while (running)
	{
		title();
		gotoXY(18, x); std::cout << "->";
		if (directory.is_open()) {
			while (!directory.eof()) {
				getline(directory, output);
				gotoXY(20, cat++); std::cout << output;

				++number_of_lines;
				NewSBSize.Y = SBInfo.dwMaximumWindowSize.Y + (number_of_lines / 3); //37
				Status = SetConsoleScreenBufferSize(hOut, NewSBSize);
			}
			gotoXY(20, cat); std::cout << "Create New Category";
			gotoXY(20, ++cat); std::cout << "Exit";
			directory.close();
		}

		std::system("pause>nul"); 
		if (GetAsyncKeyState(VK_UP) && x != 10) //up button pressed
		{
			gotoXY(18, x); std::cout << "  ";
			x--;
			gotoXY(18, x); std::cout << "->";
			menu_item--;
			continue;
		}
		else if (GetAsyncKeyState(VK_DOWN) && x != cat) //down button pressed
		{
			gotoXY(18, x); std::cout << "  ";
			x++;
			gotoXY(18, x); std::cout << "->";
			menu_item++;
			continue;
		}

		if (GetAsyncKeyState(VK_RETURN)) { // Enter key pressed
			if (x == cat - 1) {
				running = false;
				std::system("cls");
				ShowConsoleCursor(true);

				string newCat;
				fstream newFolder;
				newFolder.open(".\\entries\\dir.txt", ios::app);

				gotoXY(20, 5); std::cout << "WARNING: You can't rename categories after making one";
				gotoXY(20, 7); std::cout << "Name of category: "; getline(cin, newCat);

				newFolder << endl << newCat;
				newFolder.close();

				newCat.erase(remove_if(newCat.begin(), newCat.end(), isspace), newCat.end());
				filesystem::create_directory(".\\entries\\" + newCat);

				fstream newDir;
				newDir.open(".\\entries\\" + newCat + "\\dir.txt", ios::out);
				newDir.close();

				ShowConsoleCursor(false);
				modifyContent(newCat);
				std::system("cls");
				break;
			}

			else if (x == cat) {
				running = false;
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
				std::system("cls");
				startScreen();
			}
			
			else {
				std::system("cls");
				running = false;
				modifyCategory(menu_item);
			}

		}

	}
}

void startScreen() {
	int menu_item = 0, x = 9;
	bool running = true;

	hOut = GetStdHandle(STD_OUTPUT_HANDLE);

	GetConsoleScreenBufferInfo(hOut, &SBInfo);
	NewSBSize.X = SBInfo.dwSize.X;
	NewSBSize.Y = SBInfo.dwMaximumWindowSize.Y; //37

	Status = SetConsoleScreenBufferSize(hOut, NewSBSize);
	if (Status == 0)
	{
		Status = GetLastError();
		std::cout << "SetConsoleScreenBufferSize() failed! Reason : " << Status << endl;
		exit(Status);
	}

	menu_item = 0;
	while (running)
	{
		title();
		gotoXY(20, 9);	 std::cout << "Browse the AlmaPedia";
		gotoXY(20, 10);  std::cout << "Admin Login";
		gotoXY(20, 11);  std::cout << "Exit";
		gotoXY(18, x);	 std::cout << "->";
		std::system("pause>nul"); // the >nul bit causes it the print no message

		if (GetAsyncKeyState(VK_DOWN) && x != 11) //down button pressed
		{
			gotoXY(18, x); std::cout << "  ";
			x++;
			gotoXY(18, x); std::cout << "->";
			menu_item++;
			continue;

		}

		if (GetAsyncKeyState(VK_UP) && x != 9) //up button pressed
		{
			gotoXY(18, x); std::cout << "  ";
			x--;
			gotoXY(18, x); std::cout << "->";
			menu_item--;
			continue;
		}

		if (GetAsyncKeyState(VK_RETURN)) { // Enter key pressed

			switch (menu_item) {

			case 0: {

				encyclopedia();
				running = false;
				break;
			}


			case 1: {
				loginScreen();
				running = false;
				break;
			}

			case 2: {
				exitApp = true;
				running = false;
				_Exit(0);
				break;
			}

			}

		}

	}
}
 
void entry(string directory, string entry) {
	string output; // text to be displayed
	ifstream encyclopedia; // the file to be opened

	string page = ".\\entries\\" + directory + "\\" + entry + ".txt";
	encyclopedia.open(page);

	showScrollBar();

	int number_of_lines = 0;

	hOut = GetStdHandle(STD_OUTPUT_HANDLE);

	GetConsoleScreenBufferInfo(hOut, &SBInfo);
	NewSBSize.X = SBInfo.dwSize.X;
	
	if (Status == 0)
	{
		Status = GetLastError();
		std::cout << "SetConsoleScreenBufferSize() failed! Reason : " << Status << endl;
		exit(Status);
	}

	// reads the directory
	if (encyclopedia.is_open()) {
		while (!encyclopedia.eof()) {
			getline(encyclopedia, output);
			std::cout << output << endl;

			++number_of_lines;
			NewSBSize.Y = SBInfo.dwMaximumWindowSize.Y + number_of_lines; //37
			Status = SetConsoleScreenBufferSize(hOut, NewSBSize);
		}
		encyclopedia.close();
		std::cout << "\n\n\n...Press any key to go back...";
	}

	else {
		gotoXY(20, 25); cerr << "Entry does not exist\n\nPress any key to continue...";
	}

	_getch();
	categories(directory);
}

void categories(string category) {
	std::system("cls");
	int menu_item = 1, x = 11, cat = 10;
	bool running = true;

	int number_of_lines = 0;

	hOut = GetStdHandle(STD_OUTPUT_HANDLE);

	GetConsoleScreenBufferInfo(hOut, &SBInfo);
	NewSBSize.X = SBInfo.dwSize.X;

	if (Status == 0)
	{
		Status = GetLastError();
		std::cout << "SetConsoleScreenBufferSize() failed! Reason : " << Status << endl;
		exit(Status);
	}

	gotoXY(20, 9); cout << category;
	title();
	gotoXY(18, x); std::cout << "->";

	string output; // text to be displayed
	ifstream directory; // the file to be opened

	category.erase(remove_if(category.begin(), category.end(), isspace), category.end());
	directory.open(".\\entries\\" + category + "\\dir.txt");

	// reads the directory
	if (directory.is_open()) {
		while (!directory.eof()) {
			getline(directory, output);
			gotoXY(20, cat++); std::cout << output;

			++number_of_lines;
			NewSBSize.Y = SBInfo.dwMaximumWindowSize.Y + (number_of_lines / 3); //37
			Status = SetConsoleScreenBufferSize(hOut, NewSBSize);
		}
		gotoXY(20, cat); std::cout << "Back";
		directory.close();
	}

	while (running)
	{

		std::system("pause>nul"); // the >nul bit causes it the print no message

		if (GetAsyncKeyState(VK_UP) && x != 11) //up button pressed
		{
			gotoXY(18, x); std::cout << "  ";
			x--;
			gotoXY(18, x); std::cout << "->";
			menu_item--;
			continue;
		}
		else if (GetAsyncKeyState(VK_DOWN) && x != cat) //down button pressed
		{
			gotoXY(18, x); std::cout << "  ";
			x++;
			gotoXY(18, x); std::cout << "->";
			menu_item++;
			continue;

		}

		if (GetAsyncKeyState(VK_RETURN)) { // Enter key pressed
			running = false;
			std::system("cls");

			if (x == cat) {
				encyclopedia();
			}
			else {
				string thing;
				ifstream selection(".\\entries\\" + category + "\\dir.txt");

				for (int i = 0; i < menu_item + 1; i++)
				{
					getline(selection, thing);
				}

				thing.erase(remove_if(thing.begin(), thing.end(), isspace), thing.end());
				entry(category, thing);
			}

		}

	}
}

void encyclopedia() {
	std::system("cls");
	int menu_item = 1, x = 10, cat = 9;
	bool running = true;int number_of_lines = 0;

	hOut = GetStdHandle(STD_OUTPUT_HANDLE);

	GetConsoleScreenBufferInfo(hOut, &SBInfo);
	NewSBSize.X = SBInfo.dwSize.X;

	if (Status == 0)
	{
		Status = GetLastError();
		std::cout << "SetConsoleScreenBufferSize() failed! Reason : " << Status << endl;
		exit(Status);
	}

	title();
	gotoXY(18, x); std::cout << "->";

	string output; // text to be displayed
	ifstream directory; // the file to be opened

	directory.open(".\\entries\\dir.txt");

	// reads the directory
	if (directory.is_open()) {
		while (!directory.eof()) {
			getline(directory, output);
			gotoXY(20, cat++); std::cout << output;
			++number_of_lines;
			NewSBSize.Y = SBInfo.dwMaximumWindowSize.Y + (number_of_lines/3); //37
			Status = SetConsoleScreenBufferSize(hOut, NewSBSize);
		}
		gotoXY(20, cat); std::cout << "Exit";
		directory.close();
	}

	while (running)
	{

		std::system("pause>nul"); // the >nul bit causes it the print no message

		if (GetAsyncKeyState(VK_UP) && x != 10) //up button pressed
		{
			gotoXY(18, x); std::cout << "  ";
			x--;
			gotoXY(18, x); std::cout << "->";
			menu_item--;
			continue;
		}
		else if (GetAsyncKeyState(VK_DOWN) && x != cat) //down button pressed
		{
			gotoXY(18, x); std::cout << "  ";
			x++;
			gotoXY(18, x); std::cout << "->";
			menu_item++;
			continue;
		}

		if (GetAsyncKeyState(VK_RETURN)) { // Enter key pressed
			running = false;

			if (x == cat) {
				std::system("cls");
				startScreen();
			}
			else {
				string thing;
				ifstream selection(".\\entries\\dir.txt");

				for (int i = 0; i < menu_item + 1; i++)
				{
					getline(selection, thing);
				}

				categories(thing);
			}

		}

	}
}