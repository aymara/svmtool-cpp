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

#ifndef READER_H
#define READER_H

#include <iostream>
#include <sstream>
#include <set>

class nodo;

class reader
{
  public:
    // we use std::cin to say "don't use this input stream". It is very
    // wrong but was the simplest way to provid a valid reference...
    reader() : m_input(std::cin) { }
    reader(std::istream& input) : m_input(input), is_good(true) { }

    int nextNode(nodo**);

    /* Constructs a nodo given the textual information about it 
     * Does not fill the weights or the neighbor information */
    nodo* buildNode(std::string &word, std::string &comment);

    /* examples of accepted input: 
     *   - ## this a comment
     *   - the
     *   - the # another comment
     *   - object (NN)
     *   - object (NN) # hello.
     *   - attack (VB,NN)
     *
     * return codes:
     *   - 1 comment
     *   - 0 normal sentence
     *   - -1 end of sentence
     *   - -2 end of file
     */
    int parseWord(std::string& token, std::set<std::string> &tags, std::string &comment);
    bool good() { return is_good; }

  private:
    std::istream& m_input;
    bool is_good;
    std::string line_end(std::istringstream &iss);
};

#endif
