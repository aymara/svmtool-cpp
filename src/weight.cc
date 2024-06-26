// kate: replace-tabs on; indent-width 2; indent-mode cstyle; encoding latin15;
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

#include <cmath>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include <tgmath.h>
#include "hash.h"
#include "weight.h"


weight_struct_t::~weight_struct_t()
{
    hash->hash_destroy();
}


char WeightRepository::skipSpaces(FILE *in, char c,int jmp)
{
  while ((c==':') || (c==' ') || (c=='\n' && jmp==1)) c=fgetc(in);
  return c;
}


void WeightRepository::readMergeModel(FILE *in,float filter)
{
  char c=fgetc(in);

  while  (!feof(in))
  {
    //c = fgetc(in);
    if (c!='#')
    {
      weight_struct_t *obj = new weight_struct_t;
      obj->key = "";

      while (c!=' ')
      {
        obj->key.push_back(c);;
        c=fgetc(in);
      }

      obj->hash  = new hash_t<weight_node_t*>();
      obj->hash->hash_init(10);

      while ((c!='\n') && (!feof(in)))
      {
        std::string pos;

        c = skipSpaces(in,c,0);
        while ((c!=':') && (!feof(in)))
        {
//           std::cerr << "Adding '" << c << "' to '" << w->pos << "'" << std::endl;
          pos.push_back(c);;
          c=fgetc(in);
        }

        c = skipSpaces(in,c,0);

        std::string value;
        while ((c!=' ') && (c!='\n') && (!feof(in)) )
        {
          value.push_back(c);;
          c=fgetc(in);
        }

        long double data;
        std::istringstream iss(value);
        iss >> data;
        weight_node_t* node = new weight_node_t();
        node->data = data;
        node->pos = pos;
//         std::cerr << "weightRepository::wrReadMergeModel " << obj->key << " " << pos << " " << data << " " << (long)data << std::endl;
        if ( fabsl(data) > fabsf(filter) )
          obj->hash->hash_insert(pos,node);
        else delete node;
      }

      c = skipSpaces(in,c,1);

      wr.hash_insert(obj->key, obj);
    }
    else
    {
      char garbage[512];
      fgets(garbage,512,in); //while(c=fgetc(in)!='\n');
      c = fgetc(in);
    }
  }
}


long double WeightRepository::getWeight(const std::string& feature,
                                        const std::string& pos)
{
  weight_struct_t *obj = wr.hash_lookup(feature);
  if ((long)obj!=HASH_FAIL)
  {
    weight_node_t *ret = obj->hash->hash_lookup(pos);

    if ((long)ret!=HASH_FAIL && ret != 0)
    {
      return ret->data;
    }
  }
  return 0;
}


WeightRepository::WeightRepository(const std::string& fileName,float filter)
{
//   std::cerr << "weightRepository::weightRepository " << fileName << std::endl;
  FILE *in;
  if ((in = fopen(fileName.c_str(), "rt"))== NULL)
  {
    fprintf(stderr, "Error opening weightRepository: %s. It's going to work without it.\n",fileName.c_str());
    exit(0);
  }
  wr.hash_init(10000);
  readMergeModel(in,filter);
  fclose(in);
}


WeightRepository::WeightRepository()
{
  wr.hash_init(10000);
}


WeightRepository::~WeightRepository()
{
  wr.hash_destroy();
}


void WeightRepository::addPOS(long unsigned int obj,
                              const std::string& pos,
                              long double weight)
{
  weight_struct_t *wst = (weight_struct_t *)obj;
  weight_node_t *wnt = wst->hash->hash_lookup(pos);

  if ((long)wnt==HASH_FAIL)
  {
    //Insertamos Nueva POS
    weight_node_t *w = new weight_node_t;
    w->pos = pos;
    w->data=weight;
    wst->hash->hash_insert(w->pos,w);
  }
  else
  { //Si POS ya esta, incrementamos el peso
    wnt->data = wnt->data + weight;
  }
}


void WeightRepository::add(const std::string& feature,
                           const std::string& pos,
                           long double weight)
{
  weight_struct_t *obj = wr.hash_lookup(feature);

  if ( (long) obj == HASH_FAIL)
  {
    // Creamos nueva entrada en WeightRepository
    obj = new weight_struct_t;
    obj->key = feature;
    obj->hash  = new hash_t<weight_node_t*>;
    obj->hash->hash_init(10);
    //Añadimos el peso y la etiqueta
      addPOS((unsigned long)obj,pos,weight);
    wr.hash_insert(obj->key, obj);
  }
  else
    //Añadimos el peso y la etiqueta
    addPOS((unsigned long)obj,pos,weight);
}


void WeightRepository::write(const std::string& outName, float filter)
{
  FILE *f;

  if ((f = fopen(outName.c_str(), "w"))== NULL)
  {
    fprintf(stderr, "Error opening file: %s\n",outName.c_str());
    exit(0);
  }

  //Recorremos el hash objeto a objeto
  for (auto it  = wr.begin(); it != wr.end(); it++)
  {
    weight_struct_t *wst = (weight_struct_t *) ((*it).second);

    //Modificación 180705: añadimos filtrado de pesos
    //std::string mrg = wrGetMergeInput(wst->hash); //DEL 180705
    auto mrg = getMergeInput(wst->hash,filter); //ADD 180705

    if (!mrg.empty()) fprintf(f,"%s%s\n",wst->key.c_str(),mrg.c_str());
  }//for
  fclose (f);
}


std::string WeightRepository::getMergeInput(hash_t<weight_node_t*>* ,
                                            float filter)
{
  std::ostringstream out;

  for (auto it  = wr.begin(); it != wr.end(); it++)
  {
    weight_node_t *wnt = (weight_node_t *) ((*it).second);

    if (wnt->data!=0)
    {
      //Modificación 180705: Filtrado de pesos
      //Comprobamos que el peso a insertar en el fichero
      //cumple con el filtrado de pesos.
      if ( fabsl(wnt->data) > fabsf(filter) ) //ADD 180705
      // %s %s:%.18E
      out << " " << wnt->pos << ":" <<  wnt->data;
    }
  } //for
  return out.str();
}


void WeightRepository::writeHash(hash_t<weight_node_t*>* ,
                                 FILE *f,
                                 char separador)

{
  int cont=0;

  for (auto it  = wr.begin(); it != wr.end(); it++)
  {
    weight_node_t *wnt = (weight_node_t *) ((*it).second);

    if (separador == '\n' && cont==0)
      fprintf(f,"%s %2.10f",wnt->pos.c_str(),(float)wnt->data);
    else
      fprintf(f,"%c%s:%2.10f",separador,wnt->pos.c_str(),(float)wnt->data);
    cont++;
  } /* for */
}

