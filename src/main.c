#include <stdio.h>
#include <wayland-client-protocol.h>
#include<wayland-client.h>
#include "path.h"
#include "log.h"

#define str(x) x->name
#define EXIT_FAIL(X, Y) if (!X) {LOG(5, Y, ERROR); goto finish;}
#define CHECK(X, Y) if (!X) {LOG(5, Y, FATAL); return 0;}
#define FREE(X, Y) if(X) Y(X);


struct Paths{
    path *parent_path;
    path *vertex_path;
    path *fragment_path;
    path *wall1_path;
    path *wall2_path;
} filepath;

struct __appdata{
    struct wl_display* display;
    struct wl_surface* surface;
    struct wl_compositor* compositor;
    struct wl_registry* registry;
    struct wl_seat* seat;
} Runtime;

//bind functions

int initRuntime();
int bindListeners();
int loadPaths(const char *parent, struct Paths *path);
int loadPaths(const char *parent, struct Paths *path);

//listener functions


void handle_global(void *,struct wl_registry*, uint32_t, const char *, uint32_t);
void handle_global_remove(void *, struct wl_registry*, uint32_t);

struct __runtime{
    struct wl_registry_listener registry_listener;
} Listeners;



int main(int argc, char *argv[]){
    LOG_INIT(0);
    loadPaths(argv[0], &filepath);
    EXIT_FAIL(initRuntime(), "failed to initate runtime data");
    EXIT_FAIL(bindListeners(), "cannot bind listeners");
    getchar();




finish:
    freeallpaths();
    FREE(Runtime.registry, wl_registry_destroy);

    return 0;
}

int initRuntime(){
    Runtime.display = wl_display_connect(NULL);
    CHECK(Runtime.display, "cannot open display.\n");
    Runtime.registry = wl_display_get_registry(Runtime.display);
    CHECK(Runtime.registry, "cannot open registry");
    return 1;
}

int loadPaths(const char *parent, struct Paths *path){
    path->parent_path = getParent(Path(parent));
    path->fragment_path = join(path->parent_path, Path("./share/view.frag"));
    path->vertex_path = join(path->parent_path, Path("./share/view.vert"));
    return 1;
}

int bindListeners(){
    Listeners.registry_listener.global = handle_global;
    Listeners.registry_listener.global_remove = handle_global_remove;

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
    }
    if(strcmp(interface, wl_seat_interface.name) == 0){
        Runtime.seat = wl_registry_bind(registry, name, &wl_seat_interface, version);
    }
}

void handle_global_remove(void *data, struct wl_registry* registry, uint32_t name){
    // who cares.
}
