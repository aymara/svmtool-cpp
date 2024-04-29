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

/**
 * The WeightRepository object is in charge of containing the weights for each
 * POS-feature pair. A weight repository is formed by a hash of weight_struct_t
 * objects, containing the attributes (key). Each of these hash has a new hash
 * with all the POS for which the attribute and its respective weight have been
 * found. (weight_node_t).
*/
class WeightRepository
{
public:
  /**
   * Constructor that loads the weight repository of the file called fileName,
   * filtering the weights that are below the marked limit (filter).
   * @param std::string fileName : File name
   * @param float filter: Value to filter the weights that are read.
  */
  WeightRepository(const std::string& fileName,float filter);

  WeightRepository();

  ~WeightRepository();

  /**
  * Reads the weight for the attribute and label received as a parameter.
  * @param std::string feature: Attribute
  * @param std::string pos: Morphosyntactic label
  */
  long double getWeight(const std::string& feature,const std::string& pos);

  /**
   * Insert a new weight for the attribute feature and the label pos.
   * @param std::string feature: Attribute to an insertar
   * @param std::string pos: Label to be inserted
   * @param long double weight: Weight to be assigned to the label
   */
  void add(const std::string& feature, const std::string& pos, long double weight);

  //void wrWrite(char *outName); //DEL 180705
  /**
   * Write the weight repository in the file with name outName.
   * @param float filter used to filter weights
   */
  void write(const std::string& outName, float filter);

  /**
   * Write the content of a hash (tptr), in a file pointed by f.
   * Between each pair POS/PESO puts the separator character.
   */
  void writeHash(hash_t<weight_node_t*> *tptr, FILE *f, char separador);

private:
  /*
   * Returns a string of characters with all pairs POS/ WEIGHT contained in the
   * hash (tptr) of an attribute.
   *
   * Modification 180705:
   * Add parameter "float filter" for filtering weights
   */
  std::string getMergeInput(hash_t<weight_node_t*> *tptr, float filter);

  FILE *openFile(const std::string& name, char mode[]);

  /**
   * This method loads a weight repository from a file (f), filling in the
   * weights that are below the marked limit (filter).
   *
   * @param FILE* in : pointer to the file to be read
   * @param float filter: Value for filtering the weights to be read
   */
  void readMergeModel(FILE *in,float filter);

  char skipSpaces(FILE *in,char c,int jmp);

  /**
   * Insert a new weight for the label pos, in the indicated attribute
   * If the label already exists, the weight is increased with weight. If it
   * does not exist, it is added.
   * @param int obj: Pointer to the object containing the attribute
   * @param std::string pos: Label to be inserted:
   * @param long double weight: Weight to be assigned to the label
   */
  void addPOS(unsigned long obj, const std::string& pos, long double weight);

  /** The weights repository */
  hash_t<weight_struct_t*> wr;

};

#endif
