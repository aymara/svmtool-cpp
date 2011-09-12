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

#ifndef SWINDOW_H
#define SWINDOW_H

#include "common.h"
#include "dict.h"
#include "reader.h"
#include <iostream>
#include <stack>
#include <iosfwd>

#define CHAR_NULL	'~'
#define EMPTY_WORD	"_"
#define EMPTY_POS	"??"
#define EMPTY		""
#define LEFT_TO_RIGHT		1
#define RIGHT_TO_LEFT		2
#define PUT_MAX	0
#define RESET_VALUES 1
#define PUT_OLD 2

#define COUNTING_FROM_END   111
#define COUNTING_FROM_BEGIN 222


struct weight_node_t;
struct nodo;

class swindow
{
 public:
  std::ostream* m_output;
  reader m_reader;
  
  //List Control
  nodo *first;
  nodo *last;
  int numObj;

  //Window Control
  nodo *index,*beginWin,*endWin;
  int  lengthWin,posIndex,posBegin,posEnd;

  //User window: this allows users to specify their own windows
  std::vector<nodo*> user_window;

  void init(dictionary* dic /*= 0*/);
  int iniList(dictionary* dic /*= 0*/);

  int readSentence(dictionary* dic /*= 0*/);
  int readInput(dictionary* dic);
  void winAdd(nodo * node);

  int winLookRegExp2(void *er,const std::string& str);
  void winCompRegExp();
  void winFreeRegExp();

 public:
  int winLookRegExp(const std::string& m);
  int winMaterializePOSValues(int action);

  ~swindow();
  swindow(std::istream& input, std::ostream* output, dictionary* dic /*= 0*/);
  swindow(std::istream& input,int number, int position, std::ostream* output, dictionary* dic /*= 0*/);
  swindow(std::istream& input,int number, std::ostream* output, dictionary* dic /*= 0*/);
  swindow(int lengthWin, dictionary *dic);
  bool next();
  bool previous();
  nodo *getIndex();
  nodo *get(int position,int direction);
  nodo *get_user(int position);
  
  nodo *get_intern(int position);
  int show(int showScoresFlag, int showComments);

  void putLengthWin(int l);
  void putIndex(int i);

  int sentenceLength();

  void setWindow(const std::vector<nodo*>&);

  int winExistUnkWord(int direction, dictionary *d);

  void winPushWordFeature(void* ptr, dictionary* d, stack< string >& pila, int direction);
  void winPushPosFeature(void *ptr,dictionary *d, std::stack<std::string>& pila,int direction);
  void winPushAmbiguityFeature(void *ptr,dictionary *d, std::stack<std::string>& pila,int direction);
  void winPushMFTFeature(void *ptr,dictionary *d, std::stack<std::string>& pila,int direction);
  void winPushMaybeFeature(void *ptr,dictionary *d, std::stack<std::string>& pila,int direction);
  void winPushSwnFeature(std::stack<std::string>& pila);

  void winPushSuffixFeature(const std::string& wrd, std::stack<std::string>& pila,int longitud);
  void winPushPrefixFeature(const std::string& wrd, std::stack<std::string>& pila,int longitud);
  //void winPushStartCapFeature(const std::string& wrd, struct std::stack *pila);
  void winPushAllUpFeature(const std::string& wrd, std::stack<std::string>& pila);
  void winPushAllLowFeature(const std::string& wrd, std::stack<std::string>& pila);
  void winPushContainCapFeature(const std::string& wrd, std::stack<std::string>& pila);
  void winPushContainCapsFeature(const std::string& wrd, std::stack<std::string>& pila);
  void winPushContainPeriodFeature(const std::string& wrd, std::stack<std::string>& pila);
  void winPushContainCommaFeature(const std::string& wrd, std::stack<std::string>& pila);
  void winPushContainNumFeature(const std::string& wrd, std::stack<std::string>& pila);
  void winPushMultiwordFeature(const std::string& wrd, std::stack<std::string>& pila);
  void winPushLetterFeature(const std::string& wrd, std::stack<std::string>& pila, int where, int position );
  void winPushLenghtFeature(const std::string& wrd, std::stack<std::string>& pila);
  void winPushStartWithCapFeature(const std::string& wrd, std::stack<std::string>& pila );
  void winPushStartWithLowerFeature(const std::string& wrd, std::stack<std::string>& pila );
  void winPushStartWithNumberFeature(const std::string& wrd, std::stack<std::string>& pila );
  int iniGeneric(dictionary* dic /*= 0*/);
  void deleteList();
};


#endif
