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

#include <libusb/libusb.h>
#include <stdio.h>
#include <assert.h>
#include <dmdframe.h>

#pragma comment (lib, "libusb-1.0.lib")

PinDMD2OutputDevice::PinDMD2OutputDevice()
{
	libusb_init(NULL);

	m_PinDMD2 = find_PinDMD2();
}

PinDMD2OutputDevice::~PinDMD2OutputDevice()
{
	if (m_PinDMD2 != nullptr)
		libusb_close(m_PinDMD2);
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
	frame_buf[0] = 0x81;	// frame sync bytes
	frame_buf[1] = 0xC3;
	frame_buf[2] = 0xE7;
	frame_buf[3] = 0x0;		// command byte (not used)
	uint32_t frame_pixel_nr = 4;

	const uint8_t* const framedata = frame.const_grayscale_frame();
	for (uint32_t pixel = 0; pixel < DMDConfig::DMDWIDTH * DMDConfig::DMDHEIGHT; pixel += 2)
	{
		uint8_t leftpixel = (framedata[pixel] >> 4) << 4;
		uint8_t rightpixel = (framedata[pixel + 1] >> 4);
		frame_buf[frame_pixel_nr++] = leftpixel | rightpixel;
	}
	
	libusb_bulk_transfer(m_PinDMD2, ENDPOINT_OUT, (unsigned char*)frame_buf, PINDMD2BUFFERSIZE, NULL, 5000);
}

bool PinDMD2OutputDevice::supportsColor() const
{
	return false;
}

libusb_device_handle* PinDMD2OutputDevice::find_PinDMD2()
{
	// discover devices
	libusb_device** list;
	libusb_device_handle* pinDMD = NULL;
	ssize_t cnt = libusb_get_device_list(NULL, &list);
	ssize_t i = 0;
	int err = 0;
	//if (cnt < 0)
	//	error();
	for (i = 0; i < cnt; i++)
	{
		libusb_device *device = list[i];
		libusb_device_descriptor dd;
		libusb_get_device_descriptor(device, &dd);
		if (dd.idVendor == PINDMD2_VENDOR_ID &&dd.idProduct == PINDMD2_PRODUCT_ID)
		{
			err = libusb_open(device, &pinDMD);
			if (err)
			{
			}

			break;
		}
	}
	libusb_free_device_list(list, 1);


	//set configuration 
	const int32_t MY_CONFIG = 1;
	const int32_t MY_INTF = 0;

	// ?!? TODO blackstar, doesn't work
	//if (libusb_set_configuration(pinDMD, MY_CONFIG) < 0) {
	//	//if not succesfull, notify
	//	printf("error setting configuration\n");
	//	//close device
	//	libusb_close(pinDMD);
	//	return nullptr;
	//}
	//else
	//	printf("configuration set\n");

	//try to claim interface for use
	if (libusb_claim_interface(pinDMD, MY_INTF) < 0) {
		//if failed, print error message
		printf("error claiming interface\n");
		//close device and exit
		libusb_close(pinDMD);
		return nullptr;
	}
	//if successfull, notify
	else
		printf("interface claimed\n");


	return pinDMD;
}