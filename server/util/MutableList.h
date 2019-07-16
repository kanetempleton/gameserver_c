#ifndef H_MUTABLELIST
#define H_MUTABLELIST
#define SMALL_LIST_SIZE 20
#define MED_LIST_SIZE 100
#define LARGE_LIST_SIZE 500
#define HUGE_LIST_SIZE 2000
#include "HashMap.h"
typedef struct {
    int* items0[SMALL_LIST_SIZE];
    int* items1[MED_LIST_SIZE];
    int* items2[LARGE_LIST_SIZE];
    int* items3[HUGE_LIST_SIZE];
    int* nextPointer;
    int* curPointer;
    int* currentSize;
} MutableList;
MutableList* newMutableList();
void initMutableList(MutableList* l);
void deleteMutableList(MutableList* l);
int mutList_containsValue(MutableList* l, int v);
int mutList_addValue(MutableList* l, int v);
int mutList_addValueExclusive(MutableList* l, int v);
int mutList_removeValue(MutableList* l, int v);
int mutList_addValueAt(MutableList* l, int v, int index);
int mutList_removeValueAt(MutableList* l, int index);

int mutList_next(MutableList* l);
int mutList_atEnd(MutableList* l);
#endif
