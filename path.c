// #include "path.h"
#include "path.h"
#include<stdlib.h>
#include<stdio.h>
#include <string.h>

int totalpathstring = 0;
char **pathstring = NULL;

int totalpaths = 0;
path **paths = NULL;

char *allocate_space(int size){
    if (!size) return NULL;
    char *myspace = (char *)malloc(size);
    if(!myspace){
        printf("no space for inital path.\n");
        return NULL;
    }
    totalpathstring++;
    char **tmppath = (char **)realloc(pathstring, totalpathstring*sizeof(char *));
    if(!tmppath) {
        printf("Error cannot create path index.\n");
        free(myspace);
        return NULL;
    }
    pathstring = tmppath;
    pathstring[totalpathstring-1] = myspace;

    return myspace;
}

path *Path(const char *pathlike){
    path *p = (path *)malloc(sizeof(path));
    p->length = strlen(pathlike);
    printf("length : %ld\n", p->length );
    p->name = allocate_space(p->length+1);
    if(!p->name){
        printf("path creation failed.\n");
        free(p);
        return NULL;
    }
    strcpy(p->name, pathlike);
    printf("copied.\n");
    p->name[p->length] = '\0';
    printf("reallocation\n");
    path **tmppath = (path **)realloc(paths, sizeof(path *)*(totalpaths + 1));
    printf("finished.\n");
    if(!tmppath){
        printf("appending to path index failed.\n");
        free(p);
        return NULL;
    }
    paths = tmppath;
    paths[totalpaths++] = p;
    return p;
}

path *pathWithLength(const char *pathlike, int size){
    path *p = (path *)malloc(sizeof(path));
    p->length = size;
    p->name = allocate_space(p->length+1);
    if(!p->name){
        printf("path creation failed.\n");
        free(p);
        return NULL;
    }
    strcpy(p->name, pathlike);
    p->name[p->length] = '\0';
    path **tmppath = (path **)realloc(paths, totalpaths+1);
    if(!tmppath){
        printf("appending to path index failed.\n");
        free(p);
        return NULL;
    }
    printf("ok finishing.\n");
    paths = tmppath;
    paths[totalpaths++] = p;
    return p;
}

path *getParent(path *ch){
    if(ch->length < 2) return ch;
    path *child = pathWithLength(ch->name, ch->length);
    int length = child->length;
    if(child->name[length-1] == '/') child->length--;
    while(child->length > 1 && child->name[child->length-1] != '/'){
        child->length--;
    }
    child->name[child->length] = '\0';
    return child;
}

void freeallpaths(){
    for(int i=0;i<totalpaths;i++){
        printf("Free path object %s\n", paths[i]->name);
        free(paths[i]);
    }
    free(paths);
    for(int i=0;i<totalpathstring;i++){
        printf("Free %s\n", pathstring[i]);
        free(pathstring[i]);
    }
    free(pathstring);
}

path *join(path *parent, path* child){
    printf("parent length : %ld\n", parent->length);
    printf("child length : %ld\n", child->length);
    size_t length1 = parent->length;
    size_t length2 = child->length;
    int offset = 0;
    if(parent->name[parent->length-1] != '/') length1 +=1 ;
    if(child->name[0] == '/') offset = 1;
    // strcpy(name->parent->name)
    char name[length1 + length2 - offset + 1];
    strcpy(name, parent->name);
    name[length1-1] = '/';
    strcpy(name+length1, child->name+offset);
    name[length1 + length2 - offset] = '\0';
    printf("%s\n", name);
    return Path(name);
}
