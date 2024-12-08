#ifndef HID_INTERPRETER_H
#define HID_INTERPRETER_H
#include <stdint.h>
// This interpreter will not be made fully general purpose.
// Instead, it will interpret just enough to work for our purposes.
// We assume a maximally capable output report and just map
// less capable input reports to it. 
// Throw errors if the reports are not compatible in this way.

// See https://www.usb.org/hid
// See https://docs.kernel.org/hid/hidreport-parsing.html
// See http://eleccelerator.com/usbdescreqparser/

// See ../Device/USBLIB/CONFIG/usb_desc.c
//     ../Device/USBLIB/CONFIG/usb_desc.h

// To capture HID messages, enter 
// sudo usbhid-dump -s A:B -f -e stream
// where you replace A and B with the bus and device numbers
// for your usb device. Learn these numbers using the command
// lsusb -v

// From https://docs.kernel.org/hid/hidintro.html
// The current list of HID devices can be found at /sys/bus/hid/devices/. 
// For each device, say /sys/bus/hid/devices/0003\:093A\:2510.0002/, one can read the corresponding report descriptor:
//
// sudo hexdump -C /sys/bus/hid/devices/0003\:093A\:2510.0002/report_descriptor
// The 0003 is the bus from lsusb -v and the 0002 is the device from lsusb -v.

#define HID_MAX_REPORT_DESCRIP_LEN (128)
#define HID_MAX_USAGES (8)
enum{HID_ERROR,HID_SUCCESS};
typedef struct tag_HID_MOUSE_REPORT{
	uint16_t len;
	uint8_t bytes[HID_MAX_REPORT_DESCRIP_LEN];
	int8_t status;
	uint16_t usages[HID_MAX_USAGES];
	uint8_t usageBytes[HID_MAX_USAGES];
	uint8_t numUsages;
	uint8_t reportBytes;
}HID_MOUSE_REPORT;

typedef struct tag_HID_MOUSE_INTERPRETER{
	HID_MOUSE_REPORT *inputReport;      // This is the description of the data that is coming in.
	HID_MOUSE_REPORT *outputReport;     // and the input data is to be altered to fit this format.
	int8_t status;
}HID_MOUSE_INTERPRETER;

int8_t init_HID_MOUSE_REPORT(HID_MOUSE_REPORT *report,uint8_t *buf,uint16_t bufLen);
int8_t init_HID_MOUSE_INTERPRETER(HID_MOUSE_INTERPRETER *interpreter,HID_MOUSE_REPORT *inputReport,HID_MOUSE_REPORT *outputReport);
int8_t interpret_HID_MOUSE(HID_MOUSE_INTERPRETER *interpreter,
	                       uint8_t *inputBuf,uint8_t inputLen,
	                       uint8_t *outputBuf,uint16_t *outputLen);
void print_HID_MOUSE_REPORT(HID_MOUSE_REPORT *report);

#endif //HID_INTERPRETER_H