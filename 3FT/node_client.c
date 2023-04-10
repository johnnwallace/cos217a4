/*--------------------------------------------------------------------*/
/* dt_client.c                                                        */
/* Author: Mirabelle Weinbach and John Wallace                        */
/*--------------------------------------------------------------------*/

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "nodeFT.h"
#include "path.h"
#include "dynarray.h"

/* Tests the Node implementation with an assortment of checks.
   Prints the status of the data structure along the way to stderr.
   Returns 0. */
int main(void) {
    Path_T pathA;
    Path_T pathB;
    Node_T nodeA;
    Node_T nodeB;
    Node_T testNode;
    char *pPathA = "a";
    char *pPathB = "a/b";
    size_t testSize;
    size_t ulIndex;
    int iStatus;

    iStatus = Path_new(pPathA, &pathA);
    if(iStatus != SUCCESS)
        printf("new path error: %d\n", iStatus);
    iStatus = Node_new(pathA, IS_DIRECTORY, NULL, &nodeA);
    if(iStatus != SUCCESS)
        printf("new node error: %d\n", iStatus);

    iStatus = Node_getNumChildren(nodeA, &testSize);

    printf("printing value stored from getNumChildren, should be 0:\n");
    printf("%d\n", (int)testSize);

    iStatus = Path_new(pPathB, &pathB);
    if(iStatus != SUCCESS)
        printf("new path error: %d\n", iStatus);
    iStatus = Node_new(pathB, IS_DIRECTORY, nodeA, &nodeB);
    if(iStatus != SUCCESS)
        printf("new node error: %d\n", iStatus);

    iStatus = Node_getNumChildren(nodeA, &testSize);

    assert(Node_compare(nodeA, Node_getParent(nodeB)) == 0);

    iStatus = Node_getChild(nodeA, 0, &testNode);
    if(iStatus != SUCCESS)
        printf("getChild error: %d\n", iStatus);
    assert(Node_compare(nodeB, testNode) == 0);
        
    printf("printing value stored from getNumChildren, should be 1:\n");
    printf("%d\n", (int)testSize);

    printf("printing return val of hasChild on nodeA, shoudl be TRUE:\n");
    printf("%d\n", (int)Node_hasChild(nodeA, pathB, &ulIndex));
    printf("freeing %ld nodes", Node_free(nodeA));

    return 0;
}
