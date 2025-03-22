#define STB_IMAGE_IMPLEMENTATION

#include <EGL/eglplatform.h>
#include <bits/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <wayland-client.h>
#include <wayland-egl-core.h>
#include <wayland-egl.h>
#include <EGL/egl.h>
#include <GL/glew.h>
#include "../include/path.h"
#include "../include/log.h"
#include "../include/xdg-shell-client.h"
#include "../include/pointer.h"
#include "../include/xdg-client-output-z.h"
#include "../include/layer-shell-unstable-v1.h"
#include "../include/stb_image.h"
#include <linux/input-event-codes.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>


#define str(x) x->name
#define EXIT_FAIL(X, Y) if (!X) {LOG(5, Y, ERROR); goto finish;}
#define CHECK(X, Y) if (!X) {LOG(5, Y, FATAL); return 0;}
#define FREE(X, Y) if(X) Y(X);
#define message(X) LOG(5, X, MESSAGE)

struct Paths{
    path *parent_path;
    path *vertex_path;
    path *fragment_path;
    path *wall1_path;
    path *wall2_path;
} filepath;

enum UNIFORMS{
    U_TIME,
    U_RESOLUTION,
    U_MOUSE,
    U_TEX_0,
    U_TEX_1
};

struct __appdata{
    int width, height;
    short closed, render;
    double currentTime, initalTime, renderDuration, bound, wallstate;
    struct wl_display* display;
    struct wl_surface* surface;
    struct wl_compositor* compositor;
    struct wl_registry* registry;
    struct wl_seat* seat;
    struct wl_egl_window* egl_window;
    struct wl_callback *render_callback;
    struct wl_output* output;
    struct wl_callback *cb;
    struct zwlr_layer_shell_v1* layer_shell;
} Runtime;

struct __dev{
    struct wl_pointer *pointer;
    struct wl_keyboard *keyboard;
} Devices;



struct __eglconfig{
    EGLConfig config;
    EGLDisplay display;
    EGLContext context;
    EGLSurface surface;

} EGLData;

struct __xdg_desktop{
    struct xdg_toplevel* toplevel_window;
    struct xdg_surface* x_surface;
    struct xdg_wm_base* base;
} XDGFrame;

struct __runtime{
    struct wl_registry_listener registry_listener;
    struct xdg_toplevel_listener toplevel_listener;
    struct xdg_surface_listener surface_listener;
    struct xdg_wm_base_listener wm_base_listener;
    struct wl_callback_listener render_listener;
    struct wl_seat_listener seat_listener;
    struct wl_pointer_listener pointer_listener;
    struct zxdg_output_v1_listener output_listener;
} Listeners;

struct unstable{
    struct zxdg_output_manager_v1* output_manager;
    struct zxdg_output_v1* output;
} Misc;

struct shader{
    GLint shader_program;
    GLint vertex_shader;
    GLint fragment_shader;
    GLuint VAO, VBO;
    GLuint tex1, tex2;
    struct {
        float w1, h1, w2, h2;
    } texsize;
} Shader;

struct _uniforms{
    GLint u_time;
    GLint u_resolution;
    GLint u_mouse;
    GLint u_tex_0;
    GLint u_tex_1;
    GLint u_texsize;
} Uniforms;

struct pointer_data{
    uint32_t events, button, button_state;
    double surface_x, surface_y;
} PointerData;

float mesh[] = {
        -1.0, -1.0,
         1.0, -1.0,
         1.0,  1.0,
         1.0,  1.0,
        -1.0,  1.0,
        -1.0, -1.0
};

struct timespec tp;
//bind functions

int initRuntime();
int bindListeners();
int eglInit();
int loadPaths(const char *parent, struct Paths *path);
int bindSeat();
int bindDevices();
int createAllSurface();
int attachXDGSurface();
int bindXDGOutput();
int freeShaders();
GLint loadShader(path *mypath, GLenum shadertype);
void draw();
void uploadVBO();
int setupShaders();
GLuint loadTextures(path *pathlike, float *w, float *h);
int setupTextures();

//listener functions


void handle_global(void *,struct wl_registry*, uint32_t, const char *, uint32_t);
void handle_global_remove(void *, struct wl_registry*, uint32_t);
void configure_xdg_surface(void *, struct xdg_surface*, uint32_t);
void xdg_wm_capabilites(void *,struct xdg_toplevel *,struct wl_array *);
void xdg_configure_bounds(void *,struct xdg_toplevel *,int32_t, int32_t);
void xdg_toplevel_closed(void *, struct xdg_toplevel *);
void xdg_toplevel_configure(void *, struct xdg_toplevel *, int32_t, int32_t, struct wl_array *);
void wm_ping_listen(void *data, struct xdg_wm_base *base, uint32_t serial);
void render_frame(void *data, struct wl_callback *callback, uint32_t callbackdata);
void seat_name(void *, struct wl_seat *, const char *);
void seat_capabilities(void *, struct wl_seat *, uint32_t);
void zxdg_description(void *data, struct zxdg_output_v1* output, const char *desc);
void zxdg_name(void *data, struct zxdg_output_v1* output, const char *name);
void zxdg_logical_size(void *data, struct zxdg_output_v1 *output, int32_t width, int32_t height);
void zxdg_logical_position(void *data, struct zxdg_output_v1* output, int32_t x, int32_t y);
void pointer_motion_event_electra(void *, struct wl_pointer *,uint32_t,wl_fixed_t,wl_fixed_t);
void pointer_button_event_electra(void *,struct wl_pointer *,uint32_t,uint32_t,uint32_t,uint32_t);
void pointer_frame_event_electra(void *data, struct wl_pointer *wl_pointer);
//init setup

void initSetup(){
    clock_gettime(CLOCK_MONOTONIC, &tp);
    memset(&Runtime, 0, sizeof(Runtime));
    memset(&EGLData, 0, sizeof(EGLData));
    memset(&XDGFrame, 0, sizeof(XDGFrame));
    memset(&Devices, 0, sizeof(Devices));
    memset(&Shader, 0, sizeof(Shader));
    memset(&Uniforms, -1, sizeof(Uniforms));
    memset(&PointerData, 0, sizeof(PointerData));
    Runtime.width = 800;
    Runtime.height = 400;
    Runtime.closed = 0;
    Runtime.initalTime = tp.tv_sec + (1.0*tp.tv_nsec)/(1E+9);
    Runtime.currentTime = 0.0;
    Runtime.renderDuration = 3.0;
}

int main(int argc, char *argv[]){
    LOG_INIT(0);
    initSetup();
    loadPaths(argv[0], &filepath);

    EXIT_FAIL(initRuntime(), "failed to initate runtime data");
    EXIT_FAIL(bindListeners(), "cannot bind listeners");
    wl_display_roundtrip(Runtime.display);

    EXIT_FAIL(eglInit(), "failed to initalize EGL" );
    EXIT_FAIL(setupShaders(), "failed to load shader");
    // glDeleteProgram(Shader.shader_program);
    EXIT_FAIL(createAllSurface(), "Failed to create surface");
    EXIT_FAIL(attachXDGSurface(),"Failed to attach XDG Surface.");
    EXIT_FAIL(bindSeat(), "Seat bind failed\n");
    EXIT_FAIL(bindXDGOutput(), "XDG output failed to bind.\n");
    EXIT_FAIL(setupTextures(), "Failed to load textures.");
    uploadVBO();
    wl_surface_commit(Runtime.surface);
    wl_display_roundtrip(Runtime.display);
    EXIT_FAIL(bindDevices(), "I/O device bind failed\n");
    while(!Runtime.closed){
        wl_display_dispatch(Runtime.display);
    }

finish:
    freeallpaths();
    if(EGLData.surface){
        eglDestroySurface(EGLData.display, EGLData.surface);
        LOG(3, "Destroy EGL Surface.", INFO);
    }
    if(EGLData.context){
        LOG(3, "Destroy EGL Context", INFO);
        eglDestroyContext(EGLData.display, EGLData.context );
    }
    eglTerminate(EGLData.display);
    LOG_STOP();
    // FREE(Runtime.egl_window, eglDes)
    freeShaders();
    FREE(Misc.output, zxdg_output_v1_destroy);
    FREE(Misc.output_manager, zxdg_output_manager_v1_destroy);
    FREE(XDGFrame.toplevel_window, xdg_toplevel_destroy);
    FREE(XDGFrame.x_surface, xdg_surface_destroy);
    FREE(XDGFrame.base, xdg_wm_base_destroy);
    FREE(Devices.keyboard, wl_keyboard_destroy);
    FREE(Devices.pointer, wl_pointer_destroy);
    FREE(Runtime.output, wl_output_destroy);
    FREE(Runtime.egl_window, wl_egl_window_destroy);
    FREE(Runtime.registry, wl_registry_destroy);
    FREE(Runtime.compositor, wl_compositor_destroy);
    FREE(Runtime.seat, wl_seat_destroy);
    FREE(Runtime.surface, wl_surface_destroy);
    FREE(Runtime.display, wl_display_disconnect);
    return 0;
}

void draw(){
    glClearColor(0.0, 0.0, 0.5, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(Shader.shader_program);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ZERO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Shader.tex1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, Shader.tex2 );

    glBindVertexArray(Shader.VAO);
    glDrawArrays(GL_TRIANGLES, 0 , 6);
    eglSwapInterval(EGLData.display,0 );
    eglSwapBuffers(EGLData.display,EGLData.surface);
    // printf("yes less.\n");
    Runtime.cb = wl_surface_frame(Runtime.surface);
    wl_callback_add_listener(Runtime.cb, &Listeners.render_listener , NULL);
}



int bindSeat(){
    wl_seat_add_listener(Runtime.seat, &Listeners.seat_listener, NULL);
    return 1;
}

int attachXDGSurface(){
    XDGFrame.x_surface = xdg_wm_base_get_xdg_surface(XDGFrame.base, Runtime.surface);
    CHECK(XDGFrame.x_surface, "failed to create xdg surface.");
    XDGFrame.toplevel_window = xdg_surface_get_toplevel(XDGFrame.x_surface);
    CHECK(XDGFrame.toplevel_window, "failed to create toplevel window.");
    xdg_wm_base_add_listener(XDGFrame.base, &Listeners.wm_base_listener , NULL );
    xdg_surface_add_listener(XDGFrame.x_surface, &Listeners.surface_listener, NULL);
    xdg_toplevel_add_listener(XDGFrame.toplevel_window, &Listeners.toplevel_listener,NULL);
    return 1;
}


int createAllSurface(){
    Runtime.surface = wl_compositor_create_surface(Runtime.compositor);
    CHECK(Runtime.surface, "failed to create WL Surface.");
    Runtime.egl_window = wl_egl_window_create(Runtime.surface, Runtime.width, Runtime.height);
    CHECK(Runtime.egl_window, "failed to create Runtime.egl_window");
    EGLData.surface = eglCreateWindowSurface(EGLData.display,EGLData.config,
                                             (EGLNativeWindowType)Runtime.egl_window, NULL);
    return 1;
}


int eglInit(){
    EGLint count, size, n;
    EGLint major , minor;
    EGLint config_attrbs[] = {
      EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
      // EGL_DEPTH_SIZE, 32,
      // EGL_RED_SIZE, 10,
      // EGL_GREEN_SIZE, 10,
      // EGL_BLUE_SIZE, 10,
      EGL_ALPHA_SIZE, 8,
      EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
      EGL_NONE
    };

    EGLint context_attrbs[] = {
        EGL_CONTEXT_MAJOR_VERSION, 3,
        EGL_CONTEXT_MINOR_VERSION, 3,
        EGL_CONTEXT_OPENGL_PROFILE_MASK, EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT,
        EGL_CONTEXT_OPENGL_DEBUG, EGL_TRUE,
        EGL_NONE
    };

    EGLData.display = eglGetDisplay((EGLNativeDisplayType)Runtime.display);
    if(EGLData.display == EGL_NO_DISPLAY){
        LOG(4, "EGL DISPLAY BIND FAILED.\n", FATAL);
        return 0;
    }


    EGLConfig config;
    if(!eglInitialize(EGLData.display, &major, &minor)){
        LOG(5, "error cannot create init.\n", ERROR);
    }
    EGLBoolean ok = eglBindAPI(EGL_OPENGL_API);
    if(ok) LOG(1, "EGL OPENGL API BIND FINISHED.", MESSAGE);
    else goto error;

    printf("major egl support %d.%d\n", major, minor);
    eglGetConfigs(EGLData.display,NULL,0, &count);
    printf("found %d configs.\n", count);
    EGLConfig* configs = (EGLConfig *)calloc(count, sizeof(EGLConfig));
    eglChooseConfig(EGLData.display,config_attrbs ,configs, count , &n);
    if(n<1) {
        LOG(10, "No Supported EGL context found.\n", FATAL);
        goto error;
    };
    config = configs[0];

    EGLData.context = eglCreateContext(EGLData.display,config,EGL_NO_CONTEXT, context_attrbs);
    if(EGLData.context == EGL_NO_CONTEXT){
        LOG(10, "errror cannot grab context.", FATAL);
    }
    LOG(7, "EGL Context created successfully.", MESSAGE);
    EGLData.config = config;
    eglMakeCurrent(EGLData.display, EGLData.surface, EGLData.surface, EGLData.context );
    printf("GLEW STATE : %d\n", glewInit());
    free(configs);
    return 1;
error:
    free(configs);
    return 0;
}



int initRuntime(){
    Runtime.display = wl_display_connect(NULL);
    CHECK(Runtime.display, "cannot open display.\n");
    Runtime.registry = wl_display_get_registry(Runtime.display);
    CHECK(Runtime.registry, "cannot open registry");
    return 1;
}

int loadPaths(const char *parent, struct Paths *mPath){
    // LOG(10, parent, FATAL);
    mPath->parent_path = getParent(Path(parent));
    mPath->fragment_path = join(mPath->parent_path, Path("./share/fragment1.glsl"));
    mPath->vertex_path = join(mPath->parent_path, Path("./share/vertex1.glsl"));
    mPath->wall1_path = join(mPath->parent_path, Path("./share/breeze1.jpg"));
    mPath->wall2_path = join(mPath->parent_path, Path("./share/breeze2.jpg"));
    LOG(1, mPath->fragment_path->name, MESSAGE);
    LOG(1, mPath->vertex_path->name, MESSAGE);
    printf("loaded paths.\n");
    return 1;
}

int bindDevices(){
    if (Devices.pointer){
        LOG(0, "binding all pother pointer listeners.", INFO);
        wl_pointer_add_listener(Devices.pointer,&Listeners.pointer_listener , NULL);
        return 1;
    }
    return 0;
}

int bindXDGOutput(){
    Misc.output = zxdg_output_manager_v1_get_xdg_output(Misc.output_manager, Runtime.output);
    zxdg_output_v1_add_listener(Misc.output, &Listeners.output_listener , NULL);
    return 1;
}

int bindListeners(){
    Listeners.registry_listener.global = handle_global;
    Listeners.registry_listener.global_remove = handle_global_remove;
    Listeners.surface_listener.configure = configure_xdg_surface;
    Listeners.toplevel_listener.configure = xdg_toplevel_configure;
    Listeners.toplevel_listener.close = xdg_toplevel_closed;
    Listeners.toplevel_listener.configure_bounds = xdg_configure_bounds;
    Listeners.toplevel_listener.wm_capabilities = xdg_wm_capabilites;
    Listeners.wm_base_listener.ping = wm_ping_listen;
    Listeners.render_listener.done = render_frame;
    Listeners.seat_listener.capabilities = seat_capabilities;
    Listeners.seat_listener.name = seat_name;
    Listeners.pointer_listener.axis = pointer_axis_event;
    Listeners.pointer_listener.enter = pointer_enter_event;
    Listeners.pointer_listener.axis_discrete = pointer_axis_discrete_event;
    Listeners.pointer_listener.axis_source = pointer_axis_source_event;
    Listeners.pointer_listener.axis_relative_direction = pointer_axis_relative_direction_event;
    Listeners.pointer_listener.axis_stop = pointer_axis_top_event;
    Listeners.pointer_listener.axis_value120 = pointer_axis120_event;
    Listeners.pointer_listener.leave = pointer_leave_event;
    Listeners.pointer_listener.motion = pointer_motion_event_electra;
    Listeners.pointer_listener.button = pointer_button_event_electra;
    Listeners.pointer_listener.frame = pointer_frame_event_electra;
    Listeners.output_listener.description = zxdg_description;
    Listeners.output_listener.name = zxdg_name;
    Listeners.output_listener.logical_position = zxdg_logical_position;
    Listeners.output_listener.logical_size = zxdg_logical_size;
    wl_registry_add_listener(Runtime.registry, &Listeners.registry_listener, NULL);
    return 1;
}


void handle_global(void *data,
                   struct wl_registry* registry,
                   uint32_t name,
                   const char *interface,
                   uint32_t version)
{
    if(strcmp(interface, wl_compositor_interface.name) == 0){
        Runtime.compositor = wl_registry_bind(registry, name, &wl_compositor_interface, version);
        message("compositor loaded successfully.");

    }
    if(strcmp(interface, wl_seat_interface.name) == 0){
        Runtime.seat = wl_registry_bind(registry, name, &wl_seat_interface, version);
        message("seat loaded successfully.");
    }
    if(strcmp(interface, xdg_wm_base_interface.name) == 0){
        XDGFrame.base = wl_registry_bind(registry, name, &xdg_wm_base_interface, version);
        message("wm_base loaded successfully.");
    }
    if(strcmp(interface, zxdg_output_manager_v1_interface.name) == 0){
        Misc.output_manager = wl_registry_bind(registry, name, &zxdg_output_manager_v1_interface, version);
        message("zxdg_output_manager_v1 loaded successfully.");
    }
    if(strcmp(interface, wl_output_interface.name) == 0){
        Runtime.output = wl_registry_bind(registry, name , &wl_output_interface, version );
        message("wl_output loaded successfully.");
    }
    if(strcmp(interface, zwlr_layer_shell_v1_interface.name) == 0){
        Runtime.layer_shell = wl_registry_bind(registry, name, &zwlr_layer_shell_v1_interface, version);
        message("zwlr_layer_shell_v1_interface binded.");
    }
    printf("interface %s\n", interface);
}

void handle_global_remove(void *data, struct wl_registry* registry, uint32_t name){
    // who cares.
}

void wm_ping_listen(void *data, struct xdg_wm_base *base, uint32_t serial){
    xdg_wm_base_pong(base, serial);
}

void configure_xdg_surface(void *data, struct xdg_surface* surface, uint32_t serial){
    xdg_surface_ack_configure(surface,serial);
    wl_egl_window_resize(Runtime.egl_window, Runtime.width , Runtime.height , 0 , 0);
    glViewport(0, 0, Runtime.width, Runtime.height);
    glUniform2f(Uniforms.u_resolution, (float)Runtime.width, (float)Runtime.height);
    glUniform1i(Uniforms.u_tex_0, 0);
    glUniform1i(Uniforms.u_tex_1, 1);
    if(!Runtime.render) draw();
}

void xdg_wm_capabilites(void *data,
				struct xdg_toplevel *xdg_toplevel,
				struct wl_array *capabilities)
{
}

void xdg_configure_bounds(void *data, struct xdg_toplevel *toplevel, int32_t width, int32_t height){
    // printf("Ok done.\n");
}

void xdg_toplevel_closed(void *data, struct xdg_toplevel *toplevel){
    Runtime.closed = 1;
    Runtime.render = 0;
    printf("closed true.\n");
}

void xdg_toplevel_configure(void *data,
                            struct xdg_toplevel *toplevel,
                            int32_t width,
                            int32_t height,
                            struct wl_array *states)
{

        if(width) Runtime.width = width;
        if(height) Runtime.height = height;
}

void render_frame(void *data, struct wl_callback *callback, uint32_t callbackdata){
    clock_gettime(CLOCK_MONOTONIC, &tp);
    wl_callback_destroy(callback);
    double t= (tp.tv_sec + (1.0*tp.tv_nsec)/(1E+9));
    Runtime.currentTime = t - Runtime.initalTime;
    if(Runtime.render){
        glUniform1f(Uniforms.u_time, Runtime.currentTime);
        draw();
    }

}

void pointer_motion_event_electra(void *data,
    struct wl_pointer *wl_pointer,
    uint32_t time,
    wl_fixed_t surface_x,
    wl_fixed_t surface_y
)
{
        PointerData.events |= POINTER_MOUSE_MOVE;
        PointerData.surface_x = wl_fixed_to_double(surface_x);
        PointerData.surface_y = wl_fixed_to_double(surface_y);
}

void pointer_button_event_electra(void *data,
    struct wl_pointer *wl_pointer,
    uint32_t serial,
    uint32_t time,
    uint32_t button,
    uint32_t state
)
{
    PointerData.events |= POINTER_BUTTON_CLICK;
    PointerData.button = button;
    PointerData.button_state = state;
}

void pointer_frame_event_electra(void *data,
		      struct wl_pointer *wl_pointer)
{
    if(PointerData.events & POINTER_BUTTON_CLICK && PointerData.button_state == BUTTON_STATE_DOWN){
        printf("yes\n");
        if(!Runtime.render) {
            Runtime.render = 1;
            clock_gettime(CLOCK_MONOTONIC, &tp);
            Runtime.initalTime = tp.tv_sec + (1.0*tp.tv_nsec)/(1E+9);
            draw();
        }
        glUniform4f(Uniforms.u_mouse, PointerData.surface_x,Runtime.height-PointerData.surface_y, Runtime.currentTime, Runtime.wallstate);
        Runtime.wallstate = !Runtime.wallstate;

    }
}

void seat_capabilities(void *data, struct wl_seat *wl_seat, uint32_t capabilities)
{
    if ( capabilities & WL_SEAT_CAPABILITY_POINTER && !Devices.pointer){
        printf("pointer supported.\n");
        Devices.pointer = wl_seat_get_pointer(Runtime.seat);
        LOG(1, "Pointer ID: ", MESSAGE);
        LOG(3, "Pointer binded successfully.", MESSAGE);
    }
    if ( capabilities & WL_SEAT_CAPABILITY_KEYBOARD ){
        printf("keyboard also supported by compositor\n");
        Devices.keyboard = wl_seat_get_keyboard(Runtime.seat);
        LOG(3, "Keyboard binded successfully.", MESSAGE);
    }
    if( capabilities & WL_SEAT_CAPABILITY_TOUCH ){
        printf("Touch is also supported by pointer.\n");
    }
}

void seat_name(void *data, struct wl_seat *wl_seat, const char *name){
}

void zxdg_logical_position(void *data, struct zxdg_output_v1* output, int32_t x, int32_t y){
}

void zxdg_logical_size(void *data, struct zxdg_output_v1 *output, int32_t width, int32_t height){
    // if (width) Runtime.width = width;
    // if (height) Runtime.height = height;
}

void zxdg_name(void *data, struct zxdg_output_v1* output, const char *name){
}

void zxdg_description(void *data, struct zxdg_output_v1* output, const char *desc){
    // printf("current_name: %s\n", name);
}


void uploadVBO(){
    eglMakeCurrent(EGLData.display, EGLData.surface, EGLData.surface, EGLData.context );

    glGenVertexArrays(1, &Shader.VAO);
    glBindVertexArray(Shader.VAO);
    glGenBuffers(1, &Shader.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, Shader.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mesh), mesh, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*2, (void *)0);
    glEnableVertexAttribArray(0);
}


int setupShaders(){
    eglMakeCurrent(EGLData.display, EGLData.surface, EGLData.surface, EGLData.context );
    int status;
    char logs[234];
    Shader.vertex_shader = loadShader(filepath.vertex_path, GL_VERTEX_SHADER);
    if(!Shader.vertex_shader) goto shadererror;
    Shader.fragment_shader = loadShader(filepath.fragment_path, GL_FRAGMENT_SHADER);
    if(!Shader.fragment_shader) goto shadererror;
    Shader.shader_program = glCreateProgram();
    if(!Shader.shader_program) goto shadererror;
    glAttachShader(Shader.shader_program, Shader.fragment_shader);
    // return 1;
    glAttachShader(Shader.shader_program, Shader.vertex_shader);
    glLinkProgram(Shader.shader_program);
    glGetProgramiv(Shader.shader_program, GL_LINK_STATUS, &status);
    if(!status){
        LOG(10, "Failed to link shader program" , FATAL);
        glGetProgramInfoLog(Shader.shader_program, 234, NULL, logs );
        LOG(10, logs, ERROR);
        goto shadererror;
    }
    message("Successfully linked all shaders.");
    LOG(0, "Detaching and deleteing shader objects", INFO);
    glDetachShader(Shader.shader_program, Shader.vertex_shader);
    glDetachShader(Shader.shader_program, Shader.fragment_shader);
    glDeleteShader(Shader.vertex_shader);
    glDeleteShader(Shader.fragment_shader);
    Uniforms.u_mouse = glGetUniformLocation(Shader.shader_program, "u_mouse");
    Uniforms.u_time = glGetUniformLocation(Shader.shader_program, "u_time");
    Uniforms.u_resolution = glGetUniformLocation(Shader.shader_program, "u_resolution");
    Uniforms.u_tex_0 = glGetUniformLocation(Shader.shader_program, "u_tex_0");
    Uniforms.u_tex_1 = glGetUniformLocation(Shader.shader_program, "u_tex_1");
    Uniforms.u_texsize = glGetUniformLocation(Shader.shader_program, "u_texsize");
    sprintf(logs, "POINTER u_resolution: %d", Uniforms.u_resolution);
    LOG(4, logs, MESSAGE);
    sprintf(logs, "POINTER u_time: %d" , Uniforms.u_time);
    LOG(4, logs, MESSAGE);
    sprintf(logs, "POINTER u_mouse: %d", Uniforms.u_mouse);
    LOG(4, logs, MESSAGE);
    sprintf(logs, "POINTER u_tex_0: %d", Uniforms.u_tex_0);
    LOG(4, logs, MESSAGE);
    sprintf(logs, "POINTER u_tex_1: %d", Uniforms.u_tex_1);
    LOG(4, logs, MESSAGE);
    sprintf(logs, "POINTER u_texsize: %d", Uniforms.u_texsize);
    LOG(4, logs, MESSAGE);
    return 1;


shadererror:
    if(Shader.vertex_shader) glDeleteShader(Shader.vertex_shader);
    if(Shader.fragment_shader) glDeleteShader(Shader.fragment_shader);
    if(Shader.shader_program) glDeleteProgram(Shader.shader_program);
    Shader.vertex_shader = 0;
    Shader.fragment_shader = 0;
    Shader.shader_program = 0;
    return 0;
}

GLint loadShader(path *mypath, GLenum shadertype){
    // // return 1;
    char k[250];
    int status = 1;
    GLint shader = 0;
    shader = glCreateShader(shadertype);
    FILE *fp = fopen(mypath->name, "r");
    if(!fp) {
        return 0;

    }
    fseek(fp, 0, SEEK_END);
    size_t size = ftell(fp);
    rewind(fp);
    char buffer[size+1];
    buffer[size] = '\0';
    fread(buffer, 1, size, fp);
    const char *buff = buffer;
    printf("%s\n", buffer);
    glShaderSource(shader, 1, &buff, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if(!status){
        sprintf(k, "failed to compile shader %s", mypath->name);
        LOG(10, k , ERROR);
        glGetShaderInfoLog(shader, 250, NULL, k);
        LOG(10, k , ERROR);
        goto error;
    }
    sprintf(k, "successfully compiled %s", mypath->name);
    LOG(8, k, MESSAGE);
    fclose(fp);
    return shader;


error:
    if(shader) glDeleteShader(shader);
    printf("here error.\n");
    if(fp) fclose(fp);
    return 0;
}

int freeShaders(){
    if(Shader.tex1) glDeleteTextures(1, &Shader.tex1);
    if(Shader.tex2) glDeleteTextures(1, &Shader.tex2);
    if(Shader.VBO) glDeleteBuffers(1, &Shader.VBO);
    if(Shader.VAO) glDeleteVertexArrays(1, &Shader.VAO);
    if (Shader.vertex_shader) glDeleteShader(Shader.vertex_shader);
    if (Shader.fragment_shader) glDeleteShader(Shader.fragment_shader);
    if (Shader.shader_program) glDeleteProgram(Shader.shader_program);
    return 1;
}


GLuint loadTextures(path *pathlike, float *w1, float *h1){
    int width, height, channels;
    GLuint texid;
    char log[50];
    sprintf(log, "Texture %s failed to load.", pathlike->name);
    glGenTextures(1, &texid);
    glBindTexture(GL_TEXTURE_2D, texid);
    CHECK(texid, "Failed to generate texture.");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    stbi_set_flip_vertically_on_load(1);
    unsigned char *buff = stbi_load(pathlike->name, &width, &height, &channels, 0);
    if(!buff){
        glDeleteTextures(1, &texid);
        LOG(10, log, FATAL);
        return 0;
    }
    sprintf(log, "%s WIDTH: %d:: HEIGHT:%d", pathlike->name, width, height);
    LOG(5, log, MESSAGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, buff);
    glGenerateMipmap(GL_TEXTURE_2D);
    sprintf(log, "TEXTURE ID: %d", texid);
    LOG(8, log, INFO);
    *w1 = (float)width;
    *h1 = (float)height;
    free(buff);
    return texid;
}

int setupTextures(){
    Shader.tex1 = loadTextures(filepath.wall1_path , &Shader.texsize.w1, &Shader.texsize.h1);
    CHECK(Shader.tex1, "FAILED TO LOAD TEXTURE UNIT 1");
    Shader.tex2 = loadTextures(filepath.wall2_path, &Shader.texsize.w2, &Shader.texsize.h2);
    CHECK(Shader.tex2, "FAILED TO LOAD TEXTURE UNIT 2");
    return 1;
    return 0;
}
