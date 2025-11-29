//cpp library 
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <cmath>

extern "C"{
	//c library in this part
	#include <linux/uinput.h>
	#include <at-spi-2.0/atspi/atspi.h>
	#include <fcntl.h>
	#include <unistd.h>
	#include <string.h>
}

using namespace std;


//detect clickable ellement
void det_d();
//assign them sort, key hint  
void assign_them();
//capture keyboard input map it those ellement
void capture_key();
//simulate mouse click and focus change automatically
void stim_mouse_click();
//mouse navigation 
void mouseMove(int fd);
//k
void emit(int fd ,int type ,int code ,int val );

int main(){
	
	struct uinput_setup usetup;
								//write only //non blocking 
   	int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK); 
	if(fd < 0) {
        cout << "Error opening /dev/uinput. Try 'sudo'?" << endl;
        return -1;
    }

   /*
    * The ioctls below will enable the device that is about to be
    * created, to pass key events, in this case the space key.
    */
	//ENABLE BOTTON 
   	ioctl(fd, UI_SET_EVBIT, EV_KEY);
    ioctl(fd, UI_SET_KEYBIT, BTN_LEFT);
	
	//2.ENABLE MOVEMENT
	ioctl(fd, UI_SET_EVBIT, EV_REL);
    ioctl(fd, UI_SET_RELBIT, REL_X);
    ioctl(fd, UI_SET_RELBIT, REL_Y);

	memset(&usetup, 0, sizeof(usetup));
	usetup.id.bustype = BUS_USB;
	usetup.id.vendor = 0x1234; /* sample vendor */
	strcpy(usetup.name, "Cpp virtual mouse");

	ioctl(fd, UI_DEV_SETUP, &usetup);
	ioctl(fd, UI_DEV_CREATE);

	/*
		* On UI_DEV_CREATE the kernel will create the device node for this
		* device. We are inserting a pause here so that userspace has time
		* to detect, initialize the new device, and can start listening to
		* the event, otherwise it will not notice the event we are about
		* to send. This pause is only needed in our example code!
		*/
	sleep(1);


	/*
		* Give userspace some time to read the events before we destroy the
		* device with UI_DEV_DESTOY.
		*/
	mouseMove(fd);

	ioctl(fd, UI_DEV_DESTROY);
	close(fd);

	return 0;
   int Height = 800;
   int Width = 900;
   mouseMove(Width,Height);
 	cout<< " Hello World! ,my navigation with cpp "<<endl;
	
}

void emit(int fd, int type, int code, int val){
	struct input_event ie;
	
	memset(&ie , 0 ,sizeof(ie));
	
	ie.code = code;
	ie.value = val;
	ie.type = type;
	
	write(fd , &ie, sizeof(ie));
}



void mouseMove(int fd){
  float angle = 0.0f;
  float radios = 5.0f;
  int Height = 0;
  int Width = 0;

  //calcualate the difference
  int prevY = 0;
  int prevX = 0;

  cout << "Starting spiral ......"<<endl;
  
  for (int  i = 0; i < 100; i++) {
    
  }

} 
