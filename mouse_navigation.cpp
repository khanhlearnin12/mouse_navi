 //how to compile 
//g++ mouse_navigation.cpp -o my_mouse $(pkg-config --cflags --libs atspi-2 glib-2.0)

#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <cmath>
#include <vector>
#include <string>

// C headers must be wrapped in extern "C"
extern "C"{
    #include <linux/uinput.h>
    #include <atspi/atspi.h> // Standard path for atspi
    #include <fcntl.h>
    #include <unistd.h>
    #include <string.h>
    #include <glib.h>
}

using namespace std;

// Create struct to store our value
struct Target {
    string name;
    int x, y;
};

// --- Function Prototypes ---
// (Fixed: Added types to arguments)
void find_clickable(AtspiAccessible* node, vector<Target>& results);
void det_d(); 
void mouseMove(int fd);
void emit(int fd, int type, int code, int val);

int main(){
    // 1. Setup Virtual Mouse (UInput)
    struct uinput_setup usetup;
    
    // Open in non-blocking mode
    int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK); 
    if(fd < 0) {
        cout << "Error opening /dev/uinput. Try running with 'sudo'?" << endl;
        return -1;
    }

    // Enable Button
    ioctl(fd, UI_SET_EVBIT, EV_KEY);
    ioctl(fd, UI_SET_KEYBIT, BTN_LEFT);

    // Enable Movement
    ioctl(fd, UI_SET_EVBIT, EV_REL);
    ioctl(fd, UI_SET_RELBIT, REL_X);
    ioctl(fd, UI_SET_RELBIT, REL_Y);

    memset(&usetup, 0, sizeof(usetup));
    usetup.id.bustype = BUS_USB;
    usetup.id.vendor = 0x1234; 
    strcpy(usetup.name, "Cpp virtual mouse");

    ioctl(fd, UI_DEV_SETUP, &usetup);
    ioctl(fd, UI_DEV_CREATE);

    // Wait for system to register device
    sleep(1);

    // 2. Scan the screen for buttons (Added this call)
    det_d();

    // 3. Move the mouse in a spiral
    mouseMove(fd);

    // 4. Cleanup
    ioctl(fd, UI_DEV_DESTROY);
    close(fd);

    return 0;
}

void det_d(){
    // Initialize ATSPI
    if (atspi_init() != 0) {
        cout << "Failed to initialize ATSPI" << endl;
        return;
    }
 
    // Get the desktop
    AtspiAccessible* desktop = atspi_get_desktop(0);    
    vector<Target> buttons;
  
    cout << "Screen scanning... (this might take a second)" << endl;

    find_clickable(desktop, buttons);
  
    cout << "Found " << buttons.size() << " clickable items!" << endl;
  
    // Print first 5 items found
    for (int i = 0; i < buttons.size() && i < 5; i++) {
         cout << "Name: " << buttons[i].name  
              << " | X: " << buttons[i].x << " Y: " << buttons[i].y << endl; 
    }
}

void find_clickable(AtspiAccessible* node, vector<Target>& results) {
    GError* err = NULL;
  
    // check validity
    if (!node) return;

    // get attribute
    AtspiRole role = atspi_accessible_get_role(node, &err); 
    AtspiStateSet* states = atspi_accessible_get_state_set(node); 

    // Filter: is this node clickable?
    bool is_clickable = (role == ATSPI_ROLE_PUSH_BUTTON || role == ATSPI_ROLE_LINK);
    
    // Check if visible
    bool is_visible = atspi_state_set_contains(states, ATSPI_STATE_VISIBLE) 
                   && atspi_state_set_contains(states, ATSPI_STATE_SHOWING);

    // IF MATCH: Get Coordinates and Save
    if (is_clickable && is_visible) {
        // Get the Component interface to read X/Y positions
        AtspiComponent* comp = atspi_accessible_get_component_iface(node);
        if (comp) {
            AtspiRect* rect = atspi_component_get_extents(comp, ATSPI_COORD_TYPE_SCREEN, &err);
            
            // Fixed: used '->' because rect is a pointer
            int centerX = rect->x + (rect->width / 2);
            int centerY = rect->y + (rect->height / 2);
            
            char* name = atspi_accessible_get_name(node, &err);
            
            // Sanity check
            if (centerX > 0 && centerY > 0) { 
                if (name) {
                    results.push_back({ string(name), centerX, centerY });
                } else {
                    results.push_back({ "Unknown", centerX, centerY });
                }
            }
            
            g_free(name); 
            g_free(rect); // Free the rect structure
        }
    }

    // Recursion: Check all children
    int child_count = atspi_accessible_get_child_count(node, &err);
    for (int i = 0; i < child_count; i++) {
        AtspiAccessible* child = atspi_accessible_get_child_at_index(node, i, &err);
        find_clickable(child, results);
        g_object_unref(child); 
    }
    
    g_object_unref(states);
}

// Low-level write to uinput
void emit(int fd, int type, int code, int val){
    struct input_event ie;
    memset(&ie, 0, sizeof(ie));
    ie.type = type;
    ie.code = code;
    ie.value = val;
    write(fd, &ie, sizeof(ie));
}

void mouseMove(int fd){
      float angle = 0.0f;
      float radius = 5.0f;

      // calculate the difference
      int prevY = 0;
      int prevX = 0;

      cout << "Starting spiral mouse movement..." << endl;
      
      for (int  i = 0; i < 100; i++) {
        angle += 0.2f;
        radius += 0.5f; // Increased speed slightly

        int targetX = (int)(radius * cos(angle));
        int targetY = (int)(radius * sin(angle));

        // calculate DELTA (relative movement)
        int moveX = targetX - prevX;
        int moveY = targetY - prevY;
         
        emit(fd, EV_REL, REL_X, moveX);
        emit(fd, EV_REL, REL_Y, moveY);
        emit(fd, EV_SYN, SYN_REPORT, 0); // Fixed constant name

        // update previous position
        prevX = targetX;
        prevY = targetY;

        usleep(20000); // Faster animation
      }
}
