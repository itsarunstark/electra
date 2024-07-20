#ifndef _PATH_H
#define _PATH_H
#include<stdio.h>
#include<string.h>
struct __path{
    char *name;
    size_t length;
};

typedef struct __path path;

path *Path(const char *pathlike);
void freeallpaths();
path *getParent(path *ch);
path *join(path *parent, path* child);

#endif //_PATH_H
