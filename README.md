# Pan Tilt Zoom Control Interface for Video Camera

![alt](/demo_images/EVI-D70.jpeg)

The project was developed in Visual Studio 2017
For serial communications part, I use [CSerial](https://www.codeguru.com/cpp/i-n/network/serialcommunications/article.php/c2503/CSerial--A-C-Class-for-Serial-Communications.htm)

## How to use
1. Power on the Pan Tilt Camera
2. Connect the camera to your computer using Serial Ports(RS232)
    if your computer doesn't have serial ports, use a USB to RS232 Adapter
3. Check your Systems Serial Ports COM connection status, for example: COM1 or COM2
4. change [here](https://github.com/manymuch/PTZ/blob/master/PTZ.cpp#L20) to your COM port number
    for example, if your camera is connected to your computer on COM1, than:
    ```
    if (ptz1.Open(2, 9600) == FALSE) {

    ```

PTZ.cpp is just a demo usage to test serial.cpp,
you can use your own code to use function in serial.h to control the vedio camera
