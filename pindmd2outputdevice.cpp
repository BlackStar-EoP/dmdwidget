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

//#include <lusb0_usb.h>
//#include <stdio.h>
//
////define vendor id and product id
//#define VID 0x0314
//#define PID 0xe457
//
////configure and interface number
////TODO figure out what these mean
//#define MY_CONFIG 1
//#define MY_INTF 0
//
////endpoints for communication
//#define EP_IN 0x81
//#define EP_OUT 0x01
//
////maximum packet size, 64 for full-speed, 512 for high-speed
//#define BUF_SIZE 2052//64
//
////gets device handle
//usb_dev_handle* open_dev(void);
//
//char writeBuffer[BUF_SIZE];
//char readBuffer[1];
////context handle for transfer and receive
////think of them bit like session id:s
//void* asyncReadContext = NULL;
//void* asyncWriteContext = NULL;
//
//usb_dev_handle *device = NULL;
//
//
//
//
//
//
////*****************************************************
////* Name:			renderDMDFrame
////* Purpose:
////* In:
////* Out:
////*****************************************************
//void renderDMDFrame(int width, int height, unsigned char *currbuffer) {
//	int byteIdx = 4;
//	int bd0, bd1, bd2, bd3;
//	int pixel;
//	int i, j, v;
//	int ret = 0;
//	unsigned char frame_buf[2052];
//
//	memset(frame_buf, 0, 2052);
//	frame_buf[0] = 0x81;	// frame sync bytes
//	frame_buf[1] = 0xC3;
//	frame_buf[2] = 0xE7;
//	frame_buf[3] = 0x0;		// command byte (not used)
//
//							// dmd height
//	for (j = 0; j < height; j++) {
//		// dmd width
//		for (i = 0; i < width; i += 8) {
//			bd0 = 0;
//			bd1 = 0;
//			bd2 = 0;
//			bd3 = 0;
//			for (v = 7; v >= 0; v--) {
//				// pixel colour
//				pixel = currbuffer[(j * 128) + (i + v)];
//				// 16 color mode hue remapping for proper gradient
//				bd0 <<= 1;
//				bd1 <<= 1;
//				bd2 <<= 1;
//				bd3 <<= 1;
//				//bd4 <<= 1;
//				if (pixel & 1)
//					bd0 |= 1;
//				if (pixel & 2)
//					bd1 |= 1;
//				if (pixel & 4)
//					bd2 |= 1;
//				if (pixel & 8)
//					bd3 |= 1;
//			}
//			frame_buf[byteIdx] = bd0;
//			frame_buf[byteIdx + 512] = bd1;
//			frame_buf[byteIdx + 1024] = bd2;
//			frame_buf[byteIdx + 1536] = bd3;
//			byteIdx++;
//		}
//	}
//
//	//writeBuffer[0]=0;
//	//usb_bulk_write(device, EP_OUT, writeBuffer, 1, 5000);
//	//usb_bulk_read(device, EP_IN, readBuffer, 1, 5000);
//	memcpy(writeBuffer, frame_buf, 2052);
//	usb_bulk_write(device, EP_OUT, writeBuffer, 2052, 5000);
//	printf("frame sent: %s\n", usb_strerror());
//
//
//	/*memcpy(writeBuffer,frame_buf,2048);
//	ret = usb_submit_async( asyncWriteContext, writeBuffer, sizeof( writeBuffer ) );
//	if( ret < 0 )
//	printf("error usb_submit_async:\n%s\n", usb_strerror());
//
//	//wait for read and write transfers to complete or 5 seconds
//	usb_reap_async( asyncWriteContext, 5000 );*/
//}
//
//
//
//
//
//
//
//int main(int argc, char *argv[])
//{
//	//return value for various functions
//	int ret = 0;
//
//	//init usb library
//	usb_init();
//	//find busses
//	usb_find_busses();
//	//find devices
//	usb_find_devices();
//
//	//try to open our device
//	if (!(device = open_dev())) {
//		//if not successfull, print error message
//		printf("error opening pinDMD2 device: \n");
//		//and exit
//		return_();
//	}
//	//else notify that device was opened
//	else
//		printf("pinDMD2 device opened\n");
//
//	//set configuration
//	if (usb_set_configuration(device, MY_CONFIG) < 0) {
//		//if not succesfull, notify
//		printf("error setting configuration\n");
//		//close device
//		usb_close(device);
//		//and exit
//		return_();
//	}
//	//if successfull, notify
//	else
//		printf("configuration set\n");
//
//	//try to claim interface for use
//	if (usb_claim_interface(device, MY_INTF) < 0) {
//		//if failed, print error message
//		printf("error claiming interface\n");
//		//close device and exit
//		usb_close(device);
//		return_();
//	}
//	//if successfull, notify
//	else
//		printf("interface claimed\n");
//
//	//and transfer
//	//ret = usb_bulk_setup_async( device, &asyncWriteContext, EP_OUT );
//	//error handling
//	/*if( ret < 0 )
//	{
//	printf("error setting up async writing:\n");
//	//relese interface
//	usb_release_interface( device, MY_INTF );
//	//close device and exit
//	usb_close( device );
//	return 0;
//	}
//	//if successfull, notify
//	else
//	printf("asynchronous write set up\n");*/
//
//
//	int cnt = 0;
//	int i, j, z;
//	int state = 0;
//	unsigned char buf[32][128];
//	while (1) {
//		memset(buf, 0, 4096);
//
//		for (i = 0; i<8; i++) {
//			for (j = 0; j<16; j++) {
//				for (z = 0; z<7; z++)
//					buf[i][(j * 8) + z] = j;
//			}
//		}
//
//		switch (state) {
//		case 0:
//			for (i = 0; i<32; i++) {
//				for (j = 0; j<128; j++) {
//					if (j == cnt)
//						buf[i][j] = 15;
//				}
//			}
//			cnt++;
//			if (cnt == 128) {
//				state = 1;
//				cnt = 0;
//			}
//			break;
//		case 1:
//			for (i = 0; i<32; i++) {
//				for (j = 0; j<128; j++) {
//					if (i == cnt)
//						buf[i][j] = 15;
//				}
//			}
//			cnt++;
//			if (cnt == 32) {
//				state = 0;
//				cnt = 0;
//			}
//			break;
//		}
//
//		renderDMDFrame(128, 32, buf);
//		Sleep(41);
//	}
//
//	//clean up.
//	//kitten dies every time you don't clean up after yourself.
//	//free context handlers
//	usb_free_async(&asyncWriteContext);
//	//relese interface
//	usb_release_interface(device, MY_INTF);
//	//close device and exit
//	usb_close(device);
//
//	return EXIT_SUCCESS;
//}
//
//void return_(void) {
//	while (1) {}
//}
//
////gets device handle
////@param none
////@return device handle
//usb_dev_handle* open_dev(void)
//{
//	//contains usb busses present on computer
//	struct usb_bus *bus;
//	//contains devices present on computer
//	struct usb_device *dev;
//	//used to skip first device
//	//bool first = true;
//	//loop through busses and devices
//	for (bus = usb_get_busses(); bus; bus = bus->next) {
//		for (dev = bus->devices; dev; dev = dev->next) {
//			//if device vendor id and product id are match
//			if (dev->descriptor.idVendor == VID && dev->descriptor.idProduct == PID)
//				return usb_open(dev);
//		}
//	}
//	//return null if no devices were found
//	return NULL;
//}
