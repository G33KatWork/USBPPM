#include "Descriptors.h"

USB_Descriptor_Device_t PROGMEM DeviceDescriptor =
{
	.Header                 = {.Size = sizeof(USB_Descriptor_Device_t), .Type = DTYPE_Device},

	.USBSpecification       = VERSION_BCD(02.00),
	.Class                  = 0xFF,
	.SubClass               = 0x00,
	.Protocol               = 0x00,

	.Endpoint0Size          = FIXED_CONTROL_ENDPOINT_SIZE,

	.VendorID               = 0x1337,
	.ProductID              = 0x0001,
	.ReleaseNumber          = 0x0100,

	.ManufacturerStrIndex   = 0x01,
	.ProductStrIndex        = 0x02,
	.SerialNumStrIndex      = NO_DESCRIPTOR,

	.NumberOfConfigurations = FIXED_NUM_CONFIGURATIONS
};

USB_Descriptor_Configuration_t PROGMEM ConfigurationDescriptor =
{
	.Config = 
		{
			.Header                 = {.Size = sizeof(USB_Descriptor_Configuration_Header_t), .Type = DTYPE_Configuration},

			.TotalConfigurationSize = sizeof(USB_Descriptor_Configuration_t),
			.TotalInterfaces        = 1,

			.ConfigurationNumber    = 1,
			.ConfigurationStrIndex  = NO_DESCRIPTOR,

			.ConfigAttributes       = USB_CONFIG_ATTR_RESERVED,

			.MaxPowerConsumption    = USB_CONFIG_POWER_MA(100)
		},

	.Interface = 
		{
			.Header                 = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},

			.InterfaceNumber        = 0x00,
			.AlternateSetting       = 0x00,

			.TotalEndpoints         = 1,

			.Class                  = 0xFF,
			.SubClass               = 0x00,
			.Protocol               = 0x00,

			.InterfaceStrIndex      = NO_DESCRIPTOR
		},

	.OUTEndpoint = 
		{
			.Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

			.EndpointAddress        = (ENDPOINT_DIR_OUT | USBPPM_STREAM_OUT_EPNUM),
    		.Attributes             = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
			.EndpointSize           = USBPPM_EPSIZE,
			.PollingIntervalMS      = 0x01
		},
};

USB_Descriptor_String_t PROGMEM LanguageString =
{
	.Header                 = {.Size = USB_STRING_LEN(1), .Type = DTYPE_String},

	.UnicodeString          = {LANGUAGE_ID_ENG}
};

USB_Descriptor_String_t PROGMEM ManufacturerString =
{
	.Header                 = {.Size = USB_STRING_LEN(10), .Type = DTYPE_String},

	.UnicodeString          = L"G33KatWork"
};

USB_Descriptor_String_t PROGMEM ProductString =
{
	.Header                 = {.Size = USB_STRING_LEN(6), .Type = DTYPE_String},

	.UnicodeString          = L"PPMUSB"
};

uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
                                    const uint8_t wIndex,
                                    const void** const DescriptorAddress)
{
	const uint8_t  DescriptorType   = (wValue >> 8);
	const uint8_t  DescriptorNumber = (wValue & 0xFF);

	void*    Address = NULL;
	uint16_t Size    = NO_DESCRIPTOR;

	switch (DescriptorType)
	{
		case DTYPE_Device:
			Address = (void*)&DeviceDescriptor;
			Size    = sizeof(USB_Descriptor_Device_t);
			break;
		case DTYPE_Configuration:
			Address = (void*)&ConfigurationDescriptor;
			Size    = sizeof(USB_Descriptor_Configuration_t);
			break;
		case DTYPE_String:
			switch (DescriptorNumber)
			{
				case 0x00:
					Address = (void*)&LanguageString;
					Size    = pgm_read_byte(&LanguageString.Header.Size);
					break;
				case 0x01:
					Address = (void*)&ManufacturerString;
					Size    = pgm_read_byte(&ManufacturerString.Header.Size);
					break;
				case 0x02:
					Address = (void*)&ProductString;
					Size    = pgm_read_byte(&ProductString.Header.Size);
					break;
			}
			break;
	}

	*DescriptorAddress = Address;		
	return Size;
}
