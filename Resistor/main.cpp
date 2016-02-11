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
#include <fstream>
#include <string>
#include <exception>
#include <bitset>
#include <stdlib.h>

// Bitwise operations for extracting data from return stream
#define S1(k,n) ((k) & ((1<<(n))-1))
#define F10(k,m,n) S1((k)>>(m),((n)-(m)))

// Set global serial port handle
HANDLE Resipod;
DCB ResipodParam = { 0 };

// Not C++ recommended to make this global, but I am lazy
// and don't want to figure out how to return a byte array
byte receive[] = { 0x0,0x0,0x0 };

void GetResistance() {
	byte command[] = { 0xC1,0xD2,0x21 };
	DWORD bytesWrite = 0;
	DWORD bytesRead = 0;
	WriteFile(Resipod, command, 3, &bytesWrite, NULL);
	ReadFile(Resipod, receive, 3, &bytesRead, NULL);
	if (receive[0] != 0x02) {
		std::cout << "Received value does not contain correct start byte. Non-fatal error.\n";
	}
}

int InitializePort() {
	for (int j = 1; j <= 9; j++) {
		std::wstring comPortString(L"COM0");
		comPortString.replace(3, 1, std::to_wstring(j));
		const wchar_t* comPort = comPortString.c_str();
		std::cout << "Checking port ";
		std::wcout << comPortString;
		std::cout << "...";
		Resipod = CreateFile(comPort, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (Resipod == INVALID_HANDLE_VALUE) {
			std::cout << "invalid handle\n";
		}
		else {
			ResipodParam.DCBlength = sizeof(ResipodParam);

			// Hardcode serial parameters based on Resipod data
			GetCommState(Resipod, &ResipodParam);
			ResipodParam.BaudRate = CBR_19200;
			ResipodParam.ByteSize = 8;
			ResipodParam.StopBits = 1;
			ResipodParam.Parity = NOPARITY;

			// Set some timeout values
			COMMTIMEOUTS timeouts = { 0 };
			timeouts.ReadIntervalTimeout = 200;
			timeouts.ReadTotalTimeoutConstant = 0;
			timeouts.ReadTotalTimeoutMultiplier = 1;
			timeouts.WriteTotalTimeoutConstant = 2000;
			timeouts.WriteTotalTimeoutMultiplier = 10;

			// Get ID on port to see if it is a Proceq device
			byte getID[] = { 0x10,0x49,0x44,0x0D };
			char receiveID[8] = {0};
			char correctID[8] = "Resipod";
			DWORD bytesWriteID = 0;
			DWORD bytesReadID = 0;
			WriteFile(Resipod, getID, 5, &bytesWriteID, NULL);
			ReadFile(Resipod, receiveID, 8, &bytesReadID, NULL);
			std::cout << receiveID << "\n";
			if (strcmp(receiveID,correctID)) {
				return(j);
			}
		}
	}
	return(0);
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
	std::cout << "\n\n Current code is in testing phase. Do NOT use for research purposes...\n\n";
}

int main() {
	char TestInterface;
	char dummy;
	DisplayHeader();
	std::cout << "Initializing serial port interface...\n\n";
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
	}
	std::cout << "Current test program will obtain three readings at an interval of 3 seconds.\n";
	std::cout << "Press any key when ready to test interface...";
	std::cin >> TestInterface;

	for (int i = 1; i < 4; i++) {
		std::cout << "\n\n\nStarting measurement #" << i << "...\n\n";
		GetResistance();
		std::cout << "  Checking first byte received\n";
		std::cout << "--------------------------------\n";
		std::cout << std::bitset<8>(0x02) << "  - expected from device\n";
		std::cout << std::bitset<8>(receive[0]) << "  - received from device\n";
		std::cout << "--------------------------------\n";
		if (receive[0] = 0x02) {
			std::cout << "Bytes match, next values should be correct.\n";
		}
		else {
			std::cout << "Bytes do not match! Next values unlikely to be correct!\n";
		}
		unsigned output1 = (receive[2] << 8) | (receive[1]);
		std::cout << std::bitset<16>(output1) << "\n";
		int reading = F10(output1, 0, 11);
		double resistance = reading;
		bool divide10 = F10(output1, 11, 12);
		if (divide10) {
			resistance = reading / 10.0;
		}

		std::cout << "Decimal number: " << reading << "\n";
		std::cout << "Binary number: " << std::bitset<16>(reading) << "\n";
		std::cout << "Resistance: " << resistance << " ohms\n";
		std::cin >> reading;
	}


}