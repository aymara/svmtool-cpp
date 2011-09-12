/*
 * Copyright (C) 2004 Jesus Gimenez, Lluis Marquez and Senen Moya
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"
#include "list.h"
#include "dict.h"
#include "weight.h"
#include "swindow.h"
#include "mapping.h"
#include "learner.h"
#include "er.h"
#include "common.h"

int verbose = FALSE;

void printHelp(char *progname)
{
    fprintf(stderr,"\n\nSVMTool++ v 1.1.6 - SVMTLearner\n\n");
	fprintf(stderr,"\nUsage : %s [options] <model> <config-file>", progname);
	fprintf(stderr,"\noptions:\n");
	fprintf(stderr,"\n\t-V or -v:\tverbose");
	fprintf(stderr,"\n\nExample: %s -V config.svmt\n\n", progname);
}

int options(int argc,char *argv[])
{

  if ( argc < 2 ) return -1;

  for (int i=1;i<argc-1;i++)
    {
      if (strcmp(argv[i],"-v")!=0 || strcmp(argv[i],"-V")!=0)
	{ 
	  verbose = TRUE;	  
	  return 1;
	}
      else return -1;
    }

  return 0;
}

int main(int argc, char *argv[])
{
  erCompRegExp();

  int ret = options(argc,argv);
  if ( ret < 0 )
    {    
      printHelp(argv[0]);
      return -1;
    }

  learner L;
  L.learnerRun(argv[argc-1]);
  erFreeRegExp();
  return 0;
}

