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
int VRB=0, GFFIN, PSR=1 ,GFFOUT=0;

/* Accounting time and results */
account *m;  

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

  /* Read setup options */
  readargv(argc,argv,HSPFile);
  
  /* initializing stats... */
  m = (account*)InitAcc(); 

  /* Alloc main memory structures */
  printMess("Request Memory to System for HSPs");
  allHsp = (packHSP*) RequestMemoryHSP();
  if (allHsp == NULL) {
    FreeAcc(m);
    return(1);
  }

  /* Reading HSPs */
  printMess("Reading HSP file");
  int nHsps = ReadHSP(allHsp,HSPFile,Query);
  if (nHsps <= 0) {
    /* 
      Empty file or reading error
      Cleanup
    */
    FreeMemoryHSP(allHsp);
    FreeAcc(m);
    
    /* Exit */
    int ret = abs(nHsps);
    return(ret);
  }
  sprintf(mess,"%ld HSPs read",allHsp->nTotalHsps);
  printMess(mess);
  
  /* Sorting HSPs */
  printMess("QuickSorting HSPs");
  SortHSP(allHsp);
  
  /* Project HSP to SR */
  if (PSR)
    {
      printMess("Request Memory to System for SRs");
      allSr = (packSR*) RequestMemorySR();
      if (allSr == NULL) {
        FreeAcc(m);
        FreeMemoryHSP(allHsp);
        return(1);
      }
      printMess("Project HSPs to SRs");
      if (ProjectHSP(allHsp, allSr) != 0) {
        FreeAcc(m);
        FreeMemoryHSP(allHsp);
        FreeMemorySR(allSr);
        return(1);
      };

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
  if (PSR)
    FreeMemorySR(allSr);
  FreeMemoryHSP(allHsp);
  FreeAcc(m);

  /* Exit */
  return(0);   
}
