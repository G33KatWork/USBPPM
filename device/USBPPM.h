#ifndef _FLASHER_H_
#define _FLASHER_H_

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>

#include <LUFA/Version.h>
#include <LUFA/Drivers/Board/LEDs.h>
#include <LUFA/Drivers/USB/USB.h>
#include <LUFA/Drivers/Board/Buttons.h>

#include "Descriptors.h"
#include "PPM.h"
#include "Debug.h"

/** LED mask for the library LED driver, to indicate that the USB interface is not ready. */
#define LEDMASK_USB_NOTREADY      LEDS_LED1

/** LED mask for the library LED driver, to indicate that the USB interface is enumerating. */
#define LEDMASK_USB_ENUMERATING  (LEDS_LED2 | LEDS_LED3)

/** LED mask for the library LED driver, to indicate that the USB interface is ready. */
#define LEDMASK_USB_READY        (LEDS_LED2 | LEDS_LED4)

/** LED mask for the library LED driver, to indicate that an error has occurred in the USB interface. */
#define LEDMASK_USB_ERROR        (LEDS_LED1 | LEDS_LED3)

/** LED mask for the library LED driver, to indicate that an error has occurred in the USB interface. */
#define LEDMASK_USB_BUSY        (LEDS_LED1 | LEDS_LED2)


/* Vendor specific requests */
#define REQ_SINGLECHANNEL		0x01
#define REQ_ALLCHANNELS			0x02
#define REQ_GETCHANNELCOUNT		0x03
#define REQ_UPDATE				0xF0


typedef void (*BootloaderPtr)(void) __attribute__ ((noreturn));
BootloaderPtr RunBootloader = (BootloaderPtr)BOOT_START_ADDR;

void SetupHardware(void);
void USBPPM_Task(void);

void EVENT_USB_Device_ConfigurationChanged(void);
void EVENT_USB_Device_Connect(void);
void EVENT_USB_Device_Disconnect(void);
void EVENT_USB_Device_ConfigurationChanged(void);
void EVENT_USB_Device_ControlRequest(void);

#endif
