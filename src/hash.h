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

#ifndef SVMTOOL_HASH_H
#define SVMTOOL_HASH_H

#include <vector>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#ifndef NO_STDCPP0X
#include <unordered_map>
#else
#include <ext/hash_map>
#endif

#ifdef NO_STDCPP0X
namespace __gnu_cxx
{
  template<> struct hash< std::string >
  {
    size_t operator()( const std::string& x ) const
    {
      return hash< const char* >()( x.c_str() );
    }
  };
}
#endif

struct hash_node_t {
  hash_node_t() : data(0), key(""), next(0) {}
  ~hash_node_t() {}
  long data;                 /* data in hash node */
  const char * key;                   /* key for hash lookup */
  struct hash_node_t *next;           /* next node in hash chain */
};

#define HASH_FAIL -1

#ifndef NO_STDCPP0X
#define HASH_TYPE std::unordered_map< std::string, T >
#else
#define HASH_TYPE __gnu_cxx::hash_map< std::string, T >
#endif

template <typename T>
struct hash_t : public HASH_TYPE
{
//   void hash_init(int);
//   
//   long hash_lookup (const hash_t* tptr, const std::string& key) const;
//   
//   long hash_insert (const std::string& key, T data);
//   
//   long hash_delete (const std::string&);
//   
//   void hash_destroy();
//   
//   void hash_print(FILE *f);

  #define HASH_LIMIT 0.5
  
  /**************************************************/
  
  // /*
  //  *  hash() - Hash function returns a hash number for a given key.
  //  *
  //  *  tptr: Pointer to a hash table
  //  *  key: The key to create a hash number for
  //  */
  // static long hash(const hash_t *tptr, const char *key)
  // {
    //  int i=0;
    //  long hashvalue;
    //
    //  if (key) while (*key != '\0') i=(i<<3)+(*key++ - '0');
    //
    //  hashvalue = (((i*1103515249)>>downshift) & mask);
    //  if (hashvalue < 0) hashvalue = 0;
    //
    //  return hashvalue;
    // }
    
    /**************************************************/
    
    /**************************************************/
    
    /*
    *  hash_init() - Initialize a new hash table.
    *
    *  tptr: Pointer to the hash table to initialize
    *  buckets: The number of initial buckets to create
    */
    void hash_init(int buckets)
    {
      HASH_TYPE::clear();
#ifdef NO_STDCPP0X
      HASH_TYPE::resize(buckets);
#else
      (void)buckets;
#endif
    }
    
    /**************************************************/
    
    /*
    *  hash_lookup() - Lookup an entry in the hash table and return a pointer to
    *    it or HASH_FAIL if it wasn't found.
    *
    *  tptr: Pointer to the hash table
    *  key: The key to lookup
    */
    T hash_lookup(const std::string& key) const
    {
      typename HASH_TYPE::const_iterator it = HASH_TYPE::find(key);
      if (it == HASH_TYPE::end())
        return (T)HASH_FAIL;
      else
        return (*it).second;
    }
    
    /**************************************************/
    
    /*
    *  hash_insert() - Insert an entry into the hash table.  If the entry already
    *  exists return a pointer to it, otherwise return HASH_FAIL.
    *
    *  tptr: A pointer to the hash table
    *  key: The key to insert into the hash table
    *  data: A pointer to the data to insert into the hash table
    */
    long hash_insert(const std::string& key, T data)
    {
//       std::cerr << "hash_insert in " << this << ": " << key << " -> " << data << std::endl;
      typename HASH_TYPE::iterator it  = HASH_TYPE::find(key);
      if (it != HASH_TYPE::end())
        return (long)((*it).second);
      
      HASH_TYPE::insert(std::make_pair(key, data));
      return HASH_FAIL;
    }
    
    /**************************************************/
    
    /*
    *  hash_delete() - Remove an entry from a hash table and return a pointer
    *  to its data or HASH_FAIL if it wasn't found.
    *
    *  tptr: A pointer to the hash table
    *  key: The key to remove from the hash table
    */
    T hash_delete(const std::string& key)
    {
      typename HASH_TYPE::iterator it  = HASH_TYPE::find(key);
      if (it == HASH_TYPE::end())
        return (T)HASH_FAIL;
      T data = (*it).second;
      HASH_TYPE::erase(it);
      return data;
    }
    
    /**************************************************/
    
    /*
    * hash_destroy() - Delete the entire table, and all remaining entries.
    * we can't use a C++ destructor since HASH_TYPE has no virtual destructor.
    */
    void hash_destroy()
    {
      delete_entries(*this);
      HASH_TYPE::clear();
    }
    
    /**************************************************/
    
    /*
    * hash_print() - Print Keys in FILE *f
    *
    */
    void hash_print(FILE *f)
    {
      
      for (typename HASH_TYPE::iterator it  = HASH_TYPE::begin(); it != HASH_TYPE::end(); it++)
      {
        fprintf(f,"%s\n",(*it).first.c_str());
      }
    }
    
    
};

/*
 * delete_entries() - call the dtor on every entry in the hash. Useful is
 * those entries where allocated before getting into the hash.
 *
 *  h : the hash containing pointers.
 */
template<typename T>
void delete_entries(hash_t<T*>& h) { 
  for (typename hash_t<T*>::iterator it = h.begin(); it != h.end(); it++) {
    delete it->second;
  }
}

/*
 * delete_entries() - same as before but don't delete if it's not a pointer
 */
template<typename T> void delete_entries(hash_t<T>&) { }

#endif
