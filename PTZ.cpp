
//

#include "stdafx.h"
#include "Serial.h"
#include "stdio.h"
#include<iostream>
#include<iomanip>
#include<windows.h>

using namespace std;

int main()
{
	int message;
	int flag = 1;
	float pan, tilt;
	Pantilt ptz1;

	if (ptz1.Open(3, 9600) == FALSE) {
		cout << "failed to open the serial port" << endl;
		cin >> flag;
		return 0;
	}
	ptz1.Position(0, 0);
	ptz1.PosInq();
	//cout << "(" << ptz1.pospan << "," << ptz1.postilt << ")" << endl;
	//ptz1.Move(4, 1, 1);
	//while (1) {
	//	ptz1.PosInq();
	//	cout << "(" << ptz1.pospan << "," << ptz1.postilt << ")" << endl;
	//	Sleep(500);
	//}
	//
	//return 0;

	int i = 0;
	ptz1.Reset();
	ptz1.PosInq();
	cout << "(" << ptz1.pospan << "," << ptz1.postilt << ")" << "i=" << i << endl;
	while(1){
		
		//cout << "pan="; cin >> pan ;
		//cout << "tilt="; cin >> tilt;
		message = ptz1.Position(i, 0);
		/*if (message > 0) cout << "successfully sent " <<oct<< message << " bytes" << endl;
		else {
			switch (message) {
				case 0: cout << "SendCommand error" << endl; break;
				case -1: cout << "pan or tilt input out rang" << endl; break;
				case -2: cout << "Convert error" << endl; break;
				case -3: cout << "SendCommand byte>16" << endl; break;
			}
		}*/
		ptz1.PosInq();
		cout<<"(" << ptz1.pospan << "," << ptz1.postilt <<")"<<"i="<<i<< endl;
		////ptz1.Reset();
		//cout << "1 to continue:";
		//cin >> flag;
		Sleep(1000);
		i=i+1;
	} 
    return 0;
}

