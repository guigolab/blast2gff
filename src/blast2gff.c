/***************************************************************
*                                                              *
*   BLAST2GFF.                                                 *
*   (c) by Enrique Blanco &                                    *
*   Roderic Guigo, 2000                                        *
*                                                              *
*   Module: blast2gff.c                                        *
*                                                              *
*   Main program. Management of the actions of blast2gff       *
***************************************************************/

#include "blast2gff.h"

/* Setup Flags of blast2gff */
int   VRB=0, GFFIN, PSR=1 ,GFFOUT=0;

/* Accounting time and results */
account *m;  

void FreeSR(sr_t* q)
{
  
  if (q != NULL) {
    FreeSR(q->next);
    free(q);
  }
}

int main (int argc, char *argv[])
{
  /* Input Files */
  char   HSPFile[FILENAMELENGTH];
  
  /* HSP data structures */
  packHSP* allHsp;

  /* SR data structures */
  packSR* allSr;

  /* Query name */
  char Query[LOCUSLENGTH];

  char mess[MAXSTRING];

  /* initializing stats... */
  m = (account*)InitAcc(); 

  /* Read setup options */
  readargv(argc,argv,HSPFile);

  /* Alloc main memory structures */
  printMess("Request Memory to System");
  allHsp =      (packHSP*) RequestMemoryHSP();
  allSr  =      (packSR*) RequestMemorySR(); 

  /* Reading HSPs */
  printMess("Reading HSP file");
  ReadHSP(allHsp,HSPFile,Query);
  sprintf(mess,"%ld HSPs read",allHsp->nTotalHsps);
  printMess(mess);
  
  /* Sorting HSPs */
  printMess("QuickSorting HSPs");
  SortHSP(allHsp);
  
  /* Project HSP to SR */
  if (PSR)
    {
      printMess("Project HSPs to SRs");
      ProjectHSP(allHsp, allSr);

      printMess("Simplifiying SRs");
      JoinSR(allSr);

      printMess("MergeSorting SRs");
      SortSR(allSr);
    }
  
  /* Printing Results */
  printMess("Printing selected data");
  Output(allHsp, allSr, Query);

  /* The End */
  OutputTime(); 

   /* Free allocated memory */
   /* Free SR */
  for(int i=0; i<STRANDS*FRAMES; i++) {
    FreeSR(allSr->sr[i]);      
  }
  free(allSr);
  
  /* Free HSPs */
  for(int i=0;i<STRANDS*FRAMES;i++) {
    for(int j=0;j<allHsp->nTotalHsps;j++)
      free(allHsp->hsps[i][j]);
    free(allHsp->hsps[i]);
  }
  free(allHsp->hsps);
  free(allHsp->nHsps);
  free(allHsp);
  
  /* Free account data */
  free(m);

  /* Exit */
  exit(0);
  return(0);   
}
