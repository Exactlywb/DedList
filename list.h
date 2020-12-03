//
// Created by 1 on 17.11.2020.
//

#ifndef DEDLIST_LIST_H
#define DEDLIST_LIST_H

#include <stdlib.h>
#include <stdio.h>
#include <cmath>
#include <assert.h>

typedef double Element;
const Element poison = NAN;

struct Node {

    int next   = -1;
    int prev   = -1;
    Element value = poison;

};

struct List {

    int head         = 0;
    int head_free_el = 0;
    size_t size      = 0;
    size_t capacity  = 0;
    Node* data       = nullptr;
    int tail         = 0;
    bool sorted      = false;

};

enum InsertSide {

    LEFT_SIDE,
    RIGHT_SIDE

};

enum ErrType {

    #define DEF_ERR(name, code_list_ok, code_any_place) name,

        #include "ErrList.txt"

    #undef DEF_ERR

};

ErrType ListConstruct           (List* mainList, size_t size);
ErrType ListDestruct            (List* mainList);
void    ListDump                (List* mainList);
int     PushHead                (List* mainList, Element value);
int     PushTail                (List* mainList, Element value);
int     PopHead                 (List* mainList);
int     PopTail                 (List* mainList);
int     ListInsertConsiderSide  (List* mainList, size_t position, Element value, InsertSide whichSide);
ErrType DeleteElem              (List* mainList, size_t position);
ErrType LogicSort               (List* mainList);
ErrType LogicSortV2             (List* mainList);
int     FindValuePos            (List* mainList, Element value);
Element GetValueElem            (List* mainList, size_t position);
ErrType PushIntoFreeList        (List* mainList, size_t position);
Node    GetNode                 (List* mainList, size_t position);
ErrType ListOk                  (List* mainList);
int     FindPhysicByLogicPos    (List* mainList, size_t position);
ErrType SwapNodes               (List* mainList, size_t firstIndex, size_t secondIndex);

#endif //DEDLIST_LIST_H
