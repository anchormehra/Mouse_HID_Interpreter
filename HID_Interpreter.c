#include "HID_Interpreter.h"

/**
 * 6.2.2.7 Global Items page 45 of 97 of hid1_11.pdf
 */ 
#define TYPE_MASK 0b11111100
#define LENGTH_MASK 0b00000011
#define GLOBAL_USAGE_PAGE 0x00000100

#define LOGICAL_MINIMUM 0b00010100  // page 45 of 97 of hid1_11.pdf
#define LOGICAL_MAXIMUM 0b00100100  // page 45 of 97 of hid1_11.pdf

#define USAGE_MINIMUM   0b00011000  // page 50 of 97 of hid1_11.pdf
#define USAGE_MAXIMUM   0b00101000  // page 50 of 97 of hid1_11.pdf

#include <stdio.h>
#define TRACE_DEBUG
#ifdef TRACE_DEBUG

#define HID_DEBUG( format, arg... )    printf( format, ##arg )
#else
#define HID_DEBUG( format, arg... )
#endif //DEBUG


#define HID_ERROR( format, arg... )    printf( format, ##arg )


void print_HID_MOUSE_REPORT(HID_MOUSE_REPORT *report)
{
	uint8_t i;
	HID_DEBUG("Usage : Bytes\n");
	for(i=0;i<report->numUsages;i++)
	{
		HID_DEBUG("0x%X : %d\n",report->usages[i],report->usageBytes[i]);
	}
}

int8_t scan_COMMON_IGNORES(HID_MOUSE_REPORT *report,uint8_t *i)
{
	int8_t status = HID_SUCCESS;
	uint8_t numSkip;
	uint8_t numLoops = 0;
	uint8_t keepGoing = 1;

	
	while(keepGoing)
	{
		HID_DEBUG("COMMON IGNORES: %d: 0x%X\n",*i,report->bytes[*i]);
		if(((report->bytes[*i] & TYPE_MASK) == USAGE_MINIMUM) || ((report->bytes[*i] & TYPE_MASK) == USAGE_MAXIMUM))
		{
			HID_DEBUG("USAGE MAXMIN\n");
			numSkip = report->bytes[*i] & LENGTH_MASK;
			(*i)+=numSkip + 1;
		}
		else if(((report->bytes[*i] & TYPE_MASK) == LOGICAL_MINIMUM) || ((report->bytes[*i] & TYPE_MASK) == LOGICAL_MAXIMUM))
		{
			HID_DEBUG("LOGIC MAXMIN\n");
			numSkip = report->bytes[*i] & LENGTH_MASK;
			(*i)+=numSkip + 1;
		}
		else if(report->bytes[*i] == 0x81)
		{
			HID_DEBUG("FOUND 0x81\n");
			(*i)+=2;
		}
		else if((report->bytes[*i] == 0x05) || (report->bytes[*i] == 0xC0))
		{
			HID_DEBUG("FOUND NEXT\n");
			(*i)--;
			keepGoing = 0;
		}
		else
		{
			keepGoing = 0;
		}
		numLoops++;
		if(numLoops>6)
		{
			keepGoing = 0;
			status = HID_ERROR;
		}
	}
	

	return status;
}


int8_t scan_NUM_BITS(HID_MOUSE_REPORT *report,uint8_t *i,uint8_t *numBits)
{
	int8_t status = HID_SUCCESS;
	uint8_t reportCountSizeFlag = 0;
	uint8_t reportCount, reportSize;
	uint8_t foundBoth = 0;
	uint8_t keepGoing = 1;
	uint8_t numLoops = 0;

	while(keepGoing)
	{
		HID_DEBUG("NUMBITS SEARCH[%d]: 0x%X\n",*i,report->bytes[*i]);
		if(report->bytes[*i] == 0x75) // report size
		{
			
			(*i)++;
			reportSize = report->bytes[*i];
			HID_DEBUG("Found report size: 0x%X\n",reportSize);
			if(reportCountSizeFlag == 1)
			{
				(*numBits) += reportSize*reportCount;
				reportCountSizeFlag = 0;
				foundBoth =1 ;
			}
			else
			{
				reportCountSizeFlag = 1;
			}
			(*i)++;
		}
		
	
		else if(report->bytes[*i] == 0x95) // report count
		{
			
			(*i)++;
			reportCount = report->bytes[*i];
			HID_DEBUG("Found report count: 0x%X\n",reportCount);	
			if(reportCountSizeFlag == 1)
			{
				(*numBits) += reportSize*reportCount;
				reportCountSizeFlag = 0;
				foundBoth =1 ;
			}
			else
			{
				reportCountSizeFlag = 1;
			}
			(*i)++;	
		}
		

		else if(report->bytes[*i] == 0x81)
		{
			HID_DEBUG("Found 0x81 %d\n",*i);
			(*i)+=2;
		}
		else if (report->bytes[*i] == 0x05 || 
			     report->bytes[*i] == 0xC0 ||
			     report->bytes[*i] == 0x09)
		{
			keepGoing = 0;
			HID_DEBUG("Found End %d\n",*i);
		}

		numLoops++;
		if(numLoops>8)
		{
			HID_ERROR("NUM_BITS loops exceeded\n");
			keepGoing =0;
			status = HID_ERROR;
		}
	}

	if(foundBoth == 0)
	{
		status = HID_ERROR;
	}

	return status;
}

/**
 * 
 */ 
int8_t scan_CONSUMER_USAGE_PAGE(HID_MOUSE_REPORT *report,uint8_t *i)
{
	int8_t status = HID_SUCCESS;
	uint8_t usageByte;
	uint8_t reportCount;
	HID_DEBUG("--------------------\n");
	HID_DEBUG("CONSUMER USAGE: %d\n",*i);
	HID_DEBUG("--------------------\n");

	report->usages[report->numUsages] = 0x01 << 8;

	(*i)++;
	if(report->bytes[*i] != 0x0A)
	{
		status = HID_ERROR;
		return status;
	}
	(*i)++;
	if(report->bytes[*i] != 0x38)
	{
		status = HID_ERROR;
	   	return status;
	}
	(*i)++;
	usageByte = report->bytes[*i];
	report->usages[report->numUsages] |= usageByte;

	(*i)++;
	if(report->bytes[*i] == 0x95) // report count
	{
		HID_DEBUG("Found report count\n");
		(*i)++;
		reportCount = report->bytes[*i];	
		(*i)++;	
	}
	else
	{
		status = HID_ERROR;
	   	return status;
	}
		

	if(report->bytes[*i] == 0x81)
	{
		HID_DEBUG("Found 0x81 %d\n",*i);
		(*i)+=2;
	}
	else
	{
		status = HID_ERROR;
	   	return status;
	}


	report->usageBytes[report->numUsages] = reportCount;
	report->numUsages++;
	report->reportBytes+=reportCount;


	return status;
}


/**
 * Usually the X and Y are in a Generic Desktop Ctrls Usage Page but sometimes the wheel is as well.
 */ 
int8_t scan_GENERIC_DESKTOP_MOUSE_CTRLS(HID_MOUSE_REPORT *report,uint8_t *i)
{
	int8_t status = HID_SUCCESS;
	uint8_t numBits = 0;
	uint8_t numDesktopUsages=0;
	uint8_t usageByte;
	uint8_t startUsage;
	uint8_t j;
	uint8_t thisBytes;
	uint8_t keepGoing = 1;
	uint8_t numLoops = 0;

	HID_DEBUG("---------------------------\n");
	HID_DEBUG("DESKTOP_MOUSE_CNTRL\n");
	HID_DEBUG("---------------------------\n");

	

	(*i)++;
	//report->usages[report->numUsages] = usageByte<<8; // Remove this !

	while(keepGoing)
	{
		startUsage = report->numUsages;
		numDesktopUsages=0;
		numBits = 0;
		HID_DEBUG("DESKTOP_MOUSE_CNTRLS LOOP: %d\n",*i);
		while(report->bytes[*i]==0x09)
		{
			(*i)++;
			usageByte = report->bytes[*i];
			report->usages[report->numUsages] = usageByte<<8;
			report->numUsages++;
			(*i)++;
			numDesktopUsages++;
			if(numDesktopUsages>100){break;}// No infinite loops!
		}

		status = scan_COMMON_IGNORES(report,i);
		if(status!=HID_SUCCESS){return status;}

		status = scan_NUM_BITS(report,i,&numBits);
		if(status!=HID_SUCCESS){return status;}

		thisBytes = numBits/(8*numDesktopUsages);
		for(j=0;j<numDesktopUsages;j++)
		{
			report->usageBytes[startUsage+j] = thisBytes;
			report->reportBytes += thisBytes;
		}

		if(report->bytes[*i]==0x09)
		{
			keepGoing = 1;
			numLoops++;
			if(numLoops>5)
			{
				status = HID_ERROR;
				keepGoing=0;
			}
		}
		else
		{
			keepGoing = 0;	
		}		
		

	}
	(*i)--;
	//HID_DEBUG("DESKTOP_MOUSE_CNTRLS END: %d : 0x%X\n",*i,report->bytes[*i]);
	return status;

}

/**
 * A regular un-nested usage like for the buttons.
 */
int8_t scan_REGULAR_USAGE(HID_MOUSE_REPORT *report,uint8_t *i)
{
	int8_t status = HID_SUCCESS;
	uint8_t usageByte;
	uint8_t numBits=0;
	uint8_t thisBytes;
	HID_DEBUG("--------------------\n");
	HID_DEBUG("REGULAR USAGE: %d\n",*i);
	HID_DEBUG("--------------------\n");

	usageByte = report->bytes[*i];
	report->usages[report->numUsages] = usageByte<<8;	
	(*i)++;

	status = scan_COMMON_IGNORES(report,i);
	if(status!=HID_SUCCESS)
	{
		HID_ERROR("COMMON IGNORE PROBLEM\n");
		return status;
	}

	status = scan_NUM_BITS(report,i,&numBits);
	if(status!=HID_SUCCESS)
	{
		HID_ERROR("NUMBITS PROBLEM\n");
		return status;
	}

	thisBytes = numBits/8;
	report->usageBytes[report->numUsages] = thisBytes;
	report->reportBytes += thisBytes;
	report->numUsages++;

	(*i)--;

	return status;
}

/**
 * We look for usages in the report and figure out the number of bytes in each usage.
 */ 
int8_t scan_HID_MOUSE_REPORT(HID_MOUSE_REPORT *report)
{
	int8_t status = HID_SUCCESS;
	uint8_t i;
	uint8_t usageByte;

	report->reportBytes = 0;

	// The first byte is the global type 6.2.2.7 Global Items page 45 of 97 of hid1_11.pdf
	if(report->bytes[0] != 0x05)
	{
		HID_ERROR("REPORT BYTE0 ISSUE\n");
	   	status = HID_ERROR;
	} 

	// The second byte denotes the Generic Desktop Page (0x01) page 18 of 439 in hid1_4.pdf
	if((status == HID_SUCCESS) && (report->bytes[1] != 0x01))
	{
		HID_ERROR("REPORT BYTE1 ISSUE\n");
	   	status = HID_ERROR;
	} 

	// The third byte denotes a usage page 50 of 97 of hid1_11.pdf
	// The fourth byte denotes a mouse (0x02) page 33 of 439 in hid1_4.pdf
	if((report->bytes[2] != 0x09) || (report->bytes[3] != 0x02))
	{
		HID_ERROR("REPORT BYTE23 ISSUE\n");
	   	status = HID_ERROR;
	} 

	// Collection (Application)
	if((report->bytes[4] != 0xA1) || (report->bytes[5] != 0x01))
	{
		HID_ERROR("REPORT BYTE45 ISSUE\n");
	   	status = HID_ERROR;
	} 

	//   Usage (Pointer)
	if((report->bytes[6] != 0x09) || (report->bytes[7] != 0x01))
	{
		HID_ERROR("REPORT BYTE67 ISSUE\n");
	   	status = HID_ERROR;
	} 
	
	//   Collection (Physical)
	if((report->bytes[8] != 0xA1) || (report->bytes[9] != 0x00))
	{
		HID_ERROR("REPORT BYTE89 ISSUE\n");
	   	status = HID_ERROR;
	} 

	report->numUsages = 0;
	if(status == HID_SUCCESS)
	{
		for(i = 10; i<report->len-3; i++)
		{
			if(report->bytes[i] == 0x05)
			{		
				i++;			
				usageByte = report->bytes[i];
				switch(usageByte)
				{
					case 0x0C:
						// this is a special 'Consumer' usage							
						status = scan_CONSUMER_USAGE_PAGE(report,&i);
						break;
					case 0x01: //Usage Page (Generic Desktop Ctrls)
				    		
						status = scan_GENERIC_DESKTOP_MOUSE_CTRLS(report,&i);
						break;
					default:
						status = scan_REGULAR_USAGE(report,&i);
						HID_DEBUG("EXIT_REGULAR_USAGE %d : 0x%X\n",i,report->bytes[i]);					
						break;

				}
				if(status!=HID_SUCCESS){break;}

			
			
			}

		}
	}

	// End Collections
	if(report->bytes[report->len-2] != 0xC0 || report->bytes[report->len-1] != 0xC0)
	{
		HID_ERROR("REPORT END ISSUE\n");
		status = HID_ERROR;
	   	return status;
	}

	return status;
}

/**
 * \brief Read and initialize report data found in buf.
 * 
 * Decide if the report is appropriate for our purposes.
 */ 
int8_t init_HID_MOUSE_REPORT(HID_MOUSE_REPORT *report,uint8_t *buf,uint16_t bufLen)
{
	uint16_t i;
	report->status = HID_SUCCESS;
	if(bufLen>HID_MAX_REPORT_DESCRIP_LEN)
	{
		HID_ERROR("REPORT DESCRIP SIZE ISSUE\n");
		report->status = HID_ERROR;
		return report->status;
	}

	for(i=0;i<bufLen;i++)
	{
		//HID_DEBUG("REPORT[%d][0x%X]\n",i,buf[i]);
		report->bytes[i] = buf[i];
	}
	report->len = bufLen;

	report->status = scan_HID_MOUSE_REPORT(report);

	return report->status;
}


/**
 * \brief Setup the interpreter. Decide if we can actually interpret from input to output for these reports.
 */ 
int8_t init_HID_MOUSE_INTERPRETER(HID_MOUSE_INTERPRETER *interpreter,HID_MOUSE_REPORT *inputReport,HID_MOUSE_REPORT *outputReport)
{
	uint8_t i,j;
	uint16_t thisUsage;
	uint8_t usageFound = 0;
	interpreter->status = HID_SUCCESS;
	if(inputReport->status != HID_SUCCESS || outputReport->status != HID_SUCCESS)
	{
		interpreter->status = HID_ERROR;
		HID_ERROR("Report parsing was not successful!\n");
		return interpreter->status;
	}

	interpreter->inputReport = inputReport;
	interpreter->outputReport = outputReport;

	// Check that the input usages are all in the output and have less than or equal to the same number of bytes.
	for(i=0;i<inputReport->numUsages;i++)
	{
		thisUsage = inputReport->usages[i];
		usageFound = 0;
		for(j=0;j<outputReport->numUsages;j++)
		{
			if(outputReport->usages[j] == thisUsage)
			{
				usageFound = 1;
				if(outputReport->usageBytes[j] < inputReport->usageBytes[i])
				{
					interpreter->status = HID_ERROR;
					HID_ERROR("Usage bytes do not match! [%d][%d] < [%d][%d]\n",j,outputReport->usageBytes[j],i,inputReport->usageBytes[i]);
					return interpreter->status;
				}

				break;
			}
		}
		if(usageFound == 0)
		{
			interpreter->status = HID_ERROR;
			HID_ERROR("Usage not found!\n");
			return interpreter->status;
		}
	}
	

	return interpreter->status;
}


/**
 * \brief Given an interpreter that should work, take an input buffer of length inputLen and interpret it into the output buffer of length outputLen.
 * 
 * What we do is read the input and interpret it by the usages in its report. Then for each usage in the input we find the corresponding usage in the output
 * and fill it in. If the input has less than or equal number of bytes then we are good. Just fill in the un-used bytes with zeros.
 * 
 */ 
int8_t interpret_HID_MOUSE(HID_MOUSE_INTERPRETER *interpreter,
	                 uint8_t *inputBuf,uint8_t inputLen,
	                 uint8_t *outputBuf,uint16_t *outputLen)
{
	int8_t status = HID_SUCCESS;
	uint16_t i,j,k,l,m;
	uint16_t thisUsage;
	uint8_t thisNumInputBytes;
	uint8_t diffBytes;
	uint8_t fillByte = 0;

	if(interpreter->status != HID_SUCCESS)
	{
		*outputLen = 0;
		status = HID_ERROR;
		HID_ERROR("Interpreter not initialized!\n");
		return status;
	}

	if(inputLen != interpreter->inputReport->reportBytes)
	{
		status = HID_ERROR;
		HID_ERROR("Invalid input length %d %d\n",inputLen,interpreter->inputReport->reportBytes);
		return status;
	}
	*outputLen = interpreter->outputReport->reportBytes;
	for(i=0;i<*outputLen;i++)
	{
		outputBuf[i] = 0;
	}

	for(i=0,k=0,l=0;i<interpreter->inputReport->numUsages;i++)
	{
		thisUsage = interpreter->inputReport->usages[i];
		for(j=0;j<interpreter->outputReport->numUsages;j++)
		{
			if(interpreter->outputReport->usages[j] == thisUsage)
			{ // found this usage
				// Copy the input bytes to the output.
				HID_DEBUG("Found Usage: 0x%X\n",thisUsage);
				thisNumInputBytes = interpreter->inputReport->usageBytes[i];
				diffBytes = interpreter->outputReport->usageBytes[j] - thisNumInputBytes;				
				for(m=0;m<thisNumInputBytes;m++)
				{
					
					outputBuf[l] = inputBuf[k];
					
					k++;					
					l++;
				}
				if(diffBytes>0)
				{
					if(inputBuf[k-1]&0b10000000)
						{fillByte = 0xFF;}
					else{
						fillByte= 0x00;
					}
					for(m=0;m<diffBytes;m++)
					{
						outputBuf[l] = fillByte; // stuff first bytes with zeros.
						l++;
					}
				}
				
				break; // out of the for loop with j
			}
		}
	}

	return status;
}