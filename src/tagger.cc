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

#include "tagger.h"
#include "strategies.h"
#include "hash.h"
#include "list.h"
#include "weight.h"
#include "dict.h"
#include "swindow.h"
#include "marks.h"
#include "common.h"
#include "nodo.h"

#include <sstream>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include <cassert>


/***************************************************************/

struct tms  tbuffStartUp,tbuffEndStartUp;
clock_t startUpTime,endStartUpTime;
double  sysFexTime=0, usrFexTime=0,realFexTime=0;
double  sysSVMTime=0, usrSVMTime=0,realSVMTime=0;

/***************************************************************/

extern int verbose;


/***************************************************************/

hash_t<weight_node_t*> *tagger::taggerCreateBiasHash(const std::string& name)
{
  hash_t<weight_node_t*> *bias = new hash_t<weight_node_t*>;
  int i=0;
  char c=' ';
  weight_node_t *w;
  FILE *f;

  bias->hash_init(40);

  if ((f = fopen(name.c_str(), "rt"))== NULL)
  {
    std::cerr << "Error opening file: "<<name<<std::endl;
    exit(0);
  }

  while (!feof(f))
  {
    c = fgetc(f);
    if (c!='#')
    {
      w = new weight_node_t;
      std::string weight;
      i=0;
      w->pos = "";
      while ((c!='\n') && (!feof(f)))
      {
        if (c!=' ' && c!='\n' && c!='\t' && i==1)  weight.push_back(c);
        else if (c!=' ' && c!='\n' && c!='\t' && i==0)
        {
          if (c!=':') w->pos += c;
          else i=1;
        }
        c = fgetc(f);
      }
      w->data = (long double)0;
      std::istringstream iss(weight);
      iss >> w->data;
      bias->hash_insert(w->pos,w);
    } //end if
    else while((c=fgetc(f))!='\n');
  }
  fclose(f);
  return bias;
}

/***************************************************************/

tagger::tagger(const std::string& model) : stk(), sw(0)
{
  std::string name;
  flow = "LR";
  taggerShowScoresFlag = false;
  taggerShowCommentsFlag = true;
  taggerNumLaps = 1;
  taggerKFilter = 0;
  taggerUFilter = 0;
  taggerStrategy = STRA_1P_DEFAULT; //modstrat //0;
  taggerWinIndex = -1;
  taggerWinLength = -1;
  taggerModelName = model;
  taggerBackupDict ="";
}

/***************************************************************/

void tagger::taggerLoadModels(models_t *model, int taggerNumModel)
{
  std::string flow2,flow1;
  std::ostringstream name;
  
  //Cargamos la lista de "features" para palabras conocidas
  name << taggerModelName << ".A" << taggerNumModel;
  if (verbose)  std::cerr << std::endl << "Loading FEATURES FOR KNOWN WORDS from < "<<name.str()<<" >"<< std::endl;
  createFeatureList(name.str(),&model->featureList);
  //Cargamos la lista de "features" para palabras desconocidas
  name.str("");
  name << taggerModelName << ".A" << taggerNumModel << ".UNK";
  if (verbose)  std::cerr << std::endl << "Loading FEATURES FOR UNKNOWN WORDS from < "<<name.str()<<" >"<< std::endl;
  createFeatureList(name.str(),&model->featureListUnk);

  if (flow == "LRL")
  {
    flow1 = "LR"; flow2 = "RL";

    name.str("");
    name << flow1 << " (Right-to-Left)";
    if (verbose) std::cerr << std::endl <<"READING MODELS < direction =  "<<name.str()<<" >"<<std::endl;

    name.str("");
    name << taggerModelName << ".M"<<taggerNumModel<<"."<<flow2<<".MRG";
    if (verbose) std::cerr<<"-. Loading MERGED MODEL FOR KNOWN WORDS from < "<<name.str()<<" >"<< std::endl;
    model->wr2 = new weightRepository(name.str(),taggerKFilter);

    name.str("");
    name << taggerModelName << ".UNK.M"<<taggerNumModel<<"."<<flow2<<".MRG";
    if (verbose) std::cerr <<"-. Loading MERGED MODEL FOR UNKKNOWN WORDS from < "<<name.str()<<" >"<<std::endl<<std::endl;
    model->wrUnk2 = new weightRepository(name.str(),taggerUFilter);
  }
  else flow1 =flow;

  name.str("");
  if (flow1 =="RL") name <<flow1<<" (Right-to-Left)";
  else name << flow1 << " (Left-to-Right)";

  if (verbose) std::cerr<<std::endl<<"READING MODELS < direction =  "<<name.str()<<" >"<<std::endl;

  name.str("");
  name << taggerModelName << ".M"<<taggerNumModel<<"."<<flow1<<".MRG";
  if (verbose) std::cerr<<"-. Loading MERGED MODEL FOR KNOWN WORDS from < "<<name.str()<<" >"<<std::endl;
  model->wr = new weightRepository(name.str(),taggerKFilter);

  name.str("");
  name << taggerModelName << ".UNK.M"<<taggerNumModel<<"."<<flow1<<".MRG";
  if (verbose) std::cerr<<"-. Loading MERGED MODEL FOR UNKNOWN WORDS from < "<<name.str()<<" >"<<std::endl;
  model->wrUnk = new weightRepository(name.str(),taggerUFilter);

}

/***************************************************************/

void tagger::taggerLoadModelsForTagging()
{
  startUpTime = times(&tbuffStartUp);

  int modelsNeeded=1;

  std::string name = taggerModelName + ".DICT";
  if (!taggerBackupDict.empty())
  {
    if (verbose) std::cerr << "Loading DICTIONARY from < "<<name<<" > with BACKUP DICTIONARY from < "<<taggerBackupDict<<" >"<< std::endl;
    d  = new dictionary(name,taggerBackupDict);
  }
  else
  {
    if (verbose) std::cerr<<"Loading DICTIONARY from < "<<name<<" >"<<std::endl;
    d  = new dictionary(name);
  }

  name = taggerModelName + ".UNKP";
  if (verbose) std::cerr << "Loading UNKNOWN WORDS POS from < "<<name<<" >"<<std::endl;

  if ( taggerStrategy == STRA_2P_RELABELING //modstrat 1 
       || taggerStrategy == STRA_1P_ROBUST_UNK /*modstrat 4*/ )  modelsNeeded = 2;

  taggerModelList = new models_t[modelsNeeded];
  taggerModelRunning = &taggerModelList[0];

  if (taggerStrategy == STRA_1P_DEFAULT ) //modstrat 0)  
    taggerLoadModels(taggerModelRunning,0);
  else if (taggerStrategy == STRA_1P_UNSUPERVISED ) //modstrat 2)  
    taggerLoadModels(taggerModelRunning,3);
  else if (taggerStrategy ==  STRA_1P_ROBUST_UNK ) //modstrat 4)
    { 
        taggerLoadModels(taggerModelRunning,0);
        taggerLoadModels(&taggerModelList[1],2);
    }
  else if (taggerStrategy == STRA_1P_VERY_ROBUST_UNK ) //modstrat 5)  
    taggerLoadModels(taggerModelRunning,4);
     else  if (taggerStrategy == STRA_2P_RELABELING )//modstrat 1)
    { 
        taggerLoadModels(taggerModelRunning,2);
        taggerLoadModels(&taggerModelList[1],1);
        taggerNumLaps = 2;
    }
  else
    {  
      std::cerr<<"Execution error: Strategy "<<taggerStrategy<<" doesn't exist!!"<<std::endl<<std::endl;
      exit(0);
    }

  endStartUpTime = times(&tbuffEndStartUp);
}

void tagger::taggerInit(std::istream& input, std::ostream& output)
{
  //  int modelsNeeded=1;
  std::string name;

  if (sw != 0) delete sw;
  //Mirar si existe fichero .WIN  
  if (taggerWinIndex==-1 && taggerWinLength==-1)
  {
    name = taggerModelName + ".WIN";
    FILE *f =  fopen (name.c_str(),"r");
    if ( f == NULL ) sw = new swindow(input, &output, d);
    else
    {
      fscanf(f,"%d %d",&taggerWinLength,&taggerWinIndex);
      fclose(f);
      sw = new swindow(input,taggerWinLength,taggerWinIndex, &output, d);
    }
  }
  else if (taggerWinIndex==-1) sw = new swindow (input,taggerWinLength, &output, d);
  else sw = new swindow (input,taggerWinLength,taggerWinIndex, &output, d);
}

void tagger::taggerInit()
{
  if (sw != NULL) delete sw;
  sw = new swindow(5, d);
}

/***************************************************************/

tagger::~tagger()
{
  int modelsNeeded=1;

  if (taggerStrategy == STRA_2P_RELABELING ) //modstrat 1)  
    modelsNeeded = 2;

  delete d;
  delete sw;

  for (int i=0;i<modelsNeeded;i++)
  {

    delete taggerModelList[i].wr;
    delete taggerModelList[i].wrUnk;
    destroyFeatureList(&taggerModelList[i].featureList);
    destroyFeatureList(&taggerModelList[i].featureListUnk);

    if (flow =="LRL")
    {
      delete  taggerModelList[i].wr2;
      delete  taggerModelList[i].wrUnk2;
    }
  }
  delete[] taggerModelList;
}

/***************************************************************/
/***************************************************************/


void tagger::taggerShowComments()
{ 
  taggerShowCommentsFlag = true;
}

/***************************************************************/

void tagger::taggerShowNoComments()
{ 
  taggerShowCommentsFlag = false;
}


/***************************************************************/


void tagger::taggerActiveShowScoresFlag()
{ 
  taggerShowScoresFlag = true;
}

/***************************************************************/

void tagger::taggerDesactiveShowScoresFlag()
{ 
  this->taggerShowScoresFlag = false;
}

/***************************************************************/

void tagger::taggerPutFlow(const std::string& inFlow)
{
  flow = inFlow;
}

/***************************************************************/

void tagger::taggerPutStrategy(int num)
{
   taggerStrategy = num;
}

/***************************************************************/

void tagger::taggerPutWinLength(int l)
{
  taggerWinLength = l;
}

/***************************************************************/

void tagger::taggerPutWinIndex(int i)
{
  taggerWinIndex = i;
}

/***************************************************************/

void tagger::taggerPutBackupDictionary(const std::string& dictName)
{
  taggerBackupDict = dictName;
}

/***************************************************************/

void tagger::taggerPutKWeightFilter(float kfilter)
{
  taggerKFilter = kfilter;
}

/***************************************************************/

void tagger::taggerPutUWeightFilter(float ufilter)
{
  taggerUFilter = ufilter;
}

/***************************************************************/
/***************************************************************/
/***************************************************************/
/***************************************************************/

int tagger::taggerRightSenseSpecialForUnknown()
{
  int cont=1;

  while(sw->previous());
  nodo *elem = sw->getIndex();

  if (sw->winExistUnkWord(1,d)==-1)
    taggerModelRunning=&taggerModelList[1];
  else taggerModelRunning=&taggerModelList[0];

  taggerGenerateScore(elem,1);

  while(sw->next())
    {
      elem = sw->getIndex();

      if (sw->winExistUnkWord(1,d)==-1)
  taggerModelRunning=&taggerModelList[1];
      else taggerModelRunning=&taggerModelList[0];

      taggerGenerateScore(elem,1);
      cont++;
    }

  if (flow == "LRL") sw->winMaterializePOSValues(1);

  return cont;
}

/***************************************************************/

int tagger::taggerLeftSenseSpecialForUnknown()
{
  int cont=1;
  while(sw->next());
  nodo *elem = sw->getIndex();
  if (sw->winExistUnkWord(2,d)==-1)
    taggerModelRunning=&taggerModelList[1];
  else taggerModelRunning=&taggerModelList[0];

  taggerGenerateScore(elem,2);

  while(sw->previous())
    {
      elem = sw->getIndex();

      if (sw->winExistUnkWord(2,d)==-1)
  taggerModelRunning=&taggerModelList[1];
      else taggerModelRunning=&taggerModelList[0];

      taggerGenerateScore(elem,2);
      cont++;
    }

  if (flow=="LRL") sw->winMaterializePOSValues(0);
  return cont;
}

/***************************************************************/

int tagger::taggerRightSense()
{
  int cont=1;

  while(sw->previous());
  nodo *elem = sw->getIndex();
  if (elem == 0)
  {
    std::cerr << "tagger::taggerRightSense: ERROR index null at beginning" << std::endl;
    return -1;
  }
  taggerGenerateScore(elem,1);

  while(sw->next())
  {
    elem = sw->getIndex();
    taggerGenerateScore(elem,1);
    cont++;
  }

  if (flow =="LRL") sw->winMaterializePOSValues(1);

  return cont;
}

/***************************************************************/

int tagger::taggerLeftSense()
{
  int cont=1;
  while(sw->next());
  nodo *elem = sw->getIndex();
  taggerGenerateScore(elem,2);

  while(sw->previous())
  {
    elem = sw->getIndex();
    taggerGenerateScore(elem,2);
    cont++;
  }

  if (flow =="LRL") sw->winMaterializePOSValues(0);
  return cont;
}

/***************************************************************/

void tagger::taggerRun()
{
  int contWords=0,contSentences=0;

  struct  tms     tbuff1,tbuff2;
  clock_t start,end;
  start = times(&tbuff1);

  switch(taggerStrategy)
  {
    case STRA_1P_DEFAULT/*modstrat 0*/: taggerDoNormal(&contWords,&contSentences); break;
    case STRA_2P_RELABELING/*modstrat 1*/: taggerDoNTimes(&contWords,&contSentences,taggerNumLaps); break;
    case STRA_1P_UNSUPERVISED/*modstrat 2*/: taggerDoNormal(&contWords,&contSentences); break;
    case STRA_1P_SENTENCE_LEVEL/*modstrat 3*/: /*taggerDoNTimes(&contWords,&contSentences,taggerNumLaps);*/ break;
    case STRA_1P_ROBUST_UNK/*modstrat 4*/: taggerDoSpecialForUnknown(&contWords,&contSentences); break;
    case STRA_1P_VERY_ROBUST_UNK/*modstrat 5*/: taggerDoNormal(&contWords,&contSentences); break;
    case STRA_1P_ROBUST_SENTENCE_LEVEL: break;
  }
  end = times(&tbuff2);


  if (verbose)
    { taggerShowVerbose(contSentences,1);

    std::cerr<<"* -------------------------------------------------------------------"<<std::endl;
    showTime("Start Up Time",
       ((double)(endStartUpTime-startUpTime))/CLOCKS_PER_SECOND, 
       ((double)tbuffEndStartUp.tms_utime-(double)tbuffStartUp.tms_utime)/CLOCKS_PER_SECOND,
       ((double)tbuffEndStartUp.tms_stime-(double)tbuffStartUp.tms_stime)/CLOCKS_PER_SECOND);
    std::cerr<<"* -------------------------------------------------------------------"<<std::endl;
    showTime("Features Extraction Time",realFexTime,usrFexTime,sysFexTime);
    showTime("SVM Time",realSVMTime,usrSVMTime,sysSVMTime);
    showTime("Process Time",((double)(end-start))/CLOCKS_PER_SECOND - realFexTime - realSVMTime,
       ((double)tbuff2.tms_utime-(double)tbuff1.tms_utime)/CLOCKS_PER_SECOND - usrFexTime -usrSVMTime,
       ((double)tbuff2.tms_stime-(double)tbuff1.tms_stime)/CLOCKS_PER_SECOND - sysFexTime -sysSVMTime);
    std::cerr<<"* -------------------------------------------------------------------"<<std::endl;
    std::cerr<<"[ Tagging Time = Feature Extraction Time + SVM Time + Process Time ]"<<std::endl;
    showTime("Tagging Time",((double)(end-start))/CLOCKS_PER_SECOND, 
       ((double)tbuff2.tms_utime-(double)tbuff1.tms_utime)/CLOCKS_PER_SECOND,
       ((double)tbuff2.tms_stime-(double)tbuff1.tms_stime)/CLOCKS_PER_SECOND);
    std::cerr<<"* -------------------------------------------------------------------"<<std::endl;
    std::cerr<<"[ Overall Time = Start up Time + Tagging Time ]"<<std::endl;
    showTime("Overall Time",((double)(end-start+endStartUpTime-startUpTime))/CLOCKS_PER_SECOND,
       ((double)tbuff2.tms_utime-(double)tbuff1.tms_utime+
        (double)tbuffEndStartUp.tms_utime-(double)tbuffStartUp.tms_utime)/CLOCKS_PER_SECOND,
       ((double)tbuff2.tms_stime-(double)tbuff1.tms_stime+
        (double)tbuffEndStartUp.tms_stime-(double)tbuffStartUp.tms_stime)/CLOCKS_PER_SECOND);
        std::cerr<<"* -------------------------------------------------------------------"<<std::endl;
    taggerStadistics(contWords,contSentences,
         ((double)(end-start))/CLOCKS_PER_SECOND, 
         ((double)tbuff2.tms_utime-(double)tbuff1.tms_utime)/CLOCKS_PER_SECOND,
         ((double)tbuff2.tms_stime-(double)tbuff1.tms_stime)/CLOCKS_PER_SECOND);
    }
}

/***************************************************************/

void tagger::taggerDoNormal(int *numWords, int *numSentences)
{
  int contWordsLR=0,contWordsRL=0,contSentences=0,ret = 1;

  while (ret!=-2)
  {
    if (verbose) taggerShowVerbose(contSentences,0);

    if ((flow=="LRL") || (flow =="LR"))
      contWordsLR = contWordsLR+taggerRightSense();
    if ((flow =="LRL") || (flow == "RL"))
      contWordsRL = contWordsRL+taggerLeftSense();
    contSentences++;
    sw->show(taggerShowScoresFlag, taggerShowCommentsFlag);
    sw->deleteList();
    ret = sw->iniGeneric(d);
  }
  if (contWordsRL==0) *numWords=contWordsLR/taggerNumLaps;
  else *numWords=contWordsRL/taggerNumLaps;
  *numSentences = contSentences;
}

/***************************************************************/

void tagger::taggerDoSpecialForUnknown(int *numWords, int *numSentences)
{
  int contWordsLR=0,contWordsRL=0,contSentences=0,ret = 1;

  while (ret != -2)
  {
    if (verbose) taggerShowVerbose(contSentences,0);

    if ((flow == "LRL") || (flow =="LR"))
      contWordsLR = contWordsLR+taggerRightSenseSpecialForUnknown();
    if ((flow == "LRL") || (flow =="RL"))
      contWordsRL = contWordsRL+taggerLeftSenseSpecialForUnknown();

    contSentences++;
    sw->show(taggerShowScoresFlag, taggerShowCommentsFlag);
    sw->deleteList();
    ret = sw->iniGeneric(d);
  }
  if (contWordsRL==0) *numWords=contWordsLR/taggerNumLaps;
  else *numWords=contWordsRL/taggerNumLaps;
  *numSentences = contSentences;
}

/***************************************************************/

void tagger::taggerDoNTimes(int *numWords, int *numSentences,int laps)
{
  int contWordsLR=0,contWordsRL=0,contSentences=0,ret = 1;

  while (ret != -2)
  {
    if (verbose) taggerShowVerbose(contSentences,0);

    for (int pasadas=0;pasadas<laps;pasadas++)
    {
      taggerModelRunning = &taggerModelList[pasadas];
      if ((flow == "LRL") || (flow == "LR"))
        contWordsLR = contWordsLR+taggerRightSense();
      if (flow == "LRL" && pasadas>0)
        sw->winMaterializePOSValues(2);
      if ((flow == "LRL") || (flow =="RL"))
        contWordsRL = contWordsRL+taggerLeftSense();
    }

    contSentences++;
    sw->show(taggerShowScoresFlag, taggerShowCommentsFlag);
    sw->deleteList();
    ret = sw->iniGeneric(d);
  }
  if (contWordsRL==0) *numWords=contWordsLR/taggerNumLaps;
  else *numWords=contWordsRL/taggerNumLaps;
  *numSentences = contSentences;
}

/***************************************************************/

void tagger::taggerGenerateScore(nodo *elem,int direction)
{

  struct  tms tbuffStartFex,tbuffEndFex;
  clock_t startFexTime,endFexTime;
  struct  tms tbuffStartSVM,tbuffEndSVM;
  clock_t startSVMTime,endSVMTime;

  std::vector<weight_node_t> weight;
  weightRepository *weightRep;
  unsigned int numMaybe;
  int max=0;
  int is_unk=FALSE;
  simpleList<nodo_feature_list*> *featureList;

  startFexTime = times(&tbuffStartFex);

  dataDict* i = d->getElement(elem->wrd);
  if (!userWeight.empty()) {
    weight = userWeight;
    featureList = &taggerModelRunning->featureList;
    weightRep = taggerModelRunning->wr;
    numMaybe = weight.size();
  } else if ((long)i!=HASH_FAIL) {
    featureList = &taggerModelRunning->featureList;
    numMaybe = d->getElementNumMaybe(i);
    weight = taggerCreateWeightNodeArray(numMaybe,i);
    if ((flow =="LRL") && (direction==2))
    {
      weightRep = taggerModelRunning->wr2;  //wr2;
      //bias = taggerModelRunning->bias2;  //taggerBias2;
    }
    else
    {
      weightRep = taggerModelRunning->wr; //wr;
      //bias = taggerModelRunning->bias; //taggerBias;
    }
  }
  else
  {
    featureList = &taggerModelRunning->featureListUnk;
    weight = taggerCreateWeightUnkArray(&numMaybe);
    is_unk = TRUE;

    if (flow =="LRL" && (direction==2))
    {
      weightRep = taggerModelRunning->wrUnk2;  //wrUnk2;
      //bias =  taggerModelRunning->biasUnk2; //taggerBiasUnk2;
    }
    else
    { weightRep = taggerModelRunning->wrUnk; //wrUnk;
      //bias =taggerModelRunning->biasUnk; //taggerBiasUnk;
    }
  }

  assert(numMaybe == weight.size());
  numMaybe = weight.size();

  if (numMaybe>1)
  {
    bool ret = true;
    while (ret)
    {
      nodo_feature_list* aux= *featureList->getIndex();
      if (aux->mark == SLASTW)  sw->winPushSwnFeature(stk);
      else if (aux->mark == WMARK)  sw->winPushWordFeature((void *)aux,d,stk,direction);
      else if (aux->mark == KMARK)  sw->winPushAmbiguityFeature((void *)aux,d,stk,direction);
      else if (aux->mark == MMARK)  sw->winPushMaybeFeature((void *)aux,d,stk,direction);
      else if (aux->mark == PMARK)  sw->winPushPosFeature((void *)aux,d,stk,direction);
      else if (aux->mark == MFTMARK)  sw->winPushMFTFeature((void *)aux,d,stk,direction);
      else if (is_unk==TRUE)
      {
        int *param = NULL;
        // when aux->l is empty, our feature has no parameters (eg. "lower
        // case"), so param can be left unitialized.
        if (!aux->l.isEmpty())
        {
          param = *aux->l.getIndex();
        }
        if (aux->mark == PREFIX_MARK)  sw->winPushPrefixFeature(elem->wrd, stk, *param);
        else if (aux->mark == SUFFIX_MARK) sw->winPushSuffixFeature(elem->wrd, stk, *param);
        else if (aux->mark == CHAR_A_MARK) sw->winPushLetterFeature(elem->wrd, stk, COUNTING_FROM_BEGIN, *param);
        else if (aux->mark == CHAR_Z_MARK) sw->winPushLetterFeature(elem->wrd, stk, COUNTING_FROM_END, *param);
        else if (aux->mark == LENGTH_MARK) sw->winPushLenghtFeature(elem->wrd,stk);
        else if (aux->mark == START_CAPITAL_MARK) sw->winPushStartWithCapFeature(elem->wrd,stk);
        else if (aux->mark == START_LOWER_MARK)  sw->winPushStartWithLowerFeature(elem->wrd,stk);
        else if (aux->mark == START_NUMBER_MARK) sw->winPushStartWithNumberFeature(elem->wrd,stk);
        else if (aux->mark == ALL_UPPER_MARK) sw->winPushAllUpFeature(elem->wrd,stk);
        else if (aux->mark == ALL_LOWER_MARK) sw->winPushAllLowFeature(elem->wrd,stk);
        else if (aux->mark == CONTAIN_CAP_MARK) sw->winPushContainCapFeature(elem->wrd, stk);
        else if (aux->mark == CONTAIN_CAPS_MARK) sw->winPushContainCapsFeature(elem->wrd, stk);
        else if (aux->mark == CONTAIN_COMMA_MARK) sw->winPushContainCommaFeature(elem->wrd, stk);
        else if (aux->mark == CONTAIN_NUMBER_MARK) sw->winPushContainNumFeature(elem->wrd, stk);
        else if (aux->mark == CONTAIN_PERIOD_MARK) sw->winPushContainPeriodFeature(elem->wrd, stk);
        else if (aux->mark == MULTIWORD_MARK) sw->winPushMultiwordFeature(elem->wrd, stk);
      }
      ret = featureList->next();
    }
    featureList->setFirst();

    endFexTime = times(&tbuffEndFex);
    realFexTime = realFexTime + ((double)(endFexTime-startFexTime))/CLOCKS_PER_SECOND;
    usrFexTime = usrFexTime + (((double)tbuffEndFex.tms_utime-(double)tbuffStartFex.tms_utime)/CLOCKS_PER_SECOND);
    sysFexTime = sysFexTime + (((double)tbuffEndFex.tms_stime-(double)tbuffStartFex.tms_stime)/CLOCKS_PER_SECOND);

    startSVMTime = times(&tbuffStartSVM);


    elem->strScores = taggerSumWeight(weightRep,weight,numMaybe,&max);

    endSVMTime = times(&tbuffEndSVM);
    realSVMTime = realSVMTime + ((double)(endSVMTime-startSVMTime))/CLOCKS_PER_SECOND;
    usrSVMTime = usrSVMTime + (((double)tbuffEndSVM.tms_utime-(double)tbuffStartSVM.tms_utime)/CLOCKS_PER_SECOND);
    sysSVMTime = sysSVMTime + (((double)tbuffEndSVM.tms_stime-(double)tbuffStartSVM.tms_stime)/CLOCKS_PER_SECOND);
  }


  elem->pos = weight[max].pos;
  elem->weight = weight[max].data;
  
  if (flow =="LRL")
  {
    weight_node_t* score = new weight_node_t();
    score->data = weight[max].data;
    score->pos = weight[max].pos;
    elem->stackScores.push(score);
  }

}



/***************************************************************/

/* Returns an array ready to be filled with maybe informations */
std::vector<weight_node_t> tagger::taggerCreateWeightNodeArray(unsigned int numMaybe,dataDict* index)
{
  unsigned int j = numMaybe;
  std::vector<weight_node_t> weight(numMaybe);
  simpleList<infoDict*> *list = &d->getElementMaybe(index);
  
  bool ret=true;
  while (ret && numMaybe > 0)
    {
      infoDict *pInfoDict = *list->getIndex();
      j--;
      weight[j].pos = pInfoDict->pos;
      weight[j].data = 0;
      ret=list->next();
    }

  list->setFirst();
  assert(1 > 0);
  return weight;
}


void tagger::setPossibles(const std::vector<std::string> &possibles) {
  userWeight.clear();
  userWeight.reserve(possibles.size());
  for(vector<std::string>::const_iterator it = possibles.begin(); it != possibles.end(); it++) {
    weight_node_t w(*it);
    userWeight.push_back(w);
  }
}


/***************************************************************/

std::string tagger::taggerSumWeight(weightRepository* wRep, std::vector<weight_node_t> &weight, unsigned int numMaybe, int* max)
{
//   weight_node_t *aux;
  long double w,b = 0;
  std::string feature;
  int putBias=1;
  while (!stk.empty())
  {
    *max=0;
    feature = stk.top();
    stk.pop();
    for (unsigned int j=0; j<numMaybe;j++)
    {
      if (putBias)
        {
          b = wRep->wrGetWeight("BIASES",weight[j].pos);
          weight[j].data = weight[j].data - b;
        }
      w = wRep->wrGetWeight(feature,weight[j].pos);
      weight[j].data=weight[j].data+w;
      if (weight[*max].data < weight[j].data) {
        *max=j;
      }
    }
    putBias=0;
  }
  std::ostringstream tmp;
  if ( true )
  {
    for (unsigned int i=0; i<numMaybe; i++)
    {
      if ( i == 0 )
      {
        tmp << weight[i].pos << "_" << (float)weight[i].data;
      }
      else
      {
        tmp << " " << weight[i].pos << "_" << (float)weight[i].data;
      }
    }
  }
  return tmp.str();
}

/***************************************************************/
/***************************************************************/

std::vector<weight_node_t> tagger::taggerCreateWeightUnkArray(unsigned int *numMaybe)
{
  unsigned int i=0;
  char c=' ';
  FILE *f;
  std::string name = taggerModelName + ".UNKP";

  *numMaybe = 0;

  if ((f = fopen(name.c_str(), "rt"))== NULL)
    {
      std::cerr << "Error opening file: "<<name<<std::endl;
      exit(0);
    }

  // first read: count the number of POS
  while (!feof(f))
    { if (fgetc(f)=='\n') (*numMaybe)++;
    }

  // second read, fill in the weight nodes
  fseek(f,0,SEEK_SET);

  std::vector<weight_node_t> weight(*numMaybe);
  while (!feof(f) && (i<*numMaybe))
    { weight[i].pos = "";
    weight[i].data=0;
    c = fgetc(f);
    while ((c!='\n') && (!feof(f)))
      {
  if (c!=' ' && c!='\n' && c!='\t') weight[i].pos += c;
  c = fgetc(f);
      }
    i++;
    }
  fclose(f);
  return weight;
}

/***************************************************************/
/***************************************************************/

void tagger::taggerStadistics(int numWords, int numSentences, double /*realTime*/, double usrTime, double sysTime)
{
  std::ostringstream message;
  float media=0;
  if ((sysTime+usrTime)!=0)  media = (float) (((double) numWords)/(sysTime+usrTime));

  message << std::endl << numSentences <<" sentences were tagged." << std::endl;
  message << numWords << " words were tagged." << std::endl;
  message << media << " words/second were tagged." << std::endl;
  std::cerr << message.str();
}

/***************************************************************/

void tagger::taggerShowVerbose(int num,int isEnd)
{
  if (isEnd) { std::cerr<<"."<<num<<" sentences [DONE]"<<std::endl<<std::endl; return; }
  else if (num%100==0) std::cerr <<num;
  else if (num%10==0) std::cerr << ".";
}


