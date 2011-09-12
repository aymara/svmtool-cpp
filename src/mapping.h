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

#ifndef MAPPING_H

#include "hash.h"
/*
* Cada elemento insertado en el mapping es del tipo mapping_node_t.
* Este objeto está compuesto por:
*
* feature  char *  Atributo almacenado
* number char *  Número identificativo de el atributo en formato alfanumérico
* num  Int Número de veces que aparece
*/
class mapping_node_t
{
  public:
    char *feature;
    char *number;
    int num;
    
    ~mapping_node_t()
    {
      delete feature;
      delete number;
    }
};


class  mapping
{
private:
  hash_t<mapping_node_t*>* mapByKey;
  hash_t<mapping_node_t*>* mapByNumber;
  int mapping_counter;
public:
  void  mappingWrite(const char *,int);
  int   mappingAddByKey(const char *key);
  int   mappingAddNumber(const char *key);
  int   mappingGetNumberByFeature(const char *key);
  char *mappingGetFeatureByNumber(const char *key);
  int   mappingRepair(int maxFeat, int times);
  int   mappingNumElements();
  void  mappingBuilt(FILE *f,int mac_mapping_size, int count_cut_off);

  mapping();
  ~mapping();
};

#define MAPPING_H
#endif
