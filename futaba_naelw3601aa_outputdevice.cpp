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

#include "futaba_naelw3601aa_outputdevice.h"

///#include <libusb/libusb.h>
#include <stdio.h>
#include <assert.h>
#include <dmdframe.h>

//#pragma comment (lib, "libusb-1.0.lib")
#include "lusb0_usb.h"
//#pragma comment(lib, "C:/Users/BlackStar/Desktop/Edge of Panic/dmdwidget/vendor/libusb-win32-bin-1.2.6.0/lib/msvc_x64/libusb.lib")

FutabaNAELW3601AAOutputDevice::FutabaNAELW3601AAOutputDevice()
{
	//init usb library
	usb_init();
	//find busses
	int busses = usb_find_busses();
	//find devices
	int devices = usb_find_devices();

	m_futaba = find_Futaba();
}

FutabaNAELW3601AAOutputDevice::~FutabaNAELW3601AAOutputDevice()
{
	if (m_futaba != nullptr)
		usb_close(m_futaba);
}

bool FutabaNAELW3601AAOutputDevice::isDeviceAvailable()
{
	return m_futaba != nullptr;
}

void FutabaNAELW3601AAOutputDevice::clearDMD()
{
	sendFrame(DMDFrame());
}

void FutabaNAELW3601AAOutputDevice::sendFrame(const DMDFrame& frame)
{
	assert(m_futaba != nullptr);

	//usb_bulk_write(m_futaba, 0x01, (char*) frame_buf, 2052, 5000);

}

bool FutabaNAELW3601AAOutputDevice::supportsColor() const
{
	return false;
}

usb_dev_handle* FutabaNAELW3601AAOutputDevice::find_Futaba()
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
			if (dev->descriptor.idVendor == FUTABA_VENDOR_ID && dev->descriptor.idProduct == FUTABA_VENDOR_ID)
			{
				m_futaba =  usb_open(dev);
				goto found;
			}
		}
	}

found:

	if (m_futaba == nullptr)
		return nullptr;

	//set configuration
	if (usb_set_configuration(m_futaba, PINDMD2_CONFIGURATION) < 0) {
		//if not succesfull, notify
		printf("error setting configuration\n");
		//close device
		usb_close(m_futaba);
		//and exit
		//return_();
	}
	//if successfull, notify
	else
		printf("configuration set\n");

	//try to claim interface for use
	if (usb_claim_interface(m_futaba, PINDMD2_INTERFACE) < 0) {
		//if failed, print error message
		printf("error claiming interface\n");
		//close device and exit
		usb_close(m_futaba);
	}
	//if successfull, notify
	else
		printf("interface claimed\n");

	return m_futaba;
}
