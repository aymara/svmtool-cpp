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
 

#ifndef LEARNER_H

#include "hash.h"
#include "dict.h"
#include <string>

struct samples_counter_t
{
    char *key;
    int positive;
    int negative;
};

class learner
{
private:
	int learnerNumAMBP,learnerNumUNKP;
	hash_t<infoDict*>* learnerAMBP_H;
  hash_t<infoDict*>* learnerUNKP_H;
	swindow *sw;
  simpleList<nodo_feature_list*> learnerFeatureList;
  simpleList<infoDict*>* learnerAMBP_L;
  simpleList<infoDict*>* learnerUNKP_L;
	FILE *fKnown,*fUnknown;
	int learnerNumFeatures;
	char obtainAtrChar(FILE *channel);
	int  obtainAtrInt(FILE *channel,int *endAtr);
  // 	void learnerCreateFeatureList(char *name, simpleList *featureList);
  simpleList<infoDict*>* learnerTransformHashInList(hash_t<infoDict*> *tptr);
	void learnerCreateDefaultFile(const std::string& modelName, const std::string& str);
	void learnerCreatePOSFile(const std::string& modelName, int is_ambp, hash_t< infoDict* >* h);
	void learnerCount(const std::string& name, int* nWords, int* nSentences);
  int learnerExecSVMlight(const std::string& svmdir, const std::string& options, const std::string& posFile, const std::string& outFile);
	int learnerLeftToRight(simpleList< nodo_feature_list* >* featureList, simpleList< nodo_feature_list* >* featureListUnk, dictionary* dKnown, dictionary* dUnknown, int numWrds, int inicio);
	int learnerRightToLeft(simpleList< nodo_feature_list* >* featureList, simpleList< nodo_feature_list* >* featureListUnk, dictionary* dKnown, dictionary* dUnknown, int numWrds, int inicio);
	void learnerGetFeatures(nodo* elem, std::stack< std::string, std::deque< std::string, std::allocator< std::string > > >& stk, dictionary* d, simpleList< nodo_feature_list* >* featureList, int direction);
	void learnerGenerateFeatures(nodo* elem, simpleList< nodo_feature_list* >* featureList, dictionary* d, int direction);
	void learnerGenerateFeaturesUnk(nodo* elem, simpleList< nodo_feature_list* >* featureList, dictionary* d, dictionary* dUnk, int direction);
	void learnerTrainModel(const std::string& trainingFileName, dictionary* dKnown, int numModel, int direction, int numSent, int numWords, int numChunks);
  weightRepository* learnerBuiltWeightRepository(weightRepository *wr,mapping *m,const std::string& pos,const std::string& fileName);
  hash_t<weight_node_t*>* learnerBuiltBias(hash_t<weight_node_t*>* h, const std::string& pos, const std::string& fileName);
  void learnerDestroyBias(hash_t<weight_node_t*> *h);
  int learnerIsPossiblePOS(const std::string& wrd, const std::string& pos, int Known_or_Unknown);
  simpleList<infoDict*>* learnerGetPotser(const std::string& wrd, int Known_or_Unknown, dictionary *d);
//     void learnerTraining(FILE *f,char *modelName, int numModel,int LR_or_RL,int K_or_U,dictionary *d,simpleList *lpos);
   	void learnerPrintMessage(int numModel, int K_or_U, int LR_or_RL,int is_fex);
  	int  learnerNumChunks(const std::string& trainingFileName, float percentage, int nSentences);
  	bool learnerIsInsideList(simpleList< infoDict* >* l, const std::string& key);
  	void learnerDoLearn(FILE *f,int numModel,int LR_or_RL,int K_or_U,dictionary *d,simpleList<infoDict*>* lPosToTrain);
    void learnerDressNakedSetTrain(dictionary *d,mapping *m,FILE *f, const std::string& pos, int numModel, int direction, int K_or_U,int *nPos,int *nNeg);
    void learnerPushSample(const std::string& wrd,int numModel,int direction, int Known_or_Unknown, const std::string& pos,const std::string& samplePos,const std::string& features,dictionary *d, int *nNeg, int *nPos);
    void learnerPushSampleUnk(const std::string& wrd,int numModel,int direction, int Known_or_Unknown,const std::string& pos, const std::string& samplePos, const std::string& features,dictionary *d, int *nNeg, int *nPos);
  	std::string learnerCreateFeatureString(FILE *f,mapping *m);

  	std::string read_feature_list_from_config_file(FILE *f, char *first_feature);
    std::string read_feature_list_from_string(const std::vector< std::string >& tokens);
  	void read_config_file(const std::string& config_file);

public:
	learner(char *modelName);
	learner();
	~learner();
	void learnerRun(const std::string& train);
};

#define LEARNER_H
#endif
