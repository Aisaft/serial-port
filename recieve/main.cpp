#include <iostream>
#include <chrono>
#include <unistd.h>
#include <ctime>
#include <windows.h>
#include <conio.h>
#include "crc32.h"
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <stdio.h> 
#define size_content 8
#define SIZE_1P 100
#define SIZE_CRC 18
#define SIZE 900
using namespace std::chrono;

struct CrcPacket
{
	uint32_t crc2;
	uint64_t time2;
}__attribute__((packed));
uint64_t micros();
int check (uint8_t buffer[],int in);
int main()
{
	CrcPacket g;
	int PktNb;
	DCB dcbSerialParams = {0};
	COMMTIMEOUTS timeouts = {0};
	HANDLE hSerial;
	hSerial=CreateFile("\\COM4",GENERIC_READ, 0, NULL,OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
           
    if (hSerial == INVALID_HANDLE_VALUE)
	{
		fprintf(stderr, "45Error\n");
	    return 1;
	}
	
	//std::cout<<'\n'<<"COM4 opened";
	 dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	if (GetCommState(hSerial, &dcbSerialParams) == 0)
	{
	    fprintf(stderr, "Error getting device state\n");
	    CloseHandle(hSerial);
	    return 1;
	}
	
	dcbSerialParams.BaudRate = 9600;
	dcbSerialParams.ByteSize = 8;
	dcbSerialParams.StopBits = ONESTOPBIT;
	dcbSerialParams.Parity = NOPARITY;
	if(SetCommState(hSerial, &dcbSerialParams) == 0)
	{
	    fprintf(stderr, "Error setting device parameters\n");
	    CloseHandle(hSerial);
	    return 1;
	}
	
	// Set COM port timeout settings
	timeouts.ReadIntervalTimeout = 50;
	timeouts.ReadTotalTimeoutConstant = 50;
	timeouts.ReadTotalTimeoutMultiplier = 10;
	timeouts.WriteTotalTimeoutConstant = 50;
	timeouts.WriteTotalTimeoutMultiplier = 10;
	if(SetCommTimeouts(hSerial, &timeouts) == 0)
	{
	    fprintf(stderr, "Error setting timeouts\n");
	    CloseHandle(hSerial);
	    return 1;
	}
	std::cout<<'\n'<<"timeout and device parameter set"<<'\n';
	uint64_t time2;
	int ln=0;
	int lp=0;
	int lf=0;
	int le=0;
	uint8_t buffer[SIZE_1P];
	int x;
	int j;
	uint8_t ChkTime[8];
	uint8_t ChkCrc[4];
	uint32_t c;
	uint64_t t;
	DWORD nbbytes = 0;
	int yo=0;
	int e=0;
	uint8_t buf[SIZE_CRC];
	uint8_t d=0;
	uint32_t crc2;
	float delay;
	float SDelay=0;
	float ms;
	delay=0;
	int p=0;
	float AvDelay[2];
	int lm;	
	while(1)
	{
		lm++;
	if (ReadFile(hSerial, buffer, SIZE_1P , &nbbytes, NULL))
		{
			//std::cout<<"COM4 read"<<'\n';
			if (nbbytes!=1)
				{
					//std::cout << nbbytes << " bytes read" << '\n';	
				}
		}
			else
	          	{
	          		x = GetLastError();
	          		printf("<<< %d", x);	
				}
	yo=check(buffer , d);
    while(yo!=0)
		{
			yo=check(buffer , d);	
			d++;
			if(d>22)
			{
				d=0;
				break;
				std::cout<<"header of this packet isn't correct";
			}
		}
	time2=micros();
	PktNb++;
//	std::cout<<PktNb;
	std::cout<<'\n'<<"buffer="<<'\n';
	for(DWORD g=0; g<SIZE_1P ; g++)
		{
			std::cout << static_cast<unsigned>(buffer[g])<<'\t';	
		}
		std::cout<<'\n';

		for(int i=0 ; i<4 ; i++)
			{
				ChkCrc[i]=buffer[i+18];
			}

		memcpy (&g.crc2 , ChkCrc ,sizeof(ChkCrc)); 

			c=g.crc2;	

		//std::cout<<'\n'<<"crc= "<<c[lm];	
		for(int f=0 ; f<8 ; f++)
			{
				ChkTime[f]=buffer[f+2];
			}
		memcpy (&g.time2 , ChkTime , sizeof(ChkTime));
		t=g.time2;	
		//std::cout<<'\n'<<"Time= "<<t[lm];
		//std::cout<<'\n'<<"recieved time = "<<time2<<'\n';
		for(int ij=0 ; ij<SIZE_CRC ; ij++)
			{
				buf[ij]=buffer[ij];
			}
		crc2=crc32(0, buf , SIZE_CRC, 1);			
		//std::cout<<"recieved crc= "<<crc2<<'\n';
		
		delay=time2 - t;
		//std::cout<<"delay = "<<delay<<'\n';
		p++;

//		if(crc2 != c[lm])
//			{
//				
//				std::cout<<"data didn't recieve correctly"<<'\n'<<"crc="<<crc2/c[lm]<<'\n';
//			}
//		
//		else
//		{
//			std::cout<<"data recieved correctly"<<'\n';
//		}
		d=0;
		lp=lm;
		if(nbbytes!=0)
		{
			SDelay=SDelay+delay;
			if(lm%100==0)
			{
				AvDelay[1]=SDelay/100.0;
				AvDelay[0]=(AvDelay[1]+AvDelay[0])/2.0;
				SDelay=0;
				ms=AvDelay[0]/1000.0;
				std::cout<<'\n'<<"average="<<static_cast<float>(ms)<<" ms"<<'\n';
			}
		}
}
			
		// Close serial port com4
		if (CloseHandle(hSerial) == 0)
		{
	    fprintf(stderr, "Error\n");
	    return 1;
		}
}

int check (uint8_t buffer[],int in)
	{
		if(buffer[in]==0x55)
		{
		return 0;
		}
		else
		{
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
