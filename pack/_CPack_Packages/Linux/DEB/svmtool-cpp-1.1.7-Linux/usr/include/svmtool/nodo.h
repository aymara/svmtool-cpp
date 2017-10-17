/*
 * Author: Quentin Pradet
 * Copyright (C) 2011 CEA LIST
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

#ifndef NODO_H
#define NODO_H

#include "weight.h"
#include <stack>

struct nodo{
  nodo() :
    ord(0),
    weight(0),
    weightOld(0),
    next(NULL),
    previous(NULL) {}
  ~nodo() { 
    while(!stackScores.empty()) {
      delete stackScores.top();
      stackScores.pop();
    }
  
  }
  int ord; // word id
  std::string wrd; // word (or constant like @CARD if cardinal)
  std::string realWrd; // real word
  std::string comment;
  // to be filled by taggerSumWeight
  std::string pos, posOld;
  std::string strScores;
  long double weight, weightOld;
  std::stack<weight_node_t*> stackScores;
  // neighbors in the sentence
  nodo *next;
  nodo *previous;
};

#endif
