#include "list.h"

int main () {

    remove ("funcStack.html");

    List mainList = {};

    size_t list_size = 10;

    ListConstruct (&mainList, list_size);

    int headIndex = PushHead (&mainList, 153);
    ListInsertConsiderSide (&mainList, headIndex, 34, LEFT_SIDE);

    int nodeIndex = PushHead (&mainList, 153);
    int currentTailPos = PushTail (&mainList, 83);

    size_t newNode = ListInsertConsiderSide (&mainList, currentTailPos, 100, LEFT_SIDE);

    //SwapNodes (&mainList, newNode, mainList.head);

    for (size_t value = 1; value < list_size; value++) {

        if (value % 2 == 0)
            ListInsertConsiderSide(&mainList, nodeIndex, value, LEFT_SIDE);
        else
            ListInsertConsiderSide(&mainList, nodeIndex, value, RIGHT_SIDE);

    }

    DeleteElem (&mainList, currentTailPos);
    DeleteElem (&mainList, mainList.data[nodeIndex].next); //func GetNext, GetPrev

    LogicSortV2 (&mainList);

    ListDump (&mainList);
    ListDestruct (&mainList);

    return 0;

}
