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
#include "tagger.h"
#include "er.h"

#define ERROR1	"\nERROR: Value incorrect in -T option.\n"
#define ERROR2	"\nERROR: Value incorrect in -S option.\n"
#define ERROR3	"\nERROR: Value incorrect in -I option.\n"
#define ERROR4	"\nERROR: Value incorrect in -L option.\n"
#define ERROR5	"\nERROR: Value incorrect in -I or -L option. Window length have to be greater than Interest Point.\n"
#define ERROR6	"\nERROR: You can not change Interes Point (-I) if you don't change the length of the window (-L).\n"
#define ERROR8	"\nERROR: Incorrect Options\n"
#define ERRORK  "\nERROR: Value incorrect in -K option.\n"
#define ERRORU  "\nERROR: Value incorrect in -U option.\n"

int verbose = 0;

void printHelp(char *progname)
{
	fprintf(stderr,"\nSVMTool++ v 1.1.6 -- SVMTagger\n\n");
	fprintf(stderr,"Usage : %s [options] <model>  < stdin > stdout\n\n", progname);
	fprintf(stderr,"options:\n");
	fprintf(stderr,"\t-L or -l <Window Lenght>\n");
	fprintf(stderr,"\t\t<Window Lenght> have to be greater than 2\n");
	fprintf(stderr,"\t\t7 (default)\n");
	fprintf(stderr,"\t-I or -i <Interest Point>\n");
	fprintf(stderr,"\t\t<Interest Point> have to be greater than 0\n");
	fprintf(stderr,"\t\t4 (default)\n");
	fprintf(stderr,"\t-S or -s <direction>\n");
	fprintf(stderr,"\t\tLR\tleft-to-right\t(default)\n");
	fprintf(stderr,"\t\tRL\tright-to-left\n");
	fprintf(stderr,"\t\tLRL\tboth left-to-right and right-to-left\n");
	fprintf(stderr,"\t-T or -t <strategy>\n");
	fprintf(stderr,"\t\t0\tone-pass   [default - requires model 0]\n");
	fprintf(stderr,"\t\t1\ttwo-passes [revisiting results and relabeling - requires model 2 and model 1]\n");
	fprintf(stderr,"\t\t2\tone-pass   [robust against unknown words - requires model 0 and model 2]\n");
	fprintf(stderr,"\t\t3\tone-pass   [unsupervised learning models - requires model 3]\n");
	fprintf(stderr,"\t\t4\tone-pass   [very robust against unknown words - requires model 4]\n");	
	fprintf(stderr,"\t\t5\tone-pass   [sentence-level likelihood - requires model 0] Not implemented!!\n");
	fprintf(stderr,"\t\t6\tone-pass   [robust sentence-level likelihood - requires model 4] Not implemented!!\n");
	fprintf(stderr,"\t-B or -b <backup_lexicon>\n");
	fprintf(stderr,"\t-K <n> weight filtering threshold for known words (default is 0)\n");
	fprintf(stderr,"\t-U <n> weight filtering threshold for unknown words (default is 0)\n");
	fprintf(stderr,"\t-V or -v verbose\n");
	fprintf(stderr,"\t-A or -a show scores\n");
	fprintf(stderr,"\nmodel: model location (path/name)\n");
	fprintf(stderr,"\nUsage : %s -V -S LRL -T 0 /home/usuaris/smoya/SVMT/eng/WSJTP < WSJTP.TEST > WSJTP.TEST.OUT\n\n", progname);
}

/*
-1 -t error
-2 -s error
-3 -i error
-4 -l error
-5 length<=ip
*/

int options(int argc,char *argv[])
{
	int isIP=0,ip=4,isLength=0,length=7;

	for (int i=1;i<argc-1;i++)
	{		
	        if (strcmp(argv[i],"-t")==0 || strcmp(argv[i],"-T")==0)
		{	
		  i++;
		  //if (atoi(argv[i])==0) return -1;
		}
		else if (strcmp(argv[i],"-s")==0 || strcmp(argv[i],"-S")==0)
		{	i++;
			if (!((strcmp(argv[i],"LR")==0) ||
			(strcmp(argv[i],"RL")==0)   ||
			(strcmp(argv[i],"LRL")==0)) ) return -2;
		}
		else if (strcmp(argv[i],"-i")==0 || strcmp(argv[i],"-I")==0)
		{	i++;
			if (atoi(argv[i])<1) return -3;
			else ip = atoi(argv[i]);
			isIP = 1;
		}
		else if (strcmp(argv[i],"-l")==0 || strcmp(argv[i],"-L")==0)
		{	i++;
			if (atoi(argv[i])<0) return -4;
			else length = atoi(argv[i]);
			isLength = 1;
		}
		else if (strcmp(argv[i],"-b")==0 || strcmp(argv[i],"-B")==0)
		{
			if (strcmp(argv[i+1],"-v")!=0 && strcmp(argv[i+1],"-V")!=0
				&& strcmp(argv[i+1],"-b")!=0 && strcmp(argv[i+1],"-B")!=0
				&& strcmp(argv[i+1],"-l")!=0 && strcmp(argv[i+1],"-L")!=0
				&& strcmp(argv[i+1],"-s")!=0 && strcmp(argv[i+1],"-S")!=0
				&& strcmp(argv[i+1],"-t")!=0 && strcmp(argv[i+1],"-T")!=0
				&& strcmp(argv[i+1],"-i")!=0 && strcmp(argv[i+1],"-I")!=0) i++;

		}
		else if (strcmp(argv[i],"-k")==0 || strcmp(argv[i],"-K")==0)
		{   
		    float k;
		    if ( sscanf(argv[i],"%5f",&k) == EOF) return 'k';
		}
		else if (strcmp(argv[i],"-u")==0 || strcmp(argv[i],"-U")==0)
		{
		    float u;
		    if (sscanf(argv[i],"%5f",&u) == EOF) return 'u';
		}
		else if ( strcmp(argv[i],"-a")!=0 && strcmp(argv[i],"-A")!=0 &&
			  strcmp(argv[i],"-v")!=0 && strcmp(argv[i],"-V")!=0 ) return -8;
	}

	if (isIP && !isLength) return -6;
	if (isIP && isLength && length<=ip) return -5;

	return 0;
}

int main(int argc, char *argv[])
{
	int ret=0;
	
	erCompRegExp();

	if ((argc<=1) || ((ret=options(argc,argv))<0))
	{
		switch(ret)
		{
		    case -1: fprintf(stderr,ERROR1); break;
		    case -2: fprintf(stderr,ERROR2); break;
		    case -3: fprintf(stderr,ERROR3); break;
		    case -4: fprintf(stderr,ERROR4); break;
		    case -5: fprintf(stderr,ERROR5); break;
		    case -6: fprintf(stderr,ERROR6); break;
		    case -8: fprintf(stderr,ERROR8); break;
		    case 'k': fprintf(stderr,ERRORK); break;
		    case 'u': fprintf(stderr,ERRORU); break;
		}

		printHelp(argv[0]);
		exit(0);
	}

	tagger t(argv[argc-1]);

	for (int i=1;i<argc-1;i++)
	{
	  if (strcmp(argv[i],"-k")==0 || strcmp(argv[i],"-K")==0)
		{ t.taggerPutKWeightFilter(atof(argv[i+1])); i++;}
	  else if (strcmp(argv[i],"-u")==0 || strcmp(argv[i],"-U")==0)
		{ t.taggerPutUWeightFilter(atof(argv[i+1])); i++;}
	  else if (strcmp(argv[i],"-t")==0 || strcmp(argv[i],"-T")==0)
		{ 
		  //modstat t.taggerPutStrategy(atoi(argv[i+1])-1); 
		  t.taggerPutStrategy(atoi(argv[i+1])); 
		  i++;
		}
	  else if (strcmp(argv[i],"-s")==0 || strcmp(argv[i],"-S")==0)
		{ t.taggerPutFlow(argv[i+1]); i++;}
	  else if (strcmp(argv[i],"-v")==0 || strcmp(argv[i],"-V")==0) verbose = 1;
	  else if (strcmp(argv[i],"-a")==0 || strcmp(argv[i],"-A")==0) 
	        { t.taggerActiveShowScoresFlag(); }
	  else if (strcmp(argv[i],"-l")==0 || strcmp(argv[i],"-L")==0)
		{ t.taggerPutWinLength(atoi(argv[i+1])); i++;}
	  else if (strcmp(argv[i],"-i")==0 || strcmp(argv[i],"-I")==0)
		{ t.taggerPutWinIndex(atoi(argv[i+1])); i++;}
	  else if (strcmp(argv[i],"-b")==0 || strcmp(argv[i],"-B")==0)
		{ t.taggerPutBackupDictionary(argv[i+1]); i++;}
	}

	if (verbose) fprintf(stderr,"\nSVMTool++ v 1.1.6 -- SVMTagger\n\n");

	t.taggerLoadModelsForTagging();

	t.taggerInit(std::cin, std::cerr);
	t.taggerRun();

	erFreeRegExp();
	return 0;
}





