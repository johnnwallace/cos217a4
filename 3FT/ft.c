/*--------------------------------------------------------------------*/
/* ft.c                                                               */
/* Author: Mirabelle Weinbach and John Wallace                        */
/*--------------------------------------------------------------------*/


#include <stddef.h>
#include "ft.h"
#include "nodeFT.h"



int FT_insertDir(const char *pcPath){

}


boolean FT_containsDir(const char *pcPath){}


int FT_rmDir(const char *pcPath){}



int FT_insertFile(const char *pcPath, void *pvContents,
                  size_t ulLength){}


boolean FT_containsFile(const char *pcPath){}


int FT_rmFile(const char *pcPath){}


void *FT_getFileContents(const char *pcPath){}


void *FT_replaceFileContents(const char *pcPath, void *pvNewContents,
                             size_t ulNewLength){}


int FT_stat(const char *pcPath, boolean *pbIsFile, size_t *pulSize){}


int FT_init(void){}


int FT_destroy(void){}

char *FT_toString(void){}


