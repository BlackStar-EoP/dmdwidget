#pragma once

#include <dmdoutputdevice.h>
#include <stdint.h>

/*
MIT License

Copyright (c) 2020 BlackStar

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

struct libusb_device_handle;
struct usb_dev_handle;

class FutabaNAELW3601AAOutputDevice : public DMDOutputDevice
{
public:

	/*
	9.1 STANDARD DEVICE DESCRIPTOR
Offset Field Description Size Value Comment
0 bLength Size of descriptor in bytes 1 12h
1 bDescriptorType Device descriptor type 1 01h
2, 3 bcdUSB USB release number in BCD 2 0200h rev 2.0
4 bDeviceClass Class code 1 00h
5 bDeviceSubClass Subclass code 1 00h
6 bDeviceProtocol Protocol code 1 00h
7 bMaxPacketSize Max packet size for endpoints 1 40h 64 bytes
8, 9 idVendor Vendor ID 2 1008h Futaba
10, 11 idProduct Product ID 2 103Ah
12, 13 bcdDevice Device release number in BCD 2 6551h FW version
14 iManufacturer Index of string descriptor
describing manufacturer
1 01h Futaba
15 iProduct Index of string descriptor
describing product
1 02h NAELW3601xx
16 iSerialNumber Index of string descriptor
describing device serial number
1 00h
17 bNumConfigurations Number of possible
configurations
1 01h
	
	*/
	static const uint16_t FUTABA_VENDOR_ID = 0x1008;
	static const uint16_t FUTABA_PRODUCT_ID = 0x103A;
	static const unsigned char ENDPOINT_IN = 0x81;
	static const unsigned char ENDPOINT_OUT = 0x01;
	static const int32_t PINDMD2_CONFIGURATION = 1;
	static const int32_t PINDMD2_INTERFACE = 0;

public:
	FutabaNAELW3601AAOutputDevice();
	~FutabaNAELW3601AAOutputDevice();

	bool isDeviceAvailable() override;
	void clearDMD() override;
	void sendFrame(const DMDFrame& frame) override;

	bool supportsColor() const override;

private:
	usb_dev_handle* find_Futaba();

private:
	usb_dev_handle* m_futaba = nullptr;
};