// Serial.cpp

#include "stdafx.h" 
#include "Serial.h"
#include <string>



CSerial::CSerial()
{

	memset( &m_OverlappedRead, 0, sizeof( OVERLAPPED ) );
 	memset( &m_OverlappedWrite, 0, sizeof( OVERLAPPED ) );
	m_hIDComDev = NULL;
	m_bOpened = FALSE;

}

CSerial::~CSerial()
{

	Close();

}

BOOL CSerial::Open( int nPort, int nBaud)
{

	if( m_bOpened ) return( TRUE );

	char szPort[15];
	char szComParams[50];
	DCB dcb;

	wsprintf( szPort, "COM%d", nPort );
	m_hIDComDev = CreateFile( szPort, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL );
	if( m_hIDComDev == NULL ) return( FALSE );

	memset( &m_OverlappedRead, 0, sizeof( OVERLAPPED ) );
 	memset( &m_OverlappedWrite, 0, sizeof( OVERLAPPED ) );

	COMMTIMEOUTS CommTimeOuts;
	CommTimeOuts.ReadIntervalTimeout = 0xFFFFFFFF;
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0;
	CommTimeOuts.ReadTotalTimeoutConstant = 0;
	CommTimeOuts.WriteTotalTimeoutMultiplier = 0;
	CommTimeOuts.WriteTotalTimeoutConstant = 5000;
	SetCommTimeouts( m_hIDComDev, &CommTimeOuts );

	wsprintf( szComParams, "COM%d:%d,n,8,1", nPort, nBaud );

	m_OverlappedRead.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
	m_OverlappedWrite.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );

	dcb.DCBlength = sizeof( DCB );
	GetCommState( m_hIDComDev, &dcb );
	dcb.BaudRate = nBaud;
	dcb.ByteSize = 8;
	unsigned char ucSet;
	ucSet = (unsigned char) ( ( FC_RTSCTS & FC_DTRDSR ) != 0 );
	ucSet = (unsigned char) ( ( FC_RTSCTS & FC_RTSCTS ) != 0 );
	ucSet = (unsigned char) ( ( FC_RTSCTS & FC_XONXOFF ) != 0 );
	if( !SetCommState( m_hIDComDev, &dcb ) ||
		!SetupComm( m_hIDComDev, 10000, 10000 ) ||
		m_OverlappedRead.hEvent == NULL ||
		m_OverlappedWrite.hEvent == NULL ){
		DWORD dwError = GetLastError();
		if( m_OverlappedRead.hEvent != NULL ) CloseHandle( m_OverlappedRead.hEvent );
		if( m_OverlappedWrite.hEvent != NULL ) CloseHandle( m_OverlappedWrite.hEvent );
		CloseHandle( m_hIDComDev );
		return( FALSE );
		}

	m_bOpened = TRUE;

	return( m_bOpened );

}

BOOL CSerial::Close( void )
{

	if( !m_bOpened || m_hIDComDev == NULL ) return( TRUE );

	if( m_OverlappedRead.hEvent != NULL ) CloseHandle( m_OverlappedRead.hEvent );
	if( m_OverlappedWrite.hEvent != NULL ) CloseHandle( m_OverlappedWrite.hEvent );
	CloseHandle( m_hIDComDev );
	m_bOpened = FALSE;
	m_hIDComDev = NULL;

	return( TRUE );

}

BOOL CSerial::WriteCommByte( unsigned char ucByte )
{
	BOOL bWriteStat;
	DWORD dwBytesWritten;

	bWriteStat = WriteFile( m_hIDComDev, (LPSTR) &ucByte, 1, &dwBytesWritten, &m_OverlappedWrite );
	if( !bWriteStat && ( GetLastError() == ERROR_IO_PENDING ) ){
		if( WaitForSingleObject( m_OverlappedWrite.hEvent, 1000 ) ) dwBytesWritten = 0;
		else{
			GetOverlappedResult( m_hIDComDev, &m_OverlappedWrite, &dwBytesWritten, FALSE );
			m_OverlappedWrite.Offset += dwBytesWritten;
			}
		}

	return( TRUE );

}

int CSerial::SendData( unsigned char *buffer, int size )
{

	if( !m_bOpened || m_hIDComDev == NULL ) return( 0 );

	DWORD dwBytesWritten = 0;
	int i;
	for( i=0; i<size; i++ ){
		WriteCommByte( buffer[i] );
		dwBytesWritten++;
		}

	return( (int) dwBytesWritten );

}

int CSerial::ReadDataWaiting( void )
{

	if( !m_bOpened || m_hIDComDev == NULL ) return( 0 );

	DWORD dwErrorFlags;
	COMSTAT ComStat;

	ClearCommError( m_hIDComDev, &dwErrorFlags, &ComStat );

	return( (int) ComStat.cbInQue );

}

int CSerial::ReadData( void *buffer, int limit )
{

	if( !m_bOpened || m_hIDComDev == NULL ) return( 0 );

	BOOL bReadStatus;
	DWORD dwBytesRead, dwErrorFlags;
	COMSTAT ComStat;

	ClearCommError( m_hIDComDev, &dwErrorFlags, &ComStat );
	if( !ComStat.cbInQue ) return( 0 );

	dwBytesRead = (DWORD) ComStat.cbInQue;
	if( limit < (int) dwBytesRead ) dwBytesRead = (DWORD) limit;

	bReadStatus = ReadFile( m_hIDComDev, buffer, dwBytesRead, &dwBytesRead, &m_OverlappedRead );
	if( !bReadStatus ){
		if( GetLastError() == ERROR_IO_PENDING ){
			WaitForSingleObject( m_OverlappedRead.hEvent, 2000 );
			return( (int) dwBytesRead );
			}
		return( 0 );
		}

	return( (int) dwBytesRead );

}

Pantilt::Pantilt() {
	for (i = 0; i < 16; i++) {
		uc_hex[i] = 0xFF;
		uc_readbuffer[i] = 0x00;
	}
}

Pantilt::~Pantilt() {

}

int Pantilt::Position(float pan, float tilt, bool ifabsolute, int panspeed, int tiltspeed)
{
	if (pan > panmaxdegree || pan<panmindegree 
		|| tilt>tiltmaxdegree || tilt < tiltmindegree) return -1;
	uc_hex[0] = 0x80 + PtzAdress;
	uc_hex[1] = 0x01;
	uc_hex[2] = 0x06;
	if (ifabsolute) uc_hex[3] = 0x02;
	else uc_hex[3] = 0x03;
	uc_hex[4] = panspeed;
	uc_hex[5] = tiltspeed;
	if (!Convert(pan, true)) return -2;
	for (i = 0; i<4; i++) {
		uc_hex[i + 6] = uc_positiontemp[i];
	}
	if (!Convert(tilt, false)) return -2;
	for (i = 0; i<4; i++) {
		uc_hex[i + 10] = uc_positiontemp[i];
	}
	int sentbytes=SendCommand();
	Read();//Read for ACK
	if (uc_readbuffer[1] / 0x10 != 4) return -3;
	Read();//Read for completion
	if (uc_readbuffer[1] / 0x10 != 5) return -4;
	return sentbytes;
}

int Pantilt::Move(int way, int panspeed, int tiltspeed){
	uc_hex[0] = 0x80 + PtzAdress;
	uc_hex[1] = 0x01;
	uc_hex[2] = 0x06;
	uc_hex[3] = 0x01;
	uc_hex[4] = panspeed;
	uc_hex[5] = tiltspeed;
	switch (way){
		case 1: {
			uc_hex[6] = 0x03;
			uc_hex[7] = 0x01;
			break;
		};
		case 2: {
			uc_hex[6] = 0x03;
			uc_hex[7] = 0x02;
			break;
		};
		case 3: {
			uc_hex[6] = 0x01;
			uc_hex[7] = 0x03;
			break;
		};
		case 4: {
			uc_hex[6] = 0x02;
			uc_hex[7] = 0x03;
			break;
		};
		case 5: {
			uc_hex[6] = 0x01;
			uc_hex[7] = 0x01;
			break;
		};
		case 6: {
			uc_hex[6] = 0x02;
			uc_hex[7] = 0x01;
			break;
		};
		case 7: {
			uc_hex[6] = 0x01;
			uc_hex[7] = 0x02;
			break;
		};
		case 8: {
			uc_hex[6] = 0x02;
			uc_hex[7] = 0x02;
			break;
		};
		case 9: {
			uc_hex[6] = 0x03;
			uc_hex[7] = 0x03;
			break;
		};
	}
	uc_hex[8] = 0xFF;
	return SendCommand();
}

int Pantilt::PosInq()
{
	uc_hex[0] = 0x80 + PtzAdress;
	uc_hex[1] = 0x09;
	uc_hex[2] = 0x06;
	uc_hex[3] = 0x12;
	uc_hex[4] = 0xFF;
	if (!SendCommand()) return -1;
	Read();
	if (uc_readbuffer[1] / 0x10 != 5) return -2;
	for (i = 0; i < 4; i++) {
		uc_positiontemp[i] = uc_readbuffer[i + 2];
	}
	pospan = InvConvert(true);
	if (pospan > 360) return -3;
	for (i = 0; i < 4; i++) {
		uc_positiontemp[i] = uc_readbuffer[i + 6];
	}
	postilt = InvConvert(false);
	if (postilt > 360) return -3;
	return 0;
}

bool Pantilt::Infrared(bool infrared_on)
{
	uc_hex[0] = 0x80 + PtzAdress;
	uc_hex[1] = 0x01;
	uc_hex[2] = 0x04;
	uc_hex[3] = 0x01;
	if(infrared_on) uc_hex[4] = 0x02;
	else uc_hex[4] = 0x03;
	uc_hex[5] = 0xFF;
	return SendCommand();
}

int Pantilt::Reset(){

	uc_hex[0] = 0x80 + PtzAdress;
	uc_hex[1] = 0x01;
	uc_hex[2] = 0x06;
	uc_hex[3] = 0x05;
	uc_hex[4] = 0xFF;
	if (!SendCommand()) return -1;
	Read();//Read for ACK
	if (uc_readbuffer[1] / 0x10 != 4) return -3;
	Read();//Read for completion
	if (uc_readbuffer[1] / 0x10 != 5) return -4;
	return 0;
}

int Pantilt::Read(){
		for (i = 0; i < 16; i++) {
			while (!ReadDataWaiting());
			ReadData(uc_buffer, 1);
			uc_readbuffer[i] = uc_buffer[0];
			if (uc_buffer[0] == 0xFF) break;
		}
	return 0;
}

int Pantilt::SendCommand() {
	for (i = 0; i < 16; i++) {
		if (uc_hex[i] == 0xFF) break;
	}
	if (i == 16) return -3;
	return SendData(uc_hex, i + 1);
}

bool Pantilt::Convert(float degree,bool ispan)
{
	int i_degree;
	int i_temp[4];
	if (degree >= 0) {
		i_degree = int(
			degree
			/ float((ispan) ? panmaxdegree : tiltmaxdegree)
			* float((ispan) ? panmaxhex : tiltmaxhex)
			+ 0.5
			);
	}
	else {
		i_degree = int(
			degree
			/ float((ispan) ? panmindegree : tiltmindegree)
			* float((ispan) ? (0xFFFF - panminhex) : (0xFFFF - tiltminhex))
			+ 0.5
			);
		i_degree = 0xFFFF - i_degree;
	}
	i_temp[0] = i_degree / 0xfff;
	i_temp[1] = (i_degree - i_temp[0] * 0x1000) / 0xff;
	i_temp[2] = (i_degree - i_temp[0] * 0x1000 - i_temp[1] * 0x100) / 0xf;
	i_temp[3] = i_degree - i_temp[0] * 0x1000 - i_temp[1] * 0x100 - i_temp[2] * 0x10;
	for (i = 0; i < 4; i++) {
		uc_positiontemp[i] = unsigned char(i_temp[i]);
	}
	return true;
}

float Pantilt::InvConvert(bool ispan)
{
	int InvTemp;
	InvTemp=  uc_positiontemp[0] * 0x1000 
			+ uc_positiontemp[1] * 0x100 
			+ uc_positiontemp[2] * 0x10 
			+ uc_positiontemp[3];
	if (ispan) {
		if (InvTemp <= panmaxhex) {
			return (float)InvTemp*float(panmaxdegree) / float(panmaxhex);
		}
		else if (InvTemp > panminhex) {
			return (float)(0xFFFF - InvTemp)*float(panmindegree) / float(panminhex);
		}
		else return 361;
	}
	else {
		if (InvTemp <= tiltmaxhex) {
			return (float)InvTemp*float(tiltmaxdegree) / float(tiltmaxhex);
		}
		else if (InvTemp > tiltminhex) {
			return (float)(0xFFFF - InvTemp)*float(tiltmindegree) / float(tiltminhex);
		}
		else return 361;
	}
	return 361;
}
