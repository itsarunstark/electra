#include <fcntl.h>
#include<stdio.h>
#include<sys/mman.h>
#include<sys/fcntl.h>
#include<sys/stat.h>
// #include<sys/

#include<unistd.h>
struct MetaInfo{
    int filesize[4];
    char relative[4];
    char *vertex_src;
    char *fragment_src;
    char *wall1;
    char *wall2;
};

int main(){
    struct MetaInfo info;


}


