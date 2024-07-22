#include<wayland-client.h>
#include "../include/pointer.h"
#include<stdio.h>

enum Pointerevents{
    INE_ONE
};

void pointer_enter_event(
    void *data,
    struct wl_pointer *pointer,
    uint32_t serial,
    struct wl_surface *surface,
    wl_fixed_t surface_x,
    wl_fixed_t surface_y
){
    // printf("yes pointer event enter");

}

void pointer_leave_event(void *data,
		      struct wl_pointer *wl_pointer,
		      uint32_t serial,
		      struct wl_surface *surface)
{
    // printf("This is pointer leave event.\n");
}


void pointer_motion_even(void *data,
    struct wl_pointer *wl_pointer,
    uint32_t time,
    wl_fixed_t surface_x,
    wl_fixed_t surface_y
){
    // printf("this is pointer motion event.\n");
    printf("%0.04lf %0.4lf\n", wl_fixed_to_double(surface_x), wl_fixed_to_double(surface_y));
}

void pointer_button_event(void *data,
    struct wl_pointer *wl_pointer,
    uint32_t serial,
    uint32_t time,
    uint32_t button,
    uint32_t state
){
    // printf("this is pointer button event\n");
}

void pointer_axis_event(void *data,
    struct wl_pointer *wl_pointer,
    uint32_t time,
    uint32_t axis,
    wl_fixed_t value
){
    // printf("this is pointer axis event.\n");
}

void pointer_frame_event(void *data,
		      struct wl_pointer *wl_pointer){
                  // printf("this is pointer frame event.\n");
}

void pointer_axis_source_event(void *data,
			    struct wl_pointer *wl_pointer,
			    uint32_t axis_source){
                    // printf("this is pointer axis source event.\n");
}

void pointer_axis_top_event(void *data,
			  struct wl_pointer *wl_pointer,
			  uint32_t time,
			  uint32_t axis){
                    // printf("this is pointer aixs stop event.\n");
}

void pointer_axis_discrete_event(void *data,
			      struct wl_pointer *wl_pointer,
			      uint32_t axis,
			      int32_t discrete){
                      // printf("this is pointer axis discrete event.\n");
                }

void pointer_axis120_event(void *data,
			      struct wl_pointer *wl_pointer,
			      uint32_t axis,
			      int32_t value120){
                      // printf("this is pointer value 120 event. %d\n", value120);
                }

void pointer_axis_relative_direction_event(void *data,
					struct wl_pointer *wl_pointer,
					uint32_t axis,
					uint32_t direction){
                        // printf("this is pointer relative direction event.\n");
                    }


