#include<iostream>
#include <chrono>
#include <unistd.h>
#include <ctime>
#include <windows.h>
#include <conio.h>
#include "crc32.h"
#define SIZE_B 6600
#define size_content 8
#define SIZE_1P 22
#define SIZE_CRC 18
#define SIZE 300
using namespace std::chrono;

struct packet
{
	uint8_t header;
	uint8_t seq;
	uint64_t time;
	uint8_t content[8];
	uint32_t crc;
}__attribute__((packed));

uint64_t micros();
int main()
{
	packet s;
	int snb=0;		 
	DCB dcbSerialParams = {0};
	COMMTIMEOUTS timeouts = {0};
	HANDLE hSerial1;
	hSerial1=CreateFile("\\COM3",GENERIC_WRITE, 0, NULL,OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
            
    if (hSerial1 == INVALID_HANDLE_VALUE)
	{
		std::cout<<"3error";
	    return 1;
	}
	std::cout<<'\n'<<"COM3 opened"<<'\n';
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	if (GetCommState(hSerial1, &dcbSerialParams) == 0)
	{
	    std::cout<< "Error getting device state\n";
	    CloseHandle(hSerial1);
	    return 1;
	}
	dcbSerialParams.BaudRate = 9600;
	dcbSerialParams.ByteSize = 8;
	dcbSerialParams.StopBits = ONESTOPBIT;
	dcbSerialParams.Parity = NOPARITY;
	if(SetCommState(hSerial1, &dcbSerialParams) == 0)
	{
	    fprintf(stderr, "Error setting device parameters\n");
	    CloseHandle(hSerial1);
	    return 1;
	}
	// Set COM port timeout settings
	timeouts.ReadIntervalTimeout = 50;
	timeouts.ReadTotalTimeoutConstant = 50;
	timeouts.ReadTotalTimeoutMultiplier = 10;
	timeouts.WriteTotalTimeoutConstant = 50;
	timeouts.WriteTotalTimeoutMultiplier = 10;
	if(SetCommTimeouts(hSerial1, &timeouts) == 0)
	{
	    fprintf(stderr, "Error setting timeouts\n");
	    CloseHandle(hSerial1);
	    return 1;
	}
	uint64_t EPTime=0;
	DWORD snbbytes = 0;
	int a=0;
	uint16_t counter=0;
	int v=0;
	uint8_t buffer1[SIZE_1P];
	uint8_t buf1[SIZE_CRC];
	int r;
	int size1=0;
	uint32_t crctemp;
	int i=0;

	DWORD m=0;
	while (!kbhit())
	{
		s.header=0x55;
		s.seq=v;
		s.time= micros();
		srand(i);
		i++;
		for(int j=0 ; j<8 ; j++)
			{	
				s.content[j] = rand()%9+1 ;	
			}
		memcpy (buffer1 , &s , sizeof(s));
		for(int xy=0 ; xy<SIZE_CRC ; xy++)
			{
				buf1[xy]=buffer1[xy];
			}
		s.crc=crc32(0 , buf1 , SIZE_CRC, 1);
		memcpy (buffer1 , &s , sizeof(s));
		v++;
		m=m+32;
		snb=snb+32;
//		std::cout<<m<<" ";
//		std::cout<<'\n'<<"time="<<s.time;
//		std::cout<<'\n'<<"crc="<<s.crc<<'\n';
//		for(DWORD g=0; g<SIZE_1P ; g++)
//		{
//			std::cout << static_cast<unsigned>(buffer1[g])<<'\t';
//				
//		}
//		std::cout<<'\n';
//
//		std::cout<<std::endl;
		if (!WriteFile(hSerial1, buffer1 ,SIZE_1P, &snbbytes, NULL))
		{
			std::cout<< "23Error\n";
		    CloseHandle(hSerial1);
		    std::cout<<"closing handle";
		    return 1;
		}  
		
	}
	
	//std::cout<<'\n'<< snb<<" bytes written \n";		
	//close serial port com3
	std::cout<< "Closing serial port...";
	if (CloseHandle(hSerial1) == 0)
	{
	    std::cout<<"1Error\n";
	 return 1;
	}

}
	
uint64_t micros()
	{
    uint64_t us = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch())
            .count();
    return us; 
}