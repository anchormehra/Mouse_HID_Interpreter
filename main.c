#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "minunit.h"
#include <math.h>
#include "HID_Interpreter.h"

int tests_run = 0;

uint8_t GamingMouseHIDReport[]={0x05, 0x01,      // Usage Page (Generic Desktop Ctrls)
                                0x09, 0x02,      // Usage (Mouse)
                                0xa1, 0x01,      // Collection (Application)
                                0x09, 0x01,       //   Usage (Pointer)
                                0xa1, 0x00,       //   Collection (Physical)
                                0x05, 0x09,      //     Usage Page (Button)
                                0x19, 0x01,      //     Usage Minimum (0x01)
                                0x29, 0x10,       //     Usage Maximum (0x10)
                                0x15, 0x00,      //     Logical Minimum (0)
                                0x25, 0x01,      //     Logical Maximum (1)
                                0x75, 0x01,      //     Report Size (1)
                                0x95, 0x10,      //     Report Count (16)
                                0x81, 0x02,       //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
                                0x05, 0x01,       //     Usage Page (Generic Desktop Ctrls)
                                0x09, 0x30,       //     Usage (X)
                                0x09, 0x31,       //     Usage (Y)
                                0x16, 0x01, 0x80, //     Logical Minimum (-32767)
                                0x26, 0xff, 0x7f, //     Logical Maximum (32767)
                                0x75, 0x10,       //     Report Size (16)
                                0x95, 0x02,       //     Report Count (2)
                                0x81, 0x06,       //     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
                                0x09, 0x38,       //     Usage (Wheel)
                                0x15, 0x81,       //     Logical Minimum (-127)
                                0x25, 0x7f,       //     Logical Maximum (127)
                                0x75, 0x08,       //     Report Size (8)
                                0x95, 0x01,       //     Report Count (1)
                                0x81, 0x06,        //     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
                                0x05, 0x0c,        //     Usage Page (Consumer)
                                0x0a, 0x38, 0x02,  //     Usage (AC Pan)
                                0x95, 0x01,        //     Report Count (1)
                                0x81, 0x06,        //     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
                                0xc0,              //   End Collection 
                                0xc0};             // End Collection

uint8_t MicroSoftMouseHIDReport[]={0x05, 0x01,     // Usage Page (Generic Desktop Ctrls)
                                   0x09, 0x02,     // Usage (Mouse)
                                   0xa1, 0x01,     // Collection (Application)
                                   0x09, 0x01,     //   Usage (Pointer)
                                   0xa1, 0x00,     //   Collection (Physical)
                                   0x05, 0x09,     //     Usage Page (Button)
                                   0x19, 0x01,     //     Usage Minimum (0x01)
                                   0x29, 0x03,     //     Usage Maximum (0x03)
                                   0x15, 0x00,     //     Logical Minimum (0)
                                   0x25, 0x01,     //     Logical Maximum (1)
                                   0x75, 0x01,     //     Report Size (1)
                                   0x95, 0x03,     //     Report Count (3)
                                   0x81, 0x02,     //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
                                   0x75, 0x05,     //     Report Size (5)
                                   0x95, 0x01,     //     Report Count (1)
                                   0x81, 0x01,     //     Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
                                   0x05, 0x01,     //     Usage Page (Generic Desktop Ctrls)
                                   0x09, 0x30,     //     Usage (X)
                                   0x09, 0x31,     //     Usage (Y)
                                   0x09, 0x38,     //     Usage (Wheel)
                                   0x15, 0x81,     //     Logical Minimum (-127)
                                   0x25, 0x7f,     //     Logical Maximum (127)
                                   0x75, 0x08,     //     Report Size (8)
                                   0x95, 0x03,     //     Report Count (3)
                                   0x81, 0x06,     //     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
                                   0xc0,           //   End Collection
                                   0xc0};          // End Collection 

uint8_t LogitechMouseHIDReport[]={0x05, 0x01,      // Usage Page (Generic Desktop Ctrls)
                                  0x09, 0x02,      // Usage (Mouse)
                                  0xa1, 0x01,      // Collection (Application)
                                  0x09, 0x01,      //   Usage (Pointer)
                                  0xa1, 0x00,      //   Collection (Physical)
                                  0x05, 0x09,      //     Usage Page (Button)
                                  0x19, 0x01,      //     Usage Minimum (0x01)
                                  0x29, 0x03,      //     Usage Maximum (0x03)
                                  0x15, 0x00,      //     Logical Minimum (0)
                                  0x25, 0x01,      //     Logical Maximum (1)
                                  0x95, 0x08,      //     Report Count (8)
                                  0x75, 0x01,      //     Report Size (1)
                                  0x81, 0x02,      //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
                                  0x05, 0x01,      //     Usage Page (Generic Desktop Ctrls)
                                  0x09, 0x30,      //     Usage (X)
                                  0x09, 0x31,      //     Usage (Y)
                                  0x09, 0x38,      //     Usage (Wheel)
                                  0x15, 0x81,      //     Logical Minimum (-127)
                                  0x25, 0x7f,      //     Logical Maximum (127)
                                  0x75, 0x08,      //     Report Size (8)
                                  0x95, 0x03,      //     Report Count (3)
                                  0x81, 0x06,      //     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
                                  0xc0,            //   End Collection
                                  0xc0};           // End Collection




char * test_microsoft_mouse()
{
	int8_t status;
	uint8_t i;
	HID_MOUSE_REPORT output;
	HID_MOUSE_REPORT input;
	HID_MOUSE_INTERPRETER interp;

	uint8_t MicroSoftMouseMessage[4] = {0x01,  // button
	                                    0x02,  // X
	                                    0x03,  // Y
	                                    0x04}; // Wheel
	uint8_t GamingMouseMessage[8];
	uint16_t outputLen;

	
    status = init_HID_MOUSE_REPORT(&input,MicroSoftMouseHIDReport,sizeof(MicroSoftMouseHIDReport));
    printf("\n");
    printf("Input Report [%ld]:\n",sizeof(MicroSoftMouseHIDReport));
    print_HID_MOUSE_REPORT(&input);
     mu_assert("input report init failure",status==HID_SUCCESS);

    mu_assert("A",input.reportBytes == 4);
    mu_assert("A",input.usages[0] == 0x0900);
    mu_assert("A",input.usages[1] == 0x3000);
    mu_assert("A",input.usages[2] == 0x3100);
    mu_assert("A",input.usages[3] == 0x3800);
    mu_assert("A",input.numUsages == 4);
    for(i=0;i<4;i++)
    {
        mu_assert("B",input.usageBytes[i] == 1);
    }
    printf("\n");    


    status = init_HID_MOUSE_REPORT(&output,GamingMouseHIDReport,sizeof(GamingMouseHIDReport));
	
	printf("\n");
    printf("Output Report [%ld]:\n",sizeof(GamingMouseHIDReport));
	print_HID_MOUSE_REPORT(&output);
	mu_assert("output report init failure",status==HID_SUCCESS);

    mu_assert("A0",output.reportBytes == 8);
    mu_assert("A1",output.usages[0] == 0x0900);
    mu_assert("A2",output.usages[1] == 0x3000);
    mu_assert("A3",output.usages[2] == 0x3100);
    mu_assert("A4",output.usages[3] == 0x3800);
    mu_assert("A5",output.usages[4] == 0x0102);
    mu_assert("A6",output.numUsages == 5);
    for(i=0;i<3;i++)
    {
        mu_assert("B",output.usageBytes[i] == 2);
    }
    for(i=3;i<5;i++)
    {
        mu_assert("C",output.usageBytes[i] == 1);
    }

	// This is the wrong order. We do not expect this to actually work.
	status = init_HID_MOUSE_INTERPRETER(&interp,&output,&input);
	mu_assert("wrong interperter init not detected",status==HID_ERROR);

	// This should actually work.
	status = init_HID_MOUSE_INTERPRETER(&interp,&input,&output);
	mu_assert("interpreter init failure",status==HID_SUCCESS);

	status = interpret_HID_MOUSE(&interp,
	                             MicroSoftMouseMessage,4,
	                             GamingMouseMessage,&outputLen);

	mu_assert("interpretation failure",status==HID_SUCCESS);
	mu_assert("interpretation wrong outputLen",outputLen==8);

	printf("InputBuf:\n");
	for(i=0;i<4;i++)
	{
		printf("0x%X\n",MicroSoftMouseMessage[i]);

	}
	printf("\n");
	printf("OutputBuf:\n");
	for(i=0;i<outputLen;i++)
	{
		printf("0x%X\n",GamingMouseMessage[i]);

	}

    mu_assert("D0",GamingMouseMessage[0] == 0x1);
    mu_assert("D1",GamingMouseMessage[1] == 0x0);
    mu_assert("D2",GamingMouseMessage[2] == 0x2);
    mu_assert("D3",GamingMouseMessage[3] == 0x0);
    mu_assert("D4",GamingMouseMessage[4] == 0x3);
    mu_assert("D5",GamingMouseMessage[5] == 0x0);
    mu_assert("D6",GamingMouseMessage[6] == 0x4);
    mu_assert("D7",GamingMouseMessage[7] == 0x0);

	return 0;
}


char * test_gaming_mouse()
{
    int8_t status;
    uint8_t i;
    HID_MOUSE_REPORT output;
    HID_MOUSE_REPORT input;
    HID_MOUSE_INTERPRETER interp;

    uint8_t InputMouseMessage[8] = {0x01,  // button
                                    0x01,
                                    0x02,  // X
                                    0x02,
                                    0x03,  // Y
                                    0x03,
                                    0x04, // Wheel
                                    0x01}; // pan
    uint8_t OutputMouseMessage[8];
    uint16_t outputLen;

    
    status = init_HID_MOUSE_REPORT(&input,GamingMouseHIDReport,sizeof(GamingMouseHIDReport));
    printf("\n");
    printf("Input Report [%ld]:\n",sizeof(GamingMouseHIDReport));
    print_HID_MOUSE_REPORT(&input);
    mu_assert("input report init failure",status==HID_SUCCESS);


    status = init_HID_MOUSE_REPORT(&output,GamingMouseHIDReport,sizeof(GamingMouseHIDReport));
    
    printf("\n");
    printf("Output Report [%ld]:\n",sizeof(GamingMouseHIDReport));
    print_HID_MOUSE_REPORT(&output);
    mu_assert("output report init failure",status==HID_SUCCESS);

    mu_assert("A0",output.reportBytes == 8);
    mu_assert("A1",output.usages[0] == 0x0900);
    mu_assert("A2",output.usages[1] == 0x3000);
    mu_assert("A3",output.usages[2] == 0x3100);
    mu_assert("A4",output.usages[3] == 0x3800);
    mu_assert("A5",output.usages[4] == 0x0102);
    mu_assert("A6",output.numUsages == 5);
    for(i=0;i<3;i++)
    {
        mu_assert("B",output.usageBytes[i] == 2);
    }
    for(i=3;i<5;i++)
    {
        mu_assert("C",output.usageBytes[i] == 1);
    }

    // This should actually work.
    status = init_HID_MOUSE_INTERPRETER(&interp,&input,&output);
    mu_assert("interpreter init failure",status==HID_SUCCESS);

    status = interpret_HID_MOUSE(&interp,
                                 InputMouseMessage,8,
                                 OutputMouseMessage,&outputLen);

    mu_assert("interpretation failure",status==HID_SUCCESS);
    mu_assert("interpretation wrong outputLen",outputLen==8);

    printf("InputBuf:\n");
    for(i=0;i<4;i++)
    {
        printf("0x%X\n",InputMouseMessage[i]);

    }
    printf("\n");
    printf("OutputBuf:\n");
    for(i=0;i<outputLen;i++)
    {
        printf("0x%X\n",OutputMouseMessage[i]);
        mu_assert("D",OutputMouseMessage[i] == InputMouseMessage[i]);

    }


    return 0;
}

char * test_logitech_mouse()
{
    int8_t status;
    uint8_t i;
    HID_MOUSE_REPORT output;
    HID_MOUSE_REPORT input;
    HID_MOUSE_INTERPRETER interp;

    uint8_t InputMouseMessage[4] = {0x01,  // button
                                    0x02,  // X
                                    0x03,  // Y
                                    0x04}; // Wheel
    uint8_t OutputMouseMessage[8];
    uint16_t outputLen;

    
    status = init_HID_MOUSE_REPORT(&input,LogitechMouseHIDReport,sizeof(LogitechMouseHIDReport));
    printf("\n");
    printf("Input Report [%ld]:\n",sizeof(LogitechMouseHIDReport));
    print_HID_MOUSE_REPORT(&input);
    mu_assert("input report init failure",status==HID_SUCCESS);

    mu_assert("A",input.reportBytes == 4);
    mu_assert("A",input.usages[0] == 0x0900);
    mu_assert("A",input.usages[1] == 0x3000);
    mu_assert("A",input.usages[2] == 0x3100);
    mu_assert("A",input.usages[3] == 0x3800);
    mu_assert("A",input.numUsages == 4);
    for(i=0;i<4;i++)
    {
        mu_assert("B",input.usageBytes[i] == 1);
    }
    

    status = init_HID_MOUSE_REPORT(&output,GamingMouseHIDReport,sizeof(GamingMouseHIDReport));
    
    printf("\n");
    printf("Output Report [%ld]:\n",sizeof(GamingMouseHIDReport));
    print_HID_MOUSE_REPORT(&output);
    mu_assert("output report init failure",status==HID_SUCCESS);

    mu_assert("A0",output.reportBytes == 8);
    mu_assert("A1",output.usages[0] == 0x0900);
    mu_assert("A2",output.usages[1] == 0x3000);
    mu_assert("A3",output.usages[2] == 0x3100);
    mu_assert("A4",output.usages[3] == 0x3800);
    mu_assert("A5",output.usages[4] == 0x0102);
    mu_assert("A6",output.numUsages == 5);
    for(i=0;i<3;i++)
    {
        mu_assert("B",output.usageBytes[i] == 2);
    }
    for(i=3;i<5;i++)
    {
        mu_assert("C",output.usageBytes[i] == 1);
    }

    // This should actually work.
    status = init_HID_MOUSE_INTERPRETER(&interp,&input,&output);
    mu_assert("interpreter init failure",status==HID_SUCCESS);

    status = interpret_HID_MOUSE(&interp,
                                 InputMouseMessage,4,
                                 OutputMouseMessage,&outputLen);

    mu_assert("interpretation failure",status==HID_SUCCESS);
    mu_assert("interpretation wrong outputLen",outputLen==8);

    printf("InputBuf:\n");
    for(i=0;i<4;i++)
    {
        printf("0x%X\n",InputMouseMessage[i]);

    }
    printf("\n");
    printf("OutputBuf:\n");
    for(i=0;i<outputLen;i++)
    {
        printf("0x%X\n",OutputMouseMessage[i]);
    }

    mu_assert("D0",OutputMouseMessage[0] == 0x1);
    mu_assert("D1",OutputMouseMessage[1] == 0x0);
    mu_assert("D2",OutputMouseMessage[2] == 0x2);
    mu_assert("D3",OutputMouseMessage[3] == 0x0);
    mu_assert("D4",OutputMouseMessage[4] == 0x3);
    mu_assert("D5",OutputMouseMessage[5] == 0x0);
    mu_assert("D6",OutputMouseMessage[6] == 0x4);
    mu_assert("D7",OutputMouseMessage[7] == 0x0);

    return 0;
}

char * all_tests() {

    mu_run_test(test_microsoft_mouse);
    mu_run_test(test_gaming_mouse);
    mu_run_test(test_logitech_mouse);
    return 0;
}

int main(int argc, char **argv) {
	char *result = all_tests();

	printf("=========== TEST RESULTS ===========\n");

	if (result != 0) {
		printf("ERROR: %s\n", result);
		printf("ERRORS FOUND\n");
	} else {
		printf("ALL TESTS PASSED\n");
	}

	printf("Tests run: %d\n", tests_run);
	printf("====================================\n");

	// Add a prompt to keep the console open for selection
	printf("\nProgram finished. You can select and copy the output now.\n");
	printf("Press Enter to exit...");
	getchar();  // Wait for user input to close

	return result != 0;
}
