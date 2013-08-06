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


#include "reader.h"
#include "nodo.h"
#include "er.h"

#include <set>
#include <sstream>
#include <iostream>

int reader::nextNode(nodo **node) {
  std::string word, comment;
  std::set<std::string> tags;

  int ret;
 
  while((ret = parseWord(word, tags, comment)) == 1);

  if(ret == -2) {
    *node = NULL;
  }
  else {
    *node = buildNode(word, comment);
  }


  return ret;
}


nodo* reader::buildNode(std::string &word, std::string &comment)
{
  nodo *node = new nodo;

  // wrd and realWrd
  node->realWrd = word;
  int erRet=erLookRegExp(word);
  switch (erRet)
  {
    case CARD: node->wrd ="@CARD"; break;
    case CARDSEPS: node->wrd = "@CARDSEPS"; break;
    case CARDPUNCT: node->wrd = "@CARDPUNCT"; break;
    case CARDSUFFIX: node->wrd = "@CARDSUFFIX"; break;
    default: node->wrd = word;
  }

  // comment
  node->comment = comment;

  return node;
}

int reader::parseWord(std::string& token, std::set<std::string> &tagset, std::string &comment)
{
  if (m_input.eof()) {
    is_good = false;
    return -2;
  }

  // read the line
  std::string line, tags;
  getline(m_input, line);

  // checking for commented lines
  if (line.size() >= 2 && line[0] == '#' && line[1] == '#')
    return 1;

  // reading content: word (tag,tag,tag)
  std::istringstream iss(line);
  iss >> token >> tags;

  if(token.empty())
    return -2;


  // are tags real tags or only a comment?
  if(!tags.empty() && tags[0] == '(') {
    // parse the tags
 
    // remove parentheses around tags list
    tags = tags.substr(1,tags.size()-2);
    int i = 0;
    std::string::size_type j = tags.find_first_of(',');
    while (j != std::string::npos)
    {
      std::string tag = tags.substr(i,j-i);
      tagset.insert(tag);
      i = j+1;
      j = tags.find_first_of(',',i);
    }
    std::string tag = tags.substr(i);
    tagset.insert(tag);
  } else {
    // tags is in fact only a comment
    swap(comment, tags);
    comment += line_end(iss);
  }

  // TODO this should be configurable - does not work in every language
  if (token == "." || token == "?" || token == "!")
    return -1;

  // if we just parsed the last word in the file,
  // we also want to indicate that we are at the end of the file
  // this prevents tagging an empty word
  // m_input.eof() will be true if the file has no new line at the end
  // m_input.peek() == EOF will be true if the file has a new line at the end
  if (m_input.eof() || m_input.peek() == EOF) {
    is_good = false;
    return -2;
  }

  return 0;
}

std::string reader::line_end(std::istringstream& iss) {
  std::string result;
  getline(iss, result);
  return result;
}

