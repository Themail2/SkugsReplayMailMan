#include <iostream>
#include <Windows.h>
#include <Psapi.h>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <filesystem>
#include <vector>
namespace fs = std::filesystem;
using namespace std;

DWORD GetBaseAddress(const HANDLE hProcess) {
	if (hProcess == NULL)
		return NULL; // No access to the process

	HMODULE lphModule[1024]; // Array that receives the list of module handles
	DWORD lpcbNeeded(NULL); // Output of EnumProcessModules, giving the number of bytes requires to store all modules handles in the lphModule array

	if (!EnumProcessModules(hProcess, lphModule, sizeof(lphModule), &lpcbNeeded))
		return NULL; // Impossible to read modules

	TCHAR szModName[MAX_PATH];
	if (!GetModuleFileNameEx(hProcess, lphModule[0], szModName, sizeof(szModName) / sizeof(TCHAR)))
		return NULL; // Impossible to get module info

	return (DWORD)lphModule[0]; // Module 0 is apparently always the EXE itself, returning its address
}
bool isRunning(HWND hwnd)
{
	if (hwnd != 0) {
		return true;
	}
	else {
		return false;
	}
}

inline bool exists_test0(const std::string& name) {
	ifstream f(name.c_str());
	return f.good();
}
// Checks the current list of replays on file against the replay directory for new entries
vector<string> checkForNewReplays(vector<string> replays, string filePath) {
	cout << "checkreplay function call" << endl;
	vector<string> directoryContents;
	vector<string> x;
	// Populate directoryContents with the current contents of the replay directory
	for (const auto& entry : filesystem::directory_iterator(filePath))
		directoryContents.push_back(entry.path().string());
	// If this is true, we know there are new replays in the file system
	if ((ssize(replays) + 2) == ssize(directoryContents))
	{
		// Replay files always come in pairs, so we grab the two newest ones and return their paths
		cout << "here" << endl;
		x.push_back(directoryContents[ssize(replays) ]);
		x.push_back(directoryContents[ssize(replays) + 1]);
		cout << "done" << endl;
		return x;
	}
	return x;
}
int main()
{
	/* 
	Firstly, check if the config file exists, if it does not, we know the user has not configured the client yet.
	Thus we need to guide the user through configuring the client. 
	We need to grab the users skullgirls directory and the users replay file directory.
	"C:\Program Files (x86)\Steam\steamapps\common\Skullgirls"
	Setup steam SDK support??
	*/
	//Declare some variables for use
	int numRounds = NULL;
	string configData;
	ofstream writeConfigFile;
	ifstream readConfigFile;
	string tempString;
	vector<string> replaysOnFile;
	// Check if config file exists in the working directory
	if (exists_test0("config.cfg"))
	{
		// Open that bitch, see if its got shit in it
		readConfigFile.open("config.cfg", std::ios::in | std::ios::out);
		writeConfigFile.open("config.cfg", std::ios::in | std::ios::out);
		if (readConfigFile && writeConfigFile) 
		{
			readConfigFile.seekg(0, ios_base::beg);
			readConfigFile >> configData;
			// No shit? gotta add shit
			if (configData == "")
			{
				cout << "Error, No config data found" << endl;
				
				cout << "Hello, welcome to initial setup for the skullgirls replay catalog client!" << endl;
				cout << "I'm just going to need the path to the folder where your skullgirls replays live," << endl;
				cout << "To do this, simply navigate to where they are in your file explorer." << endl;
				cout << "Right click on the folder, and left click on 'Copy as Path', then, move" << endl;
				cout << "back to this window and right click on it." << endl;
				cout << "It's typically in Documents\\SkullGirls\\Replays_SG2EPlus\\*bunchOfNumbers*" << endl << endl;
				cout << "Please put the path to your skullgirls replay folder in now..." << endl;

				getline(cin, tempString);

				tempString.erase(remove(tempString.begin(), tempString.end(), '\"'), tempString.end());
				cout << tempString;
				
				writeConfigFile << tempString;
				cout << endl;
				writeConfigFile.close();
				readConfigFile.close();
			}
			else
			{
				cout << "configData.cfg found!" << endl << endl;
				
			}
		}
	}
	else
	{
		// stupid user doesnt have a config.cfg file.....
		// guess I gotta make one
		cout << "bad" << endl;
		ofstream newConfigFile("config.cfg");
		newConfigFile.close();
		writeConfigFile.open("config.cfg", std::ios::in | std::ios::out);
		readConfigFile.open("config.cfg", std::ios::in | std::ios::out);
		cout << "Error, No config data found" << endl << endl;

		cout << "Hello, welcome to initial setup for the skullgirls replay catalog client!" << endl;
		cout << "I'm just going to need the path to the folder where your skullgirls replays live," << endl;
		cout << "To do this, simply navigate to where they are in your file explorer." << endl;
		cout << "Right click on the folder, and left click on 'Copy as Path', then, move" << endl;
		cout << "back to this window and right click anywhere in it." << endl;
		cout << "It's typically in Documents\\SkullGirls\\Replays_SG2EPlus\\*bunchOfNumbers*" << endl << endl;
		cout << "Please put the path to your skullgirls replay folder in now..." << endl;

		getline(cin, tempString);
		tempString.erase(remove(tempString.begin(), tempString.end(), '\"'), tempString.end());
		cout << tempString;

		writeConfigFile << tempString;
		cout << endl;
		writeConfigFile.close();
		readConfigFile.close();
	}

	if (exists_test0("replayList.xd"))
	{
		//if we have one already, reset the list to the current working directory contents
		cout << "replayList.xd found!" << endl;
		cout << "Verifying integrity of replaylist.xd" << endl << endl;
		ofstream newFile("replayList.xd");
		if (newFile.is_open()) 
		{
			
			for (const auto& entry : filesystem::directory_iterator(configData))
				newFile << entry.path().string() << endl;
			newFile.close();
		}
		ifstream file("replayList.xd");
		if (file.is_open())
		{
			string line;
			while (getline(file, line))
			{
				// using printf() in all tests for consistency
				replaysOnFile.push_back(line.c_str());
			}
			file.close();
		}

		cout << "Found: " << replaysOnFile.size() / 2 << " replays in " << configData << endl;
	}
	else
	{
		//Idiot user doesn't have a replayList.xd, guess we gotta make one
		//Grab all files in path and add them to the replayList.xd
		ofstream newReplayList("replayList.xd");
		cout << "Did not find replayList.xd,\nPopulating replayList.xd" << endl;
		for (const auto& entry : filesystem::directory_iterator(configData))
			newReplayList << entry.path().string() << endl;
		newReplayList.close();
		
		ifstream file("replayList.xd");
		if (file.is_open())
		{
			string line;
			while (getline(file, line))
			{
				// using printf() in all tests for consistency
				replaysOnFile.push_back(line.c_str());
			}
			file.close();
		}

		cout << "Found: " << replaysOnFile.size() / 2 << " replays in " << configData << endl;
	}
	char esc_char = 27; // the decimal code for escape character is 27
	cout << esc_char << "[1m" << "Launch Skullgirls now? Y/N?" << esc_char << "[0m" << endl;
	getline(cin, tempString);
	// Once we've established a proper config file and replaylist, launch skugs and start LOOKIN
	if (tempString == "y" || tempString == "Y")
	{
		ShellExecute(NULL, "open", R"(SkullGirls.exe)", NULL, NULL, SW_SHOWDEFAULT);
	}
	else if (tempString == "n" || tempString == "N")
	{
		cout << "Goodbye.";
		return 0;
	}
	else
	{
		cout << "Incorrect input, goodbye.";
		return 0;
	}
	// Gotta wait for that bitch to INITIALIZE
	Sleep(10000);
	int readP2Win = 0;
	int readP1Win = 0;
	HWND hwnd = FindWindowA(NULL, "Skullgirls Encore");
	if (hwnd == NULL)
	{
		cout << "Cannot Find Skullgirls. Check to make sure this .exe is in the Skullgirls folder within steam." << endl;
		cout << "Closing the exectuable...";
		Sleep(3000);
		return 0;
	}
	else
	{
		DWORD procID;
		GetWindowThreadProcessId(hwnd, &procID);
		HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);

		DWORD AddressOfFirstPointer, ValueAtFirstPointer, AddressOfP1Win, AddressOfP2Win, ValueOfP2Win, ValueOfP1Win;
		DWORD P1WinOffset = 0x434;
		DWORD P2WinOffset = 0x43C;
		DWORD BaseAddress = GetBaseAddress(handle);
		DWORD BaseAddressOffset = 0x0080B3B4;
		
		//Uncomment the print statements for de-bugging reasons!

		//cout << "The Base Address of SkullGirls is: " << std::hex << BaseAddress << endl;

		AddressOfFirstPointer = BaseAddress + BaseAddressOffset;

		//cout << "The Address of The Static Pointer To UI CLass Object is: " << std::hex << AddressOfFirstPointer << endl;

		ReadProcessMemory(handle, (DWORD*)AddressOfFirstPointer, &ValueAtFirstPointer, sizeof(int), 0);

		//cout << "The Value of The Static Pointer To UI CLass Object is: " << std::hex << ValueAtFirstPointer << endl;

		AddressOfP1Win = ValueAtFirstPointer + P1WinOffset;

		//cout << "The Address of The Player 1 Win Bool is: " << std::hex << AddressOfP1Win << endl;

		ReadProcessMemory(handle, (DWORD*)AddressOfP1Win, &ValueOfP1Win, sizeof(int), 0);

		//cout << "The Value of The Player 1 Win Bool is: " << std::hex << ValueOfP1Win << endl;

		AddressOfP2Win = ValueAtFirstPointer + P2WinOffset;

		//cout << "The Address of The Player 2 Win Bool is: " << std::hex << AddressOfP2Win << endl;

		ReadProcessMemory(handle, (DWORD*)AddressOfP2Win, &ValueOfP2Win, sizeof(int), 0);

		//cout << "The Value of The Player 2 Win Bool is: " << std::hex << ValueOfP2Win << endl;

		if (procID == NULL)
		{
			cout << "Cannot Find Skullgirls Proccess ID" << endl;
			Sleep(3000);
			return 0;
		}
		else
		{
			cout << "Scanning for new replays..." << endl;
			int p1WinTemp = 0;
			int p2WinTemp = 0;
			vector<string> newReplays;
			string currentWinner;
			while (true) 
			{
				Sleep(500);
				// Check win ints in mem
				ReadProcessMemory(handle, (PBYTE*)AddressOfP1Win, &readP1Win, sizeof(int), 0);
				ReadProcessMemory(handle, (PBYTE*)AddressOfP2Win, &readP2Win, sizeof(int), 0);
				// if the win ints are zero we need to keep checking for them to not be zero
				if (readP1Win == 0 && readP2Win == 0) 
				{
					cout << "P1 win int " << readP1Win << " P2 win int " << readP2Win << endl;
					while (true) 
					{
						// Once per second we need to check if the win ints are not zero
						Sleep(1000);
						ReadProcessMemory(handle, (PBYTE*)AddressOfP1Win, &readP1Win, sizeof(int), 0);
						ReadProcessMemory(handle, (PBYTE*)AddressOfP2Win, &readP2Win, sizeof(int), 0);
						p1WinTemp = 0;
						p2WinTemp = 0;
						// Checks to make sure the game hasn't crashed or closed
						if (isRunning(FindWindowA(NULL, "Skullgirls Encore")))
						{
							// If the win ints were zero and are now not zero 
							if (readP1Win == 1 || readP2Win == 1)
							{
								cout << "P1 win int " << readP1Win << " P2 win int " << readP2Win << endl;
								// Enter this loop and check to make see when they return to zero
								while (true)
								{
									if (isRunning(FindWindowA(NULL, "Skullgirls Encore")))
									{
										Sleep(1000);
										ReadProcessMemory(handle, (PBYTE*)AddressOfP1Win, &readP1Win, sizeof(int), 0);
										ReadProcessMemory(handle, (PBYTE*)AddressOfP2Win, &readP2Win, sizeof(int), 0);
										if (readP1Win == 0 && readP1Win == 0)
										{
											if (p1WinTemp > p2WinTemp)
											{
												currentWinner = "Player 1";
											}
											else
											{
												currentWinner = "Player 2";
											}
											newReplays = checkForNewReplays(replaysOnFile, configData);
											if (newReplays.size() == 2)
											{
												// Package request to backend
												replaysOnFile.push_back(newReplays[0]);
												replaysOnFile.push_back(newReplays[1]);
												cout << newReplays[0] << '\n' << newReplays[1] << '\n' << currentWinner << endl;

											}
											else
											{
												cout << "No new replays found" << endl;
											}
											break;
										}
										p1WinTemp = readP1Win;
										p2WinTemp = readP2Win;
									}
								}
							}
						}
						else
						{
							cout << "Game Crash!\nPlease Restart this program" << endl;
							Sleep(5000);
							return 0;
						}
					}
				}
			}
			return 0;
			}
		}
	}