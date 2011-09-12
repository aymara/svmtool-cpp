#include <stdio.h>
#include <stdlib.h>
#include "api.h"

int main(int argc, char *argv[])
{
  if ( argc <= 1 )
    {
      fprintf(stderr, "Usage: main <Model>\n");
      fprintf(stderr, "For example: main /mnt/hda4/pfc/models/eng/WSJTP\n");
      exit(0);
    }

  fprintf(stderr,"Api Sample Program: Tagging \"Hello World! It is a sample program.\"\n\n");
  fprintf(stderr,"\n Steps\n");
  fprintf(stderr,"_______\n");
  fprintf(stderr,"1.- Create Tagger with function --> apiTaggerCreate(<Model>)\n");
  fprintf(stderr,"\n\t\t apiTaggerCreate(\"%s\")\n\n",argv[1]);
  apiTaggerCreate(argv[1]);

  fprintf(stderr,"2.- Initialize Tagger --> apiTaggerInitialize(<Strategy>,\n");
  fprintf(stderr,"                                              <Direction>,\n");
  fprintf(stderr,"                                              <Sliding Window Length>,\n");
  fprintf(stderr,"                                              <Sliding Window Interest Point>,\n");
  fprintf(stderr,"                                              <Weight Filter for Known Words>,\n");
  fprintf(stderr,"                                              <Weight Filter for Unkown>)\n");

  fprintf(stderr,"\n\t\tapiTaggerInitialize( 0, \"LR\", 7, 3, 0, 0)\n\n");
  apiTaggerInitialize( 0, "LR", 7, 3, 0, 0);

  fprintf(stderr,"3.- Tagging Sentence --> apiTaggerRun(<Tokenized Text>,<Number of words>)\n");	
  fprintf(stderr,"\n\t\tapiTaggerRun(\"Hello world ! It is a sample program .\",9)\n\n");
  apiResult *ret = apiTaggerRun("Hello world ! It is a sample program .",9);
  
  fprintf(stderr,"4.- Results\n\n");
  ret->print();
  delete ret;
  
  fprintf(stderr,"\n5.- Destroying objects --> apiTaggerDestroy()\n\n");
  apiTaggerDestroy();

  return 0;
}
