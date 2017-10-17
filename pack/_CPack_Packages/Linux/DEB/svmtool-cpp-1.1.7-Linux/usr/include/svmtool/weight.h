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

#ifndef WEIGHT_H
#define WEIGHT_H

#include "hash.h"

struct weight_node_t
{
  weight_node_t() : pos(), data(0) {}
  weight_node_t(std::string _pos) : pos(_pos), data(0) {}
  std::string pos;
  long double data;
};

//Definición de weight_struct_t
class weight_struct_t
{
public:
  std::string key;
  hash_t<weight_node_t*> *hash;

  // free inner memory and then free the hash.
  ~weight_struct_t();
};

class  weightRepository
{
 private:
   hash_t<weight_struct_t*> wr;
  
   std::string wrGetMergeInput(hash_t<weight_node_t*> *tptr, float filter); //ADD 180705
  //char *wrGetMergeInput(hash_t *tptr); //DEL 180705
  FILE *openFile(const std::string& name, char mode[]);
  void wrReadMergeModel(FILE *in,float filter);
  char wrSaltarBlancs(FILE *in,char c,int jmp);
  void wrAddPOS(unsigned long obj, const std::string& pos, long double weight);
public:
  long double wrGetWeight(const std::string& feature,const std::string& pos);
  void wrAdd(const std::string& feature, const std::string& pos, long double weight);
  //void wrWrite(char *outName); //DEL 180705
  void wrWrite(const std::string& outName, float filter); //ADD 180705
  void wrWriteHash(hash_t<weight_node_t*> *tptr,FILE *f,char separador);
  weightRepository(const std::string& fileName,float filter);
  weightRepository();
  ~weightRepository();
};

#endif
