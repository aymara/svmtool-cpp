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

#include "swindow.h"

#include "hash.h"
#include "nodo.h"
#include "list.h"
#include "dict.h"
#include "weight.h"
#include "er.h"
#include "common.h"
#include "marks.h"
#include <sys/types.h>
#include <cassert>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <limits>
#include <set>

#define MAX_SENTENCE_LENGTH 1000
#define MAX_LINE_LENGTH 4096

/*****************************************************************
 * Feature Generation
 *****************************************************************/

void swindow::winPushStartWithLowerFeature(const std::string& wrd, std::stack<std::string>&pila)
{
  int startlower=0;

  //Comienza por Minuscula?
  if (erLookRegExp2(&erStartLower,wrd)) 
    { 
      startlower = 1;
  
      std::ostringstream feat;
      feat << START_LOWER_MARK<<":"<<startlower;

      pila.push(feat.str());
    }
}

void swindow::winPushStartWithNumberFeature(const std::string& wrd, std::stack<std::string>&pila)
{
  int startnumber=0;

  //Comienza por Numero?
  if (erLookRegExp2(&erStartNumber,wrd)) 
  {
    startnumber = 1;

    //mod Correcting dynamic memory errors
    std::ostringstream feat;
    feat << START_NUMBER_MARK<<":" << startnumber;

    pila.push(feat.str());
  }
}

void swindow::winPushSuffixFeature(const std::string& wrd, std::stack<std::string>& pila,int longitud)
{
  //Obtenemos la longitud de la palabra
   
  std::ostringstream feat;
  int len = wrd.size();
  std::ostringstream suf;
  //int a=0;

  for (int i=len-longitud;  i<=len-1; i++)
  {
    if (i>=0) suf<<wrd[i];
    else suf<<"~";

  }
    
  feat<<SUFFIX_MARK<<longitud<<":"<<suf.str();
  pila.push(feat.str());
}

/*
 * void winPushPreffixFeatures ( std::string wrd, struct std::stack *pila, int longitud)
 * esta funcion creara las "features" para la palabra desconocida
 * <wrd> y las apilara en en el parametro <pila>
 */
void swindow::winPushPrefixFeature(const std::string& wrd, std::stack<std::string>& pila, int longitud)
{
  //Obtenemos la longitud de la palabra
  std::ostringstream feat;
  int len = wrd.size();
  std::ostringstream pref;

  for (int i=0; i<longitud; i++)
    {
      if (len > i) pref<<wrd[i];
      else /*if (i > len-1  )*/  pref<<"~";
    }

  feat<<PREFIX_MARK<<longitud<<":"<<pref.str();
  pila.push(feat.str());
}


void swindow::winPushStartWithCapFeature(const std::string& wrd, std::stack<std::string>&pila)
{
  int startcap=0;

  //Comienza por Mayuscula?
  if (erLookRegExp2(&erStartCap,wrd)) 
    {
      startcap = 1;
      //mod Correcting dynamic memory errors
      //std::string feat = new char[strlen(START_CAPITAL_MARK)+4];
      std::ostringstream feat;

      feat << START_CAPITAL_MARK<<":"<<startcap;
      pila.push(feat.str());
    }
}

void swindow::winPushAllUpFeature(const std::string& wrd, std::stack<std::string>& pila)
{
  int allup=0;

  //Esta toda la palabra en mayusculas?
  if (erLookRegExp2(&erAllUp,wrd)) 
    {
      allup = 1;

      //mod Correcting dynamic memory errors
      //std::string feat = new char[4];
      std::ostringstream feat;

      feat << ALL_UPPER_MARK<<":"<<allup;
      pila.push(feat.str());
    }
}

void swindow::winPushAllLowFeature(const std::string& wrd, std::stack<std::string>& pila)
{
  int alllow = 0;
  //Esta toda la palabra en minusculas?
  if (erLookRegExp2(&erAllLow,wrd)) 
    {
      alllow = 1;

      //mod Correcting dynamic memory errors
      //std::string feat = new char[4];
      std::ostringstream feat;

      feat << ALL_LOWER_MARK<<":"<<alllow;
      pila.push(feat.str());
    }
}

void swindow::winPushContainCapFeature(const std::string& wrd, std::stack<std::string>& pila)
{
  int containcap = 0;
  if (erLookRegExp2(&erContainCap,wrd)) 
    { 
      containcap = 1;

      //mod Correcting dynamic memory errors
      //std::string feat = new char[4];
      std::ostringstream feat;

      feat << CONTAIN_CAP_MARK<<":"<<containcap;
      pila.push(feat.str());
    }
}

void swindow::winPushContainCapsFeature(const std::string& wrd, std::stack<std::string>& pila)
{
  int containcaps = 0;
  if (erLookRegExp2(&erContainCaps,wrd)) 
    {
      containcaps = 1;

      //mod Correcting dynamic memory errors
      //std::string feat;
      std::ostringstream feat;

      feat << CONTAIN_CAPS_MARK <<":"<<containcaps;
      pila.push(feat.str());
    }
}

void swindow::winPushContainPeriodFeature(const std::string& wrd, std::stack<std::string>& pila)
{
  int containperiod = 0;
  //Contiene un punto?
  if (erLookRegExp2(&erContainPeriod,wrd)) 
    {
      containperiod = 1;
      std::stringstream feat;
      feat << CONTAIN_PERIOD_MARK<<":"<<containperiod;
      pila.push(feat.str());
    }
}

void swindow::winPushContainCommaFeature(const std::string& wrd, std::stack<std::string>& pila)
{
  int containcomma = 0;
  //Contiene un punto?
  if (erLookRegExp2(&erContainComma,wrd)) 
    {
      containcomma = 1;
      std::ostringstream feat;
      feat << CONTAIN_COMMA_MARK<<":"<<containcomma;
      pila.push(feat.str());
    }
}

void swindow::winPushContainNumFeature(const std::string& wrd, std::stack<std::string>& pila)
{
  int containnum = 0;
  //Contiene un numero?
  if (erLookRegExp2(&erContainNum,wrd)) 
    {  
      containnum = 1;
      
      std::ostringstream feat;
      //mod
      //sprintf(feat,"CN:%d",containnum);
      feat << CONTAIN_NUMBER_MARK<<":"<<containnum;
      pila.push(feat.str());
    }
}

void swindow::winPushMultiwordFeature(const std::string& wrd, std::stack<std::string>& pila)
{
  int multiword = 0;
  //Es una palabra multiple?
  if (erLookRegExp2(&erMultiWord,wrd)) 
    {
      multiword = 1;

      //mod Correcting dynamic memory errors
      //std::string feat = new char[6];
      //sprintf(feat,"MW:%d",multiword);
      std::ostringstream feat;
      feat << MULTIWORD_MARK<<":"<<multiword;

      pila.push(feat.str());
    }
}

void swindow::winPushLetterFeature(const std::string& wrd, std::stack<std::string>&pila, int where, int position)
{
  std::ostringstream feature;
    
  if (COUNTING_FROM_END==where) 
    {
      feature<<CHAR_Z_MARK << position<<":"<<wrd[wrd.size()-position];
    }
  else 
    {
      feature<<CHAR_A_MARK<<position<<":"<<wrd[position-1];
    }

  pila.push(feature.str());
}

void swindow::winPushLenghtFeature(const std::string& wrd, std::stack<std::string>& pila)
{
  //Obtenemos la longitud de la palabra
  int len = wrd.size();

  //Longitud de la palabra
  //mod Correcting dynamic memory errors
  //std::string feat = new char[4];
  std::ostringstream feat;

  feat <<LENGTH_MARK<< ":"<<len;
  pila.push(feat.str());
}

/*
 * void winPushSwnFeature (struct std::stack *pila)
 * Recibe como parametro <pila>, donde se apilara la "feature"
 * Swn.Swn es el elemento final de frase que puede ser
 * ! ? o .
 */
void swindow::winPushSwnFeature(std::stack<std::string>& pila)
{
  std::string feature = "Swn:";
  feature += ".";
  pila.push(feature);
}


/*
 * void winPushAmbiguityFeature(void *ptr, dictionary *d, std::stack *pila, int direction)
 * Genera el atributo que representa la ambiguedad de una palabra.
 * Recibe como parametros:
 *      ptr, que es un puntero a un nodo de la lista de atributos (nodo_feature_list)
 *           aunque se recibe como un void*.
 *      d,   es el diccionario con el que estamos trabajarando
 *      pila,es la pila donde apilaremos el atributo generado
 *      direction, es la direccion en que estamos recorriendo el corpus (LEFT_TO_RIGHT
 *           o RIGHT_TO_LEFT).
 */
void swindow::winPushAmbiguityFeature(void* ptr, dictionary* d, std::stack<std::string>& pila, int direction)
{
  std::ostringstream value;
  nodo_feature_list *p = (nodo_feature_list *)ptr;
  nodo *pn;
  int *num;
  infoDict *pInfoDict;


  num = *p->l.getIndex();
  value << p->mark << *num << ":";
  pn = get(*num, direction);
  if (pn!=NULL)
  {
    dataDict* w = d->getElement(pn->wrd);
    if ((long)w!=HASH_FAIL)
    {
      simpleList<infoDict*>& list = d->getElementMaybe(w);
      int numMaybe = d->getElementNumMaybe(w);
      bool ret = true;
      while (ret)
      {
        pInfoDict = *list.getIndex();
        numMaybe--;
        if (numMaybe>0) value << pInfoDict->pos << "~";
        else value << pInfoDict->pos;
        ret=list.next();
      }
      list.setFirst();
    }
    else value << "UNKNOWN"; //is unknown word
  }
  else value << EMPTY_POS;

  pila.push(value.str());
}


/*
 * void winPushMFTFeature(void *ptr, dictionary *d, std::stack *pila, int direction)
 * Genera el atributo con la "Most Frequent Tag", la etiqueta mas frecuente.
 * Recibe como parametros:
 *      ptr, que es un puntero a un nodo de la lista de atributos (nodo_feature_list)
 *           aunque se recibe como un void*.
 *      d,   es el diccionario con el que estamos trabajarando
 *      pila,es la pila donde apilaremos el atributo generado
 *      direction, es la direccion en que estamos recorriendo el corpus (LEFT_TO_RIGHT
 *           o RIGHT_TO_LEFT).
 */
void swindow::winPushMFTFeature(void* ptr, dictionary* d, std::stack<std::string>& pila, int direction)
{
  std::string value;
  std::string mft;
  nodo_feature_list *p = (nodo_feature_list *)ptr;
  nodo *pn;
  int *num,max=0;
  infoDict *pInfoDict;

  num = *p->l.getIndex();
  value = p->mark;
  value += *num + ":";
  pn = get(*num, direction);
  if (pn!=NULL)
  {
    dataDict* w = d->getElement(pn->wrd);
    if ((long)w!=HASH_FAIL)
    {
      simpleList<infoDict*>& list = d->getElementMaybe(w);
      int numMaybe = d->getElementNumMaybe(w);
      bool ret = true;
      while (ret)
      {
        pInfoDict = *list.getIndex();
        numMaybe--;
        if (pInfoDict->num>max) mft = pInfoDict->pos;
        ret=list.next();
      }
      list.setFirst();
      value += mft;
    }
    else value += "UNKNOWN"; //is unknown word
  }
  else value += EMPTY_POS;
  
  pila.push(value);
}


/*
 * void winPushMaybeFeature(void *ptr, dictionary *d, std::stack *pila, int direction)
 * Genera tantos atributos "maybe" como posibles POS pueda tener la palabra, y los
 * apila en <pila>.
 * Recibe como parametros:
 *      ptr, que es un puntero a un nodo de la lista de atributos (nodo_feature_list)
 *           aunque se recibe como un void*.
 *      d,   es el diccionario con el que estamos trabajarando
 *      pila,es la pila donde apilaremos el atributo generado
 *      direction, es la direccion en que estamos recorriendo el corpus (LEFT_TO_RIGHT
 *           o RIGHT_TO_LEFT).
 */
void swindow::winPushMaybeFeature(void* ptr, dictionary* d, std::stack<std::string>& pila, int direction)
{
  std::string value;
  std::ostringstream txt;
  nodo_feature_list *p = (nodo_feature_list *)ptr;
  nodo *pn;
  int *num;
  infoDict *pInfoDict;

  num = *p->l.getIndex();
  txt << p->mark << *num << "~";
  pn = get(*num, direction);
  if (pn!=NULL)
  {
    dataDict* w = d->getElement(pn->wrd);

    if ((long)w!=HASH_FAIL)
    {
      simpleList<infoDict*>& list = d->getElementMaybe(w);
      bool ret = true;
      while (ret)
      {
        std::string feature;
        pInfoDict = *list.getIndex();
        feature  += txt.str() + pInfoDict->pos + ":1";
        pila.push(feature);
        ret=list.next();
      }
      list.setFirst();
    }
    else
    {
      std::string feature;
      feature  += txt.str() + "UNKNOWN:1";
      pila.push(feature);
    }
  }
  else
  {
    std::string feature;
    feature += txt.str() + EMPTY_POS + ":1";
    pila.push(feature);
  }
}


/*
 * void winPushPosFeature(void *ptr, dictionary *d, std::stack *pila, int direction)
 * Genera un atributo con la POS de algunos elementos de la ventana.
 * Recibe como parametros:
 *      ptr, que es un puntero a un nodo de la lista de atributos (nodo_feature_list)
 *           aunque se recibe como un void*.
 *      d,   es el diccionario con el que estamos trabajarando
 *      pila,es la pila donde apilaremos el atributo generado
 *      direction, es la direccion en que estamos recorriendo el corpus (LEFT_TO_RIGHT
 *           o RIGHT_TO_LEFT).
 */
void swindow::winPushPosFeature(void* ptr, dictionary* d, std::stack<std::string>& pila, int direction)
{
  std::string value;
  std::string txt;
  nodo_feature_list *p = (nodo_feature_list *)ptr;
  nodo *pn;
  infoDict *pInfoDict;
  std::string feature;

  int *num;

  bool end = false;
  while (!end)
  {
    num = *p->l.getIndex();

    pn = get(*num, direction);

    if (pn==NULL) txt = EMPTY_POS;
    else if ( (pn->pos == EMPTY) || (*num==0) )  //AKI3
    {

      dataDict* w = d->getElement(pn->wrd);

      if ((long)w!=HASH_FAIL)
      {
        simpleList<infoDict*>& list = d->getElementMaybe(w);
        int numMaybe = d->getElementNumMaybe(w);

        txt = EMPTY;
        bool ret = true;
        while ( ret )
        {
          pInfoDict = *list.getIndex();
          numMaybe--;
          if (numMaybe>0) txt += pInfoDict->pos + "_";
          else txt += pInfoDict->pos;
          ret=list.next();
        }
        list.setFirst();
      }
      else txt = "UNKNOWN"; //is unknown word
    }
    else txt = pn->pos; //AKI3

    if (value.empty()) value = txt; //AKI3
    else value += "~" + txt;

    end = !p->l.next();
  }
  p->l.setFirst();

  feature = ":" + value;
  //std::cerr << feature << std::endl;
  pila.push(feature);
}

/*
 * void winPushPOSFeature(void *ptr, dictionary *d, std::stack *pila, int direction)
 * Genera un atributo con la palabra de algunos elementos de la ventana.
 * Recibe como parametros:
 *      ptr, que es un puntero a un nodo de la lista de atributos (nodo_feature_list)
 *           aunque se recibe como un void*.
 *      d,   es el diccionario con el que estamos trabajarando
 *      pila,es la pila donde apilaremos el atributo generado
 *      direction, es la direccion en que estamos recorriendo el corpus (LEFT_TO_RIGHT
 *           o RIGHT_TO_LEFT).
 */
void swindow::winPushWordFeature(void* ptr, dictionary* /*d*/, std::stack<std::string>& pila, int direction)
{
  std::string value;
  std::ostringstream name;
  std::string txt;
  nodo_feature_list *p = (nodo_feature_list *)ptr;
  nodo *pn=NULL;

  int *num = *p->l.getIndex();
  pn = get(*num, direction);

  if (pn==NULL) value = EMPTY_WORD;
  else value = pn->wrd;
  name << std::string(p->mark) << *num;

  while (p->l.next())
  {
    num = *p->l.getIndex();
    name << "," << *num;

    pn = get(*num, direction);

    if (pn==NULL) txt = EMPTY_WORD;
    else txt  = pn->wrd;
    value += "~" + txt;
  }
  p->l.setFirst();
  name << ":" << value;

  pila.push(name.str());
}


/****************************************************************************/


int swindow::sentenceLength()
{
  //Retorna el número de palabras que tiene la frase cargada en este objeto
  return this->numObj;
}

/*
 * void deleteList()
 * Elimina todas las palabras existentes en la ventana
 * Retorna el número de elementos que poseia la ventana
 */
void swindow::deleteList()
{
//   std::cerr << "swindow::deleteList " << numObj << " elements" << std::endl;
  if (first==0) return;

  int i = 0;
  while (first->next!=0)
  {
    i++;
//     std::cerr << "swindow::deleteList delete " << i << "th element: " << &(first->strScores) << " " << first->strScores << std::endl;
    assert(first->next->previous == first);
    first = first->next;
    delete first->previous;
  }
  i++;
  if ( last != 0 )
  {
//     std::cerr << "swindow::deleteList delete " << i << "th element" << std::endl;
    delete last;
  }

  first=0;
  last=0;
  index=0;
  numObj=0;
}


void swindow::init(dictionary* dic)
{
  if(m_output != NULL)
    iniGeneric(dic);
}

int swindow::iniGeneric(dictionary* dic)
{
  posBegin = posIndex;
  posEnd = posIndex;
  
  int ret = iniList(dic);
  endWin = last;
  if (ret>=0) readSentence(dic);

  if (ret==-1) return -1;
  else if (ret==0) posEnd = posIndex+last->ord;
  else posEnd=posIndex+ret;

  beginWin = first;

  return ret;
}

int swindow::iniList(dictionary* dic)
{
  int j=0,ret=1;

  for(j=posIndex; ((j<lengthWin) && (ret>=0)); j++) ret = readInput(dic);

  /*
   * ret codes:
   *   - 0 normal sentence
   *   - -1 end of sentence
   *   - -2 end of file
   */
  if (ret>=0) ret=j-posIndex-1;
	
  return ret;
}

void swindow::setWindow(const std::vector<nodo*>& user_window) {
  this->user_window = user_window;
}


/****************************************************************************/

int swindow::readSentence(dictionary* dic)
{
  int ret=1;
  while (ret>=0) ret = readInput(dic);
  return ret;
}


int swindow::readInput(dictionary* dic) {
  std::string word, comment;
  std::set<std::string> tagset;

  int ret;
  while((ret = m_reader.parseWord(word, tagset, comment)) == 1);
  nodo* node = m_reader.buildNode(word, comment);

  winAdd(node);
  if(!tagset.empty()) dic->addBackupEntry(word, tagset);

  return ret;
}


/****************************************************************************/

void swindow::winAdd(nodo *aux)
{
  // add the node in the software window
  if(numObj == 0)
  {
    first = aux;
    last = aux;
    index = aux;
  }
  else
  {
    aux->previous = last;
    last->next = aux;
    last = aux;
  }

  numObj++;
}


/****************************************************************************/

swindow::~swindow()
{
//   deleteList();
}

swindow::swindow(istream& in, std::ostream* output, dictionary* dic) : m_output(output), m_reader(in),
first(0), last(0), numObj(0),index(0),beginWin(0),endWin(0),posBegin(0),posEnd(0)
{
  lengthWin = 7;
  posIndex = 3;

  init(dic);
}

swindow::swindow(int lengthWin, dictionary *dic): m_output(NULL), m_reader(),
first(0), last(0), numObj(0), beginWin(0), endWin(0), posBegin(0), posEnd(lengthWin)
{
  this->lengthWin = lengthWin;
  posIndex = 2;
  init(dic);
}

swindow::swindow(istream& in, int number, int position, std::ostream* output, dictionary* dic) : m_output(output), m_reader(in),
first(0), last(0), numObj(0),index(0),beginWin(0),endWin(0),posBegin(0),posEnd(0)
{
  
  if ((number<3) || (number<=position))
    { fprintf(stderr,"\nWindow Length can not be first or last element.\nLength should be greater than \"Interest Point Position\" or 3.\n");
    exit(0);
    }

  lengthWin = number;
  posIndex = position-1;

  init(dic);
}

swindow::swindow(istream& in, int number, std::ostream* output, dictionary* dic) : m_output(output), m_reader(in), 
first(0), last(0), numObj(0),index(0),beginWin(0),endWin(0),posBegin(0),posEnd(0)
{

  lengthWin = number;
  posIndex = number/2;
     
  init(dic);
}

/****************************************************************************/

/* Move Interest Point to next element */
bool swindow::next()
{
  bool ret = false;
  if (endWin->next!=0) ret=true;

  if  ((index==0) || (index->next==0)) return false;
  if  ((posIndex>=posEnd) && (!ret)) return false;

  if  ((posIndex<posEnd) && (!ret)) posEnd--;
  if ((posEnd==lengthWin-1) && (ret)) endWin = endWin->next;

  if (posBegin==0) beginWin = beginWin->next;
  else if  ((posIndex>=posBegin) && (posBegin>0)) posBegin--;

  index = index->next;
  return true;
}

/****************************************************************************/

/* Move Interest Point to previous element */
bool swindow::previous()
{
  if ((index==NULL) || (index->previous==NULL)) return false;

  if  ((posBegin==0) && (beginWin->previous!=NULL)) beginWin = beginWin->previous;
  else if  (posIndex>posBegin) posBegin++;

  if  (posEnd<lengthWin-1) posEnd++;
  else endWin = endWin->previous;

  index = index->previous;
  return true;
}

/****************************************************************************/

/* Get Interest Point */
nodo *swindow::getIndex()
{
  return index;
}

/****************************************************************************/

nodo *swindow::get(int position, int direction)
{
  if (direction==2) position = -position;
  if ( ((position<0) && (posIndex+position+1<posBegin))
       || ((position>0) && (posIndex+position>posEnd)) )
    return NULL;

  if(!user_window.empty())
    return get_user(position);
  else
    return get_intern(position);
}

// position must be valid, ie. "in the window"
nodo *swindow::get_user(int position)
{
  return user_window[position+2];
}

nodo *swindow::get_intern(int position)
{
  nodo *aux=index;
  int i=0;

  if (position == 0) return index;

  while (i!=position)
  {
    if (position>0)
    { i++;
      if (aux->next != NULL) aux = aux->next;
      else 	return NULL;
    }
    else
    { i--;
      if (aux->previous != NULL) aux = aux->previous;
      else return NULL;
    }
  }

  return aux;
}


/****************************************************************************/

int swindow::show(int showScoresFlag, int showComments)
{
  std::string wrd;

  if (first==NULL) return 0;

  nodo *actual = first;

  while(actual != NULL) {
    *m_output << actual->realWrd << " " << actual->pos;

    if ( showScoresFlag == TRUE && !actual->strScores.empty() ) {
      *m_output << " " << actual->strScores;
    }

    if ( showComments == TRUE && !actual->comment.empty() ) {
      *m_output << " " << actual->comment;
    }

    *m_output << std::endl;

    actual = actual->next;
  }

  return 0;
}

/****************************************************************************/

void swindow::putLengthWin(int l)
{
  lengthWin = l;
}

/****************************************************************************/

void swindow::putIndex(int i)
{
  posIndex = i;
}

/****************************************************************************/

/*
 * Modifica el valor de los pesos para una palabra
 * Si:
 * action = 0 --> Pone el peso máximo (put max score)
 * action = 1 --> Inicializa los pesos (reset values)
 * action = 2 --> Restaura el valor de la vuelta anterior(last lap value)
 */
int swindow::winMaterializePOSValues(int action)
{
  if (first==NULL) return 0;

  int inicio=1;
  weight_node_t *w,max;
  nodo *actual=first;

  while (actual!=NULL)
    {

      switch (action)
	{
	case 0: //PUT MAX
	  inicio = 1;
    while(!actual->stackScores.empty())
    {
      w = actual->stackScores.top();
      actual->stackScores.pop();

      if (inicio || w->data>max.data)
      {
        max.data=w->data;
        max.pos = w->pos;
        inicio = 0;
      }
      delete w;
    }
	  actual->weight=max.data;
	  actual->pos = max.pos;
	  //Added for 2 laps tagging
	  actual->weightOld=max.data;
	  actual->posOld = max.pos;
	  break;
	case 1: //RESET VALUES
	  actual->pos = "";
	  actual->weight=0;
	  break;
	case 2: //PUT OLD
	  actual->pos = actual->posOld;
	  actual->weight=actual->weightOld;
	  break;
	}
      actual=actual->next;
    }
  return 0;
}

/****************************************************************************/

/*
 * int winExistUnkWord(int direction, dictionary *d)
 * Esta funcion comprueba si hay parabras desconocidas.
 * En caso de que el parametro direction sea:
 *   LEFT_TO_RIGHT - mira si hay desconocidas a la
 *                   derecha del punto de interes de la ventana.
 *   RIGHT_TO_LEFT - mira si hay desconocidas a la izquierda
 *                   del punto de interes de la ventana.
 * Esta funcion devuelve:
 *   un entero >=0, si no hay desconocidas
 *              -1, si hay desconocidas
 */
int swindow::winExistUnkWord(int direction, dictionary *d)
{
  nodo *aux=index;
  int ret=0,i=posIndex;

  if (index==NULL) return 1;
  aux = index;

  while (ret>=0)
    {
      switch (direction)
	{
	case LEFT_TO_RIGHT:
	  if (aux->next==NULL || aux==endWin) ret=-1;
	  else aux = aux->next;
	  if ((long)d->getElement(aux->wrd)==HASH_FAIL)	return -1;
	  i++;
	  break;
	case RIGHT_TO_LEFT:
	  if (aux->previous==NULL || aux==beginWin) ret=-1;
	  else aux = aux->previous;
	  if ((long)d->getElement(aux->wrd)==HASH_FAIL) return -1;
	  i--;
	  break;
	}
    }
  return 0;
}

