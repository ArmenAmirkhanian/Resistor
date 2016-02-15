//   /$$$$$$$  /$$$$$$$$  /$$$$$$  /$$$$$$  /$$$$$$  /$$$$$$$$ /$$$$$$  /$$$$$$$ 
//  | $$__  $$| $$_____/ /$$__  $$|_  $$_/ /$$__  $$|__  $$__//$$__  $$| $$__  $$
//  | $$  \ $$| $$      | $$  \__/  | $$  | $$  \__/   | $$  | $$  \ $$| $$  \ $$
//  | $$$$$$$/| $$$$$   |  $$$$$$   | $$  |  $$$$$$    | $$  | $$  | $$| $$$$$$$/
//  | $$__  $$| $$__/    \____  $$  | $$   \____  $$   | $$  | $$  | $$| $$__  $$
//  | $$  \ $$| $$       /$$  \ $$  | $$   /$$  \ $$   | $$  | $$  | $$| $$  \ $$
//  | $$  | $$| $$$$$$$$|  $$$$$$/ /$$$$$$|  $$$$$$/   | $$  |  $$$$$$/| $$  | $$
//  |__/  |__/|________/ \______/ |______/ \______/    |__/   \______/ |__/  |__/



#include <Windows.h>
#include <iostream>
#include <string>
#include <sstream>
#include <exception>
#include <bitset>
#include <stdlib.h>
#include <fstream>
#include <chrono>
#include <ctime>

// Bitwise operations for extracting data from return stream
#define S1(k,n) ((k) & ((1<<(n))-1))
#define F10(k,m,n) S1((k)>>(m),((n)-(m)))

// Set global serial port handle
HANDLE Resipod;
DCB ResipodParam = { 0 };

// Get timer ready
HANDLE timer = NULL;
LARGE_INTEGER timerValue;

double GetResistance() {
	byte command[] = { 0xC1,0xD2,0x21 };
	byte receive[] = { 0x0, 0x0, 0x0 };
	DWORD bytesWrite = 0;
	DWORD bytesRead = 0;
	WriteFile(Resipod, command, 3, &bytesWrite, NULL);
	ReadFile(Resipod, receive, 3, &bytesRead, NULL);
	if (receive[0] != 0x02) {
		return(-1.0);
	}

	unsigned output1 = (receive[2] << 8) | (receive[1]);
	int reading = F10(output1, 0, 11);
	double resistance = reading;
	bool divide10 = F10(output1, 11, 12);
	if (divide10) {
		resistance = reading / 10.0;
	}

	return(resistance);
}

int InitializePort() {
	int userComPort = 3;
	std::cout << "Enter COM port number that Resipod is connected to: ";
	std::cin >> userComPort;
	std::wstringstream comPort;
	comPort << "\\\\.\\COM" << userComPort;

	std::cout << "\nChecking port COM" << userComPort << "...";

	Resipod = CreateFile(comPort.str().c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (Resipod == INVALID_HANDLE_VALUE) {
		DWORD dwError = GetLastError();
		if (dwError == ERROR_ACCESS_DENIED || dwError == ERROR_GEN_FAILURE || dwError == ERROR_SHARING_VIOLATION || dwError == ERROR_SEM_TIMEOUT) {
			std::cout << "resource in use and not usable\n";
		}
		else {
			std::cout << "error opening port\n";
		}
		return(0);
	}
	else {
		std::cout << "success!\n";
		ResipodParam.DCBlength = sizeof(ResipodParam);

		// Hardcode serial parameters based on Resipod data
			
		ResipodParam.BaudRate = CBR_19200;
		ResipodParam.ByteSize = 8;
		ResipodParam.StopBits = 1;
		ResipodParam.Parity = NOPARITY;
		ResipodParam.fOutX = FALSE;
		ResipodParam.fInX = FALSE;
		ResipodParam.fTXContinueOnXoff = FALSE;
		SetCommState(Resipod, &ResipodParam);

		// Set some timeout values
		COMMTIMEOUTS timeouts = { 0 };
		timeouts.ReadIntervalTimeout = 200;
		timeouts.ReadTotalTimeoutConstant = 1000;
		timeouts.ReadTotalTimeoutMultiplier = 1;
		timeouts.WriteTotalTimeoutConstant = 200;
		timeouts.WriteTotalTimeoutMultiplier = 1;

		// Get ID on port to see if it is a Proceq device
		/*byte command[] = { 0xC1,0xD2,0x21 };
		byte getID[] = { 0x10,0x49,0x44,0x0D };
		char receiveID[8] = {0};
		char correctID[8] = "Resipod";
		DWORD bytesWriteID = 0;
		DWORD bytesReadID = 0;
		WriteFile(Resipod, command, 3, &bytesWriteID, NULL);
		ReadFile(Resipod, receiveID, 3, &bytesReadID, NULL);
		WriteFile(Resipod, getID, 5, &bytesWriteID, NULL);
		ReadFile(Resipod, receiveID, 8, &bytesReadID, NULL);
		if (SetCommTimeouts(Resipod, &timeouts) == 0) {
			std::cout << "timeout\n";
		}
		else {
			std::cout << receiveID << "\n";
			if (strcmp(receiveID, correctID)) {
				return(j);
			}
		}*/
			
	}
	
	return(userComPort);
}

void DisplayHeader() {
	std::cout << "   /$$$$$$$  /$$$$$$$$  /$$$$$$  /$$$$$$  /$$$$$$  /$$$$$$$$ /$$$$$$  /$$$$$$$\n";
	std::cout << "  | $$__  $$| $$_____/ /$$__  $$|_  $$_/ /$$__  $$|__  $$__//$$__  $$| $$__  $$\n";
	std::cout << "  | $$  \\ $$| $$      | $$  \\__/  | $$  | $$  \\__/   | $$  | $$  \\ $$| $$  \\ $$\n";
	std::cout << "  | $$$$$$$/| $$$$$   |  $$$$$$   | $$  |  $$$$$$    | $$  | $$  | $$| $$$$$$$/\n";
	std::cout << "  | $$__  $$| $$__/    \\____  $$  | $$   \\____  $$   | $$  | $$  | $$| $$__  $$\n";
	std::cout << "  | $$  \\ $$| $$       /$$  \\ $$  | $$   /$$  \\ $$   | $$  | $$  | $$| $$  \\ $$\n";
	std::cout << "  | $$  | $$| $$$$$$$$|  $$$$$$/ /$$$$$$|  $$$$$$/   | $$  |  $$$$$$/| $$  | $$\n";
	std::cout << "  |__/  |__/|________/ \\______/ |______/ \\______/    |__/   \\______/ |__/  |__/\n";
	//std::cout << "\n\n    Current code is in testing phase. Do NOT use for research purposes...\n\n";
}

void DisplayMainMenu(){
	std::cout << "##################################################################################\n";
	std::cout << "#                                    Main Menu                                   #\n";
	std::cout << "#     1) Take single immediate reading                                           #\n";
	std::cout << "#     2) Take multiple evenly spaced readings for defined length of time (NO!)   #\n";
	std::cout << "#     3) Take multiple evenly spaced readings for undefined length of time       #\n";
	std::cout << "#     4) Exit                                                                    #\n";
	std::cout << "#                                                                                #\n";
	std::cout << "##################################################################################\n";
}

void UndefinedLengthTest(){
	system("cls");
	std::ofstream dataFile;
	std::string operatorName, projectName, specimenID, fileName;
	int lengthType, intervalType;
	double lengthTest, intervalTest;
	char dum;
	std::cout << "DEFINED LENGTH TEST SETUP\n";
	std::cout << "Data file will be stored in same directory as this executable.\nYou may add any extension to it.\nDate and time are automatically pulled from this computer and added to data file.\n";
	std::cout << "Filename: ";
	std::cin >> fileName;

	dataFile.open(fileName, std::ios::out);

	std::cout << "Operator Name: ";
	std::cin >> operatorName;
	dataFile << operatorName << std::endl;

	std::cout << "\nProject Name: ";
	std::cin >> projectName;
	dataFile << projectName << std::endl;

	std::cout << "\nSpecimen Name/ID: ";
	std::cin >> specimenID;
	dataFile << specimenID << std::endl;

	std::time_t curTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	dataFile << curTime << std::endl;
	dataFile << "Time (min), Resistance (kOhms)" << std::endl;

	// Future work for defined length test function
	/*std::cout << "\nWould you like to enter the length of testing in\n1) minutes\n2) hours\n3) days\n4) weeks\nChoice: ";
	std::cin >> lengthType;
	std::cout << "\nHow long should the test run ";
	switch (lengthType){
	case 1:
		std::cout << "(minutes): ";
		std::cin >> lengthTest;
		break;
	case 2:
		std::cout << "(hours): ";
		std::cin >> lengthTest;
		break;
	case 3:
		std::cout << "(days): ";
		std::cin >> lengthTest;
		break;
	case 4:
		std::cout << "(weeks): ";
		std::cin >> lengthTest;
		break;
	default:
		std::cout << "(INVALID CHOICE, RETURN TO MAIN MENU): ";
		std::cin >> lengthTest;
		return;
		break;
	}*/
	std::cout << "\nWould you like to enter the interval of sampling in\n1) hertz\n2) minutes\nChoice: ";
	std::cin >> intervalType;
	switch (intervalType){
	case 1:
		std::cout << "\nEnter sampling interval (Hz): ";
		std::cin >> intervalTest;
		intervalTest = 1.0 / intervalTest;
		break;
	case 2:
		std::cout << "\nEnter sampling interval (minutes): ";
		std::cin >> intervalTest;
		intervalTest = intervalTest * 60.0;
		break;
	default:
		std::cout << "\nINVALID CHOICE, RETURN TO MAIN MENU";
		std::cin >> intervalTest;
		return;
		break;
	}

	// Get timer value ready. Value is in 100 nanosecond intervals
	timerValue.QuadPart = intervalTest*1E7;

	std::cout << "\nPress any key to start test...";
	std::cin >> dum;

	bool stopRecording = false;
	int count = 0;
	timer = CreateWaitableTimer(NULL, TRUE, NULL);
	double tempResistance, tempTime;

	while (stopRecording == false){
		SetWaitableTimer(timer, &timerValue, 0, NULL, NULL, 0);
		tempResistance = GetResistance();
		tempTime = (double)(count*intervalTest) / 60;
		dataFile << tempTime << ", ";
		dataFile << tempResistance << std::endl;
		system("cls");
		std::cout << "CURRENT TEST STATUS\n";
		std::cout << tempTime << " minutes into testing.\n";
		std::cout << count + 1 << " data points saved so far.\n";
		std::cout << tempResistance << " kOhms (most current data point).\n\n";
		std::cout << "Hit (ESC) key to stop data recording. Program will stop after next data point is collected...";

		// Duct tape way to get a decent timer function
		if (WaitForSingleObject(timer, INFINITE) != WAIT_OBJECT_0)
			dataFile << "Timer function failed due to error code " << GetLastError();
		else{
			if (GetAsyncKeyState(VK_ESCAPE)){
				stopRecording = true;
			}
		}

	}

}

int main() {
	char TestInterface;
	char dummy;
	int choice;
	DisplayHeader();
	std::cout << "Initializing serial port interface...";
	int PortOpen = InitializePort();
	if (PortOpen = 0) {
		std::cout << "no Resipod device found!\n";
		std::cout << "Press any key to terminate program...";
		std::cin >> dummy;
		return(EXIT_FAILURE);
	}
	else {
		std::cout << "success!\n\n";
		std::cout << "Resipod device is on COM" << PortOpen << "\n";
		std::cout << "Press any key to continue...";
		std::cin >> dummy;
	}

	do{
		system("cls");
		DisplayHeader();
		DisplayMainMenu();
		char dum;
		std::cout << "\n\n\nEnter choice: ";
		std::cin >> choice;
		switch (choice){
		case 1:
			double resistance;
			std::cout << "\n\nMeasuring resistance...";
			resistance = GetResistance();
			if (resistance > 0){
				std::cout << "success!\n\nResistance measured: " << resistance << " kOhms\n";
				std::cout << "Press any key to return to main menu...";
				std::cin >> dum;
			}
			else{
				std::cout << "failure!\n\nReturn byte incorrect. Try again or check serial parameters...\n";
				std::cout << "Press any key to return to main menu...";
				std::cin >> dum;
			}
			break;
		case 2:
			//DefinedLengthTest();

			std::cout << "\nNOT PROGRAMMED YET, RETURNING TO MAIN MENU! PRESS ANY KEY...";
			std::cin >> dum;
			break;
		case 3:
			UndefinedLengthTest();
			break;
		case 4:
			break;
		default:
			break;
		}
	} while (choice != 4);
}