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

#ifndef DICT_H

#include "list.h"
#include "hash.h"
#include <set>
#include <string>

#define TAMTXT 100

struct infoDict
{
  std::string pos;
  int  num;
};

struct dataDict
{
  std::string wrd;
  int  numWrd;
  int  numMaybe;
  simpleList<infoDict*> maybe;
};

class  dictionary
{
  public:
    dictionary(const std::string& name, const std::string& backup);
    dictionary(const std::string& name);
    dictionary(const std::string& name,int limInf, int limSup);
    ~dictionary();
    
    void dictAddBackup(const std::string& name);
    void addBackupEntry(const std::string& token, const std::set<std::string>& tags);
    
    dataDict* getElement(const std::string& key);
    std::string& getElementWord(dataDict*  ptr);
    int getElementNumWord(dataDict*  ptr);
    int getElementNumMaybe(dataDict*  ptr);
    simpleList<infoDict*>& getElementMaybe(dataDict*  ptr);
    infoDict* getMFT(dataDict* w);
    std::string getAmbiguityClass(dataDict*  w);
    
    hash_t<infoDict*>* dictFindAmbP(int *numPOS);
    hash_t<infoDict*>* dictFindUnkP(int *numPOS);
    void dictRepairFromFile(const std::string& fileName);
    void dictRepairHeuristic(float dratio);
    
    void dictCleanListInfoDict(simpleList< infoDict* >* l, int num);
    
    void dictWrite(const std::string& outName);

private:
  void dictLoad(FILE *in);
  void dictCreate(FILE *f,int offset, int limit);
  void dictIncInfo(dataDict *elem, const std::string& pos);
  
  int readInt(FILE *in);
  infoDict *readData(FILE *in);

  hash_t<dataDict*> d;
};

#define DICT_H
#endif
