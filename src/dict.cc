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

#include "dict.h"

#include "common.h"
#include "er.h"
#include "hash.h"
#include "list.h"
#include "swindow.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
using namespace std;

extern int verbose;

/**************************************************/

infoDict* dictionary::getMFT(dataDict*  w)
{
  if ((long)w==HASH_FAIL) return NULL;

  int max=0;
  bool ret = true;
  simpleList<infoDict*>* l = &this->getElementMaybe(w);
  infoDict *ptr;

  l->setFirst();
  while (ret)
  {
    ptr = *l->getIndex();
    if (max<ptr->num)
    {
      max = ptr->num;
    }
    ret = l->next();
  }
  l->setFirst();
  return ptr;
}

/**************************************************/

std::string dictionary::getAmbiguityClass(dataDict* w)
{
	char *amb = new char[200];
	if ((long)w==HASH_FAIL)
	{
	 sprintf(amb,"UNKNOWN");
	 return amb;
	}

	bool ret = true;
	strcpy(amb,"");
	simpleList<infoDict*> *l = &this->getElementMaybe(w);
	int numMaybe = this->getElementNumMaybe(w);
	infoDict *ptr;

	l->setFirst();
	while (ret)
	{
	  numMaybe--;
  	  ptr = *l->getIndex();
      // fprintf(stderr," %s %d",ptr->pos,ptr->num);
	  if (numMaybe>0) sprintf(amb,"%s%s_",amb,ptr->pos.c_str());
    else sprintf(amb,"%s%s",amb,ptr->pos.c_str());
	  ret = l->next();
	}
	l->setFirst();
	return amb;
}

/**************************************************/

void dictionary::dictIncInfo(dataDict* elem, const std::string& pos)
{
  bool ret=true;
  infoDict *pInfoDict;

  elem->numWrd++;
  while (ret)
  {
    pInfoDict = *elem->maybe.getIndex();
    if (pInfoDict->pos == pos)
    {
      pInfoDict->num++;
      elem->maybe.setFirst();
      return;
    }
    ret=elem->maybe.next();
  }
  pInfoDict = new infoDict;
  pInfoDict->pos = pos;
  pInfoDict->num=1;
  elem->maybe.add(pInfoDict);
  elem->numMaybe++;
  elem->maybe.setFirst();
}

/**************************************************/

void dictionary::dictWrite(const std::string& outName)
{
  infoDict *data;
//   dataDict *aux;
  int cont=0,contWords=0;
  char stringPOS[1000];

  FILE *f = openFile(outName.c_str(),"w");

//   hash_node_t *old_hash, *tmp;
//   int old_size, h, i;

  for (hash_t<dataDict*>::iterator it  = d.begin(); it != d.end(); it++)
  {
    dataDict *aux = (dataDict *) ((*it).second);
    //fprintf(stderr,"\n%s %d %d",aux->wrd,aux->numWrd,aux->numMaybe);
    fprintf(f,"%s %d %d",aux->wrd.c_str(),aux->numWrd,aux->numMaybe);

    cont++;
    contWords = aux->numWrd+contWords;
    bool ret=true;
    strcpy(stringPOS,"");
    while (aux->numMaybe>0 && ret)
    {
      data = *aux->maybe.getIndex();
      //fprintf(stderr," [ %s %d ]",data->pos,data->num);

      if (strlen(stringPOS)==0) sprintf(stringPOS,"%s %d",data->pos.c_str(),data->num);
      else sprintf(stringPOS,"%s %s %d",stringPOS,data->pos.c_str(),data->num);

      ret=aux->maybe.next();
    }

    char *szOut = new char[strlen(stringPOS)+1];
    szOut[strlen(stringPOS)] = '\0';
    ordenarStringPorParejas(stringPOS, szOut, 0, stringPOS);
    fprintf(f," %s\n",szOut);

    delete[] szOut;
  } /* for */

  fclose(f);
  return;
}

/**************************************************/

void dictionary::dictCreate(FILE *f,int limitInf,int limitSup)
{
  int retW=0,retP=0,contWords=0,cont=0,contWordsAdded=0;
  infoDict *data;
  dataDict *aux,*aux2;
//   nodo *elem;
  std::string wrd,pos;
  int no_chunk = FALSE;

  if ( verbose == TRUE ) fprintf(stderr,"\nCreating Dictionary");

  if (limitInf == 0 && limitSup == 0) no_chunk = TRUE;

  while (retP>=0 && retW>=0)
  {
    if ( verbose  == TRUE) showProcessDone(contWordsAdded, 1000, FALSE,"");
    retW = readString(f, wrd);
    std::string real(wrd);
    retP = readString(f, pos);

    //cout << retW << "_" << retP << " :: " << wrd << "_" << pos << " :: (" << real << ")\n";

    if (retW>=0 && retP>=0)
    {
      int erRet=erLookRegExp(wrd);
      switch (erRet)
      {
        case CARD: wrd = "@CARD"; break;
        case CARDSEPS: wrd = "@CARDSEPS"; break;
        case CARDPUNCT: wrd = "@CARDPUNCT"; break;
        case CARDSUFFIX: wrd = "@CARDSUFFIX"; break;
      }

      int is_valid_for_limit_inf =  ( (contWords < limitInf)  || contWords == 0 );
      int is_valid_for_limit_sup =  (contWords >= limitSup );

      if ( no_chunk == TRUE || is_valid_for_limit_inf || is_valid_for_limit_sup )
      {
        if ((long)(aux=d.hash_lookup(wrd)) == HASH_FAIL)
        {
          aux= new dataDict;
          aux->wrd = wrd;
          aux->numMaybe = 1;
          aux->numWrd = 1;
          data = new infoDict;
          data->pos = pos;
          data->num=1;
          aux->maybe.add(data);
          d.hash_insert(aux->wrd,aux);
          cont++;
        }
        else dictIncInfo(aux,pos);
        contWordsAdded++;

        if (wrd =="@CARD" || wrd == "@CARDPUNCT"
          || wrd == "@CARDSEPS" ||  wrd == "@CARDSUFFIX")
        {
          if ((long)(aux2=d.hash_lookup(real)) == HASH_FAIL)
          {
            aux2 = new dataDict;
            aux2->wrd = real;
            aux2->numMaybe = 1;
            aux2->numWrd = 1;
            data = new infoDict;
            data->pos = pos;
            data->num = 1;
            aux2->maybe.add(data);
            d.hash_insert(aux2->wrd,aux2);
            cont++;
          }
          else dictIncInfo(aux2,pos);
        }
      }
    }
    contWords++;
  }

   if ( verbose == TRUE ) fprintf(stderr,"[ %d words ]",cont);
}

/**************************************************/

void dictionary::dictRepairFromFile(const std::string& fileName)
{
  if ( verbose == TRUE ) fprintf(stderr,"\nReparing Dictionary with file < %s >",fileName.c_str());
  FILE *f = openFile(fileName.c_str(),"r");

  char wrd[250],pos[10];
  int numWrd,numMaybe,numWrdxPOS;
  dataDict *aux;


  // Bucle para leer lista de palabras
  while (!feof(f))
  {
    fscanf(f,"%s %d %d",wrd,&numWrd,&numMaybe);
    dataDict* w = d.hash_lookup(wrd);
    if ((long)w!=HASH_FAIL)
    {
      aux = new dataDict;
      aux->wrd = wrd;
      aux->numWrd = getElementNumWord(w);
      aux->numMaybe = 0;

      simpleList<infoDict*> *l = &getElementMaybe(w);

      for (int i=0;i<numMaybe;i++)
      {
        fscanf(f,"%s %d",pos,&numWrdxPOS);
        bool ret=true;

        while (ret)
        {
          infoDict *ptr = *l->getIndex();
          if (pos == ptr->pos)
          {
            //Copiamos  elemento a añadir
            infoDict *tmpInfoDict = new infoDict;
            tmpInfoDict->pos = ptr->pos;
            tmpInfoDict->num = ptr->num;

            aux->maybe.add(tmpInfoDict);
            aux->numMaybe++;
            ret = false;
            delete ptr; //Borrar substituido
          }
          else ret = l->next();
        }
        l->setFirst();
      }

      delete d.hash_delete(wrd);
      d.hash_insert(aux->wrd,aux);
    }
  }
  fclose(f);
}

/**************************************************/

void dictionary::dictRepairHeuristic(float dratio)
{
  for (hash_t<dataDict*>::iterator it  = d.begin(); it != d.end(); it++)
  {
    dataDict *dd = (dataDict *)((*it).second);

    bool ret=true;

    simpleList<infoDict*> *l = &dd->maybe;
    l->setFirst();
    int iNumWrdsAfterDelete = dd->numWrd;
    while (ret)
    {
      infoDict *ptr = *l->getIndex();

      float fRange = (float) ptr->num / (float) dd->numWrd ;

      if (fRange < dratio)
      {
        dd->numMaybe--;
        iNumWrdsAfterDelete =  iNumWrdsAfterDelete - ptr->num;
        l->delIndex(); //Eliminar pos
      }
      ret = l->next();
    }
    dd->numWrd =  iNumWrdsAfterDelete; 
    l->setFirst();
  }
}

/**************************************************/

int dictionary::readInt(FILE *in)
{	int i=0;
 char value[10];
 char c=' ';

 strcpy(value,"");

 while ((c==' ') && (!feof(in))) c=fgetc(in);
 while ((i<10) && (c!=' ') && (c!='\n') && (!feof(in)))
   {
     sprintf(value,"%s%c",value,c); 
     c=fgetc(in); i++;
   }
 return atoi(value);
}

/**************************************************/

infoDict *dictionary::readData(FILE *in)
{
  infoDict *data = new infoDict;
  char c=fgetc(in);
  int i = 0;

  data->pos = "";

  while ( (i<TAMTXT) && (c!=' ' && c!='\n' && c!='\t') && (!feof(in)) )
    {
      data->pos += c;
      c=fgetc(in);
      i++;
    }
  data->num  = readInt(in);
  return data;
}

/**************************************************/

void dictionary::dictAddBackup(const std::string& name)
{
  FILE *f = openFile(name,"r");
  
//   char wrd[250],pos[10];
  int i;
  dataDict *aux;
  infoDict *data;
  
  // Loop to read list of words
  while (!feof(f))
  {
    data = readData(f);
    i = readInt(f);
    long w = (long)d.hash_lookup(data->pos);
    if (w==HASH_FAIL)
    {
      aux = new dataDict;
      aux->wrd = data->pos;
      aux->numWrd = 0;
      aux->numMaybe = 0;
    }
    else  aux = (dataDict *) w;

    aux->numWrd += data->num;
    delete data;
    while (i>0)
    {
      data = readData(f);
      bool ret=true;
      //If not found add it to the list
      for (int j=aux->numMaybe;ret>=0 &&  j>0; j--)
      {
        infoDict *element = *aux->maybe.getIndex();
        if (data->pos == element->pos)
        {
          ret = false;
          element->num += data->num;
        }
        else ret = aux->maybe.next();
      }
      //If not found add it to the list
      if (ret)
      {
        aux->maybe.add(data);
        aux->numMaybe++;
      }
      else delete data;
      i--;
    }
    if (w==HASH_FAIL) d.hash_insert(aux->wrd,aux);

  } //End while not eof
  fclose(f);
}

void dictionary::addBackupEntry(const std::string& token, const std::set<std::string>& tags)
{
  dataDict* aux = d.hash_lookup(token);
  if ((long)aux==HASH_FAIL)
  {
    aux = new dataDict;
    aux->wrd = token;
    aux->numWrd = 1;
    aux->numMaybe = 0;
    d.hash_insert(aux->wrd,aux);
  }

  for (std::set<std::string>::const_iterator it = tags.begin(); it != tags.end(); it++)
  {
    infoDict* data = new infoDict();
    data->pos = *it;
    data->num = 0;
    bool ret=true;
    aux->maybe.setFirst();
    //If not found add it to the list
    for (int j=aux->numMaybe; j>0; j--)
    {
      infoDict *element = *aux->maybe.getIndex();
      if (data->pos == element->pos)
      {
        ret = false;
        // increase the number of occurences
        element->num += data->num;
      }
      aux->maybe.next();
    }
    aux->maybe.setFirst();
    
    //If not found add it to the list
    if (ret)
    {
      aux->maybe.add(data);
      aux->numMaybe++;
      aux->maybe.setFirst();
    }
    else delete data;
  }
}

/**************************************************/

void dictionary::dictLoad(FILE *in)
{
  while (!feof(in))
  {
    infoDict *data = readData(in);
    int i = readInt(in);
    dataDict *aux = new dataDict;

    aux->wrd = data->pos;
    aux->numWrd = data->num;
    aux->numMaybe = i;
    delete data;

    // If we read a line with numMaybe = 0, ie without any label, do not load
    if ( aux->numMaybe > 0)
    {
      while (i>0)
      {
        data = readData(in);
        aux->maybe.add(data);

        i--;
      }
      d.hash_insert(aux->wrd,aux);
    }
    else delete aux;
  }
}

/**************************************************/

dataDict* dictionary::getElement(const std::string& key)
{
  return d.hash_lookup(key);
}

/**************************************************/

std::string& dictionary::getElementWord(dataDict* ptr)
{
  return ptr->wrd;
}

/**************************************************/

int dictionary::getElementNumWord(dataDict* ptr)
{
  return ptr->numWrd;
}

/**************************************************/

int dictionary::getElementNumMaybe(dataDict* ptr)
{
  return ptr->numMaybe;
}

/**************************************************/

simpleList<infoDict*>& dictionary::getElementMaybe(dataDict* ptr)
{
  return ptr->maybe;
}

/**************************************************/

dictionary::dictionary(const std::string& name,const std::string& backup)
{
  FILE *in = openFile(name.c_str(),"r");
  d.hash_init(1000);
  dictLoad(in);
  fclose(in);
  dictAddBackup(backup);
}

/**************************************************/

dictionary::dictionary(const std::string& name)
{
  FILE *in = openFile(name.c_str(),"r");
  d.hash_init(1000);
  dictLoad(in);
  fclose(in);
}

/**************************************************/

dictionary::dictionary(const std::string& name,int limInf, int limSup)
{
  FILE *in = openFile(name.c_str(),"r");
  //char str[200];
  d.hash_init(1000);
  dictCreate(in,limInf,limSup);
  fclose(in);
}

/**************************************************/

void dictionary::dictCleanListInfoDict(simpleList<infoDict *>* l, int num)
{
  infoDict *data = NULL;

	  l->setFirst();
  	  if ( num >= 1 ) //Si tiene mas de un maybe es ambigua
	  {
		bool ret  = true;
		while (ret)
		{
		  data = *l->getIndex();
		  delete data;
		  ret=l->next();
		}
		l->setFirst();
	  } /* if */
}

/**************************************************/

dictionary::~dictionary()
{
  //Recorrer cada entrada del diccionario eliminando el contenido de las listas
  for (hash_t<dataDict*>::iterator it  = d.begin(); it != d.end(); it++)
  {
    dataDict *aux = (*it).second;
    (*it).second = 0;
    dictCleanListInfoDict(&aux->maybe,aux->numMaybe);
    delete aux;
  } /* for */

  //Destruir hashing
  d.clear();
}

/**************************************************/

hash_t<infoDict*> *dictionary::dictFindAmbP(int *numPOS)
{
  infoDict *data;
//   dataDict *aux;

  hash_t<infoDict*> *ambp = new hash_t<infoDict*>();
  ambp->hash_init(30);


  for (hash_t<dataDict*>::iterator it  = d.begin(); it != d.end(); it++)
  {
    dataDict *aux = (*it).second;
  
//     hash_node_t *old_hash, *tmp;
//     int old_size, h, i;

    *numPOS = 0;

    aux->maybe.setFirst();
    if (aux->numMaybe>1) //Si tiene mas de un maybe es ambigua
    {
      bool ret  = true;
      while (ret)
      {
        data = *aux->maybe.getIndex();
        infoDict * tmp = new infoDict;
        tmp->pos = data->pos;
        tmp->num = data->num;
        ambp->hash_insert(tmp->pos,tmp);

        *numPOS++;
        ret=aux->maybe.next();
      }
      aux->maybe.setFirst();
    } /* if */
  } /* for */
  return ambp;
}

/**************************************************/

hash_t<infoDict*> *dictionary::dictFindUnkP(int *numPOS)
{
  infoDict *data;
//   dataDict *aux;

  hash_t<infoDict*> *unkp = new hash_t<infoDict*>();
  unkp->hash_init(30);


  for (hash_t<dataDict*>::iterator it  = d.begin(); it != d.end(); it++)
  {
    dataDict *aux = (*it).second;
  
    *numPOS = 0;

    aux->maybe.setFirst();
    if (aux->numWrd==1) //Si solo aparece una vez desconocida
    {
      bool ret  = true;
      while (ret)
      {
        data = *aux->maybe.getIndex();
        infoDict * tmp = new infoDict;
        tmp->pos = data->pos;
        tmp->num = data->num;
        unkp->hash_insert(tmp->pos,tmp);
        *numPOS++;
        ret=aux->maybe.next();
      }
      aux->maybe.setFirst();
    } /* if */
  } /* for */
  return unkp;
}

