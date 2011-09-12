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

#include <iostream>
#include <iomanip>
#include <iterator>
#include <fstream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <algorithm>
#include <vector>
#include "marks.h"
#include "hash.h"
#include "list.h"
#include "dict.h"
#include "swindow.h"
#include "mapping.h"
#include "weight.h"
#include "learner.h"
#include "common.h"
#include "nodo.h"

/**************************************************************/

extern int verbose;
double time_svmlight = 0;

/**************************************************************/

std::stack<int*> DO;
int KERNEL=0;
int DEGREE=0;
float CK = 0;
float CU = 0;
float X  = 3;
int MAX_MAPPING_SIZE = 100000;
int COUNT_CUT_OFF = 2;
int WINDOW_SIZE = 5;
int CORE_POSITION = 2;
char *TRAINSET = NULL;
char *SVMDIR = NULL;
char *NAME = NULL;
char *BLEX = NULL;
char *R = NULL;
float DRATIO = 0.001;
float ERATIO=0;
float KFILTER = 0;
float UFILTER = 0;
int   REMOVE_FILES = TRUE;

/**************************************************************/

char *UP = NULL;
char *AP = NULL;

/**************************************************************/

//ambiguous-right [default]
std::string A0 = std::string( "w(-2)\nw(-1)\nw(0)\nw(1)\nw(2)\nw(-2,-1)\nw(-1,0)\nw(0,1)\nw(-1,1)\nw(1,2)\nw(-2,-1,0)\nw(-2,-1,1)\nw(-1,0,1)\nw(-1,1,2)\nw(0,1,2)\np(-2)\np(-1)\np(-2,-1)\np(-1,1)\np(1,2)\np(-2,-1,1)\np(-1,1,2)\nk(0)\nk(1)\nk(2)\nm(0)\nm(1)\nm(2)\n");
std::string A0UNK = std::string( "w(-2)\nw(-1)\nw(0)\nw(1)\nw(2)\nw(-2,-1)\nw(-1,0)\nw(0,1)\nw(-1,1)\nw(1,2)\nw(-2,-1,0)\nw(-2,-1,1)\nw(-1,0,1)\nw(-1,1,2)\nw(0,1,2)\np(-2)\np(-1)\np(-2,-1)\np(-1,1)\np(1,2)\np(-2,-1,1)\np(-1,1,2)\nk(0)\nk(1)\nk(2)\nm(0)\nm(1)\nm(2)\na(2)\na(3)\na(4)\nz(2)\nz(3)\nz(4)\nca(1)\ncz(1)\nL\nSA\nAA\nSN\nCA\nCAA\nCP\nCC\nCN\nMW\n");

/**************************************************************/

//unambiguous-right
std::string A1 = std::string( "w(-2)\nw(-1)\nw(0)\nw(1)\nw(2)\nw(-2,-1)\nw(-1,0)\nw(0,1)\nw(-1,1)\nw(1,2)\nw(-2,-1,0)\nw(-2,-1,1)\nw(-1,0,1)\nw(-1,1,2)\nw(0,1,2)\np(-2)\np(-1)\np(1)\np(2)\np(-2,-1)\np(-1,0)\np(-1,1)\np(0,1)\np(1,2)\np(-2,-1,0)\np(-2,-1,1)\np(-1,0,1)\np(-1,1,2)\nk(0)\nk(1)\nk(2)\nm(0)\nm(1)\nm(2)\n");
std::string A1UNK = std::string( "w(-2)\nw(-1)\nw(0)\nw(1)\nw(2)\nw(-2,-1)\nw(-1,0)\nw(0,1)\nw(-1,1)\nw(1,2)\nw(-2,-1,0)\nw(-2,-1,1)\nw(-1,0,1)\nw(-1,1,2)\nw(0,1,2)\np(-2)\np(-1)\np(1)\np(2)\np(-2,-1)\np(-1,0)\np(-1,1)\np(0,1)\np(1,2)\np(-2,-1,0)\np(-2,-1,1)\np(-1,0,1)\np(-1,1,2)\nk(0)\nk(1)\nk(2)\nm(0)\nm(1)\nm(2)\na(1)\na(2)\na(3)\na(4)\nz(1)\nz(2)\nz(3)\nz(4)\nL\nSA\nAA\nSN\nCA\nCAA\nCP\nCC\nCN\nMW\n");

/**************************************************************/

//no-right
std::string A2 = std::string( "w(-2)\nw(-1)\nw(0)\nw(1)\nw(2)\nw(-2,-1)\nw(-1,0)\nw(0,1)\nw(-1,1)\nw(1,2)\nw(-2,-1,0)\nw(-2,-1,1)\nw(-1,0,1)\nw(-1,1,2)\nw(0,1,2)\np(-2)\np(-1)\np(-2,-1)\nk(0)\nm(0)\n");
std::string A2UNK = std::string( "w(-2)\nw(-1)\nw(0)\nw(1)\nw(2)\nw(-2,-1)\nw(-1,0)\nw(0,1)\nw(-1,1)\nw(1,2)\nw(-2,-1,0)\nw(-2,-1,1)\nw(-1,0,1)\nw(-1,1,2)\nw(0,1,2)\np(-2)\np(-1)\np(-2,-1)\nk(0)\nm(0)\na(1)\na(2)\na(3)\na(4)\nz(1)\nz(2)\nz(3)\nz(4)\nL\nSA\nAA\nSN\nCA\nCAA\nCP\nCC\nCN\nMW\n");

/**************************************************************/

//unsupervised-learning
std::string A3 = std::string( "w(-2)\nw(-1)\nw(0)\nw(1)\nw(2)\nw(-2,-1)\nw(-1,0)\nw(0,1)\nw(-1,1)\nw(1,2)\nw(-2,-1,0)\nw(-2,-1,1)\nw(-1,0,1)\nw(-1,1,2)\nw(0,1,2))\np(-2))\np(-1))\np(-2,-1))\np(-1,1))\np(1,2))\np(-2,-1,1))\np(-1,1,2))\nk(-2))\nk(-1))\nk(1))\nk(2)\n)\nm(-2))\nm(-1))\nm(1))\nm(2)\n");
std::string A3UNK = std::string( "w(-2)\nw(-1)\nw(0)\nw(1)\nw(2)\nw(-2,-1)\nw(-1,0)\nw(0,1)\nw(-1,1)\nw(1,2)\nw(-2,-1,0)\nw(-2,-1,1)\nw(-1,0,1)\nw(-1,1,2)\nw(0,1,2))\np(-2))\np(-1))\np(-2,-1))\np(-1,1))\np(1,2))\np(-2,-1,1))\np(-1,1,2))\nk(-2))\nk(-1))\nk(1))\nk(2)\n)\nm(-2))\nm(-1))\nm(1))\nm(2)\na(1)\na(2)\na(3)\na(4)\nz(1)\nz(2)\nz(3)\nz(4)\nL\nSA\nAA\nSN\nCA\nCAA\nCP\nCC\nCN\nMW\n");

/**************************************************************/

//ambiguous-right ++ unknown words on training
std::string A4 = std::string( "w(-2)\nw(-1)\nw(0)\nw(1)\nw(2)\nw(-2,-1)\nw(-1,0)\nw(0,1)\nw(-1,1)\nw(1,2)\nw(-2,-1,0)\nw(-2,-1,1)\nw(-1,0,1)\nw(-1,1,2)\nw(0,1,2)\np(-2)\np(-1)\np(-2,-1)\np(-1,1)\np(1,2)\np(-2,-1,1)\np(-1,1,2)\nk(0)\nk(1)\nk(2)\nm(0)\nm(1)\nm(2)\n");
std::string A4UNK = std::string( "w(-2)\nw(-1)\nw(0)\nw(1)\nw(2)\nw(-2,-1)\nw(-1,0)\nw(0,1)\nw(-1,1)\nw(1,2)\nw(-2,-1,0)\nw(-2,-1,1)\nw(-1,0,1)\nw(-1,1,2)\nw(0,1,2)\np(-2)\np(-1)\np(-2,-1)\np(-1,1)\np(1,2)\np(-2,-1,1)\np(-1,1,2)\nk(0)\nk(1)\nk(2)\nm(0)\nm(1)\nm(2)\na(1)\na(2)\na(3)\na(4)\nz(1)\nz(2)\nz(3)\nz(4)\nL\nSA\nAA\nSN\nCA\nCAA\nCP\nCC\nCN\nMW\n");

/**************************************************************/

std::string trim(std::string const& source, char const* delims = " \t\r\n") {
  std::string result(source);
  std::string::size_type index = result.find_last_not_of(delims);
  if(index != std::string::npos)
    result.erase(++index);

  index = result.find_first_not_of(delims);
  if(index != std::string::npos)
    result.erase(0, index);
  else
    result.erase();
  return result;
}

std::string learner::read_feature_list_from_config_file(FILE *f, char *first_feature)
{
  std::string tmp,str;
  tmp = first_feature;
  tmp += "\n";

  int cont=1;
  int ret=1;

  while (ret>0)
  {
    str.clear();
    ret = readTo(f,' ','\n',str);
    if (ret>=0)
    {
      tmp += str + "\n";
      cont++;
    }
  }
  return tmp;
}

std::string learner::read_feature_list_from_string(const std::vector<std::string>& tokens)
{
  std::string tmp;

  vector<string>::const_iterator token = tokens.begin();
  for(; token != tokens.end(); token++ )
  {
    string f(*token);
    tmp += *token + "\n";
  }
  return tmp;
}

/**************************************************************/

void learner::read_config_file(const std::string& config_file)
{
  std::string configFile = config_file;

  std::ifstream file(configFile.c_str());
  std::string line;

  using namespace std;

  string delimiter(" ");
  string equaldelimiter("=");

  //copy(tokens.begin(), tokens.end(), ostream_iterator<string>(cout, ", "));
  //std::string command = trim(line.substr(1, line.find(' ')-1));
  //while( token != tokens.end() ) { cout << *token + " "; token++; }
  //cout << "\n";
  //exit(0);

  while (std::getline(file,line)) {
      if (! line.length()) continue;
      if (line[0] == '#') continue;

      vector<string> tokens;
      Tokenize(trim(line), tokens, delimiter);
      vector<string>::iterator token = tokens.begin();
      //while( token != tokens.end() ) { cout << *token + " "; token++; }
      //cout << "\n";

      if (tokens.size() == 0) continue;
      std::string command = tokens[0];
      int posEqual=line.find('=');

      if (command == "do") {
	 int *modelo = new int; *modelo = 0;
         int *direction = new int; *direction = LEFT_TO_RIGHT;
         if (tokens.size() >= 3) {
            std::string MODEL = trim(tokens[1].substr(1, line.find('M')));
	    *modelo = atoi(MODEL.c_str());
	    if (tokens[2].compare("LR") == 0) { *direction = LEFT_TO_RIGHT; }
	    else if (tokens[2].compare("RL") == 0) { *direction = RIGHT_TO_LEFT; }
	    else if (tokens[2].compare("LRL")==0) { *direction = LR_AND_RL; }
	    DO.push(direction);
      DO.push(modelo);
	 }
      }
      else if (posEqual >= 0) {        
         vector<string> tokenseq;
         Tokenize(trim(line), tokenseq, equaldelimiter);
         vector<string>::iterator token = tokens.begin();

         if (tokens.size() < 2) continue;

	 string arg = trim(tokenseq[0]);
	 string param = trim(tokenseq[1]);

         //cout << arg << " :: " << param << "\n";
         
         vector<string> params;
         Tokenize(trim(param), params, delimiter);

         if ((arg.compare("A0k") == 0) || (arg.compare("A0") == 0)) { A0 = read_feature_list_from_string(params); }
         else if ((arg.compare("A1k") == 0) || (arg.compare("A1") == 0)) { A1 = read_feature_list_from_string(params); }
         else if ((arg.compare("A2k") == 0) || (arg.compare("A2") == 0)) { A2 = read_feature_list_from_string(params); }
         else if ((arg.compare("A3k") == 0) || (arg.compare("A3") == 0)) { A3 = read_feature_list_from_string(params); }
         else if ((arg.compare("A4k") == 0) || (arg.compare("A4") == 0)) { A4 = read_feature_list_from_string(params); }
         else if ((arg.compare("A0u") == 0) || (arg.compare("A0unk") == 0)) { A0UNK = read_feature_list_from_string(params); }
         else if ((arg.compare("A1u") == 0) || (arg.compare("A1unk") == 0)) { A1UNK = read_feature_list_from_string(params); }
         else if ((arg.compare("A2u") == 0) || (arg.compare("A2unk") == 0)) { A2UNK = read_feature_list_from_string(params); }
         else if ((arg.compare("A3u") == 0) || (arg.compare("A3unk") == 0)) { A3UNK = read_feature_list_from_string(params); }
         else if ((arg.compare("A4u") == 0) || (arg.compare("A4unk") == 0)) { A4UNK = read_feature_list_from_string(params); }

         
         else if (arg.compare("F") == 0) {
	   if (params.size() >= 2) { COUNT_CUT_OFF = atoi(params[0].c_str()); MAX_MAPPING_SIZE = atoi(params[1].c_str()); }
         }
         else if (arg.compare("W") == 0) {
	   if (params.size() >= 2) { WINDOW_SIZE = atoi(params[0].c_str()); CORE_POSITION = atoi(params[1].c_str()); }
	 }
         else if (arg.compare("TRAINSET") == 0) {
	   if (params.size() >= 1) { TRAINSET = new char[params[0].length()+1]; strcpy(TRAINSET, params[0].c_str()); }
	 }
         else if (arg.compare("BLEX") == 0) {
	   if (params.size() >= 1) { BLEX = new char[params[0].length()+1]; strcpy(BLEX, params[0].c_str()); }
	 }
         else if (arg.compare("R") == 0) {
	   if (params.size() >= 1) { R = new char[params[0].length()+1]; strcpy(R, params[0].c_str()); }
	 }
         else if (arg.compare("SVMDIR") == 0) {
	   if (params.size() >= 1) { SVMDIR = new char[params[0].length()+1]; strcpy(SVMDIR, params[0].c_str()); }
	 }
         else if (arg.compare("NAME") == 0) {
	   if (params.size() >= 1) { NAME = new char[params[0].length()+1]; strcpy(NAME, params[0].c_str()); }
	 }
         else if (arg.compare("REMOVE_FILES") == 0) {
	   if (params.size() >= 1) { REMOVE_FILES = atoi(params[0].c_str()); }
	 }
         else if (arg.compare("CK") == 0) {
	   if (params.size() >= 1) { CK = atof(params[0].c_str()); }
	 }
         else if (arg.compare("CU") == 0) {
	   if (params.size() >= 1) { CU = atof(params[0].c_str()); }
	 }
         else if (arg.compare("Dratio") == 0) {
	   if (params.size() >= 1) { DRATIO = atof(params[0].c_str()); }
	 }
         else if (arg.compare("Eratio") == 0) {
	   if (params.size() >= 1) { ERATIO = atof(params[0].c_str()); }
	 }
         else if (arg.compare("Kfilter") == 0) {
	   if (params.size() >= 1) { KFILTER = atof(params[0].c_str()); }
	 }
         else if (arg.compare("Ufilter") == 0) {
	   if (params.size() >= 1) { UFILTER = atof(params[0].c_str()); }
	 }
        else if (arg.compare("X") == 0)
        {
          if (params.size() >= 1) { X = atof(params[0].c_str()); }
        }
        else if (arg.compare("AP") == 0)
        {
          learnerAMBP_H = new hash_t<infoDict*>;
          learnerAMBP_H->hash_init(30);
          vector<string>::iterator p = params.begin();
          while( p != params.end())
          {
            infoDict * etiq = new infoDict;
            string tag = *p;
            etiq->pos = tag.c_str();
            learnerAMBP_H->hash_insert(etiq->pos,etiq);
            p++;
          }
        }
        else if (arg.compare("UP") == 0) {
	   learnerUNKP_H = new hash_t<infoDict*>;
     learnerUNKP_H->hash_init(30);
           vector<string>::iterator p = params.begin();
           while( p != params.end()) {
   	      infoDict * etiq = new infoDict;
              string tag = *p;
              etiq->pos  = tag.c_str();
              learnerUNKP_H->hash_insert(etiq->pos,etiq);
              p++;
           }
	 }
      }
   }
  
   if (verbose==TRUE) {
      fprintf(stderr,"\n* ===================== SVMTlearn configuration ==========================");
      fprintf(stderr,"\n* config file   = [ %s ]\n* trainset      = [ %s ]\n* model name    = [ %s ]",config_file.c_str(),TRAINSET,NAME);
      fprintf(stderr,"\n* SVM-light dir = [ %s ]",SVMDIR);
      fprintf(stderr,"\n* ========================================================================");
      fprintf(stderr,"\n* unknown words expected   = [ X  = %f  %% ]",X);
      fprintf(stderr,"\n* C parameter for known    = [ CK = %f ]",CK);
      fprintf(stderr,"\n* C parameter for unknown  = [ CU = %f ]",CU);
      fprintf(stderr,"\n* D ratio                  = [ Dratio = %f ]",DRATIO);
      fprintf(stderr,"\n* E ratio                  = [ Eratio = %f ]",ERATIO);
      fprintf(stderr,"\n* Known weights filter     = [ Kfilter = %f ]",KFILTER);
      fprintf(stderr,"\n* Unknown weights filter   = [ Ufilter = %f ]",UFILTER);
      fprintf(stderr,"\n* sliding window settings  = [ WINDOW SIZE = %d , CORE POSITION = %d ]",WINDOW_SIZE,CORE_POSITION);
      fprintf(stderr,"\n* mapping settings         = [ COUNT CUT OFF = %d , MAX MAPPING SIZE = %d ]",COUNT_CUT_OFF,MAX_MAPPING_SIZE);
      fprintf(stderr,"\n* remove temporal files    = [ %d ] (1) TRUE, (0) FALSE",REMOVE_FILES);	
      fprintf(stderr,"\n* ========================================================================");
   }

   if (TRAINSET == NULL) { fprintf (stderr,"\nError: TRAINSET parameter not found in %s.\n",config_file.c_str()); exit(-1); }
   if (NAME == NULL) { fprintf (stderr,"\nError: MODEL NAME parameter not found in %s.\n",config_file.c_str()); exit(-1); }
   if (SVMDIR == NULL) {  fprintf (stderr,"\nError: SVM DIRECTORY parameter not found in %s.\n",config_file.c_str()); exit(-1); }
}

/**************************************************************/

learner::learner()
{
  learnerAMBP_H = NULL ;
  learnerUNKP_H = NULL ;
  learnerNumFeatures = 1000;
}

learner::~learner()
{
 if (REMOVE_FILES==TRUE)
 {
   removeFiles(NAME,RM_TEMP_FILES,0,0,verbose);
 }

 if ( verbose == TRUE ) fprintf(stderr,"\n\nTERMINATION ... ");

 if (TRAINSET!=NULL) delete TRAINSET;
 if (SVMDIR!=NULL) delete SVMDIR;
 if (NAME!=NULL) delete NAME;
 if (BLEX!=NULL) delete BLEX;
 if (R!=NULL) delete R;

 if ( verbose == TRUE ) fprintf(stderr,"[DONE]\n\n");
}

/***************************************************************/

void learner::learnerCreatePOSFile(const std::string& modelName, int is_ambp, hash_t< infoDict* >* h)
{
  std::string name;
  if (is_ambp==TRUE)
    {
      if ( verbose == TRUE ) fprintf(stderr,"\nStoring %s.AMBP",modelName.c_str());
      name = modelName + ".AMBP";
      FILE *f = openFile (name.c_str(),"w");
      h->hash_print(f);
      fclose(f);
    }
  else
    {
      if ( verbose == TRUE ) fprintf(stderr,"\nStoring %s.UNKP",modelName.c_str());
      name = modelName + ".UNKP";
      FILE *f = openFile (name.c_str(),"w");
      h->hash_print(f);
      fclose(f);
    }
}

/***************************************************************/

void learner::learnerCreateDefaultFile(const std::string &modelName, const std::string& str)
{
  std::string name;
  name = modelName+ "." + str;
  FILE *f = openFile(name.c_str(), "w");

  if (str == "A0")
  {
    if ( verbose == TRUE )  fprintf(stderr,"\nStoring %s.A0",modelName.c_str()); fprintf(f,"%s",A0.c_str());
    name = modelName + "."+str+".UNK";
    if ( verbose == TRUE ) fprintf(stderr,"\nStoring %s.A0.UNK",modelName.c_str());
    FILE *funk = openFile(name.c_str(), "w");
    fprintf(funk,"%s",A0UNK.c_str());
    fclose(funk);
  }
  else if (str == "A1")
  {
    if ( verbose == TRUE ) fprintf(stderr,"\nStoring %s.A1",modelName.c_str()); fprintf(f,"%s",A1.c_str());
    name = modelName + "."+str+".UNK";
    if ( verbose == TRUE ) fprintf(stderr,"\nStoring %s.A1.UNK",modelName.c_str());
    FILE *funk = openFile(name.c_str(), "w");
    fprintf(funk,"%s",A1UNK.c_str());
    fclose(funk);
  }
  else if (str == "A2")
  {
    if ( verbose == TRUE ) fprintf(stderr,"\nStoring %s.A2",modelName.c_str()); fprintf(f,"%s",A2.c_str());
    name = modelName+"."+str+".UNK";
    if ( verbose == TRUE ) fprintf(stderr,"\nStoring %s.A2.UNK",modelName.c_str());
    FILE *funk = openFile(name.c_str(), "w");
    fprintf(funk,"%s",A2UNK.c_str());
    fclose(funk);
  }
  else if (str == "A3")
  {
    if ( verbose == TRUE ) fprintf(stderr,"\nStoring %s.A3",modelName.c_str()); fprintf(f,"%s",A3.c_str());
    name = modelName+"."+str+".UNK";
    if ( verbose == TRUE )  fprintf(stderr,"\nStoring %s.A3.UNK",modelName.c_str());
    FILE *funk = openFile(name.c_str(), "w");
    fprintf(funk,"%s",A3UNK.c_str());
    fclose(funk);
  }
  else if (str == "A4")
  {
    if ( verbose == TRUE ) fprintf(stderr,"\nStoring %s.A4",modelName.c_str()); fprintf(f,"%s",A4.c_str());
    name = modelName + "."+str+".UNK";
    if ( verbose == TRUE ) fprintf(stderr,"\nStoring %s.A4.UNK",modelName.c_str());
    FILE *funk = openFile(name.c_str(), "w");
    fprintf(funk,"%s",A4UNK.c_str());
    fclose(funk);
  }
  else if (str == "WIN")
  {
    if ( verbose == TRUE )  fprintf(stderr,"\nStoring %s.WIN",modelName.c_str());
    fprintf(f,"%d\n%d\n",WINDOW_SIZE,CORE_POSITION);
  }
  fclose(f);
}

/***************************************************************/

/*
 * Return CHAR_NULL if end of file
 * Return w,p,k (if we readed a) or s (if we readed m) if it's found
 */
char learner::obtainAtrChar(FILE *channel)
{
  char c;
  while (!feof(channel))
    {
      c=fgetc(channel);
      if (c=='w' || c=='p' || c=='k' || c=='m')
	{
	  fgetc(channel);
	  switch (c)
	    {
	    case 'k': return 'k';
	    case 'm': return 's';
	    default:  return c;
	    }
	}
    }
  return CHAR_NULL;
}

/**************************************************************/

/*
 * Cada atributo de una lista de atributos tiene la forma
 * <Marca>(<cadena de enteros separados por comas>)
 * Usaremos esta función para leer cada uno de los enteros de
 * la cadena que indica las posiciones de la ventana a considerar.
 * Lee del fichero (channel) un entero entre '(' y coma, comas,
 * coma y ') o paréntesis. Devuelve el número leído como entero.
 */
int learner::obtainAtrInt(FILE *channel,int *endAtr)
{
  char c=' ';
  std::string num;

  while ( (!feof(channel)) && (c!='(') && (c!=',') && (c!=')') )
    {
      c=fgetc(channel);
      if ((c!='(') && (c!=')')) num += c;
    }
  if (c==')') *endAtr=1;
  return atoi(num.c_str());
}

/***************************************************************/

/* int learnerCount(char *name, int *nWords, int *nSentences)
 * Cuenta el numero de palabras y frases que tiene el corpus
 * contenido en el fichero con nombre <name>. El numero de
 * palabras y de frases es devuelto como parametro de salida
 * mediante <nWords> y <nSentences>.
 */
void learner::learnerCount(const std::string& name, int *nWords, int *nSentences)
{
  int ret1=0,ret2=0,cont=0,contWords=0;
//   char c=' ';
  std::string str;

  FILE *f = openFile(name.c_str(), "r");
  while (!feof(f) && ret1>=0 && ret2>=0)
    {
      ret1 = readTo(f,' ',0,str);
      if (str == "!" || str == "?" || str  == ".") cont++;
      ret2 = readTo(f,'\n',0,str);
      if ( !feof(f) && ret1 >= 0 ) contWords++;
    }
  fclose(f);

  if ( contWords <= 1 ) 
    {
      fprintf(stderr,"\n\nInput corpus too short to begin training!! Program stopped.\n\n"); 
      exit(0);
    }

  *nWords = contWords;
  *nSentences = cont;
}


/********************************************************/

void learner::learnerPrintMessage(int numModel, int K_or_U, int LR_or_RL, int is_fex)
{
  if (verbose==TRUE)
    {
      fprintf(stderr,"\n\n* ========================================================================");
      if (is_fex==FALSE) fprintf(stderr,"\n* TRAINING MODEL %d ",numModel);
      else fprintf(stderr,"\n* FEATURES EXTRACTION FOR MODEL %d ",numModel);
      if (is_fex==TRUE) fprintf(stderr,"[ KNOWN AND UNKNOWN WORDS  -  ");
      else if (K_or_U==KNOWN) 	fprintf(stderr,"[ KNOWN WORDS  -  ");
      else fprintf(stderr,"[ UNKNOWN WORDS  -  ");
      if (LR_or_RL==LEFT_TO_RIGHT) fprintf(stderr,"LEFT TO RIGHT ]");
      else if (LR_or_RL==RIGHT_TO_LEFT) fprintf(stderr,"RIGHT TO LEFT ]");
      else if (LR_or_RL==LR_AND_RL)fprintf(stderr,"LEFT TO RIGHT AND RIGHT TO LEFT ]");
      fprintf(stderr,"\n* ========================================================================");
    }
}

/**************************************************************/

/*
 * Parámetros:
 * wrd		char *	Palabra de ejemplo que se está tratando.
 * numModel	int	Modelo que estamos entrenando.
 * direction	int	Dirección en la cual se realiza el entrenamiento
 			(Derecha a izquierda o izquierda a derecha).
 * Known_or_Unknown	int	Si se está entrenando para palabras conocidas
 				o desconocidas.
 * pos	char *	La etiqueta morfosintáctica que estamos entrenando.
 * samplePos	char *	Etiqueta morfosintáctica del ejemplo  que estamos tratando.
 * features	char *	Lista de atributos generados para el ejemplo.
 * d	dictionary *	Diccionario que se está usando para el entrenamiento.
 * nNeg	int *	Apuntador al número de palabras seleccionadas como ejemplos negativos.
 * nPos	int *	Apuntador al número de palabras seleccionadas como ejemplos positivos.
 *
 * Este método puede usarse para seleccionar ejemplos para palabras conocidas
 */
void learner::learnerPushSample(const std::string& wrd,int numModel,int direction, int Known_or_Unknown,const std::string& pos, const std::string& samplePos, const std::string& features,dictionary *d, int *nNeg, int *nPos)
{
    std::string fileName;
    //Se abre el fichero donde se debe insertar el ejemplo.
    generateFileName(NAME,pos,numModel,direction, Known_or_Unknown, "POS", fileName);
  
    FILE *f = openFile(fileName,"a+");

    //Se obtiene la lista de posibles etiquetas morfosintácticas para
    //la palabra wrd.
    simpleList<infoDict*>* l = learnerGetPotser(wrd,Known_or_Unknown,d);
    l->setFirst();
    for (bool stop=false; !stop  ; stop = l->next())
    {
    	//Se busca la etiqueta pos en la lista obtenida.
    	//Si se encuentra y es igual a samplePos se selecciona el ejemplo
    	//como positivo, si no es igual a samplePos se selecciona como
    	//negativo.
      infoDict *pInfo = *l->getIndex();
      if (pInfo != NULL )
      {
        if (pInfo->pos == pos)
        {
          if (pInfo->pos  == samplePos)
          {
            //Positive Sample
            *nPos=(*nPos)+1;
            fprintf (f,"+1 %s\n",features.c_str());
          }
          else
          {
            //Negative Sample
            *nNeg=(*nNeg)+1;
            fprintf (f,"-1 %s\n",features.c_str());
          }
        }
      }
     }

   d->dictCleanListInfoDict(l,l->numElements());
   delete l;
   //Se cierra el fichero.
   fclose(f);
}

/**************************************************************/
/*
 * Parámetros:
 * wrd	Char *	Palabra del ejemplo que se está tratando.
 * numModel	Int	Modelo que estamos entrenando.
 * direction	Int	Dirección en la cual se realiza el entrenamiento
 			(Derecha a izquierda o izquierda a derecha).
 * Known_or_Unknown	Int	Si se está entrenando para palabras conocidas
 				o desconocidas.
 * pos	Char *	La etiqueta morfosintáctica que estamos entrenando.
 * samplePos	Char *	Etiqueta morfosintáctica del ejemplo  que estamos tratando.
 * features	Char *	Lista de atributos generadas para el ejemplo.
 * d	dictionary *	Diccionario que se está usando para el entrenamiento.
 * nNeg	int *	Apuntador al número de palabras seleccionadas como ejemplos negativos.
 * nPos	int *	Apuntador al número de palabras seleccionadas como ejemplos positivos.
 * Este método se encarga de seleccionar ejemplos para palabras desconocidas.
*/
void learner::learnerPushSampleUnk(const std::string& /*wrd*/, int numModel, int direction, int Known_or_Unknown, const std::string& pos, const std::string& samplePos, const std::string& features, dictionary* /*d*/, int* nNeg, int* nPos)
{
    std::string fileName;
    generateFileName(NAME,pos,numModel,direction, Known_or_Unknown, "POS", fileName);
    //Abrimos el fichero
    FILE *f = openFile(fileName,"a+");

    //Si pos es igual a samplePos se selecciona el ejemplo como positivo,
    //en cualquier otro caso se selecciona como negativo.
    if (samplePos == pos)
	   {
	   	//Positive Sample
		*nPos=(*nPos)+1;
		fprintf (f,"+1 %s\n",features.c_str());
	   }
    else
	    {
	    	//Negative Sample
		*nNeg=(*nNeg)+1;
		fprintf (f,"-1 %s\n",features.c_str());
	    }
   //Cerramos el fichero
   fclose(f);
}

/**************************************************************/
/*
 * Este método recibe como parámetros un apuntador a un fichero (f)
 * y el apuntador a un objeto del tipo mapping. El objetivo de este
 * método es leer los atributos generados para un ejemplo del fichero
 * f y devolver un string con la lista de atributos en el formato
 * esperado por SVM-light.
 */
std::string learner::learnerCreateFeatureString(FILE *f,mapping *m)
{
	std::string features;
	int array[learnerNumFeatures];
	int ret1=1,i = 0;
	std::string str;

	//Construimos un array de enteros con los
	//identificadores numéricos de cada atributo
	while (ret1>0 && !feof(f))
	{
	  ret1 = readTo(f,' ','\n',str);
	  int num = m->mappingGetNumberByFeature(str.c_str());
	  if (ret1>=0 && num>-1)
	    {
	      array[i]=num;
	      i++;
	    }
	}

	//qsort --> ordena ascendetemente un array de enteros
	qsort(array,0,i-1);

	for (int j=0;j<i;j++)
	{
	  //Contruimos la cadena de caracteres con losidentificadores
	  //numéricos
	  if (j!=i-1) features += array[j] + ":1 ";
	  else features += array[j] + ":1";
	}
	return features;
}

/**************************************************************/

/*
 * Parámetros:
 * d	dictionary*	Diccionario que se está usando para el entrenamiento.
 * m	Mapping*	Mapping entre atributos e identificadores numéricos
 * f	FILE*	Apuntador al fichero de ejemplos
 * pos	char *	La etiqueta morfosintáctica que estamos entrenando.
 * numModel	int	Modelo que estamos entrenando.
 * direction	int	Dirección en la cual se realiza el entrenamiento (Derecha a
 			izquierda o izquierda a derecha).
 * K_or_U	int	Si se está entrenando para palabras conocidas o desconocidas.
 * nNeg	int *	Apuntador al número de palabras seleccionadas como ejemplos negativos.
 * nPos	int *	Apuntador al número de palabras seleccionadas como ejemplos positivos.
 *
 * Este método lee el fichero en el cual están contenidos los ejemplos seleccionados
 * para el entrenamiento.
*/
void learner::learnerDressNakedSetTrain(dictionary* d, mapping* m, FILE* f, const std::string& pos, int numModel, int direction, int K_or_U, int* nPos, int* nNeg)
{
  std::string wrd, samplePos;
  std::string features;
  std::string garbage;
  int cont = 0;

  *nNeg=0; *nPos=0;


  fseek(f,0,SEEK_SET);
  //Para cada ejemplo seleccionado
  while (!feof(f))
  {
    int ret1 = readTo(f,':',0,wrd);
    int ret2 = readTo(f,' ',0,samplePos);
    if (ret1>=0 && ret2>=0)
    {
      int isPossiblePOS = learnerIsPossiblePOS(wrd,pos,K_or_U);
      if (isPossiblePOS==TRUE)
      {
        //Preparamos la lista de features
        features = learnerCreateFeatureString(f,m);

        if (K_or_U==KNOWN) learnerPushSample(wrd,numModel,direction,K_or_U,pos,samplePos,features,d,nNeg,nPos);
        else learnerPushSampleUnk(wrd,numModel,direction,K_or_U,pos,samplePos,features,d,nNeg,nPos);

        if ( verbose  == TRUE) showProcessDone(cont , 1000, FALSE,"samples");
      }
      else readTo(f,'\n','\n',garbage);
      cont++;
    } //if
  } //While
}

/**************************************************************/

/*
 * Parámetros:
 * f	FILE*	Apuntador al fichero de ejemplos
 * numModel	Int	Modelo que estamos entrenando.
 * LR_or_RL	Int	Dirección en la cual se realiza el entrenamiento
 * 			(Derecha a izquierda o izquierda a derecha).
 * K_or_U	Int	Si se está entrenando para palabras conocidas o desconocidas.
 * d	dictionary*	Diccionario que se está usando para el entrenamiento.
 * lPosToTrain	simpleList *	Lista de etiquetas morfosintácticas a entrenar
 *
 * Este método es el encargado de realizar el aprendizaje. Prepara las opciones de
 * ejecución  para SVM-light.  Construye el mapping mediante los datos contenidos
 * en el fichero de entrada. Crea un depósito de pesos (weightRepository) y
 * un hashing en el que almacenar los sesgos obtenidos para cada etiqueta.
 * Y prepara los datos para que puedan ser procesados por SVM-light.
 * Para cada etiqueta morfosintáctica de la lista lPosToTrain, se
 * llama al método learnerDressNakedSetTrain para conseguir los ficheros
 * de entrada para SVM-light. Una vez hecho esto, se llama a learnerExecSVMlight,
 * para ejecutar SVM-light. Con el fichero de salida generado por la herramienta
 * de Joachims, se rellenan las estructuras de datos con los pesos
 * (learnerBuiltWeightRepository) y los sesgos (learnerBuiltBias)
 * Una vez procesadas todas las etiquetas morfosintácticas a entrenar, el depósito
 * de pesos y el hashing de sesgos contienen todos los datos del modelo y se escriben 
 * en disco mediante los métodos weightRepository.wrWriteHash para los sesgos y 
 * weightRepository.wrWrite para los pesos.
 */

void learner::learnerDoLearn(FILE* f, int numModel, int LR_or_RL, int K_or_U, dictionary* d, simpleList< infoDict* >* lPosToTrain)
{
  std::string posFileName,svmFileName,mapFileName;

  //Preparamos las opciones con que se ejecutará svm-light
  std::ostringstream options;
  options << std::fixed << std::setprecision(6);
  if (CK!=0 && K_or_U==KNOWN) options << " -c "<<CK<<" ";
  else if (CU!=0 && K_or_U==UNKNOWN) options << " -c "<<CU<<" ";
  if (KERNEL!=0) options << " -t "<<KERNEL<<" ";
  if (DEGREE!=0) options << " -d "<<DEGREE<<" ";

  learnerPrintMessage(numModel,K_or_U,LR_or_RL,FALSE);

  if ( verbose == TRUE )
    {
      fprintf(stderr,"\nBuilding MAPPING for MODEL %d [ ",numModel);
      if (K_or_U==KNOWN) fprintf(stderr," KNOWN WORDS - ");
      else fprintf(stderr," UNKNOWN WORDS - ");
      if (LR_or_RL==LEFT_TO_RIGHT) fprintf(stderr,"LEFT TO RIGHT ]\n");
      else  fprintf(stderr,"RIGHT_TO_LEFT ]");
    }

  //Construimos el mapping a partir de los ejemplos seleccionados
  mapping *m =  new mapping();
  m->mappingBuilt(f,MAX_MAPPING_SIZE,COUNT_CUT_OFF);
  generateFileName(NAME,"",numModel,LR_or_RL,K_or_U,"MAP",mapFileName);
  m->mappingWrite(mapFileName.c_str(),FALSE);

  //Creamos el depósito de pesos y el hash de sesgos
  hash_t<weight_node_t*> *b = new hash_t<weight_node_t*>;
  b->hash_init(30);
  weightRepository *wr = new weightRepository;

  infoDict *pInfo;
  int nPositive=0,nNegative=0;

  //Nos situamos en el primer  elemento de lista
  lPosToTrain->setFirst();
  //Para cada elemento de la lista de etiquetas
  for (int ret=true; ret; ret=lPosToTrain->next())
  {
      if ( verbose == TRUE ) fprintf(stderr,"\n-----------------------------------------------------------");

      //Obtenemos la etiqueta morfosintáctica
      pInfo = *lPosToTrain->getIndex();
      nPositive=0;
      nNegative=0;

      //Preparamos el entrenamiento
      generateFileName(NAME,pInfo->pos,numModel,LR_or_RL,K_or_U,"POS",posFileName);
      generateFileName(NAME,pInfo->pos,numModel,LR_or_RL,K_or_U,"SVM",svmFileName);

      //Seleccionamos los ejemplos para el entrenamiento para la POS que estamos viendo
      if ( verbose == TRUE ) { fprintf(stderr,"\nPreparing training set for [ %s ] ..",pInfo->pos.c_str()); }

learnerDressNakedSetTrain(d,m,f,pInfo->pos,numModel,LR_or_RL, K_or_U,&nPositive,&nNegative);

      //cout << "\nPUTOOOOOOO\n";

      //exit(0);


      //Realizamos el entrenamiento llamando a SVM-light
      if ( verbose == TRUE ) fprintf(stderr,"\nTraining [ %s ] with %d samples: [+] = %d samples ; [-] = %d samples\n",pInfo->pos.c_str(),nPositive+nNegative,nPositive,nNegative);
       learnerExecSVMlight(SVMDIR,options.str(),posFileName,svmFileName);

      //Se insertan los valores obtenidos del entrenamiento en el depósito de pesos
      //y el el hashing de sesgos
      if ( verbose == TRUE ) fprintf(stderr,"\nAdding elements to MERGED MODEL from [ %s ]",posFileName.c_str());
      wr = learnerBuiltWeightRepository(wr,m,pInfo->pos,svmFileName);
      if ( verbose == TRUE ) fprintf(stderr," [DONE]");
      if ( verbose == TRUE ) fprintf(stderr,"\nAdding biases from [ %s ]",posFileName.c_str());
      b =  learnerBuiltBias(b,pInfo->pos,svmFileName.c_str());
      if ( verbose == TRUE ) fprintf(stderr," [DONE]");

      if (REMOVE_FILES == TRUE)
	{
	  //char cmd[150];
	  //sprintf(cmd,"rm -f %s",posFileName);
	  remove(posFileName.c_str()); //system(cmd);
	  //sprintf(cmd,"rm -f %s",svmFileName);
	  remove(svmFileName.c_str()); //system(cmd);
	}
  }
  lPosToTrain->setFirst();

  if ( verbose == TRUE ) fprintf(stderr,"\n-----------------------------------------------------------");
  std::string fileName;
  generateFileName(NAME,"",numModel,LR_or_RL,K_or_U,"MRG",fileName);
  if ( verbose == TRUE ) fprintf(stderr,"\nStoring MERGED MODEL [ %s ]",fileName.c_str());
  //Modificación 180705: Filtrado de pesos
  if ( K_or_U == KNOWN ) wr->wrWrite(fileName, KFILTER); //ADD 180705
  else wr->wrWrite(fileName, UFILTER); //ADD 180705
  //Escribir deposito de pesos en disco
  //wr->wrWrite(fileName); //DEL 180705
  if ( verbose == TRUE ) fprintf(stderr," [DONE]");

  FILE *fwr = openFile(fileName,"a+");
  fprintf (fwr,"BIASES ");
  wr->wrWriteHash(b,fwr,' ');  //Escribir biases en fichero de depósito de pesos
  fclose(fwr);

  generateFileName(NAME,"",numModel,LR_or_RL,K_or_U,"B",fileName);
  if ( verbose == TRUE ) fprintf(stderr,"\nStoring BIASES [ %s ]",fileName.c_str());
  FILE *fb =openFile(fileName,"w");
  wr->wrWriteHash(b,fb,'\n');  //Escribir biases en fichero de sesgos
  fclose(fb);

  if ( verbose == TRUE ) fprintf(stderr," [DONE]");

  delete m;
  delete wr;
  learnerDestroyBias(b);
}

/*******************************************************/
/*
 * Recibe como parámetro el nombre del fichero de entrenamiento
 * (trainingFileName), el diccionario para palabras conocidas (dKnown),
 * el número del modelo a entrenar (numModel), la dirección en la cual
 * se realiza el entrenamiento (direction), el número de frases del
 * corpus (numSent), el número de palabras del corpus (numWords) y
 * el número de fragmentos en que se ha de dividir el corpus para
 * entrenar palabras desconocidas (numChunks).
 *
 * Este método selecciona ejemplo y realiza el entrenamiento del modelo
 */
void learner::learnerTrainModel(const std::string& trainingFileName, dictionary *dKnown,int numModel, int direction, int /*numSent*/,int numWords, int numChunks)
{
  FILE *fKnownRL,*fUnknownRL,*fUnknownLR,*fKnownLR;
  int contSentences = 0, ret = 1;
  std::ostringstream name;

  //Carga las listas de atributos
  simpleList<nodo_feature_list*> featureList,featureListUnk;
  name << std::string(NAME) << std::string(".A") << numModel;
  createFeatureList(name.str(),&featureList);
  name.rdbuf()->str("");
  name << NAME << ".A"<<numModel<<".UNK";
  createFeatureList(name.str(),&featureListUnk);

  if (direction==LEFT_TO_RIGHT) learnerPrintMessage(numModel,-1,LEFT_TO_RIGHT,TRUE);
  else if (direction==RIGHT_TO_LEFT) learnerPrintMessage(numModel,-1,RIGHT_TO_LEFT,TRUE);
  else if (direction==LR_AND_RL) learnerPrintMessage(numModel,-1,LR_AND_RL,TRUE);

  //Abrimos los ficheros de ejemplos
  if (direction==RIGHT_TO_LEFT || direction==LR_AND_RL)
  {
    name.rdbuf()->str("");
    name << std::string(NAME) << ".M" << numModel << ".RL.SAMPLES";
    fKnownRL=openFile(name.str().c_str(),"w+");
    name.rdbuf()->str("");
    name << std::string(NAME) << ".UNK.M" << numModel << ".RL.SAMPLES";
    fUnknownRL=openFile(name.str().c_str(),"w+");
  }
  if (direction==LEFT_TO_RIGHT || direction==LR_AND_RL)
  {
    name.rdbuf()->str("");
    name << std::string(NAME)<<".M"<<numModel<<".LR.SAMPLES";
    std::cerr << "Problematic file is '"<<name<<"'" << std::endl;
    fKnownLR=openFile(name.str().c_str(),"w+");
    name.rdbuf()->str("");
    name << std::string(NAME) << ".UNK.M"<<numModel<<".LR.SAMPLES";
    fUnknownLR=openFile(name.str().c_str(),"w+");
  }

  int chunkSize =  (numWords/numChunks) + 1;

  if (verbose == TRUE)
  {
    fprintf(stderr,"\n* X = %f :: CHUNKSIZE = %d :: CHUNKS = %d",X,chunkSize,numChunks);
    fprintf(stderr,"\n* ========================================================================");
  }

  int nWordsRL = 0, nWordsLR = 0;
  int inicioRL = -1, inicioLR = -1;

  //Para cada chunk
  for (int i=0; (ret>=0 && i<numChunks);i++)
  {
    std::cerr << "chunk " << i << std::endl;
    ret=1;

    int is_end_of_chunk = FALSE;
    int is_end_of_sentence = FALSE;

    if ( verbose == TRUE ) fprintf(stderr,"\nChunk %d [ %d ] ",i+1,i*chunkSize);

    //Creamos  el diccionario para entrenar palabras desconocidas
    dictionary *dUnknown = new dictionary(trainingFileName,i*chunkSize, (i+1)*chunkSize - 1 );

    if (R!=NULL) dUnknown->dictRepairFromFile(R);
    else dUnknown->dictRepairHeuristic(DRATIO);
    name.rdbuf()->str("");
    name << std::string(NAME) << ".DICT."<<i;
    dUnknown->dictWrite(name.str());
    delete dUnknown;
    dUnknown  = new dictionary(name.str());

    //Si el modelo en 4 usaremos el mismo diccionario para conocidas y desconocidas
    dictionary *d_for_known = NULL;
    //if (numModel==4) dKnown = dUnknown;
    if ( numModel == 4 ) {  d_for_known = dUnknown; }
    else { d_for_known = dKnown; }

    if ( verbose ==  TRUE ) fprintf(stderr,"\nExtracting features : ");

    nWordsLR = chunkSize;
    nWordsRL = chunkSize;

    //Mientras haya palabras por leer y no estemos al final del chunk
    while ( ret>=0 && is_end_of_chunk==FALSE )
    {
      is_end_of_sentence = FALSE;
      //Si es LR o LRL
      if (direction==LEFT_TO_RIGHT || direction==LR_AND_RL)
      {
        fKnown = fKnownLR;  fUnknown = fUnknownLR;
        //Recorremos el texto en sentido LR para seleccionar ejemplos
        nWordsLR = learnerLeftToRight(&featureList,&featureListUnk, d_for_known, dUnknown, nWordsLR, inicioLR);
        inicioLR = sw->getIndex()->ord;
        if (!sw->next())
        {
          is_end_of_sentence = TRUE;
          inicioLR = -1;
        }
      }
      //Si es RL o LRL
      if (direction==RIGHT_TO_LEFT || direction==LR_AND_RL)
      {
        fKnown = fKnownRL; fUnknown = fUnknownRL;
        //Recorremos el texto en sentido RL para seleccionar ejemplos
        nWordsRL = learnerRightToLeft(&featureList,&featureListUnk, d_for_known , dUnknown, nWordsRL, inicioRL);
        inicioRL = sw->getIndex()->ord;
        if ( !sw->previous() )
        {
          is_end_of_sentence = TRUE;
          inicioRL = -1;
        }
      }

      contSentences++;

      if ( verbose  == TRUE) showProcess(contSentences,0);
      //Si es fin de frase recargamos la ventana
      if ( is_end_of_sentence == TRUE )
      {
        sw->deleteList();
        ret = sw->iniGeneric(dKnown);
      }

      //Si hemos recorrido todas la palabras del chunk es fin de chunk
      if ( nWordsRL <= 0 || nWordsLR <= 0 )
      {
        is_end_of_chunk = TRUE;
      }
    }

    if ( verbose  == TRUE) showProcess(contSentences,1);
    delete dUnknown;

    //Si es necesario borramos los ficheros temporales
    //char cmd[200];
    //sprintf(cmd, "rm -f %s",name);
    if (REMOVE_FILES == TRUE) remove(name.str().c_str());//system(cmd);
  }

  //Creamos una copia de la lista de etiquetas para palabras conocidas
  simpleList<infoDict*>* copyOfUNKP_L = learnerTransformHashInList(learnerUNKP_H);
  //para cada tipo known o unk
  //para cada direccion
  if (direction==LEFT_TO_RIGHT || direction==LR_AND_RL)
  {
    //Realizamos el entrenamiento
    learnerDoLearn(fKnownLR,numModel,LEFT_TO_RIGHT,KNOWN,dKnown,learnerAMBP_L);
    learnerDoLearn(fUnknownLR,numModel,LEFT_TO_RIGHT,UNKNOWN,dKnown,copyOfUNKP_L);
    fclose (fKnownLR);
    fclose (fUnknownLR);
  }

  if (direction==RIGHT_TO_LEFT || direction==LR_AND_RL)
  {
    //Realizamos el entrenamiento
    learnerDoLearn(fKnownRL,numModel,RIGHT_TO_LEFT,KNOWN,dKnown,learnerAMBP_L);
    learnerDoLearn(fUnknownRL,numModel,RIGHT_TO_LEFT,UNKNOWN,dKnown,copyOfUNKP_L);
    fclose(fKnownRL);
    fclose(fUnknownRL);
  }
  //fin para cada direccion
  //fin para cada tipo
  
  destroyFeatureList(&featureList);
  destroyFeatureList(&featureListUnk);
}

/**********************************************************/

/*
 * Recibe como parámetros: wrd un string con una palabra,  
 * pos un string con una etiqueta, Known_or_Unknown si la 
 * palabra es conocida o desconocida. Si la palabra es conocida 
 * devuelve TRUE si la etiqueta pos está en la lista de etiquetas 
 * morfosintácticas ambiguas. Si la palabra es desconocida 
 * devuelve TRUE si la etiqueta pos pertenece a lista de posibles 
 * categorías morfosintácticas para palabras desconocidas. En cualquier 
 * otro caso devuelve FALSE. 
 */
int learner::learnerIsPossiblePOS(const std::string& /*wrd*/, const std::string& pos, int Known_or_Unknown)
{
  if (Known_or_Unknown==KNOWN)
  {
    if (HASH_FAIL != (long)learnerAMBP_H->hash_lookup(pos))
    {
      return TRUE;
    }
  }
  else if (Known_or_Unknown==UNKNOWN)
  {
    if (HASH_FAIL != (long)learnerUNKP_H->hash_lookup(pos))
    {
      return TRUE;
    }
  }
  return FALSE;
}

/**************************************************************/
/*
 * Recibe como parámetros: 
 *	wrd un string con una palabra, 
 *	Known_or_Unknown un entero que indica si la palabra 
 *			 es conocida o desconocida y 
 *	un apuntador d al diccionario. Devuelve un apuntador a una lista. 
 */
simpleList<infoDict*>* learner::learnerGetPotser(const std::string& wrd, int Known_or_Unknown, dictionary* d)
{
  bool stop=false,ret=true;
  dataDict* w = d->getElement(wrd);
  infoDict *pInfoDict;
  simpleList<infoDict*>* lout = new simpleList<infoDict*>();

  //Si es conocida recoge las posibles etiquetas que contiene el diccionario 
  //para la palabra wrd  y mira si existen en la lista de categorías
  //morfosintácticas ambiguas. Si existen en la lista de etiquetas ambiguas 
  //las añade a la lista de salida, en caso contrario si no esta es posible 
  //etiqueta ambigua y es la etiqueta más frecuente se devuelve una lista 
  //sólo con la etiqueta más frecuente.
  if (Known_or_Unknown==KNOWN)
  {
    if ((long)w != HASH_FAIL)
    {
      simpleList<infoDict*>* list = &d->getElementMaybe(w);
      list->setFirst();
      while (ret && !stop )
      {
        pInfoDict = *list->getIndex();

        if (HASH_FAIL!=(long)learnerAMBP_H->hash_lookup(pInfoDict->pos))
        {
          infoDict *ptr = new infoDict;
          ptr->pos = pInfoDict->pos;
          ptr->num = pInfoDict->num;
          lout->add(ptr);
        }
        else if ( d->getMFT(w)->pos == pInfoDict->pos ) //si es most frequent tag
        {
          d->dictCleanListInfoDict(lout,lout->numElements());
          lout->deleteList();
          infoDict *ptr = new infoDict;
          ptr->pos = pInfoDict->pos;
          ptr->num = pInfoDict->num;
          lout->add(ptr);
          stop = true;
        }
        ret=list->next();
      }
      list->setFirst();
    }
  }
  //Si la palabra es desconocida la lista de salida contiene todas las
  //categorías morfosintácticas posibles desconocidas. 
  else	if (Known_or_Unknown==UNKNOWN)
    {
	 learnerUNKP_L->setFirst();
         while (ret>=0)
         {
	   pInfoDict =*learnerUNKP_L->getIndex();
	   infoDict *ptr = new infoDict;
     ptr->pos = pInfoDict->pos;
	   ptr->num = pInfoDict->num;
	   lout->add(ptr);

	   ret=learnerUNKP_L->next();
         }
         learnerUNKP_L->setFirst();
      }

  return lout;
}


/**********************************************************/
/*
 * Recibe como parámetros: 
 * un apuntador al depósito de pesos (wr), 
 * un apuntador a mapping (m), 
 * un string con la etiqueta (pos) que estamos entrenando y 
 * el nombre del fichero  (fileName) en el cual se almacenan las SVs
 * para la etiqueta indicada. 
 *
 * Partiendo de estos datos recorre el fichero fileName,  traduciendo
 * cada uno de los atributos leídos gracias al mapping (m). 
 * Y añadiendo la correspondiente pareja etiqueta/atributo 
 * al depósito de pesos wr mediante el método wrAdd.
 *
 * Se devuelve el apuntador al depósito de pesos (weightRepository) 
 * que ha sido modificado.
 */
weightRepository *learner::learnerBuiltWeightRepository(weightRepository* wr, mapping* m, const std::string& pos, const std::string& fileName)
{
  std::string str;
  std::string key;
  FILE *f = openFile(fileName,"r");
  int ret=1,trobat=FALSE;

  while (!feof(f) && ret>=0)
  {
    if (trobat==FALSE)
    {
      ret= readTo(f,'\n',0,str);
      if (str.find("threshold")!=std::string::npos) trobat=TRUE;
    }
    else
    {
      ret = readTo(f,' ',0,str);
      long double ld;

      ld  = atof(str.c_str());

      while (ret>0)
      {
        ret = readTo(f,':','\n',str);
        if (ret>0)
        {
          key = m->mappingGetFeatureByNumber(str.c_str());
          ret = readTo(f,' ','\n',str);
          //if ((int)key != HASH_FAIL) wr->wrAdd(key,pos,ld);
          if (atoi(key.c_str()) != HASH_FAIL) wr->wrAdd(key,pos,ld);
        }
      }
    }
  }
  fclose (f);
  return wr;
}

/**************************************************************/

void learner::learnerDestroyBias(hash_t<weight_node_t*> *h)
{
  h->hash_destroy();
}

/**************************************************************/

/* 
 * Recibe como parámetros: 
 * Un apuntador a hash_t (h) que es el lugar donde almacenaremos los sesgos, 
 * la etiqueta (pos) que estamos entrenando y el fichero (fileName) de salida
 * de SVM-light en el cual se encuentran los datos esperados.
 * Este método lee el sesgo del fichero y lo añade (hash_insert) 
 * al hashing h para la etiqueta pos indicada. 
 * Posteriormente devuelve el apuntador al hashing modificado. 
 */
hash_t<weight_node_t*> *learner::learnerBuiltBias(hash_t<weight_node_t*>* h, const std::string& pos, const std::string& fileName)
{
  std::string str;
  FILE *f = openFile(fileName.c_str() ,"r");
  int ret=1,trobat=FALSE;

  while (!feof(f) && trobat==FALSE && ret>=0)
    {
      ret= readTo(f,'\n',0,str);
      if (str.find("threshold") != std::string::npos) trobat=TRUE;
    }

  std::string bias;
  bool sortir = false;
  for (std::string::size_type i=0;(i<str.size() && !sortir) ;i++)
  {
    if (str[i]==' ' || str[i]=='#') sortir = true;
    else bias += str[i];
  }

  weight_node_t *w = new weight_node_t;
  w->pos = pos;
  w->data = (long double)0;
  w->data = atof (bias.c_str());
  h->hash_insert(w->pos, w);

  fclose(f);
  return h;
}

/********************************************************/

/*
 * Recibe como parámetro el nombre del fichero de configuración (train).
 * Ejecuta el aprendizaje 
 */
void learner::learnerRun(const std::string& train)
{
  int iWrd=0,iSent=0;

  //Leemos el fichero de configuración
  read_config_file(train);
  //obtener tamaño del corpus
  learnerCount(TRAINSET,&iWrd,&iSent);

  if ( verbose == TRUE )
    {
      fprintf(stderr,"\n* trainset # words        = [ %d ]",iWrd);
      fprintf(stderr,"\n* trainset # sentences    = [ %d ]",iSent);
      fprintf(stderr,"\n* ========================================================================");
      fprintf(stderr,"\n\n* ========================================================================");
      fprintf(stderr,"\n* PREPARING TRAINING");
      fprintf(stderr,"\n* ========================================================================");
    }
  struct  tms tbuff1,tbuff2;
  clock_t start,end;
  start = times(&tbuff1);

  //Creamos el diccionario
  std::string name;
  name = std::string(NAME) + ".DICT";
  dictionary *d = new dictionary(TRAINSET, 0,0);
  if (R!=NULL) d->dictRepairFromFile(R); // "/mnt/hda4/pfc/WSJ.200");
	else d->dictRepairHeuristic(DRATIO);
  d->dictWrite(name);
  delete d;
  d  = new dictionary(name);

  //Obtenemos las listas de etiquetas
  if (learnerAMBP_H == NULL)
    learnerAMBP_H = d->dictFindAmbP(&learnerNumAMBP);
  if (learnerUNKP_H == NULL)
    learnerUNKP_H = d->dictFindUnkP(&learnerNumUNKP);
  //Creamos los ficheros de etiquetas
  learnerCreatePOSFile(NAME,TRUE,learnerAMBP_H);
  learnerCreatePOSFile(NAME,FALSE,learnerUNKP_H);
  //Creamos unos hashings con las listas de etiquetas
  learnerAMBP_L = learnerTransformHashInList(learnerAMBP_H);
  learnerUNKP_L = learnerTransformHashInList(learnerUNKP_H);

  //creamos el fichero de configuración de la ventana
  learnerCreateDefaultFile(NAME,"WIN");

  //Calculamos en numero de chunks
  int chunks = learnerNumChunks(TRAINSET,X,iSent);

  //Mientras haya modelos por entrenar
  while (!DO.empty())
  {
    int *numModel = DO.top();
    DO.pop();
    int *direction = DO.top();
    DO.pop();

    //Eliminamos los ficheros anteriores
    removeFiles(NAME, RM_MODEL_FILES,*numModel, *direction, verbose);
    removeFiles(NAME, RM_TEMP_FILES ,*numModel, *direction, verbose);

    std::ostringstream name;
    name << std::string("A") << *numModel;
    learnerCreateDefaultFile(NAME,name.str());

    //Creamos la ventana
    std::ifstream f(TRAINSET);
    sw = new swindow(f,WINDOW_SIZE,CORE_POSITION, &std::cerr, d);

    //ejecutamos el entrenamiento para el modelo
    learnerTrainModel(TRAINSET,d,*numModel,*direction,iSent,iWrd,chunks);
    delete sw;
  }
  delete d;

  end = times(&tbuff2);
  if ( verbose == TRUE )
  {
    fprintf(stderr,"\n\n* ========================================================================\n");
    showTime ("* SVM-light Time",  time_svmlight, time_svmlight, 0);
    showTime ("* SVMTlearn Time",
      ((double)(end-start))/CLOCKS_PER_SECOND - time_svmlight, //CLK_TCK,
      ((double)tbuff2.tms_utime-(double)tbuff1.tms_utime)/CLOCKS_PER_SECOND,
      ((double)tbuff2.tms_stime-(double)tbuff1.tms_stime)/CLOCKS_PER_SECOND);
    showTime ("* Total Learning Time",
      time_svmlight + ((double)(end-start))/CLOCKS_PER_SECOND,
      time_svmlight + ((double)tbuff2.tms_utime-(double)tbuff1.tms_utime)/CLOCKS_PER_SECOND,
      ((double)tbuff2.tms_stime-(double)tbuff1.tms_stime)/CLOCKS_PER_SECOND);
    fprintf(stderr,"* ========================================================================\n\n");
  }
}

/**************************************************************/

/*
 * Recorre el texto de izquierda a derecha seleccionando ejemplos
 */
int learner::learnerLeftToRight(simpleList<nodo_feature_list*>* featureList, simpleList<nodo_feature_list*>* featureListUnk, dictionary *dKnown, dictionary *dUnknown, int numWrds, int inicio)
{
  std::cerr << "learner::learnerLeftToRight" << std::endl;
  bool ret = true;

  if ( inicio == -1 ) while (sw->previous());
  else if (sw->getIndex()->ord!=inicio)
  {
    while(ret && sw->getIndex()->ord != inicio)
    {
      if (inicio < sw->getIndex()->ord ) ret = sw->previous();
      if (inicio > sw->getIndex()->ord ) ret = sw->next();
    }
  }
  nodo *elem = sw->getIndex();
  numWrds--;
  learnerGenerateFeatures(elem,featureList,dKnown, LEFT_TO_RIGHT);
  learnerGenerateFeaturesUnk(elem,featureListUnk,dKnown, dUnknown, LEFT_TO_RIGHT);
  
  while(numWrds>=0)
    {
      if ( !sw->next() ) return numWrds;

      elem = sw->getIndex();
      numWrds--;
      learnerGenerateFeatures(elem,featureList,dKnown,LEFT_TO_RIGHT);
      learnerGenerateFeaturesUnk(elem,featureListUnk,dKnown, dUnknown, LEFT_TO_RIGHT);
    }

   return (numWrds);
}

/**************************************************************/

/*
 * Recorre el texto de derecha a izquierda seleccionando ejemplos
 */
int learner::learnerRightToLeft(simpleList<nodo_feature_list*>* featureList, simpleList<nodo_feature_list*>* featureListUnk, dictionary *dKnown, dictionary *dUnknown, int numWrds, int inicio)
{
  std::cerr << "learner::learnerRightToLeft" << std::endl;
  bool ret = true;

  if ( inicio == -1 ) while (sw->next());
  else  if ( sw->getIndex()->ord != inicio )
    {
     while(ret && sw->getIndex()->ord != inicio)
      {
	if (inicio < sw->getIndex()->ord ) ret = sw->previous();
	if (inicio > sw->getIndex()->ord ) ret = sw->next();
      }
    }

  nodo *elem = sw->getIndex();
  numWrds--;
  learnerGenerateFeatures(elem,featureList,dKnown,RIGHT_TO_LEFT);
  learnerGenerateFeaturesUnk(elem,featureListUnk,dKnown,dUnknown,RIGHT_TO_LEFT);

  while( numWrds>=0 )
    {
      if ( !sw->previous() ) return numWrds;

      elem = sw->getIndex();
      numWrds--;
      learnerGenerateFeatures(elem,featureList,dKnown, RIGHT_TO_LEFT);
      learnerGenerateFeaturesUnk(elem,featureListUnk,dKnown, dUnknown, RIGHT_TO_LEFT);
    }

  return numWrds;
}

/**************************************************************/
/*
 * Esta función recibe como parámetros: 
 * el apuntador a un nodo de la ventana (elem),
 * una pila donde apilara los atributos generados (stk), 
 * la lista de atributos que debe generar (featureList), 
 * el diccionario con la información necesaria para el cálculo de features (d) 
 * y  la dirección en que se recorre el corpus (direction). 	
 * Recorre la lista featureList y ejecuta los métodos necesarios 
 * de la ventana (swindow) para generar los atributos y que al final 
 * de la ejecución de este método esten apilados en stk.
*/
void learner::learnerGetFeatures(nodo* elem, std::stack<std::string>& stk, dictionary* d, simpleList<nodo_feature_list*>* featureList, int direction)
{
  nodo_feature_list* aux = NULL;
  bool ret = true;
  //Recorre la lista de atributos y crea los atributos correspondientes
  while (ret)
  {
    aux = *featureList->getIndex();
    if (aux->mark == SLASTW)  sw->winPushSwnFeature(stk);
    else if (aux->mark == WMARK)  sw->winPushWordFeature((void *)aux,d,stk,direction);
    else if (aux->mark == KMARK)  sw->winPushAmbiguityFeature((void *)aux,d,stk,direction);
    else if (aux->mark == MMARK)  sw->winPushMaybeFeature((void *)aux,d,stk,direction);
    else if (aux->mark == PMARK)  sw->winPushPosFeature((void *)aux,d,stk,direction);
    else if (aux->mark == MFTMARK)  sw->winPushMFTFeature((void *)aux,d,stk,direction);
    else
    {
      int *param;
      if (!aux->l.isEmpty())
      {
        param = (int *) aux->l.getIndex();
      }
      if (aux->mark == PREFIX_MARK)  sw->winPushPrefixFeature(elem->wrd, stk, *param);
      else if (aux->mark == SUFFIX_MARK) sw->winPushSuffixFeature(elem->wrd, stk, *param);
      else if (aux->mark == CHAR_A_MARK) sw->winPushLetterFeature(elem->wrd, stk, COUNTING_FROM_BEGIN, *param);
      else if (aux->mark == CHAR_Z_MARK) sw->winPushLetterFeature(elem->wrd, stk, COUNTING_FROM_END, *param);
      else if (aux->mark == LENGTH_MARK) sw->winPushLenghtFeature(elem->wrd,stk);
      else if (aux->mark == START_CAPITAL_MARK) sw->winPushStartWithCapFeature(elem->wrd,stk);
      else if (aux->mark == START_LOWER_MARK)  sw->winPushStartWithLowerFeature(elem->wrd,stk);
      else if (aux->mark == START_NUMBER_MARK) sw->winPushStartWithNumberFeature(elem->wrd,stk);
      else if (aux->mark == ALL_UPPER_MARK) sw->winPushAllUpFeature(elem->wrd,stk);
      else if (aux->mark == ALL_LOWER_MARK) sw->winPushAllLowFeature(elem->wrd,stk);
      else if (aux->mark == CONTAIN_CAP_MARK) sw->winPushContainCapFeature(elem->wrd, stk);
      else if (aux->mark == CONTAIN_CAPS_MARK) sw->winPushContainCapsFeature(elem->wrd, stk);
      else if (aux->mark == CONTAIN_COMMA_MARK) sw->winPushContainCommaFeature(elem->wrd, stk);
      else if (aux->mark == CONTAIN_NUMBER_MARK) sw->winPushContainNumFeature(elem->wrd, stk);
      else if (aux->mark == CONTAIN_PERIOD_MARK) sw->winPushContainPeriodFeature(elem->wrd, stk);
      else if (aux->mark == MULTIWORD_MARK) sw->winPushMultiwordFeature(elem->wrd, stk);
    }
    ret = featureList->next();
  }
  featureList->setFirst();
}

/**************************************************************/

/*
 * El objetivo de este mï¿½todo es seleccionar o descartar una palabra para
 * realizar en el entrenamiento de palabras desconocidas, calcular respectivos
 * atributos e insertar esta informaciï¿½n en el fichero de ejemplos correspondiente. 
 */
void learner::learnerGenerateFeaturesUnk(nodo *elem, simpleList<nodo_feature_list*>* featureList,dictionary *d, dictionary *dUnk, int direction)
{
  std::stack<std::string> stk;
//   nodo_feature_list *aux;
  int is_selected = FALSE;
  std::string feature;
  std::string buffer;

  if (d==NULL || elem==NULL || featureList==NULL) return;

//   init_stack(&stk);

  buffer = elem->wrd + ":" + elem->comment;

  dataDict* i = dUnk->getElement(elem->wrd);
  if ((long)i!=HASH_FAIL)
  {
    /*
    int i2 = d->getElement(elem->wrd);
    if (dUnk->getElementNumMaybe(i) == 1 && hash_lookup(learnerUNKP_H,d->getMFT(i2))!=HASH_FAIL )
    {
          fprintf(fUnknown,buffer,strlen(buffer));
    is_selected = TRUE;
    }
  */
  }
  else
  {
    fprintf(fUnknown,buffer.c_str(),buffer.size());
    is_selected = TRUE;
  }

  if ( is_selected == TRUE) learnerGetFeatures(elem, stk,dUnk, featureList, direction );

  while (!stk.empty() && is_selected == TRUE)
  {
    feature = stk.top();
    stk.pop();
    buffer = std::string(" ") + feature;
    fprintf(fUnknown,buffer.c_str(),buffer.size());
  }

  if (is_selected == TRUE) fprintf(fUnknown,"\n");

  elem->pos = elem->comment;
 }

/**************************************************************/

/*
 * El objetivo de este mï¿½todo es seleccionar o descartar una palabra para
 * realizar en el entrenamiento de palabras conocidas, calcular respectivos
 * atributos e insertar esta informaciï¿½n en el fichero de ejemplos correspondiente. 
 */
void learner::learnerGenerateFeatures(nodo *elem, simpleList<nodo_feature_list*>* featureList,dictionary *d, int direction)
{
  std::stack<std::string> stk;
//   nodo_feature_list *aux;
  int is_selected = FALSE;
//   int is_unk = FALSE;
  std::string feature;
  std::ostringstream buffer;

  if (d==NULL || elem==NULL || featureList==NULL) return;

//   init_stack(&stk);

  buffer << elem->wrd<<":"<<elem->comment;

  dataDict* i = d->getElement(elem->wrd.c_str());
  if ((long)i!=HASH_FAIL)
  {

    if ( d->getElementNumMaybe(i)>1 && (long)(learnerAMBP_H->hash_lookup(d->getMFT(i)->pos))!=HASH_FAIL )
    {
      fprintf(fKnown,buffer.str().c_str(),buffer.str().size());
      is_selected = TRUE;
    }
  }

  if ( is_selected == TRUE) learnerGetFeatures(elem, stk,d, featureList, direction );

  while (!stk.empty() && is_selected == TRUE)
  {
    feature = stk.top();
    stk.pop();
    buffer.clear();
    buffer << " " << feature;
    fprintf(fKnown,buffer.str().c_str(),buffer.str().size());
  }
  buffer.clear();
  buffer << std::endl;

  if ( is_selected == TRUE)
    fprintf(fKnown,buffer.str().c_str(),buffer.str().size());

  elem->pos = elem->comment;
}

/************************************************************/
/*
 * Ejecuta SVM-light. Recibe como parámetros cuatro cadenas de 
 * caracteres: svmdir es el directorio en el que se encuentra SVM-light,  
 * options son las opciones con que se lanzará SVM-light, posFile el nombre
 * del fichero de ejemplos usado como entrada para la herramienta 
 * de Joachims y, por último, outFile que es el nombre del fichero 
 * de salida.  Esta función devuelve 0.
*/
int learner::learnerExecSVMlight(const std::string& svmdir, const std::string& options, const std::string& posFile, const std::string& outFile)
{
  time_t begin, finish;

  begin = time (0);

  std::string command;
  command = svmdir + "/svm_learn -v 0  "+options+" "+posFile+" "+outFile;
  
  if ( verbose == TRUE ) fprintf(stderr,"Executing Joachims svm_light [ with options: %s ]  ",options.c_str());
  system(command.c_str());
  if ( verbose == TRUE ) fprintf(stderr," [DONE]");

  finish = time(0);

  time_svmlight = difftime(finish,begin) +  time_svmlight;

  return 0;
}

/**************************************************************/

simpleList<infoDict*>* learner::learnerTransformHashInList(hash_t<infoDict*> *tptr)
{
//   hash_node_t *node, *last;
//   int i;
  simpleList<infoDict*>* l = new simpleList<infoDict*>();

  for (hash_t<infoDict*>::iterator it  = tptr->begin(); it != tptr->end(); it++)
  {
    infoDict *p = ((*it).second);
    l->add(p);
  }

  l->setFirst();
  return l;
}

/**************************************************************/

/*
 * Calcularemos el número de fragmentos en el que se ha de dividir 
 * el corpus para conseguir un porcentaje de palabras desconocidas 
 * determinado. Lo parámetros de entrada son el nombre del fichero 
 * de entrenamiento (trainingFileName), el porcentaje de palabras 
 * desconocidas deseado (percentage) y el número de frases del corpus 
 * (nSentences). El valor devuelto es un entero indicando el número de
 * fragmentos. Si el número de fragmentos calculado es mayor que el número
 * de frases que contiene el corpus de entrenamiento se devolverá como el 
 * número de frases (nSentences).
 */
int learner::learnerNumChunks(const std::string& trainingFileName,float /*percentage*/,int nSentences)
{
  int ret=0,ndwords=0,nwords=0;
  std::string wrd;

  FILE *f = openFile (trainingFileName.c_str(),"r");
  hash_t<int> h1;
  h1.hash_init(10000);
  while (!feof(f))
  {
    ret = readTo(f,' ','\n',wrd);
    if (ret>=0)
    {
      nwords++;
      std::string w(wrd);
      //if ((int)hash_insert(&h1,w,(int) w)==HASH_FAIL) ndwords++;
      if ((int)h1.hash_insert(w,atoi(w.c_str()))==HASH_FAIL) ndwords++;
      if (ret>0) readTo(f,'\n','\n',wrd);
    }
  }

   //Read again until a certain point where X is met   --> $ndwords * (100 - $X) / 100;
   float meeting = ndwords * (100 - X ) /100;
   int nwords2=0,ndwords2=0;
   fseek(f,0,SEEK_SET);
   hash_t<int> h2;
   h2.hash_init(10000);
   while (!feof(f) && ndwords2<meeting)
  {
    ret = readTo(f,' ','\n',wrd);
    if (ret>=0)
    {
      nwords2++;
      std::string w(wrd);
      //if (hash_insert(&h2,w,(int) w)==HASH_FAIL) ndwords2++;
      if (h2.hash_insert(w,atoi(w.c_str()))==HASH_FAIL) ndwords2++;
      if (ret>0) readTo(f,'\n','\n',wrd);
    }
  }

   int chunks = nwords/(nwords - nwords2);
   if (nSentences<=chunks) chunks = nSentences;

   fclose (f);

   h2.hash_destroy();
   h1.hash_destroy();
   return chunks;
}

/**************************************************************/

bool learner::learnerIsInsideList(simpleList<infoDict*>* l, const std::string& key)
{
  if (l==NULL || key.empty()) return false;
  int ret = 0;
  while (ret>=0)
  {
    infoDict* ptr = (infoDict *)l->getIndex();
    if (key == ptr->pos)
    {
      return true;
    }
  }
  l->setFirst();
  return false;
}
