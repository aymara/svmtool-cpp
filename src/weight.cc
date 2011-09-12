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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <cmath>
#include "hash.h"
#include "weight.h"


/***********************************************************/

/*
 * El objeto WeightRepository es el encargado de contener los pesos 
 * para cada pareja POS-feature. Un depósito de pesos está formado por
 * un hash de objetos weight_struct_t, conteniendo los atributos (key).
 * Cada uno de estos hash es poseedor de un nuevo hash con todas las POS
 * para las cuales se ha encontrado el atributo y su respectivo peso.
 * (weight_node_t).
*/

/***********************************************************/

//Definición de weight_struct_t
class weight_struct_t
{
public:
  std::string key;
  hash_t<weight_node_t*> *hash;

  // free inner memory and then free the hash.
  ~weight_struct_t()
  {
    hash->hash_destroy();
  }
};



/***********************************************************/

char weightRepository::wrSaltarBlancs(FILE *in, char c,int jmp)
{
  while ((c==':') || (c==' ') || (c=='\n' && jmp==1)) c=fgetc(in);
  return c;
}

/***********************************************************/

/*
 * void weightRepository::wrReadMergeModel(FILE *in,float filter)
 * Parámetros:
 *  FILE *in : apuntador al fichero que ha de leer
 *  float filter: Valor para filtrar los pesos que se lean
 * Este método carga un depósito de pesos de un fichero (f), filltrando
 * los pesos que esten por debajo del límite marcado (filter)
 */
void weightRepository::wrReadMergeModel(FILE *in,float filter)
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

        c = wrSaltarBlancs(in,c,0);
        while ((c!=':') && (!feof(in)))
        {
//       std::cerr << "Adding '" << c << "' to '" << w->pos << "'" << std::endl;
          pos.push_back(c);;
          c=fgetc(in);
        }

        c = wrSaltarBlancs(in,c,0);

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
        if ( fabsf(data) > fabsf(filter) )
          obj->hash->hash_insert(pos,node);
        else delete node;
      }

      c = wrSaltarBlancs(in,c,1);

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

/***********************************************************/

/*
 * long double weightRepository::wrGetWeight(std::string feature,std::string pos)
 * Parámetros:
 *  std::string feature: Atributo
 *  std::string pos: Etiqueta morfosintáctica
 * Lee el peso para el atributo y la etiqueta recibidos como parámetro.
 */
long double weightRepository::wrGetWeight(const std::string& feature,const std::string& pos)
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

/***********************************************************/

/*
 * weightRepository(std::string fileName,float filter)
 * Parámetros:
 *  std::string fileName : Nombre del fichero
 *  float filter: Valor para filtrar los pesos que se lean
 * Constructor que carga el depóosito de pesos del fichero llamado
 * fileName , filltrando los pesos que esten por debajo del límite
 * marcado (filter)
 */
weightRepository::weightRepository(const std::string& fileName,float filter)
{
//   std::cerr << "weightRepository::weightRepository " << fileName << std::endl;
  FILE *in;
  if ((in = fopen(fileName.c_str(), "rt"))== NULL)
    {
      fprintf(stderr, "Error opening weightRepository: %s. It's going to work without it.\n",fileName.c_str());
      exit(0);
    }
  wr.hash_init(10000);
  wrReadMergeModel(in,filter);
  fclose(in);
}

/***********************************************************/

/*
 * weightRepository()
 * Contructor
 */
weightRepository::weightRepository()
{
  wr.hash_init(10000);
}

/***********************************************************/

/*
 * ~weightRepository()
 * Destructor
 */
weightRepository::~weightRepository()
{
  wr.hash_destroy();
}

/*******************************************************/

/*
 * void wrAddPOS(int obj, std::string pos, long double weight)
 * Parámetros:
 *  int obj: Apuntador al objeto que contiene el atributo
 *  std::string pos: Etiqueta a insertar:
 *  long double weight: Peso a asignar a la etiqueta
 * Insertamos un nuevo peso para la etiqueta pos, en el atributo indicado
 * por obj. Si la etiqueta ya existe se incrementa el peso con weight. Si 
 * no existe se añade.
 */
void weightRepository::wrAddPOS(long unsigned int obj, const std::string& pos, long double weight)
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

/*******************************************************/

/*
 * void wrAdd(std::string feature, std::string pos, long double weight)
 * Parámetros:
 *  std::string feature: Atributo a insertar
 *  std::string pos: Etiqueta a insertar
 *  long double weight: Peso a asignar a la etiqueta
 * Insertamos un nuevo peso para para el atributo feature y la etiqueta pos.
 */
void weightRepository::wrAdd(const std::string& feature, const std::string& pos, long double weight)
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
      wrAddPOS((unsigned long)obj,pos,weight);
      wr.hash_insert(obj->key, obj);
    }
  else
    //Añadimos el peso y la etiqueta
    wrAddPOS((unsigned long)obj,pos,weight);
}

/*******************************************************/

/*
 * wrWrite(std::string outName)
 * Escribe el depósito de pesos  en el fichero con nombre outName.
 *
 * Modificación 180705:
 *     Añadimos el parámetro "float filter", se utiliza para filtrar pesos
 */
void weightRepository::wrWrite(const std::string& outName, float filter)
{
  FILE *f;

  if ((f = fopen(outName.c_str(), "w"))== NULL)
    {
      fprintf(stderr, "Error opening file: %s\n",outName.c_str());
      exit(0);
    }

  //Recorremos el hash objeto a objeto
  for (hash_t<weight_struct_t*>::iterator it  = wr.begin(); it != wr.end(); it++)
  {
    weight_struct_t *wst = (weight_struct_t *) ((*it).second);

    //Modificación 180705: añadimos filtrado de pesos
    //std::string mrg = wrGetMergeInput(wst->hash); //DEL 180705
    std::string mrg = wrGetMergeInput(wst->hash,filter); //ADD 180705

    if (!mrg.empty()) fprintf(f,"%s%s\n",wst->key.c_str(),mrg.c_str());
  }//for
  fclose (f);
}

/*******************************************************/
/* 
 * std::string wrGetMergeInput(hash_t *tptr)
 * Devuelve una cadena de caracteres con todas las parejas
 * POS/PESO contenidas en el hash (tptr) de un atributo.
 *
 * Modificación 180705:
 *        Añadimos parámetro "float filter" para filtrado de pesos
 */
std::string weightRepository::wrGetMergeInput(hash_t<weight_node_t*> *tptr, float filter)
{
  std::ostringstream out;

  for (hash_t<weight_struct_t*>::iterator it  = wr.begin(); it != wr.end(); it++)
  {
    weight_node_t *wnt = (weight_node_t *) ((*it).second);

    if ((float)wnt->data!=0)
    {
      //Modificación 180705: Filtrado de pesos
      //Comprobamos que el peso a insertar en el fichero
      //cumple con el filtrado de pesos.
      if ( fabsf(wnt->data) > fabsf(filter) ) //ADD 180705
      // %s %s:%.18E
      out << " " << wnt->pos << ":" <<  (float) wnt->data;
    }
  } //for
  return out.str();
}

/*******************************************************/

/*
 * void wrWriteHash(hash_t *tptr,FILE *f, char separador)
 * Escribe el contenido de un hash (tptr), en fichero apuntado por f.
 * Entre cada pareja POS/PESO pone el caracter separador.
 */
void weightRepository::wrWriteHash(hash_t<weight_node_t*> *tptr,FILE *f, char separador)

{
  int cont=0;

  for (hash_t<weight_struct_t*>::iterator it  = wr.begin(); it != wr.end(); it++)
  {
    weight_node_t *wnt = (weight_node_t *) ((*it).second);

    if (separador == '\n' && cont==0) fprintf(f,"%s %2.10f",wnt->pos.c_str(),(float)wnt->data);
    else  fprintf(f,"%c%s:%2.10f",separador,wnt->pos.c_str(),(float)wnt->data);
    cont++;
  } /* for */
}

