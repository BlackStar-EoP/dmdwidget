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

#include "pindmd2outputdevice.h"

///#include <libusb/libusb.h>
#include <stdio.h>
#include <assert.h>
#include <dmdframe.h>

//#pragma comment (lib, "libusb-1.0.lib")
#include "lusb0_usb.h"
//#pragma comment(lib, "C:/Users/BlackStar/Desktop/Edge of Panic/dmdwidget/vendor/libusb-win32-bin-1.2.6.0/lib/msvc_x64/libusb.lib")

PinDMD2OutputDevice::PinDMD2OutputDevice()
{
	//init usb library
	usb_init();
	//find busses
	usb_find_busses();
	//find devices
	usb_find_devices();

	m_PinDMD2 = find_PinDMD2();
}

PinDMD2OutputDevice::~PinDMD2OutputDevice()
{
	if (m_PinDMD2 != nullptr)
		usb_close(m_PinDMD2);
}

bool PinDMD2OutputDevice::isDeviceAvailable()
{
	return m_PinDMD2 != nullptr;
}

void PinDMD2OutputDevice::clearDMD()
{
	sendFrame(DMDFrame());
}

void PinDMD2OutputDevice::sendFrame(const DMDFrame& frame)
{
	assert(m_PinDMD2 != nullptr);
	const uint32_t PINDMD2BUFFERSIZE = 2052;
	unsigned char frame_buf[PINDMD2BUFFERSIZE];
	memset(frame_buf, 0, PINDMD2BUFFERSIZE);

	int byteIdx = 4;
	int bd0, bd1, bd2, bd3;
	int pixel;
	int i, j, v;
	int ret = 0;

	frame_buf[0] = 0x81;	// frame sync bytes
	frame_buf[1] = 0xC3;
	frame_buf[2] = 0xE7;
	frame_buf[3] = 0x0;		// command byte (not used)

							// dmd height
	for (j = 0; j < 32; j++) {
		// dmd width
		for (i = 0; i < 128; i += 8) {
			bd0 = 0;
			bd1 = 0;
			bd2 = 0;
			bd3 = 0;
			for (v = 7; v >= 0; v--) {
				// pixel colour
				pixel = frame.const_grayscale_frame()[(j * 128) + (i + v)] >> 4;
				// 16 color mode hue remapping for proper gradient
				bd0 <<= 1;
				bd1 <<= 1;
				bd2 <<= 1;
				bd3 <<= 1;
				//bd4 <<= 1;
				if (pixel & 1)
					bd0 |= 1;
				if (pixel & 2)
					bd1 |= 1;
				if (pixel & 4)
					bd2 |= 1;
				if (pixel & 8)
					bd3 |= 1;
			}
			frame_buf[byteIdx] = bd0;
			frame_buf[byteIdx + 512] = bd1;
			frame_buf[byteIdx + 1024] = bd2;
			frame_buf[byteIdx + 1536] = bd3;
			byteIdx++;
		}
	}

	//writeBuffer[0]=0;
	//usb_bulk_write(device, EP_OUT, writeBuffer, 1, 5000);
	//usb_bulk_read(device, EP_IN, readBuffer, 1, 5000);
	//memcpy(writeBuffer, frame_buf, 2052);
	usb_bulk_write(m_PinDMD2, 0x01, (char*) frame_buf, 2052, 5000);
	//printf("frame sent: %s\n", usb_strerror());
}

bool PinDMD2OutputDevice::supportsColor() const
{
	return false;
}

//libusb_device_handle* PinDMD2OutputDevice::find_PinDMD2()
usb_dev_handle* PinDMD2OutputDevice::find_PinDMD2()
{
	//contains usb busses present on computer
	struct usb_bus *bus;
	//contains devices present on computer
	struct usb_device *dev;
	//used to skip first device
	//bool first = true;
	//loop through busses and devices
	
	for (bus = usb_get_busses(); bus; bus = bus->next) {
		for (dev = bus->devices; dev; dev = dev->next) {
			//if device vendor id and product id are match
			if (dev->descriptor.idVendor == PINDMD2_VENDOR_ID && dev->descriptor.idProduct == PINDMD2_PRODUCT_ID)
			{
				m_PinDMD2 =  usb_open(dev);
				goto found;
			}
		}
	}

found:

	if (m_PinDMD2 == nullptr)
		return nullptr;

	//set configuration
	if (usb_set_configuration(m_PinDMD2, PINDMD2_CONFIGURATION) < 0) {
		//if not succesfull, notify
		printf("error setting configuration\n");
		//close device
		usb_close(m_PinDMD2);
		//and exit
		//return_();
	}
	//if successfull, notify
	else
		printf("configuration set\n");

	//try to claim interface for use
	if (usb_claim_interface(m_PinDMD2, PINDMD2_INTERFACE) < 0) {
		//if failed, print error message
		printf("error claiming interface\n");
		//close device and exit
		usb_close(m_PinDMD2);
	}
	//if successfull, notify
	else
		printf("interface claimed\n");

	return m_PinDMD2;
}
