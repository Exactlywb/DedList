#include <cstdarg>
#include "../list.h"

ErrType my_errno = NOERR;

#define ASSERT_OK my_errno = ListOk (mainList); \
                  if ( my_errno != NOERR )              \
                        return ( my_errno );

void DbgFunc (const char* format, const char* funcName, ...) {

    static size_t callNumber = 0;

    FILE* htmlFile = fopen ("funcStack.html", "a");
    assert (htmlFile);

    if (callNumber == 0) {

        fprintf (htmlFile, "<!DOCTYPE html>\n"
                           "<html lang=\"en\">\n"
                           "<head>\n"
                           "    <meta charset=\"UTF-8\">\n"
                           "    <title>List dump</title>\n"
                           "    <link href=\"https://fonts.googleapis.com/css2?family=Open+Sans&display=swap\" rel=\"stylesheet\"><link rel=\"stylesheet\" href=\"style.css\">\n"
                           "</head>\n"
                           "<body>\n"
                           "\n"
                           "<div class=\"container\">");

    }

    va_list args = {};
    va_start(args, funcName);

    fprintf (htmlFile, "<div class = \"func\"><h2>%s</h2>\t", funcName);
    vfprintf (htmlFile, format, args);
    fprintf  (htmlFile, "</div>");

    fclose (htmlFile);
    va_end(args);

    callNumber++;

}

ErrType DeleteElem (List* mainList, const size_t position) {

    DbgFunc ("Pointer on main list = %p <br>\tSize position = %zu<br>\n", __FUNCTION__, mainList, position);

    ASSERT_OK

    if (position >= mainList->size) {

        printf ("You entered position to delete more than size of your list in function %s\n", __FUNCTION__);
        return BADPOSITION;

    }

    size_t prevElemNum = mainList->data[position].prev;
    size_t nextElemNum = mainList->data[position].next;

    if ( prevElemNum != 0 )
        mainList->data[prevElemNum].next = mainList->data[position].next;
    else
        mainList->head = nextElemNum;

    if ( nextElemNum != 0 )
        mainList->data[nextElemNum].prev = prevElemNum;
    else
        mainList->tail = prevElemNum;

    mainList->data[position].value = poison;
    mainList->data[position].next = mainList->head_free_el;
    mainList->data[position].prev = 0;

    PushIntoFreeList (mainList, position);

    mainList->capacity--;

    char listHead = 1;
    char edge = position == listHead && position == mainList->capacity; //магическая константа?
    mainList->sorted = mainList->sorted & edge;

    ASSERT_OK

    return NOERR;

}

int ListInsertConsiderSide (List* mainList, const size_t position, const Element value, InsertSide whichSide) {

    DbgFunc ("Pointer on mainList = %p<br>\tPosition = %zu <br>\tValue = %lg<br>\tSide = %d<br>\n", __FUNCTION__, mainList, position, value, whichSide);

    ASSERT_OK

    if (mainList->capacity + 1 >= mainList->size) {

        printf ("New element can't be putted into list! You should set more size for list!");
        return 0;

    }

    if (position >= mainList->size) {

        printf ("You can not insert list on position more than you have memory!\n");
        return 0;

    }

    int new_physical_position = mainList->head_free_el;

    mainList->head_free_el = mainList->data[mainList->head_free_el].next;

    if ( mainList->head_free_el != 0 )
        mainList->data[mainList->head_free_el].prev = 0;

    if (whichSide == RIGHT_SIDE) {

        mainList->data[new_physical_position].next = mainList->data[position].next;
        mainList->data[new_physical_position].prev = position;
        mainList->data[position].next = new_physical_position;

        if (mainList->data[new_physical_position].next != 0)
            mainList->data[mainList->data[new_physical_position].next].prev = new_physical_position;
        else
            mainList->tail = new_physical_position;

    } else if (whichSide == LEFT_SIDE) {

        mainList->data[new_physical_position].next = position;
        mainList->data[new_physical_position].prev = mainList->data[position].prev;

        mainList->data[position].prev = new_physical_position;

        if (mainList->data[new_physical_position].prev != 0)
            mainList->data[mainList->data[new_physical_position].prev].next = new_physical_position;
        else
            mainList->head = new_physical_position;

    } else {

        printf ("You putted into argument in function %s wrong side!\n", __FUNCTION__);
        return 0;

    }

    mainList->data[new_physical_position].value = value;

    mainList->capacity++;

    ASSERT_OK

    return new_physical_position;

}

int PopHead (List* mainList) {

    DbgFunc ("Pointer on mainList = %p<br>\n", __FUNCTION__, mainList);

    ASSERT_OK

    if (mainList->head == mainList->tail) //if there is only one element
        mainList->tail = 0;

    int new_head_index = mainList->data[mainList->head].next;

    mainList->data[new_head_index].prev = 0;

    mainList->data[mainList->head].prev = 0;
    mainList->data[mainList->head].value = poison;

    PushIntoFreeList (mainList, mainList->head);

    mainList->head = new_head_index;

    mainList->capacity--;

    ASSERT_OK

    return new_head_index;

}

int PopTail (List* mainList) {

    DbgFunc ("Pointer on mainList = %p<br>\n", __FUNCTION__, mainList);

    ASSERT_OK

    if (mainList->head == mainList->tail) //if there is only one element
        mainList->head = 0;

    int new_tail_index = mainList->data[mainList->tail].prev;

    mainList->data[new_tail_index].next = 0;

    mainList->data[mainList->tail].prev = 0;
    mainList->data[mainList->tail].value = poison;

    PushIntoFreeList (mainList, mainList->tail);

    mainList->tail = new_tail_index;

    mainList->capacity--;

    ASSERT_OK

    return new_tail_index;

}

int PushTail (List* mainList, const Element value) {

    DbgFunc ("Pointer on mainList = %p<br>\tValue = %lg<br>\n", __FUNCTION__, mainList, value);

    ASSERT_OK

    int new_Pos = mainList->head_free_el;
    mainList->head_free_el = mainList->data[mainList->head_free_el].next;

    if ( mainList->data[new_Pos].next != 0)
        mainList->data[mainList->data[new_Pos].next].prev = 0;


    if ( mainList->tail != 0 ) { //if there is tail
        mainList->data[new_Pos].prev = mainList->tail;
        mainList->data[mainList->tail].next = new_Pos;
    } else {
        mainList->data[new_Pos].prev = 0;
    }

    if ( mainList->head == 0 ) //if there is none head
        mainList->head = new_Pos;

    mainList->data[new_Pos].value = value;
    mainList->data[new_Pos].next = 0;
    mainList->tail = new_Pos;

    mainList->capacity++;

    ASSERT_OK

    return new_Pos;

}

int PushHead (List* mainList, const Element value) {

    DbgFunc ("Pointer on mainList = %p<br>\tValue = %lg<br>\n", __FUNCTION__, mainList, value);

    ASSERT_OK

    int new_Pos = mainList->head_free_el;
    mainList->head_free_el = mainList->data[mainList->head_free_el].next;

    if ( mainList->data[new_Pos].next != 0)
        mainList->data[mainList->data[new_Pos].next].prev = 0;


    if ( mainList->head != 0 ) { //if there is head
        mainList->data[new_Pos].next = mainList->head;
        mainList->data[mainList->head].prev = new_Pos;
    } else {
        mainList->data[new_Pos].next = 0;
    }

    if ( mainList->tail == 0 ) //if there is none tail
        mainList->tail = new_Pos;

    mainList->data[new_Pos].value = value;
    mainList->data[new_Pos].prev = 0;
    mainList->head = new_Pos;

    mainList->capacity++;

    ASSERT_OK

    return new_Pos;

}

void ListFillPoison (Node* data, const size_t size) {

    DbgFunc ("Pointer on data = %p<br>\tSize = %zu<br>\n", __FUNCTION__, data, size);

    assert (data);
    assert (size != 0);

    for (size_t i = 1; i < size; i++) {

        data[i].prev = i - 1;
        data[i].next = (i + 1) % size;
        data[i].value = poison;

    }

    assert (data);
    assert (size != 0);

}

ErrType ListConstruct (List* mainList, const size_t size) {

    DbgFunc ("Pointer on mainList = %p<br>\tSize = %zu<br>\n", __FUNCTION__, mainList, size);

    if (mainList == nullptr) {

        printf ("Null pointer on mainList in function %s", __FUNCTION__);
        return NULLPTR;

    }

    mainList->size = size;
    mainList->capacity = 0;
    mainList->head = mainList->tail = 0;

    mainList->data = (Node*)calloc(size, sizeof(Node));

    ListFillPoison (mainList->data, size);

    mainList->head_free_el = 1;
    mainList->sorted = true;

    ASSERT_OK

}

ErrType ListDestruct (List* mainList) {

    DbgFunc ("Pointer on mainList = %p<br>\n", __FUNCTION__, mainList);

    ASSERT_OK

    mainList->size = 0;
    mainList->capacity = 0;
    free ( mainList->data );
    mainList->data = nullptr;

    mainList->head_free_el = mainList->head = mainList->tail = 0;
    mainList->sorted = false;

    mainList = nullptr;

}

ErrType MakeListGraph (List* mainList) {

    DbgFunc ("Pointer on mainList = %p<br>\n", __FUNCTION__, mainList);

    ASSERT_OK

    FILE* graph = fopen ("graph.dot", "w");
    assert (graph);

    fprintf (graph, "digraph LIST{\n");
    fprintf (graph, "\t" "rankdir = TB;\n");

    for (int i = 0; i < mainList->size; i++) {

        fprintf (graph, "%d [shape=record,",i);

        if (_isnan(mainList->data[i].value)) {

            fprintf (graph,"style = \"filled\", fillcolor = \"#4dadf7\", label=\" value = %lg | { <prev> prev = %d | <current> current = %d | <next> next = %d } \"];\n",mainList->data[i].value, mainList->data[i].prev, i, mainList->data[i].next);
            continue;

        }

        if (i == 0) {

            fprintf (graph,"style = \"filled\", fillcolor = \"#73e364\", label=\" value = %lg | { <prev> prev = %d | <current> current = %d | <next> next = %d } \"];\n",mainList->data[i].value, mainList->data[i].prev, i, mainList->data[i].next);
            continue;

        }

        if (i == mainList->head) {

            fprintf (graph,"style = \"filled\", fillcolor = \"#c478ff\", label=\" value = %lg | { <prev> prev = %d | <current> current = %d | <next> next = %d } \"];\n",mainList->data[i].value, mainList->data[i].prev, i, mainList->data[i].next);
            continue;

        }

        if (i == mainList->tail) {

            fprintf (graph,"style = \"filled\", fillcolor = \"#ffdb78\", label=\" value = %lg | { <prev> prev = %d | <current> current = %d | <next> next = %d } \"];\n",mainList->data[i].value, mainList->data[i].prev, i, mainList->data[i].next);
            continue;

        }

        fprintf (graph,"style = \"filled\", fillcolor = \"#ed8677\", label=\" value = %lg | { <prev> prev = %d | <current> current = %d | <next> next = %d } \"];\n",mainList->data[i].value, mainList->data[i].prev, i, mainList->data[i].next);

    }

    fprintf (graph, "\n");

    fprintf (graph, "edge[color = \"white\", weight = 0, dir = \"both\"];\n");

    fprintf (graph, "{rank = \"same\"\n");

    for (int i = 1; i < mainList->size; i++)
        fprintf (graph, "%d\n", i);

    fprintf (graph, "0");
    for (int i = 0; i < mainList->size; i++) {

        fprintf(graph, " -> %d", i);

    }

    fprintf (graph, ";\n}\n");

    fprintf (graph, "edge[color = \"red\", weight = 0, dir = \"both\"];\n");
    fprintf (graph, " %d -> 0;", mainList->tail);

    for (int i = 1; i < mainList->size; i++) {

        fprintf (graph, "edge[color = \"red\", weight = 10000, dir = \"both\"];\n");
        fprintf (graph,"\t" "%d -> %d;\n", i, mainList->data[i].prev);

    }

    fprintf (graph,"}\n");

    fclose (graph);
    system ("dot -Tpng graph.dot -o graph.png");

    ASSERT_OK

}

void ListDump (List* mainList) {

    DbgFunc ("Pointer on mainList = %p<br>\n", __FUNCTION__, mainList);

    FILE* html_output = fopen ("dump.html", "w");

    MakeListGraph (mainList);

    fprintf (html_output, "<!DOCTYPE html>\n"
                          "<html lang=\"en\">\n"
                          "<head>\n"
                          "\t<meta charset=\"UTF-8\">\n"
                          "\t<title>List dump</title>\n"
                          "<link href=\"https://fonts.googleapis.com/css2?family=Open+Sans&display=swap\" rel=\"stylesheet\">"
                          "<link rel=\"stylesheet\" href=\"style.css\">\n"
                          "</head>\n"
                          "<body>\n");
    //log file in html. записать историю дампов. Каждая функция в дамп пишет то, что она делает.
    fprintf (html_output, "<div class = \"container\">");

    fprintf (html_output, "<ul>"
                          "<li>Head index = %d</li>"
                          "<li>Tail index = %d</li>"
                          "<li>Head index of free element's list = %d</li>"
                          "</ul>", mainList->head, mainList->tail, mainList->head_free_el);

    fprintf (html_output, "<span> List size = %zu </span> <br>\n <span> List capacity = %zu </span><br>\n", mainList->size, mainList->capacity);

    fprintf (html_output, "<img src = \"graph.png\">");

    fprintf (html_output, "<a href = \"graph.png\" download>Download graphic dump</a>");

    fprintf (html_output, "</div>\n"
                          "</body>\n"
                          "</html>");

    fclose (html_output);

    system ("dump.html");

}

ErrType LogicSort (List* mainList) {

    DbgFunc ("Pointer on mainList = %p<br>\n", __FUNCTION__, mainList);

    ASSERT_OK

    size_t listSize = mainList->size;

    List tempList = {};
    ListConstruct (&tempList, listSize);

    for (size_t i = mainList->head; i != 0; i = mainList->data[i].next) {

        PushTail (&tempList, mainList->data[i].value);

    }

    ListDestruct (mainList);

    ListConstruct (mainList, listSize);

    for (size_t i = tempList.head; i != 0; i = tempList.data[i].next) {

        PushTail (mainList, tempList.data[i].value);

    }

    mainList->sorted = true;

    ASSERT_OK

}

ErrType SwapNodes (List* mainList, size_t firstIndex, size_t secondIndex) {

    ASSERT_OK

    if (firstIndex == 0 || secondIndex == 0) {

        printf ("swap index can not be 0 (zero)!\n");
        return ZEROINDEX;

    }

    //меняем значения
    Element tempValue = mainList->data[firstIndex].value;
    mainList->data[firstIndex].value = mainList->data[secondIndex].value;
    mainList->data[secondIndex].value = tempValue;
    //поменяли значения

    //меняем next-ы, предварительно их сохранив
    int firstNext  = mainList->data[firstIndex].next;
    int secondNext = mainList->data[secondIndex].next;

    mainList->data[secondIndex].next = firstNext;
    mainList->data[firstIndex].next  = secondNext;
    //махнули next-ы

    //меняем prev-ы, предварительно их сохранив
    int firstPrev  = mainList->data[firstIndex].prev;
    int secondPrev = mainList->data[secondIndex].prev;

    mainList->data[secondIndex].prev = firstPrev;
    mainList->data[firstIndex].prev  = secondPrev;
    //махнули prev-ы

    //исправляем индексацию среди предыдущих элементов, связанных с нашими двумя
    if (firstNext != 0 && firstNext != secondIndex) {
        mainList->data[firstNext].prev = secondIndex;
    } else if (firstNext == 0)
        mainList->tail = secondIndex;

    if (secondNext != 0 && secondNext != firstIndex) {
        mainList->data[secondNext].prev = firstIndex;
    } else if (secondNext == 0)
        mainList->tail = firstIndex;

    if (firstPrev != 0 && firstPrev != secondIndex) {
        mainList->data[firstPrev].next = secondIndex;
    } else if (firstPrev == 0)
        mainList->head = secondIndex;

    if (secondPrev != 0 && secondPrev != firstIndex) {
        mainList->data[secondPrev].next = firstIndex;
    } else if (secondPrev == 0)
        mainList->head = firstIndex;
    //исправили вроде

    ASSERT_OK

}

ErrType LogicSortV2 (List* mainList) {

    DbgFunc ("Pointer on mainList = %p<br>\n", __FUNCTION__, mainList);

    ASSERT_OK

    size_t currentLogicIndex = 1;
    size_t currentPhysIndex  = mainList->head;

    for (; mainList->data[currentPhysIndex].next != 0; currentPhysIndex = mainList->data[currentPhysIndex].next, currentLogicIndex++) {

        if (currentPhysIndex != currentLogicIndex) {

            SwapNodes (mainList, currentLogicIndex, currentPhysIndex);
            currentPhysIndex = currentLogicIndex;

        }

    }

    if (currentPhysIndex != currentLogicIndex) SwapNodes (mainList, currentLogicIndex, currentPhysIndex);

    mainList->sorted = true;

    ASSERT_OK

    return NOERR;

}

Node GetNode (List* mainList, const size_t position) {

    DbgFunc ("Pointer on mainList = %p<br>\tPosition = %zu<br>\n", __FUNCTION__, mainList, position);

    return mainList->data[position];

}

int FindValuePos (List* mainList, const Element value) {

    DbgFunc ("Pointer on mainList = %p<br>\tValue = %lg<br>\n", __FUNCTION__, mainList, value);

    for (size_t i = mainList->head; i != 0; i = mainList->data[i].next) {

        if ( mainList->data[i].value == value ) return i;

    }

    return 0;

}

int FindPhysicByLogicPos (List* mainList, const size_t position) {

    DbgFunc ("Pointer on mainList = %p<br>\tPosition = %zu<br>\n", __FUNCTION__, mainList, position);

    ASSERT_OK

    if (position >= mainList->size) {

        printf ("You entered position to delete more than size of your list in function %s\n", __FUNCTION__);
        return BADPOSITION;

    }

    if (mainList->sorted)
        return position;

    size_t indexRes = mainList->head;
    Node currentNode = mainList->data[mainList->head];

    for (int i = 0; i < position; i++) {

        indexRes    = currentNode.next;
        currentNode = mainList->data[currentNode.next];

    }

    if (indexRes == 0) {

        printf ("There are not element in position like that!\n");
        return NO_ELEM;

    }
    //may plus another one ASSERT_OK
    //set const
    ASSERT_OK

    return indexRes;

}

Element GetValueElem (List* mainList, const size_t position) {

    DbgFunc ("Pointer on mainList = %p<br>\tPosition = %zu<br>\n", __FUNCTION__, mainList, position);

    ASSERT_OK

    assert (position < mainList->size);

    return mainList->data[position].value;

}

ErrType PushIntoFreeList (List* mainList, const size_t position) {

    DbgFunc ("Pointer on mainList = %p<br>\tPosition = %zu<br>\n", __FUNCTION__, mainList, position);

    ASSERT_OK

    if (mainList->head_free_el != 0) //if there is a head
        mainList->data[mainList->head_free_el].prev = position;


    mainList->data[position].next = mainList->head_free_el;
    mainList->head_free_el = position;

    ASSERT_OK

}

ErrType ListOk (List* mainList) {

    DbgFunc ("Pointer on mainList = %p<br>\n", __FUNCTION__, mainList);

    #define DEF_ERR(name, code_list_ok, code_any_place) code_list_ok

        #include "../ErrList.txt"

    #undef DEF_ERR

    return NOERR;

}
