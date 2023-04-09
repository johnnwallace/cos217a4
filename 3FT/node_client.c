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

/* Tests the Node implementation with an assortment of checks.
   Prints the status of the data structure along the way to stderr.
   Returns 0. */
int main(void) {
    Path_T *pathA;
    Path_T *pathB;
    Node_T *nodeA;
    Node_T *nodeB;
    char *pPathA = "a";
    char *pPathB = "a/b";
    size_t *testSize;
    int iStatus;

    iStatus = Path_new(pPathA, pathA);
    if(iStatus != SUCCESS)
        printf("%d", iStatus)
    iStatus = Node_new(pathA, DIRECTORY, NULL, nodeA);
    if(iStatus != SUCCESS)
        printf("%d", iStatus);

    iStatus = Node_getNumChildren(nodeA, testSize);

    printf("printing value stored from getNumChildren, should be 0\n");
    printf(*testSize);

    iStatus = Path_new(pPathB, pathB);
    if(iStatus != SUCCESS)
        printf("%d", iStatus)
    iStatus = Node_new(pathB, DIRECTORY, NULL, nodeB);
    if(iStatus != SUCCESS)
        printf("%d", iStatus);

    iStatus = Node_getNumChildren(nodeA, testSize);
        
    printf("printing value stored from getNumChildren, should be 1\n");
    printf(*testSize);

    printf("printing return val of hasChild on nodeA, shoudl be TRUE\n");
    printf(Node_hasChild(nodeA));

    return 0;
}
