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

#ifndef SIMPLELIST_H


template <typename T>
struct listNode
{
        int ord;
        T data;
        listNode<T>* next;
        listNode<T>* previous;
};

template <typename T>
class simpleList
{

private:
        //List Control
        listNode<T>* first;
        listNode<T>* last;
        listNode<T>* index;
        int numObj;

public:
// 	~simpleList();
// 	simpleList();
// 	void deleteList();
//         int next();
//         int previous();
//         void setFirst();
//         void *get(int position);
//         void *getIndex();
//         void *getFirst();
//         void *getLast();
// 	int show();
//         int add(void *object);
// 	int delIndex();
// 	int isEmpty();
// 	int numElements();

  /****************************************************************************/
  /* Simple List */
  /****************************************************************************/
  void deleteList()
  {
    int cont = numObj;
    
    if (first==0) return;
    listNode<T>* aux=first;
    
    while (first->next!=0 && cont >= 1)
    {
      aux  = first;
      first = first->next;
      cont = cont - 1;
      delete aux;
    }
    
    delete last;
    numObj = 0;
    first = 0;
    last = 0;
    index = 0;
  }
  
  /****************************************************************************/
  
  ~simpleList()
  {
    deleteList();
  }
  
  /****************************************************************************/
  
  simpleList()
  {
    numObj = 0;
    first = 0;
    last = 0;
    index = 0;
  }
  
  /****************************************************************************/
  
  /*Move Interest Point to next element */
  bool next()
  {
    if  ((index == 0) || (index->next == 0)) return false;
    index = index->next;
    return true;
  }
  
  /****************************************************************************/
  
  /* Move Interest Point to previous element */
  bool previous()
  {
    if ((index==0) || (index->previous==0)) return false;
    index = index->previous;
    return true;
  }
  
  /****************************************************************************/
  
  /* Get Interest Point */
  T* getIndex()
  {
    if ( index == 0 ) return 0;
    else return &index->data;
  }
  
  /****************************************************************************/
  
  /* Get Interest Point */
  T* getFirst()
  {
    return &first->data;
  }
  
  /****************************************************************************/
  
  T* getLast()
  {
    return &last->data;
  }
  
  /****************************************************************************/
  
  void setFirst()
  {
    index = first;
  }
  
  /****************************************************************************/
  
  T* get(int position)
  {
    listNode<T>* aux;
    int i;
    
    if (numObj == 0 || position >= numObj)
      return 0;
    
    aux = first;
    
    for(i=0; i<position; i++)
    {
      if(aux->next != 0) aux = aux->next;
      else return 0;
    }
    return aux->data;
  }
  
  /****************************************************************************/
  
  /* Show list elements */
  int show()
  {
    if (first==0) return 0;
    
    listNode<T>* actual=first;
    
    while (actual->next!=0)
    {
      actual=actual->next;
      
    }
    return 0;
  }
  
  /****************************************************************************/
  
  int add(T object)
  {
    listNode<T>* aux = new listNode<T>();
    
    if(numObj == 0)
    {
      aux->previous=0;
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
    
    aux->ord = numObj;
    aux->data = object;
    aux->next=0;
    numObj++;
    return numObj;
  }
  
  /****************************************************************************/
  
  int delIndex()
  {
    listNode<T>* aux = index;
    
    if(numObj == 0) return -1;
    
    if (index==last && index==first)
    {
      /*      first = aux->next;
      aux->previous = 0;
      index = first;
      last = aux->previous;
      last->next = 0;
      index = last;*/
      first = 0;
      index = 0;
      last = 0;
      aux->previous = 0;
      aux->next = 0;
    }
    else if (index==first)
    {
      
      first = aux->next;
      first->previous = 0;
      index = first;
    }
    else if (index==last)
    {
      last = aux->previous;
      last->next = 0;
      index = last;
    }
    else
    {
      index = index->previous;
      aux->previous->next = aux->next;
      aux->next->previous = aux->previous;
    }
    
    numObj--;
    delete aux;
    return numObj;
  }
  
  /****************************************************************************/
  
  bool isEmpty()
  {
    return (numObj == 0 || first == 0);
  }
  
  /****************************************************************************/
  
  int numElements()
  {
    return numObj;
  }
  
  
  
};

#define SIMPLELIST_H
#endif
