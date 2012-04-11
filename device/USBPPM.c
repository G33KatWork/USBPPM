#include "USBPPM.h"

int main(void)
{
	SetupHardware();
	LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);

	sei();

	for (;;)
	{
		USBPPM_Task();
		USB_USBTask();
	}
}

void SetupHardware(void)
{
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable clock division */
	clock_prescale_set(clock_div_1);

	LEDs_Init();
	Debug_Init();
	PPM_Init();

	USB_Init();
}

void USBPPM_Task()
{
	if (USB_DeviceState != DEVICE_STATE_Configured)
		return;

	Endpoint_SelectEndpoint(USBPPM_STREAM_OUT_EPNUM);
	if (Endpoint_IsOUTReceived())
	{
		LEDs_SetAllLEDs(LEDMASK_USB_BUSY);

		if (Endpoint_IsReadWriteAllowed())
		{
			int8_t allChannelsData[RC_MAX_CHANNEL];
			Endpoint_Read_Stream_LE(&allChannelsData, sizeof(allChannelsData), NULL);
			for(int i = 0; i < RC_MAX_CHANNEL; i++)
			{
				dbg_P(PSTR("Chan %i: %i\r\n"), i, allChannelsData[i]);
				rc_ch[i] = allChannelsData[i] - 8;
			}
		}
		Endpoint_ClearOUT();

		LEDs_SetAllLEDs(LEDMASK_USB_READY);
	}
}

void EVENT_USB_Device_Connect(void)
{
	LEDs_SetAllLEDs(LEDMASK_USB_ENUMERATING);
	dbg_P(PSTR("Device connected\r\n"));
}

void EVENT_USB_Device_Disconnect(void)
{
	LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
	dbg_P(PSTR("Device disconnected\r\n"));
}

void EVENT_USB_Device_ConfigurationChanged(void)
{
	LEDs_SetAllLEDs(LEDMASK_USB_READY);
	dbg_P(PSTR("Configuration changed\r\n"));

	if (!(Endpoint_ConfigureEndpoint(USBPPM_STREAM_OUT_EPNUM, EP_TYPE_INTERRUPT,
		                             ENDPOINT_DIR_OUT, USBPPM_EPSIZE,
	                                 ENDPOINT_BANK_SINGLE)))
	{
		LEDs_SetAllLEDs(LEDMASK_USB_ERROR);
		dbg_P(PSTR("Output endpoint configuration failed\r\n"));
	}
}

void EVENT_USB_Device_ControlRequest(void)
{
	if(USB_ControlRequest.bmRequestType != (REQDIR_HOSTTODEVICE | REQDIR_DEVICETOHOST | REQTYPE_VENDOR | REQTYPE_STANDARD))
		return;

	dbg_P(PSTR("Received vendor control request\r\n"));
	LEDs_SetAllLEDs(LEDMASK_USB_BUSY);

	int8_t singleChannelData[2];
	int8_t allChannelsData[RC_MAX_CHANNEL];

	switch(USB_ControlRequest.bRequest)
	{
		case REQ_SINGLECHANNEL:
			Endpoint_ClearSETUP();
			Endpoint_Read_Control_Stream_LE(&singleChannelData, sizeof(singleChannelData));
			
			dbg_P(PSTR("Received vendor request REQ_SINGLECHANNEL: %x - Chan: %x, Val: %x\r\n"), USB_ControlRequest.bRequest, singleChannelData[0], singleChannelData[1]);
			if(singleChannelData[0] < RC_MAX_CHANNEL && singleChannelData[0] >= 0)
				rc_ch[singleChannelData[0]] = singleChannelData[1] - 8;
			
			Endpoint_ClearOUT();
			Endpoint_ClearStatusStage();
			break;

		case REQ_ALLCHANNELS:
			Endpoint_ClearSETUP();
			Endpoint_Read_Control_Stream_LE(&allChannelsData, sizeof(allChannelsData));

			dbg_P(PSTR("Received vendor request REQ_ALLCHANNELS: %x\r\n"), USB_ControlRequest.bRequest);
			
			for(uint8_t i = 0; i < RC_MAX_CHANNEL; i++)
			{
				dbg_P(PSTR("Chan %i: %i\r\n"), i, allChannelsData[i]);
				rc_ch[i] = allChannelsData[i] - 8;
			}

			Endpoint_ClearOUT();
			Endpoint_ClearStatusStage();
			break;

		case REQ_GETCHANNELCOUNT:
			Endpoint_ClearSETUP();
			
			dbg_P(PSTR("Received vendor request REQ_GETCHANNELCOUNT: %x\r\n"), USB_ControlRequest.bRequest);
			Endpoint_Write_8(RC_MAX_CHANNEL);
			
			Endpoint_ClearIN();
			Endpoint_ClearStatusStage();
			break;

		case REQ_UPDATE:
			Endpoint_ClearSETUP();
			Endpoint_ClearStatusStage();
			dbg_P(PSTR("Jumping to bootloader. I hope we'll meet again!\r\n"));
			USB_Disable();
			RunBootloader();
			break;
	}

	LEDs_SetAllLEDs(LEDMASK_USB_READY);
}
