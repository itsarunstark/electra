#include<wayland-client.h>
#include "path.h"

int main(int argc, char* argv[]){
    path *parent = Path(argv[0]);
    path *m = getParent(parent);
    // printf("ok\n");
    path *file = Path("./namenew");
    path *j = join(parent, file);

    freeallpaths();

    return 0;

}
