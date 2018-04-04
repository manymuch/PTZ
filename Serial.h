// Serial.h
#include "windows.h"
#ifndef __SERIAL_H__
#define __SERIAL_H__

#define FC_DTRDSR       0x01
#define FC_RTSCTS       0x02
#define FC_XONXOFF      0x04
#define ASCII_BEL       0x07
#define ASCII_BS        0x08
#define ASCII_LF        0x0A
#define ASCII_CR        0x0D
#define ASCII_XON       0x11
#define ASCII_XOFF      0x13
#define PtzAdress		0x01//when using multiple vedio camera, this address is used to figure out which camera to control
#define panmaxdegree	170//
#define panmindegree	-170
#define panmaxhex		0x1CE8//hex coed correspond to the pan max degree
#define panminhex		0xE317
#define tiltmaxdegree	90
#define tiltmindegree	-30
#define tiltmaxhex		0xE94
#define tiltminhex		0xFCFF

class CSerial
{

public:
	CSerial();
	~CSerial();

	BOOL Open( int nPort = 2, int nBaud = 9600 );
	BOOL Close( void );

	int ReadData( void *, int );
	int SendData( unsigned char *, int );
	int ReadDataWaiting( void );

	BOOL IsOpened( void ){ return( m_bOpened ); }

protected:
	BOOL WriteCommByte( unsigned char );
	HANDLE m_hIDComDev;
	OVERLAPPED m_OverlappedRead, m_OverlappedWrite;
	BOOL m_bOpened;

};
class Pantilt:public CSerial {
public:
	Pantilt();
	~Pantilt();


	//Position return: -4:completion error -3:ACK error   -2:Convert error  -1:pan or tilt input out range 0:SendCommand error
	int Position(float pan,float tilt,bool ifabsolute=true,int panspeed=0x18,int tiltspeed=0x17);
	
	int Move(int way,int panspeed = 0x18, int tiltspeed = 0x17);

	//PosInq return: -1:SendInqCommand error -2:inquary failed -3:read data out range
	int PosInq();

	//toggle ICR mode
	bool Infrared(bool infrared_on);
	int Reset();
	float pospan, postilt;
private:

	int SendCommand();
	int Read();//read one message end with 0xFF,write to readbuffer

	//convert the degree to 32bit hex and seperate them in to 4 8bit unit,put them into uc_positiontemp[4]
	bool Convert(float degree,bool ispan);
	float InvConvert(bool ispan);//return 361 read data out range

	unsigned char uc_buffer[1];
	unsigned char uc_readbuffer[16];
	
	unsigned char uc_hex[16];//for data to be sent
	int i;//for counting
	unsigned char uc_positiontemp[4];//written by convert function,read by position function
};
#endif
