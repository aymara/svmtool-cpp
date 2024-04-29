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
  auto node = new struct nodo();

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
  // read the line
  std::string line, tags;
  while (line.empty()) {
    if (m_input.eof()) {
      is_good = false;
      return -3; // end of file
    }
    char c;
    while (m_input.good() && (c=m_input.get()) != '\n') line.push_back(c);
//     getline(m_input, line);
  }
//   std::cerr << "line: '" << line << "'" << std::endl;
  // checking for commented lines
  if (line.size() >= 2 && line[0] == '#' && line[1] == '#')
    return 1;

  // reading content: word (tag,tag,tag)
  auto position = line.find_first_of(' ');
  if (position == std::string::npos) {
    return -2; // new line: end of sentence that is not a punctuation mark
  }
  token = line.substr(0,position);
  if(token.empty())
    return -2; // new line: end of sentence that is not a punctuation mark
  auto open = line.find_first_of(position+1, '(');
  if (open == std::string::npos) {
    return -2; // new line: end of sentence that is not a punctuation mark
  }
  auto close = line.find_first_of(open+1, ')');
  if (close == std::string::npos) {
    return -2;  // new line: end of sentence that is not a punctuation mark
  }
  tags = line.substr(open, close-open-1);
  comment = line.substr(close+1);
  // are tags real tags or only a comment?
  if(!tags.empty()) {
    // parse the tags

    // remove parentheses around tags list
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
  }

  // TODO this should be configurable - does not work in every language
  if (token == "." || token == "?" || token == "!")
    return -1; // end of sentence that is a punctuation mark

  return 0;
}

std::string reader::line_end(std::istringstream& iss) {
  std::string result;
  getline(iss, result);
  return result;
}

