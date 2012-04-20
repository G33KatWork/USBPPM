#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include <libusb.h>
#include <SDL.h>

#include "USB.h"

#define UPDATE_MS 20

#define NUM_AXIS        4
#define NUM_AUX         3
#define OUT_MAX         100
#define OUT_MIN         -100
#define IN_MAX          SHRT_MAX
#define IN_MIN          SHRT_MIN

//PPM-Channels
#define RFCHAN_THROTTLE  0
#define RFCHAN_ROLL      1
#define RFCHAN_PITCH     2
#define RFCHAN_YAW       3
#define RFCHAN_AUX1      4
#define RFCHAN_AUX2      5
#define RFCHAN_AUX3      6

//Controller
#define AXIS_THROTTLE     1
#define AXIS_ROLL         2
#define AXIS_PITCH        3
#define AXIS_YAW          0

#define BTN_AUX1          0
#define BTN_AUX2          1
#define BTN_AUX3          2
#define BTN_AUX4          3

#define INVERT_THROTTLE   -1
#define INVERT_PITCH      -1
#define INVERT_ROLL       1
#define INVERT_YAW        1

static struct libusb_device_handle *devh = NULL;

#define MAP(X, INMIN, INMAX, OUTMIN, OUTMAX) \
  ((X - INMIN) * (OUTMAX - OUTMIN) / (INMAX - INMIN) + OUTMIN)

typedef struct {
  int8_t throttle;
  int8_t pitch;
  int8_t roll;
  int8_t yaw;
  int8_t aux1;
  int8_t aux2;
  int8_t aux3;
  int8_t aux4;
} ControllerState;

int8_t getAxis(SDL_Joystick* joystick, int axis)
{
  short a = SDL_JoystickGetAxis(joystick, axis);
  printf("Axis %i: %i\n", axis, a);
  return (int8_t)MAP(a, IN_MIN, IN_MAX, OUT_MIN, OUT_MAX);
}

int8_t getBtn(SDL_Joystick* joystick, int btn)
{
  return SDL_JoystickGetButton(joystick, btn) > 0 ? -1 : 1;
}

int main(int argc, char* argv[])
{
  int res = SDL_InitSubSystem(SDL_INIT_JOYSTICK);
  if (res < 0) {
      fprintf(stderr, "Unable to initialize Joystick: %s\n", SDL_GetError());
      return -1;
  }

  if(SDL_NumJoysticks() <= 0) {
    fprintf(stderr, "No joysticks found. Exiting...\n");
    return -2;
  }
  printf("%i joysticks found\n", SDL_NumJoysticks());

  SDL_Joystick* joy = SDL_JoystickOpen(0);
  if(joy == NULL) {
    fprintf(stderr, "Joystick couldn't be opened\n");
    return -3;
  }

  if(SDL_JoystickNumAxes(joy) < NUM_AXIS) {
    fprintf(stderr, "Joystick hasn't enough axes. Need %u\n", NUM_AXIS);
    return -4;
  }

	res = libusb_init(NULL);
	if(res < 0) {
    fprintf(stderr, "Unable to intialize libusb\n");
    return -5;
  }
	
	devh = libusb_open_device_with_vid_pid(NULL, 0x1337, 0x1);
	if(devh == 0) {
    fprintf(stderr, "Unable to find USBPPM device\n");
    return -6;
  }
	
	res = libusb_claim_interface(devh, 0);
	if (res < 0) {
		fprintf(stderr, "usb_claim_interface error %d\n", res);
		return -7;
	}
	printf("claimed interface\n");

  uint8_t channelCount;
	res = libusb_control_transfer(devh, CTRL_IN, REQUEST_GETCHANNELCOUNT, 0, 0, (unsigned char*)&channelCount, sizeof(channelCount), 0);
	if(res < 0) {
    fprintf(stderr, "libusb_control_transfer error %d\n", res);
    return -8;
  }
  printf("Received channel count %u\r\n", channelCount);

  if(NUM_AUX + NUM_AXIS > channelCount) {
    fprintf(stderr, "You want to transmit %d channels, but receiver only supports %u\n", NUM_AXIS+NUM_AUX, channelCount);
    return -9;
  }

  int8_t* channelData = malloc(channelCount * sizeof(int8_t));
  memset(channelData, 0, channelCount * sizeof(int8_t));

  ControllerState* ctrlState = malloc(sizeof(ControllerState));
  memset(ctrlState, 0, sizeof(ControllerState));
  ctrlState->aux1 = -100;
  ctrlState->aux2 = -100;
  ctrlState->aux3 = -100;
  ctrlState->aux4 = -100;


  SDL_JoystickEventState(SDL_QUERY);

  while(1) {
    SDL_Event event;
    SDL_PollEvent(&event);
    if(event.type == SDL_QUIT)
      break;

    /*int transferred;
    channelData[THROTTLE]   = 000;
    channelData[YAW]        = 000;
    channelData[ROLL]       = 000;
    channelData[PITCH]      = 000;
    channelData[AUX1]       = 000;
    channelData[AUX2]       = 000;
    channelData[AUX3]       = 000;
    libusb_interrupt_transfer(devh, USBPPM_STREAM_OUT_EPNUM, (unsigned char*)channelData, channelCount, &transferred, 0);*/

    clock_t start = clock();
    SDL_JoystickUpdate();

    ctrlState->throttle = getAxis(joy, AXIS_THROTTLE) * INVERT_THROTTLE;
    ctrlState->pitch = getAxis(joy, AXIS_PITCH) * INVERT_PITCH;
    ctrlState->yaw = getAxis(joy, AXIS_YAW) * INVERT_YAW;
    ctrlState->roll = getAxis(joy, AXIS_ROLL) * INVERT_ROLL;
  
    ctrlState->aux1 *= getBtn(joy, BTN_AUX1);
    ctrlState->aux2 *= getBtn(joy, BTN_AUX2);
    ctrlState->aux3 *= getBtn(joy, BTN_AUX3);
    ctrlState->aux4 *= getBtn(joy, BTN_AUX4);

    channelData[RFCHAN_THROTTLE]   = ctrlState->throttle;
    channelData[RFCHAN_YAW]        = ctrlState->yaw;
    channelData[RFCHAN_ROLL]       = ctrlState->roll;
    channelData[RFCHAN_PITCH]      = ctrlState->pitch;
    channelData[RFCHAN_AUX1]       = ctrlState->aux1;
    channelData[RFCHAN_AUX2]       = ctrlState->aux2;
    channelData[RFCHAN_AUX3]       = ctrlState->aux3;

    printf(
      "Throttle: %i - Pitch: %i - Yaw: %i - Roll: %i - Aux1: %i\n",
      ctrlState->throttle,
      ctrlState->pitch,
      ctrlState->yaw,
      ctrlState->roll,
      ctrlState->aux1
    );

    int transferred;
    res = libusb_interrupt_transfer(devh, USBPPM_STREAM_OUT_EPNUM, (unsigned char*)channelData, channelCount, &transferred, 0);
    
    if(res < 0) {
      fprintf(stderr, "libusb_control_transfer error %d\n", res);
      return -10;
    }

    if(transferred != channelCount) {
      fprintf(stderr, "ERROR: Attempted to transmit %u Byte, but only %u transmitted", channelCount, transferred);
      return -11;
    }

    while(((double)(clock() - start) / CLOCKS_PER_SEC*1000.0) < UPDATE_MS);
    
    clock_t end = clock();
    double ms = (double)(end - start) / CLOCKS_PER_SEC*1000.0;
    printf("%f ms\r\n", ms);
  }

  SDL_Quit();
  libusb_release_interface(devh, 0);
  libusb_close(devh);
  free(channelData);
  free(ctrlState);
  exit(0);
}
