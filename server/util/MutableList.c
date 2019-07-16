#include "MutableList.h"
#include <stdio.h>
#include <stdlib.h>

MutableList* newMutableList() {
    return (MutableList*)malloc(sizeof(MutableList));
}
void initMutableList(MutableList* l) {
    for (int i=0; i<SMALL_LIST_SIZE; i++) {
        l->items0[i] = malloc(sizeof(int));
        *(l->items0[i]) = -1;
    }
    l->nextPointer = malloc(sizeof(int));
    *(l->nextPointer) = 0;
    l->curPointer = malloc(sizeof(int));
    *(l->curPointer) = 0;
    l->currentSize = malloc(sizeof(int));
    *(l->currentSize) = SMALL_LIST_SIZE;
}
void deleteMutableList(MutableList* l) {
    for (int i=0; i<SMALL_LIST_SIZE; i++) {
        free(l->items0[i]);
    }
}
int mutList_containsValue(MutableList* l, int v) {
    for (int i=0; i<*(l->nextPointer); i++) {
        if (*(l->items0[i])==v)
            return i;
    }
    return -1;
}
int mutList_addValue(MutableList* l, int v) {
    int ind = *(l->nextPointer);
    if (ind>=*(l->currentSize)) {
        return -1;
    }
    *(l->items0[ind])=v;
    *(l->nextPointer)=ind+1;
    return v;
}
int mutList_addValueExclusive(MutableList* l, int v) {
    for (int i=0; i<*(l->nextPointer); i++) {
    }
    int ind = *(l->nextPointer);
    if (ind>=*(l->currentSize)) {
        return -1;
    }
    *(l->items0[ind])=v;
    *(l->nextPointer)=ind+1;
    return v;
}
int mutList_removeValue(MutableList* l, int v) {
    return -1;
}
int mutList_addValueAt(MutableList* l, int v, int index) {
    *(l->items0[index])=v;
    //*(l->nextPointer)=index+1;
    return -1;
}
int mutList_removeValueAt(MutableList* l, int index) {
    return -1;
}

int mutList_next(MutableList* l) {
    int i = *(l->curPointer);
    if (i>=SMALL_LIST_SIZE || i>= *(l->nextPointer)) {
        *(l->curPointer)=0; //cycle back
        return -1; //list is full
    }
    *(l->curPointer)=i+1;
    int rv = *(l->items0[i]);
    printf("next in list was %d\n",rv);
    return rv;
}

int mutList_atEnd(MutableList* l) {
    if (*(l->nextPointer)>=SMALL_LIST_SIZE)
        return 1;
    if (*(l->curPointer)>=*(l->nextPointer))
        return 1;
    return 0;
}
