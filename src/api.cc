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

#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "api.h"
#include "hash.h"
#include "list.h"
#include "dict.h"
#include "weight.h"
#include "swindow.h"
#include "tagger.h"
#include "er.h"

/*****************************************************************/

nodoResult::nodoResult()
{
  this->pos = NULL;
  this->wrd = NULL;
  this->scores = NULL;
}

nodoResult::~nodoResult()
{
  delete[] this->wrd;
  delete[] this->pos;
  delete[] this->scores;
  index = 0;
}

void nodoResult::pushIndex(int iNum)
{
  this->index = iNum;
}

void nodoResult::pushScores(char *text)
{
  this->scores = new char[strlen(text)+1];
  strcpy(this->scores,text);
}

void nodoResult::pushPOS(char *text)
{
  this->pos = new char[strlen(text)+1];
  strcpy(this->pos,text);
}

void nodoResult::pushWord(char *text)
{
  this->wrd = new char[strlen(text)+1];
  strcpy(this->wrd,text);
}

int nodoResult::getIndex()
{
  return this->index;
}

char *nodoResult::getScores()
{
  return this->scores;
}

char *nodoResult::getWord()
{
  return this->wrd;
}

char *nodoResult::getPOS()
{
  return this->pos;
}

/*****************************************************************/

apiResult::apiResult(int iNum)
{
  numItems = iNum;
  array = new nodoResult[iNum];
}

apiResult::~apiResult()
{
  delete[] array;
}

int apiResult::pushScores(char *text, int iPos)
{
  if (iPos >= this->numItems) return -1;
  array[iPos].pushScores(text);
  return 0;
}

int apiResult::pushPOS(char *text, int iPos)
{
  if (iPos >= this->numItems) return -1;
  array[iPos].pushPOS(text);
  return 0;
}

int apiResult::pushWord(char *text, int iPos)
{
  if (iPos >= this->numItems) return -1;
  array[iPos].pushWord(text);
  return 0;
}

char *apiResult::getScores(int iPos)
{
  return array[iPos].getScores();
}

char *apiResult::getPOS(int iPos)
{
  return array[iPos].getPOS();
}

char *apiResult::getWord(int iPos)
{
  return array[iPos].getWord();
}

void apiResult::print()
{
  for (int i=0; i < this->numItems; i++)
    {
      fprintf(stdout, "%s %s %s\n",this->getWord(i),array[i].getPOS(),array[i].getScores());
    }
}
/*****************************************************************/

tagger *t;
int verbose = 0;

int apiInsertSentence(const char *szSentence)
{
  	char wrd[200];
	FILE *f = fopen ("stdin.tmp","w");
	unsigned int i = 0;

	if ( t == NULL ) return -1;

	for (i=0; szSentence[i] !='\0' && i < strlen(szSentence); i++)
	{
		strcpy(wrd,"");
		int ret = sscanf(szSentence+i,"%s ",wrd);
		
		if (ret > 0)
		{		
			fprintf(f,"%s\n",wrd);
			i = i + strlen(wrd);
		}
		else break;
		
	}

	fclose(f);
	
	return i;
}

apiResult *apiTaggerRun(const char *szSentence, int iNumWords)
{
	if ( t == NULL ) return NULL;
	
	int ret = apiInsertSentence (szSentence);
	
	if ( ret == -1 ) return NULL;
	
	char tmp1[500],tmp2[500], tmp3[2000]; 
	char aux[3000];

	fflush(stdin);
	std::ifstream in("stdin.tmp");
	std::ofstream fout("stdout.tmp");
	
	t->taggerShowNoComments();
	t->taggerActiveShowScoresFlag();

	t->taggerInit(in,fout);
	t->taggerRun();
  fout.close();
  
	FILE *f = fopen("stdout.tmp","r");
	
	int index = 0;
	apiResult *out = new apiResult(iNumWords);

	while (!feof(f) && index < iNumWords)
	{
	        strcpy(aux,"");
		strcpy(tmp1,""); strcpy(tmp2,""); strcpy(tmp3,"");
		char *ret = fgets(aux,3000,f);
		if (ret != NULL )
		  { 
		    sscanf(aux,"%s %s",tmp1, tmp2);
		    if ((strlen(tmp1)+strlen(tmp2)+2) < strlen(aux))
		    {
		      strcpy(tmp3,aux+strlen(tmp1)+1+strlen(tmp2)+1); 
		      tmp3[strlen(tmp3)-1] = '\0';
		    }	       
		    out->pushWord(tmp1, index);
		    out->pushPOS (tmp2, index);
		    out->pushScores (tmp3,index);
		    index++;
		  }
		
	}
			
	remove ( "stdin.tmp" );	
	remove ( "stdout.tmp" );
	return out;
}


int apiTaggerCreate( char *szModelName )
{
	if ( strcmp(szModelName,"")  == 0 ) return -1;	
	
	t = new tagger(szModelName);	
	
	return 0;
}

int apiTaggerInitialize ( int   iStrategy, 
			  const char  *szSense,
			  int   iWinLength,
			  int   iWinIndex,
			  float fWFKnown, 
			  float fWFUnk)
{
	if ( t == NULL ) return -1;
	
	verbose = 0;
	
	erCompRegExp();
	
	if ( fWFKnown != -1 ) t->taggerPutKWeightFilter(fWFKnown);
	if ( fWFUnk != -1 ) t->taggerPutUWeightFilter(fWFUnk);
	if ( iStrategy != -1 ) t->taggerPutStrategy(iStrategy); 	
	if ( strcmp (szSense,"") == 0 ) t->taggerPutFlow(szSense);
	if ( iWinLength != -1 ) t->taggerPutWinLength(iWinLength);
	if ( iWinIndex != -1 ) t->taggerPutWinIndex(iWinIndex);
	
	t->taggerLoadModelsForTagging();
	return 0;
}

void apiTaggerDestroy()
{
	erFreeRegExp();
	if ( t != NULL ) delete t;
	t = NULL;
}

