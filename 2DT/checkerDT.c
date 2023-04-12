/*--------------------------------------------------------------------*/
/* checkerDT.c                                                        */
/* Author:                                                            */
/*--------------------------------------------------------------------*/

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "checkerDT.h"
#include "dynarray.h"
#include "path.h"



/* see checkerDT.h for specification */
boolean CheckerDT_Node_isValid(Node_T oNNode) {
   Node_T oNParent;
   Path_T oPNPath;
   Path_T oPPPath;

   /* Sample check: a NULL pointer is not a valid node */
   if(oNNode == NULL) {
      fprintf(stderr, "A node is a NULL pointer\n");
      return FALSE;
   }

   /* Sample check: parent's path must be the longest possible
      proper prefix of the node's path */
   oNParent = Node_getParent(oNNode);
   if(oNParent != NULL) {
      oPNPath = Node_getPath(oNNode);
      oPPPath = Node_getPath(oNParent);

      if(Path_getSharedPrefixDepth(oPNPath, oPPPath) !=
         Path_getDepth(oPNPath) - 1) {
         fprintf(stderr, "P-C nodes don't have P-C paths: (%s) (%s)\n",
                 Path_getPathname(oPPPath), Path_getPathname(oPNPath));
         return FALSE;
      }
   }

   return TRUE;
}

/*
   Performs a pre-order traversal of the tree rooted at oNNode.
   Returns FALSE if a broken invariant is found and
   returns TRUE otherwise.

   You may want to change this function's return type or
   parameter list to facilitate constructing your checks.
   If you do, you should update this function comment.
*/
static boolean CheckerDT_treeCheck(Node_T oNNode, size_t ulCount) {
   size_t ulIndex;
   size_t ulIndexB;
   size_t ulCheck = 0;

   if(oNNode!= NULL) {

      /* Sample check on each node: node must be valid */
      /* If not, pass that failure back up immediately */
      if(!CheckerDT_Node_isValid(oNNode))
         return FALSE;

      /* Recur on every child of oNNode */
      for(ulIndex = 0; ulIndex < Node_getNumChildren(oNNode); ulIndex++)
      {

         Node_T oNChild = NULL;
         Node_T oNChildPrev = NULL;
         int iStatus;

        ulCheck++;

        /* check that two consecutive child nodes have the same parent
        */
         iStatus = Node_getChild(oNNode, ulIndex, &oNChild);
         if (ulIndex != 0) {
            Node_getChild(oNNode, ulIndex - 1, &oNChildPrev);
            if(Node_compare(Node_getParent(oNChild),
                            Node_getParent(oNChildPrev)) != 0) {
                fprintf(stderr,
                "two consecutive child nodes must have the same parent\n"
                );
                return FALSE;
            }
         }

         if(iStatus != SUCCESS) {
            fprintf(stderr,
            "getNumChildren claims more children than getChild returns\n"
        );
        return FALSE;
         
      }

        /* make sure nodes are stored lexicographically */
        if (oNChildPrev != NULL){
            if (strcmp(Path_getPathname(Node_getPath(oNChild)),
                   Path_getPathname(Node_getPath(oNChildPrev))) < 0) {
                fprintf(stderr, "Nodes are not stored lexographically\n");
                return FALSE;
            }
        }
        
        /* check if there is another child of the same name */
        for(ulIndexB = 0;
            ulIndexB < Node_getNumChildren(oNNode);
            ulIndexB++) {
            Node_T oNChildB = NULL;
            
            /* ignore case where indices are the same */
            if (ulIndex == ulIndexB)
                continue;

            iStatus = Node_getChild(oNNode, ulIndexB, &oNChildB);
            /* DO WE NEED TO CHECK iStatus??????? */

            if(Node_compare(oNChild, oNChildB) == 0) {
                fprintf(stderr, "More than one identical node at %s\n",
                        Path_getPathname(Node_getPath(oNNode)));
                return FALSE;
            }
        }

        /* if recurring down one subtree results in a failed check
            farther down, passes the failure back up immediately */
        if(!CheckerDT_treeCheck(oNChild, ulCount))
            return FALSE;
      }

      if (ulCheck != ulCount){
         fprintf(stderr, "discrepancy in number of nodes in tree: %ul vs. %ul \n", ulCheck, ulCount);
         return FALSE;
      }
   }
   return TRUE;
}

/* see checkerDT.h for specification */
boolean CheckerDT_isValid(boolean bIsInitialized, Node_T oNRoot,
                          size_t ulCount) {

   /* Sample check on a top-level data structure invariant:
      if the DT is not initialized, its count should be 0. */
   if(!bIsInitialized)
      if(ulCount != 0) {
         fprintf(stderr, "Not initialized, but count is not 0\n");
         return FALSE;
      }

   /* Now checks invariants recursively at each node from the root. */
   return CheckerDT_treeCheck(oNRoot, ulCount);
}
