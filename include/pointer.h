#ifndef _POINTER_H
#define _POINTER_H

#include <wayland-util.h>

enum ButtonState{
    BUTTON_STATE_UP=0,
    BUTTON_STATE_DOWN=1
};

enum PointerD{
    POINTER_BUTTON_CLICK=1,
    POINTER_MOUSE_MOVE=2,
    POINTER_MOUSE_LEAVE=4
};
void pointer_enter_event(
    void *data,
    struct wl_pointer *wl_pointer,
    uint32_t serial,
    struct wl_surface *surface,
    wl_fixed_t surface_x,
    wl_fixed_t surface_y
);

void pointer_leave_event(void *data,
    struct wl_pointer *wl_pointer,
    uint32_t serial,
    struct wl_surface *surface
);

void pointer_motion_even(void *data,
    struct wl_pointer *wl_pointer,
    uint32_t time,
    wl_fixed_t surface_x,
    wl_fixed_t surface_y
);

void pointer_button_event(void *data,
    struct wl_pointer *wl_pointer,
    uint32_t serial,
    uint32_t time,
    uint32_t button,
    uint32_t state
);

void pointer_axis_event(void *data,
    struct wl_pointer *wl_pointer,
    uint32_t time,
    uint32_t axis,
    wl_fixed_t value
);

void pointer_frame_event(void *data,
		      struct wl_pointer *wl_pointer);

void pointer_axis_source_event(void *data,
			    struct wl_pointer *wl_pointer,
			    uint32_t axis_source);

void pointer_axis_top_event(void *data,
			  struct wl_pointer *wl_pointer,
			  uint32_t time,
			  uint32_t axis);

void pointer_axis_discrete_event(void *data,
			      struct wl_pointer *wl_pointer,
			      uint32_t axis,
			      int32_t discrete);

void pointer_axis120_event(void *data,
			      struct wl_pointer *wl_pointer,
			      uint32_t axis,
			      int32_t value120);

void pointer_axis_relative_direction_event(void *data,
					struct wl_pointer *wl_pointer,
					uint32_t axis,
					uint32_t direction);



#endif // _POINTER_H
