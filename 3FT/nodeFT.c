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
    /* the type of node - either IS_DIRECTORY or IS_FILE */
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

    if(oNParent -> type != IS_DIRECTORY)
        return NOT_A_DIRECTORY;

    if(DynArray_addAt(oNParent->oDChildren, ulIndex, oNChild))
        return SUCCESS;
    else
        return MEMORY_ERROR;
}

/* ------------------------------------------------------------------ */

/*
  Compares the string representation of oNfirst with a string
  pcSecond representing a node's path.
  Returns <0, 0, or >0 if oNFirst is "less than", "equal to", or
  "greater than" pcSecond, respectively.
*/

static int Node_compareString(const Node_T oNFirst,
                                 const char *pcSecond) {
   assert(oNFirst != NULL);
   assert(pcSecond != NULL);

   return Path_compareString(oNFirst->oPPath, pcSecond);
}

/* ------------------------------------------------------------------ */

int Node_new(Path_T oPPath, nodeType type, Node_T oNParent,
             Node_T *poNResult) {
    struct node *psNew;
    Path_T oPParentPath = NULL;
    Path_T oPNewPath = NULL;
    int iStatus;
    size_t ulIndex;
    
    assert(oPPath != NULL);

    /* allocate space for a new node */
    psNew = calloc(1, sizeof(struct node));
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
        size_t ulParentDepth;

        oPParentPath = oNParent->oPPath;
        ulParentDepth = Path_getDepth(oPParentPath);
        ulSharedDepth = Path_getSharedPrefixDepth(psNew->oPPath,
                                                    oPParentPath);

        /* parent must be a directory */
        if (oNParent -> type == IS_FILE){
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

        if(Path_getDepth(psNew->oPPath) != 1) {
            Path_free(psNew->oPPath);
            free(psNew);
            *poNResult = NULL;
            return NO_SUCH_PATH;
        }
    }
    psNew->oNParent = oNParent;

    /* initialize the new node */
    psNew->oDChildren = DynArray_new(0);
    if(psNew->oDChildren == NULL) {
        Path_free(psNew->oPPath);
        free(psNew);
        *poNResult = NULL;
        return MEMORY_ERROR;
    }

    /* Link into parent's children list */
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


int Node_compare(Node_T oNFirst, Node_T oNSecond) {
   assert(oNFirst != NULL);
   assert(oNSecond != NULL);

   return Path_comparePath(oNFirst->oPPath, oNSecond->oPPath);
}

/* ------------------------------------------------------------------ */


size_t Node_free(Node_T oNNode) {
    size_t ulIndex;
    size_t ulCount = 0;

    assert(oNNode != NULL);

    /* remove from parent's list */
    if(oNNode->oNParent != NULL) {
        if(DynArray_bsearch(
                oNNode->oNParent->oDChildren,
                oNNode, &ulIndex,
                (int (*)(const void *, const void *)) Node_compare)
            )
            (void) DynArray_removeAt(oNNode->oNParent->oDChildren,
                                    ulIndex);
    }

    /* recursively remove children */
    while(DynArray_getLength(oNNode->oDChildren) != 0) {
        ulCount += Node_free(DynArray_get(oNNode->oDChildren, 0));
    }
    DynArray_free(oNNode->oDChildren);

    /* remove path */
    Path_free(oNNode->oPPath);

    /* remove contents if file */
    free(oNNode -> pvContents);

    /* finally, free the struct node */
    free(oNNode);
    ulCount++;
    return ulCount;
}

/* ------------------------------------------------------------------ */

Path_T Node_getPath(Node_T oNNode) {
   assert(oNNode != NULL);

   return oNNode->oPPath;
}

/* ------------------------------------------------------------------ */


boolean Node_hasChild(Node_T oNParent, Path_T oPPath,
                         size_t *pulChildID) {
    assert(oNParent != NULL);
    assert(oPPath != NULL);
    assert(pulChildID != NULL);

    /* invariant */
    if (oNParent -> type == IS_FILE){
        return FALSE;
    }

    /* *pulChildID is the index into oNParent->oDChildren */
    return DynArray_bsearch(oNParent->oDChildren,
            (char*) Path_getPathname(oPPath), pulChildID,
            (int (*)(const void*,const void*)) Node_compareString);
}

/* ------------------------------------------------------------------ */

/* Returns the number of children that oNParent has. */
int Node_getNumChildren(Node_T oNParent, size_t *pulNum) {
    assert(oNParent != NULL);

    /* verify oNParent is a directory */
    if (oNParent -> type == IS_FILE){
        return NOT_A_DIRECTORY;
    }

    *pulNum = DynArray_getLength(oNParent -> oDChildren);
    return SUCCESS;
}

/* ------------------------------------------------------------------ */

int Node_getChild(Node_T oNParent, size_t ulChildID,
                  Node_T *poNResult) {

    int iStatus;
    size_t iChildren;
    
    assert(oNParent != NULL);
    assert(poNResult != NULL);

    iStatus = Node_getNumChildren(oNParent, &iChildren);
    if (iStatus != SUCCESS) {
        return iStatus;
    }

    /* ulChildID is the index into oNParent->oDChildren */
    if(ulChildID >= iChildren) {
        *poNResult = NULL;
        return NO_SUCH_PATH;
    }
    else {
        *poNResult = DynArray_get(oNParent->oDChildren, ulChildID);
        return SUCCESS;
    }
}


/* ------------------------------------------------------------------ */

Node_T Node_getParent(Node_T oNNode) {
   assert(oNNode != NULL);

   return oNNode->oNParent;
}

/* ------------------------------------------------------------------ */


