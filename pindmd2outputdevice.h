#pragma once

#include <dmdoutputdevice.h>
#include <stdint.h>

/*
MIT License

Copyright (c) 2019 BlackStar

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

class PinDMD2OutputDevice : public DMDOutputDevice
{
public:
	static const uint16_t PINDMD2_VENDOR_ID = 0x0314;
	static const uint16_t PINDMD2_PRODUCT_ID = 0xE457;
	static const unsigned char ENDPOINT_IN = 0x81;
	static const unsigned char ENDPOINT_OUT = 0x01;
	static const int32_t PINDMD2_CONFIGURATION = 1;
	static const int32_t PINDMD2_INTERFACE = 0;

public:
	PinDMD2OutputDevice();
	~PinDMD2OutputDevice();

	bool isDeviceAvailable() override;
	void clearDMD() override;
	void sendFrame(const DMDFrame& frame) override;

	bool supportsColor() const override;

private:
	//libusb_device_handle* find_PinDMD2();
	usb_dev_handle* find_PinDMD2();

private:
	//libusb_device_handle* m_PinDMD2 = nullptr;
	usb_dev_handle* m_PinDMD2 = nullptr;
};