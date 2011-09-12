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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "common.h"

using namespace std;

/**************************************************/

/*
 * FILE *openFile(char *name, char mode[])
 * Abre un fichero con el nombre <name> y en el modo <mode> 
 * (r lectura, w escritura, a actualización ...).
 * Devuelve el puntero al fichero
 * En caso de no poder abrir un fichero, termina la ejecucion
 */
FILE *openFile(const std::string &name, const char mode[])
{
    FILE *f;
    if ((f = fopen(name.c_str(), mode))== NULL)
    {
		fprintf(stderr, "Error opening file: %s\n",name.c_str());
		exit(0);
    }
    return f;
}

/**************************************************/

void generateFileName(const std::string& name, const std::string& added, int numModel, int direction, int what, const std::string& type, std::string& out)
{
	out = name;
  if (!added.empty())
  { 
    out += ".";
    for (std::string::size_type i=0; i<added.size(); i++)
    {
      if (added[i]==':') out += "DOSPUNTS";
      else if (added[i]=='`')  out += "COMETA2";
      else if (added[i]=='\'') out += "COMETA";
      else out += added[i];

    }
	}
	if (what==UNKNOWN) 	out += ".UNK";
	if (numModel>=0) out += ".M" + numModel;
	if (direction==LEFT_TO_RIGHT) out += ".LR";
	else if (direction==RIGHT_TO_LEFT) out += ".RL";
	if (!type.empty()) out += "." + type;
}

/**************************************************/

void showProcess(int num,int isEnd)
{
  if (isEnd) { fprintf(stderr,".%d sentences [DONE]\n\n",num); return; }
  else if (num%100==0) fprintf(stderr,"%d",num);
  else if (num%10==0) fprintf(stderr,".");
}

/**************************************************/

void showProcessDone(int num, int freq, int isEnd, const std::string& what)
{
  if (isEnd) { std::cerr <<"."<<num<<" "<<what<<" [DONE]"<<std::endl; return; }
  else if (num%freq==0) fprintf(stderr,".");
}

/**************************************************/

/* 
 * int goToWord(FILE *f, int offset) 
 * Lee <offset> lineas del canal o fichero <f>
 * Retorna -1 si encuentra eof
 * retorna el numero de lineas leidas si todo va bien         
 */
int goToWord(FILE *f, int offset)
{
	int cont=0;

	while (!feof(f) && cont<offset)
	{
	   if (fgetc(f)=='\n') cont++;
	}
	
	if (feof(f)) return -1;
	return cont;
}

/**************************************************/

/*
 * int readString(FILE *f, char *out)
 * Lee un String entre espacios o espacios y fines de linea
 *    FILE *f - es fichero o canal de donde leeremos el String
 *    char *out - es un parametro de salida, contendra el 
 *                String que leeremos
 * Retorna -1 si encuentra eof
 * retorna  0 si todo va bien
 */
int readString(FILE *f, std::string &out)
{
  if (feof(f)) return -1;
  
  char c = fgetc(f);
  out = "";
  
  while (!feof(f) && c==' ' && c=='\n') c=fgetc(f);
  
  while (!feof(f) && c!=' ' && c!='\n')
   { 
     out += c;
     c=fgetc(f);
   }
   if (feof(f) && out.empty()) return -1;
   return 0;	
}

/**************************************************/

/*
 * int readTo(FILE *f, char endChar, chas endLine, char *out)
 * Lee un del canal o fichero <f>, el String leido sera devuelto como el
 * parametro de salida <out>. Para leer el String se leera hasta encontrar
 * el <endChar> o el caracter <endLine>
 * Retorna 0 si encuentra <endLine> 
 * retorna -1 si eof
 * retorn 1 si todo va bien y encuentra <endChar>
 */
int readTo(FILE *f, char endChar, char endLine, std::string &out)
{
  out = "";
  char c = endChar+1;
  while (!feof(f) && c!=endChar && (endLine==0 || c!=endLine))
    { 
      c=fgetc(f);      
      if (c!=endChar && c!=endLine) out += c;
    }
  if (feof(f)) return -1;
  if (c==endLine) return 0;
  return 1;  
}

/*******************************************************/

void qsort(int a[], int lo, int hi) {
      int h, l, p, t;

      if (lo < hi) {
         l = lo;
         h = hi;
         p = a[hi];

         do {
            while ((l < h) && (a[l] <= p))
               l = l+1;
            while ((h > l) && (a[h] >= p))
               h = h-1;
            if (l < h) {
               t = a[l];
               a[l] = a[h];
               a[h] = t;
            }
         } while (l < h);

         t = a[l];
         a[l] = a[hi];
         a[hi] = t;

         qsort(a, lo, l-1);
         qsort(a, l+1, hi);
      } // if
   }

/**************************************************/

void showTime(const std::string& what, double real, double utime, double stime)
{ 
//   char message[200]="";
  std::cerr << what << ": [ Real Time = %5.3lf "<< real << " secs.( %5.3lf "<<utime<<" usr + %5.3lf "<< stime << " sys = %5.3lf " << utime+stime << " CPU Time) ]"<< std::endl;
}

/**************************************************/

int buscarMenorEnString(const char *szIn,char *szMenor,int *iMenor)
{
  char szString[100];
  std::vector<char> p_szTemp(strlen(szIn)+1);
  char *szTemp = &p_szTemp[0];

  int  iString;

  if (strcmp(szIn,"")==0 || szIn==NULL) return 1;

  strcpy(szTemp,szIn);
  if (*iMenor==-1)
      sscanf(szIn,"%s%d",szMenor,iMenor);
  else 
  {
    sscanf(szIn,"%s%d",szString,&iString);
    if (strcmp(szString,szMenor)<0)
    {
      strcpy(szMenor,szString);
      *iMenor = iString;
    }
  }

  int cont=0;
  unsigned int i = 0;
  for (; cont<2 && i<strlen(szTemp) ;i++)
  {
    if (szTemp[i]==' ') cont++;
  }

  return buscarMenorEnString(szTemp+i,szMenor,iMenor);
}

/**************************************************/

int ordenarStringPorParejas(const char *szIn, char *szOut, int depth, char *szInicial)
{
  char szMenor[100];
  const char* p;
  szMenor[0] = '\0';
  char szTempMenor[100];
  std::string szTemp;
  int  iMenor = -1;

  // fprintf(stderr,"1 in: %s  out: %s\n",szIn,szOut);
  if (strcmp(szIn,"")==0 || szIn==NULL
      || szInicial==NULL || szIn>(szInicial+strlen(szInicial))) return depth;
  if (depth==0) strcpy(szOut,"");

  buscarMenorEnString(szIn,szMenor,&iMenor);
  sprintf(szTempMenor,"%s %d",szMenor,iMenor);
  p = strstr(szIn,szTempMenor);

  // Copiamos string szIn sin pareja menor
  unsigned int i = 0;
  while (i<strlen(szIn) && p!=NULL)
  {
    if (&szIn[i]<p || &szIn[i]>(p+strlen(szTempMenor)))
      {
         szTemp += szIn[i];
      }
    i++;
  }

  if (strlen(szOut)==0) sprintf(szOut,"%s %d",szMenor,iMenor);
  else  sprintf(szOut,"%s %s %d",szOut,szMenor,iMenor);

  return   ordenarStringPorParejas(szTemp.c_str(),szOut,depth+1,szInicial);
}

/**************************************************/

int obtainMark(FILE *channel,std::string& mark)
{  
    int ret;
    mark = "";
    while (mark.empty()) ret = readTo(channel,'(','\n',mark);
 
    return ret;
}

/**************************************************/

int obtainAtrInt(FILE *channel,int *endAtr)
{
    int i=0;
    char c=' ',num[5]="";

    while ( (!feof(channel)) && (c!='(') && (c!=',') && (c!=')') )
    {
	c=fgetc(channel);
	if ((c!='(') && (c!=')')) num[i]=c;
	i++;
    }
    if (c==')') *endAtr=1;
    num[i]='\0';
    return atoi(num);
}

/**************************************************/


void destroyFeatureList(simpleList<nodo_feature_list*> *fl)
{
  nodo_feature_list *data = 0;

  fl->setFirst();
  for(int i = 0; i < fl->numElements(); i++, fl->next()) {
    data = *fl->getIndex();
    data->l.setFirst();
    for(int j = 0; j < data->l.numElements(); j++, data->l.next()) {
      delete *(data->l.getIndex());
    }
    delete data;
  }
}

/**************************************************/

void createFeatureList(const std::string &name,simpleList<nodo_feature_list*> *featureList)
{
   int *i,endAtr;
//    char c;
   int ret = 1;
   //char temp[100];
   nodo_feature_list *data;

   FILE *f;
    if ((f = fopen(name.c_str(), "rt"))== NULL)
    {
	fprintf(stderr, "Error opening file %s!!",name.c_str());
	exit(0);
    }

    //Insert feature Swn
    data = new nodo_feature_list; 
    data->mark = "Swn";
    featureList->add(data);

    std::string temp;
    ret = obtainMark(f,temp);
    while (ret!=-1)
    {
      data = new nodo_feature_list;
      data->mark = temp;

      endAtr=0;

      while (endAtr==0 && ret!=0)
      {
        i = new int;
        *i = obtainAtrInt(f,&endAtr);
        data->l.add(i);
      }
      featureList->add(data);
      temp.clear();
      ret = obtainMark(f,temp);
    }
    fclose(f);      

}

/**************************************************/

void removeFiles(const std::string &path, int type,int numModel, int direction, int verbose)
{
  char szRemove[200];
  switch (type)
    {
      case RM_TEMP_FILES: 
	if (verbose==TRUE) fprintf(stderr,"\nErasing temporal files.");
	/*
	sprintf(remove,"rm -f %s.M%d*.SVM",path,numModel);
	system(remove);
	sprintf(remove,"rm -f %s*M%d*.POS",path,numModel);
	system(remove);
	sprintf(remove,"rm -f %s*M%d*.SAMPLES",path,numModel);
	system(remove);
	sprintf(remove,"rm -f %s*M%d*.MAP",path,numModel);
	system(remove);
	sprintf(remove,"rm -f %s*DICT.*",path);
	system(remove);
	*/
	sprintf(szRemove,"%s.M%d*.SVM",path.c_str(),numModel);
	remove(szRemove);
	sprintf(szRemove,"%s*M%d*.POS",path.c_str(),numModel);
	remove(szRemove);
	sprintf(szRemove,"%s*M%d*.SAMPLES",path.c_str(),numModel);
	remove(szRemove);
	sprintf(szRemove,"%s*M%d*.MAP",path.c_str(),numModel);
	remove(szRemove);
	sprintf(szRemove,"%s*DICT.*",path.c_str());
	remove(szRemove);
	break;
     case RM_MODEL_FILES:
        if (direction==LEFT_TO_RIGHT || direction==LR_AND_RL)
	  {
	    if (verbose==TRUE) fprintf(stderr,"\nErasing old files for MODEL %d in LEFT TO RIGHT sense.",numModel);
	    //sprintf(szRemove,"rm -f %s*M%d.LR.*",path,numModel);
	    //system(szRemove);
	    sprintf(szRemove,"%s*M%d.LR.*",path.c_str(),numModel);
	    remove(szRemove);
	  }
        if (direction==RIGHT_TO_LEFT || direction==LR_AND_RL)
	  {
	    if (verbose==TRUE) 
	      fprintf(stderr,"\nErasing old files for MODEL %d in RIGHT TO LEFT sense.",numModel);
	    //sprintf(szRemove,"rm -f %s*M%d.RL.*",path,numModel);
	    //system(szRemove);
	    sprintf(szRemove,"%s*M%d.RL.*",path.c_str(),numModel);
	    remove(szRemove);
	  }
	//sprintf(szRemove,"rm -f %s*A%d.*",path,numModel);
	//system(szRemove);
	sprintf(szRemove,"%s*A%d.*",path.c_str(),numModel);
	remove(szRemove);
        break;
    }
}

void Tokenize(const string& str, vector<string>& tokens, const string& delimiters = " ")
{
  // Skip delimiters at beginning.
  string::size_type lastPos = str.find_first_not_of(delimiters, 0);
  // Find first "non-delimiter".
  string::size_type pos     = str.find_first_of(delimiters, lastPos);

  while (string::npos != pos || string::npos != lastPos)
    {
      // Found a token, add it to the vector.
      tokens.push_back(str.substr(lastPos, pos - lastPos));
      // Skip delimiters.  Note the "not_of"
      lastPos = str.find_first_not_of(delimiters, pos);
      // Find next "non-delimiter"
      pos = str.find_first_of(delimiters, lastPos);
    }
}

