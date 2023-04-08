/*--------------------------------------------------------------------*/
/* nodeFT.c                                                          */
/* Author: Mirabelle Weinbach and John Wallace                        */
/*--------------------------------------------------------------------*/

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "dynarray.h"
#include "nodeFT.h"


/* A node in a FT */
struct node {
    /* the object corresponding to the node's absolute path */
    Path_T oPPath;
    /* this node's parent */
    Node_T oNParent;
    /* the object containing links to this node's children */
    DynArray_T oDChildren;
    /* the type of node - either DIRECTORY or FILE */
    nodeType type;
    /* the contents of the file; null if node is a directory */
    void *pvContents;
};

/* ------------------------------------------------------------------ */

/*
  Links new child oNChild into oNParent's children array at index
  ulIndex. Returns SUCCESS if the new child was added successfully,
  or  MEMORY_ERROR if allocation fails adding oNChild to the array.
*/

static int Node_addChild(Node_T oNParent, Node_T oNChild,
                         size_t ulIndex) {
    assert(oNParent != NULL);
    assert(oNChild != NULL);

    if(oNParent -> type != DIRECTORY)
        return NOT_A_DIRECTORY;

    if(DynArray_addAt(oNParent->oDChildren, ulIndex, oNChild))
        return SUCCESS;
    else
        return MEMORY_ERROR;
}

/* ------------------------------------------------------------------ */

int Node_new(Path_T oPPath, nodeType type, Node_T oNParent,
             Node_T *poNResult) {
    struct node *psNew;
    Path_T oPParentPath = NULL;
    Path_T oPNewPath = NULL;
    
    assert(oPPath != NULL);

    /* allocate space for a new node */
    psNew = malloc(sizeof(struct node));
    if(psNew == NULL) {
        *poNResult = NULL;
        return MEMORY_ERROR;
    }
    psNew->pvContents = NULL;
    psNew->type = type; /* set the node's type */

    /* set the new node's path */
    iStatus = Path_dup(oPPath, &oPNewPath);
    if(iStatus != SUCCESS) {
        free(psNew);
        *poNResult = NULL;
        return iStatus;
    }
    psNew->oPPath = oPNewPath;

    /* validate and set the new node's parent */
    if(oNParent != NULL) {
        size_t ulSharedDepth;

        oPParentPath = oNParent->oPPath;
        ulParentDepth = Path_getDepth(oPParentPath);
        ulSharedDepth = Path_getSharedPrefixDepth(psNew->oPPath,
                                                    oPParentPath);

        /* parent must be a directory */
        if (oNParent -> type == FILE){
            Path_free(psNew->oPPath);
            free(psNew);
            *poNResult = NULL;
            return NOT_A_DIRECTORY;
        }

        /* parent must be an ancestor of child */
        if(ulSharedDepth < ulParentDepth) {
            Path_free(psNew->oPPath);
            free(psNew);
            *poNResult = NULL;
            return CONFLICTING_PATH;
        }

        /* parent must be exactly one level up from child */
        if(Path_getDepth(psNew->oPPath) != ulParentDepth + 1) {
            Path_free(psNew->oPPath);
            free(psNew);
            *poNResult = NULL;
            return NO_SUCH_PATH;
        }

        /* parent must not already have child with this path */
        if(Node_hasChild(oNParent, oPPath, &ulIndex)) {
            Path_free(psNew->oPPath);
            free(psNew);
            *poNResult = NULL;
            return ALREADY_IN_TREE;
        }
    }
    else {
        /* new node must be root and therefore must be directory*/
        /* can only create one "level" at a time */
        if(oNParent -> type == FILE) {
            Path_free(psNew->oPPath);
            free(psNew);
            *poNResult = NULL;
            return NOT_A_DIRECTORY;
        }

        if(Path_getDepth(psNew->oPPath) != 1) {
            Path_free(psNew->oPPath);
            free(psNew);
            *poNResult = NULL;
            return NO_SUCH_PATH;
        }
    }
    psNew->oNParent = oNParent;

    if(oNParent != NULL) {
        iStatus = Node_addChild(oNParent, psNew, ulIndex);
        if (iStatus != SUCCESS) {
            Path_free(psNew->oPPath);
            free(psNew);
            *poNResult = NULL;
            return iStatus;
        }
    }

    *poNResult = psNew;

    return SUCCESS;
}

/* ------------------------------------------------------------------ */

size_t Node_free(Node_T oNNode) {
    
}