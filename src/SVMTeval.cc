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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "hash.h"
#include "common.h"
#include "dict.h"

int verbose = TRUE;

/******************************************************************/

/* 
 * Esta estructura contiene el número de aciertos (hits),
 * intentos (trials) y aciertos en caso de etiquetar con la 
 * etiqueta más frecuente (mft) para las palabras de una 
 * clase determinada (key) de palabras. 
 */
struct stat_t{
	std::string key;
	int hits;
	int trials;
	int mft;
};

/******************************************************************/

void printHelp(char *progname)
{
	fprintf(stderr,"\nSVMTool++ v 1.1.6 -- SVMTeval\n\n");
	fprintf(stderr,"Usage : %s [mode] <model> <gold> <pred>\n\n", progname);
	fprintf(stderr,"\t- mode:\t0 - complete report (everything)\n");
	fprintf(stderr,"\t\t1 - overall accuracy only [default]\n");
	fprintf(stderr,"\t\t2 - accuracy of known vs unknown words\n");
	fprintf(stderr,"\t\t3 - accuracy per level of ambiguity\n");
	fprintf(stderr,"\t\t4 - accuracy per kind of ambiguity\n");
	fprintf(stderr,"\t\t5 - accuracy per part-of-speech\n");
	fprintf(stderr,"\t- model: model location (path + name)\n");
	fprintf(stderr,"\t- gold:  correct tagging file\n");
	fprintf(stderr,"\t- pred:  predicted tagging file\n\n");
	fprintf(stderr,"Example : %s WSJTP WSJTP.IN WSJTP.OUT\n\n", progname);
}

/************************************************************************/

class eval
{
private:
	char sModel[500];
	char sGold[500];
	char sPred[500];
	FILE *gold;
	FILE *pred;
	dictionary *d;
  hash_t<stat_t *> stat_Amb_Level;
  hash_t<stat_t *> stat_Class_Amb;
  hash_t<stat_t *> stat_POS;
	int report_type;
	int numAmbLevel;
	int numAmbClass;

  void printHashStats(hash_t<stat_t *> *tptr, int put_eol, const char *column_name);
	void printKnownVsUnknown(int knownAmb, int knownUnamb, int unknown,int unkHits, int knownHitsAmb,int knownHitsUnamb);
	void printTaggingSumary(int known,int unknown,int ambiguous,int well,int wellMFT);
	void printOverallAccuracy(int total, int well, int wellMFT, float pAmb);
  void printStatsByLevel(hash_t<stat_t *> *h);
  void printStatsByAmbiguityClass(hash_t<stat_t *> *h);
  void printStatsByPOS(hash_t<stat_t *> *h);
	void addStatsToHash(hash_t< stat_t* >* h, const std::string& key, int is_hit, int is_mft);
	void makeReport(dictionary *d,FILE *gold, FILE *pred);
  
public:
	eval(char *model, char *goldName, char *predName);
	void evalPutReportType(int report);
	void evalRun();
};

/******************************************************************/

void eval::printHashStats(hash_t<stat_t *> *tptr, int put_eol, const char *column_name)
{
  fprintf(stderr,"%s\tHITS\t\tTRIALS\t\tACCURACY\t\tMFT-ACCURACY\n",column_name);
  fprintf(stderr,"* ------------------------------------------------------------------------- \n");
//   hash_node_t *node, *last;
//   int i;

  char c='\0';
  if (put_eol==TRUE) c='\n';

  for (hash_t<stat_t *>::iterator it  = tptr->begin(); it != tptr->end(); it++)
  {
    stat_t *s = (stat_t *)((*it).second);
    fprintf(stderr,"%s%c\t%d\t/\t%d\t=\t%.4f %%\t\t%.4f %%\n",s->key.c_str(),c,s->hits,s->trials, 100*((float)s->hits/s->trials), 100*((float)s->mft/s->trials) );
  }
}

/******************************************************************/

void eval::printKnownVsUnknown(int knownAmb, int knownUnamb, int unknown,int unkHits, int knownHitsAmb,int knownHitsUnamb)
{
	int knownHits = knownHitsAmb + knownHitsUnamb;
	int known = knownAmb + knownUnamb;
	
	fprintf(stderr,"* ================= KNOWN vs UNKNOWN WORDS ================================\n");
	fprintf(stderr,"\tHITS\t\tTRIALS\t\tACCURACY\n");
	fprintf(stderr,"* -------------------------------------------------------------------------\n");
	fprintf(stderr,"* ======= known ===========================================================\n");
	fprintf(stderr,"\t%d\t/\t%d\t=\t%.4f %%\n",knownHits,known,100*((float)knownHits/known));
	fprintf(stderr,"-------- known unambiguous words ------------------------------------------\n");
	fprintf(stderr,"\t%d\t/\t%d\t=\t%.4f %%\n",knownHitsUnamb,knownUnamb,100*((float)knownHitsUnamb/knownUnamb));
	fprintf(stderr,"-------- known ambiguous words --------------------------------------------\n");
	fprintf(stderr,"\t%d\t/\t%d\t=\t%.4f %%\n",knownHitsAmb,knownAmb,100*((float)knownHitsAmb/knownAmb));
	fprintf(stderr,"* ======= unknown =========================================================\n");
	fprintf(stderr,"\t%d\t/\t%d\t=\t%.4f %%\n",unkHits,unknown,100*((float)unkHits/unknown));
	fprintf(stderr,"* =========================================================================\n");
}

/******************************************************************/

void eval::printTaggingSumary(int known,int unknown,int ambiguous,int /*well*/,int wellMFT)
{
	fprintf(stderr,"* ================= TAGGING SUMMARY =======================================\n");
	fprintf(stderr,"#WORDS\t\t = %d\n",known+unknown);
	fprintf(stderr,"#KNOWN\t\t = %d\t/\t%d\t--> (%.4f %%)\n",known,known+unknown,100*((float)known/(known+unknown)));
	fprintf(stderr,"#UNKNOWN\t = %d\t/\t%d\t--> (%.4f %%)\n",unknown,known+unknown,100*((float)unknown/(known+unknown)));
	fprintf(stderr,"#AMBIGUOUS\t = %d\t/\t%d\t--> (%.4f %%)\n",ambiguous,known+unknown,100*((float)ambiguous/(known+unknown)));
	fprintf(stderr,"#MFT baseline\t = %d\t/\t%d\t--> (%.4f %%)\n",wellMFT,known+unknown,100*((float)wellMFT/(known+unknown)));
}

/******************************************************************/

void eval::printOverallAccuracy(int total, int well, int wellMFT, float pAmb)
{
	fprintf(stderr,"* ================= OVERALL ACCURACY ======================================\n");
	fprintf(stderr,"\tHITS\t\tTRIALS\t\tACCURACY\tMFT-baseline\n");
	fprintf(stderr,"* -------------------------------------------------------------------------\n");
	fprintf(stderr,"\t%d\t/\t%d\t=\t%.4f\t\t%.4f%%\n",well,total,100*((float)well/total),100*((float)wellMFT/total));
	fprintf(stderr,"* =========================================================================\n");
	fprintf(stderr,"\tAmbiguity Average for Known words = %5f POS/word\n",pAmb);
	fprintf(stderr,"* =========================================================================\n");
}

/******************************************************************/

void eval::printStatsByLevel(hash_t<stat_t *> *h)
{
	fprintf(stderr,"* ================= ACCURACY PER LEVEL OF AMBIGUITY =======================\n");
	fprintf(stderr,"#CLASSES = %d\n",numAmbLevel);
	fprintf(stderr,"* =========================================================================\n");
	printHashStats(h,FALSE,"LEVEL");
}

/******************************************************************/

void eval::printStatsByAmbiguityClass(hash_t<stat_t *> *h)
{
	fprintf(stderr,"* ================= ACCURACY PER CLASS OF AMBIGUITY =======================\n");
	fprintf(stderr,"#CLASSES = %d\n",numAmbClass);
	fprintf(stderr,"* =========================================================================\n");
	printHashStats(h,TRUE,"CLASS");
}

/******************************************************************/

void eval::printStatsByPOS(hash_t<stat_t *> *h)
{
	fprintf(stderr,"* =================== ACCURACY PER PART-OF_SPEECH =========================\n");
	printHashStats(h,FALSE,"POS");
}

/******************************************************************/

void eval::addStatsToHash(hash_t<stat_t *>* h, const std::string& key, int is_hit, int is_mft)
{
  stat_t * s = h->hash_lookup(key);
	if ((long)s!=HASH_FAIL)
	{
		s->key = key;
		if (is_hit==TRUE) s->hits++;
		if (is_mft==TRUE) s->mft++;
		s->trials++;
	}
	else
	{
		if (report_type==3 || report_type==0) numAmbLevel++;
		if (report_type==4 || report_type==0) numAmbClass++;
		s = new stat_t;
		s->key = key;
		s->trials=1;
		s->hits=0;
		s->mft=0;
		if (is_hit==TRUE) s->hits++;
		if (is_mft==TRUE) s->mft++;
		h->hash_insert(s->key,s);
	}
}

/******************************************************************/

void eval::makeReport(dictionary *d,FILE *gold, FILE *pred)
{
	std::string mft;
  char wrd1[150],wrd2[150],pos1[5],pos2[5];
	int totalWords=0, well = 0,known=0,unknown=0,wellMFT=0;
	int ambiguous=0,unambiguous=0;
	int unkHits=0,knownHitsAmb=0,knownHitsUnamb=0;
	int ret1=0,ret2=0;
	int is_mft=FALSE,is_hit=FALSE;
	int contAmbiguities=0;

	while (!feof(gold) && !feof(pred))
	{
		is_mft=FALSE;
		is_hit=FALSE;
		
		char gold_line[250] = "\n";
		char pred_line[250] = "\n";
	
		while ( !feof(gold) && ( strcmp(gold_line,"\n") == 0 || ( gold_line[0]=='#' && gold_line[1]=='#') ) )
		  fgets(gold_line,250,gold);
		while ( !feof(pred) && ( strcmp(pred_line,"\n") == 0 || ( pred_line[0]=='#' && pred_line[1]=='#') ) )
		  fgets(pred_line,250,pred);

		ret1 = sscanf (gold_line,"%s %s",wrd1,pos1);
		ret2 = sscanf (pred_line,"%s %s",wrd2,pos2);
	
		if ( ret1 >= 0 && ret2 >= 0 )
		{
			dataDict* w = d->getElement(wrd1);
			int numMaybe;
			if ((long)w!=HASH_FAIL) //Si es conocida
			{
				known++;
				numMaybe = d->getElementNumMaybe(w);
				
				if (numMaybe>1) 
				    ambiguous++; //Si es ambigua
				else unambiguous++; //Si no es ambigua

				contAmbiguities += numMaybe;

        mft = d->getMFT(w)->pos;
				if (mft == pos1)
				{  is_mft = TRUE;
				   wellMFT++;
				}
			}
			else unknown++;

			if (strcmp(wrd1,wrd2)==0 && strcmp(pos1,pos2)==0)
			{
			 well++;
			 is_hit=TRUE; //Es acierto
			 if ((long)w==HASH_FAIL) unkHits++; //Acierto para desconocidas
			 else if (numMaybe>1) 
			 	knownHitsAmb++; //Acierto para conocidas ambiguas
			 else knownHitsUnamb++; //Acierto para conocidas no ambiguas
			}

			if (report_type==3 || report_type==0)
			{
			  //Acumulamos por nivel de ambigüedad
			  char level[4];
        if ((long)w!=HASH_FAIL) sprintf(level,"%d",numMaybe);
			  else sprintf(level,"UNKOWN");
			  addStatsToHash(&stat_Amb_Level,level,is_hit,is_mft);
			}
			if (report_type==4 || report_type==0)
			{
		 	  //Acumulamos por clase de ambigüedad
			  std::string ambClass = d->getAmbiguityClass(w);
			  addStatsToHash(&stat_Class_Amb,ambClass,is_hit,is_mft);
			}
			if (report_type==5 || report_type==0)
			{
			  //Acumulamos por etiqueta
			  addStatsToHash(&stat_POS,pos2,is_hit,is_mft);
			}

			showProcessDone(totalWords,2000,FALSE,"words");

			totalWords++;
		}

	}

  showProcessDone(totalWords,2000,TRUE,"words");
  printTaggingSumary(known,unknown,ambiguous,well,wellMFT);
  if (report_type==2 || report_type==0)	printKnownVsUnknown(ambiguous, unambiguous, unknown,unkHits,knownHitsAmb,knownHitsUnamb);
  if (report_type==3 || report_type==0) printStatsByLevel(&stat_Amb_Level);
  if (report_type==4 || report_type==0) printStatsByAmbiguityClass(&stat_Class_Amb);
  if (report_type==5 || report_type==0) printStatsByPOS(&stat_POS);

  float porcentageAmbiguedad = (float)contAmbiguities/ (float) known;
  printOverallAccuracy(unknown+known,well,wellMFT,porcentageAmbiguedad);
}


/************************************************************************/

eval::eval(char *model, char *goldName, char *predName)
{
	report_type = 1;
	numAmbLevel = 0;
	numAmbClass = 0;

  stat_Amb_Level.hash_init(10);
  stat_Class_Amb.hash_init(100);
  stat_POS.hash_init(100);
	
	strcpy(sModel, model);
	strcpy(sGold, goldName);
	strcpy(sPred, predName);		
}


void eval::evalPutReportType(int report)
{
	report_type = report;
}

void eval::evalRun()
{
	fprintf(stderr,"* ========================= SVMTeval report ==============================\n");
	fprintf(stderr,"* model     = [%s]\n* testset   = [%s]\n* predicted = [%s]\n",sModel,sGold,sPred);
	fprintf(stderr,"* ========================================================================\n");

	char name[200];
	sprintf(name,"%s.DICT",sModel);
	gold = openFile(sGold,"rt");
	pred = openFile(sPred,"rt");
	d  = new dictionary(name);

	makeReport(d,gold,pred);
	
	delete d;
}
/************************************************************************/

int main(int argc, char *argv[])
{
	int i = 0;
	int report_type;

	if (argc<4)
	{
		fprintf(stderr,"Waiting 3 or more parameters\n");
		printHelp(argv[0]);
		exit(0);
	}
	if (argc>4)
	{
		report_type = atoi(argv[1]);
		i=1;
		if (report_type>5 || report_type<0)
		{
			printHelp(argv[0]);
			exit(0);
		}
	}

	eval *e = new eval(argv[1+i], argv[2+i], argv[3+i]);
	if (i == 1) { e->evalPutReportType(report_type); }
	e->evalRun();
}
