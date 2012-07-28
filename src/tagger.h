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

#ifndef TAGGER_H
#define TAGGER_H

#include "common.h"
#include "weight.h"
#include "list.h"
#include "hash.h"
#include "swindow.h"
#include "dict.h"
#include "er.h"

struct models_t{
    weightRepository *wr,*wr2,*wrUnk,*wrUnk2;
    //hash_t *bias,*bias2,*biasUnk,*biasUnk2;
    simpleList<nodo_feature_list*> featureList;
    simpleList<nodo_feature_list*> featureListUnk;
};


class tagger
{
 public:
    bool taggerShowScoresFlag,taggerShowCommentsFlag;

    //Flow Control
    std::string flow;
    int  taggerStrategy,taggerNumLaps,taggerWinIndex,taggerWinLength;
    float  taggerKFilter,taggerUFilter;
    std::string taggerBackupDict,taggerModelName;

    std::stack<std::string> stk;
    models_t  *taggerModelList;
    models_t  *taggerModelRunning;
    dictionary *d;
    swindow *sw;

    int taggerRightSense();
    int taggerLeftSense();

    std::vector<weight_node_t> userWeight;
    void setPossibles(const std::vector<std::string> &possibles);

    std::string taggerSumWeight(weightRepository* wRep, std::vector<weight_node_t> &weight, unsigned int numMaybe, int* max);
    void taggerGenerateScore(nodo *elem,int direction);

    std::vector<weight_node_t> taggerCreateWeightNodeArray(unsigned int numMaybe, dataDict* index);
    std::vector<weight_node_t> taggerCreateWeightUnkArray(unsigned int *numMaybe);
    hash_t<weight_node_t*> *taggerCreateBiasHash(const std::string& name);
    void taggerLoadModels(models_t *model, int taggerNumModel);

    void taggerStadistics(int numWords, int numSentences, double realTime,double usrTime, double sysTime);
    void taggerShowVerbose(int num,int isEnd);


  int taggerRightSenseSpecialForUnknown();
  int taggerLeftSenseSpecialForUnknown();
  void taggerDoNormal(int *numWords, int *numSentences);
  void taggerDoSpecialForUnknown(int *numWords, int *numSentences);
  void taggerDoNTimes(int *numWords, int *numSentences,int laps);

public:
  void taggerRun();
  void taggerLoadModelsForTagging();
  void taggerShowNoComments();
  void taggerShowComments();
  void taggerActiveShowScoresFlag();
  void taggerDesactiveShowScoresFlag();
  void taggerPutFlow(const std::string& inFlow);
  void taggerPutBackupDictionary(const std::string& dictName);
  void taggerPutStrategy(int num);
  void taggerPutWinLength(int l);
  void taggerPutWinIndex(int i);
  void taggerPutKWeightFilter(float kfilter);
  void taggerPutUWeightFilter(float ufilter);
  void taggerInit(std::istream& input, std::ostream& output);
  void taggerInit();
  tagger(const std::string& model);
  ~tagger();
};


#endif
