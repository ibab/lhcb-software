/*
 * HLTFileEqualizer.h
 *
 *  Created on: Jun 8, 2012
 *      Author: Beat Jost
 */

#ifndef FARMSTATUS_H
#define FARMSTATUS_H
#include <map>
#include <string>
#include <list>
#include <set>
#include "dic.hxx"
#include "dis.hxx"
#define MBM_IMPLEMENTATION
#include "ROMon/ROMon.h"
#include "ROMon/CPUMon.h"
#include "stdio.h"
#include "time.h"
#include "ROMon/TaskSupervisorParser.h"
#include <iostream>
#include <sstream>

using namespace ROMon;
using namespace std;
typedef map<int,int> RunMap;
//class MBMStat
//{
//  public:
//    string name;
//    int produced;
//    int seen;
//    float p_rate;
//    float s_rate;
//    MBMStat()
//    {
//      produced = 0;
//      seen = 0;
//      p_rate = 0.0;
//      s_rate = 0.0;
//    };
//    MBMStat(const MBMStat & c): name(c.name),produced(c.produced),seen(c.seen),p_rate(c.p_rate),s_rate(c.s_rate)
//    {
//    };
//    void calcRate(MBMStat &p,long dtime)
//    {
//      double delta = dtime;
//      if (produced >= p.produced)
//      {
//        p_rate = 1000.0*float(produced-p.produced)/delta;
//      }
//      else
//      {
//        p_rate = 0.0;
//      }
//      if (seen >=p.seen)
//      {
//        s_rate = 1000.0*float(seen-p.seen)/delta;
//      }
//      else
//      {
//        s_rate = 0.0;
//      }
//    }
//};

class MyNode
{
  public:
    string m_name;
    string m_subfarm;
    int m_state;
    int m_badtasks;
    int m_badconns;
    bool m_excl;
    MyNode(string n)
    {
      m_name = n;
      m_state = 0;
      m_badtasks = 0;
      m_badconns = 0;
      m_excl = false;
    };
};
class SFarm
{
  public:
    string m_svcnam;

};
typedef ROMon::SubfarmSummary _SFSumm;
class StatusInfoHandler;
class MBMInfoHandler;
//class Inventory;
class TaskSummaryInfoHandler;
typedef map<string,MyNode*> myNodeMap;
typedef map<string,list<pair<string,int> > > myActionMap; //list of nodes per subfarm to execute an action on.
typedef set<string> NodeSet;
typedef set<string> StrSet;
class Dictionary
{
  public:
    vector<string> WordList;
    map<string,int> WordMap;
    size_t NumEls;
    DimService *DictService;
    string SvcCont;
    string OCont;
    Dictionary(string &svc)
    {
      NumEls = 0;
      SvcCont.clear();
      OCont.clear();
      DictService = new DimService(svc.c_str(),"C",(void*)"\0",1);
      WordMap.clear();
      WordList.clear();
    }
    void Add(string &word)
    {
      map<string,int>::iterator i;
      i = WordMap.find(word);
      if (i != WordMap.end())
      {
        return;
      }
      else
      {
        WordList.push_back(word);
        WordMap.insert(make_pair(word,NumEls));
        if (NumEls >0)
        {
          SvcCont += "|"+word+"\0";
        }
        else
        {
          SvcCont += word+"\0";
        }
        NumEls++;
      }
      return;
    }

    void Add(vector<string> &svec)
    {
      size_t i;
      for (i=0;i<svec.size();i++)
      {
        Add(svec[i]);
      }
      return;
    }

    int Find(string &word)
    {
      map<string,int>::iterator i;
      i = WordMap.find(word);
      if (i != WordMap.end())
      {
        return (*i).second;
      }
      return -1;
    }
    void Update()
    {
      if (OCont != SvcCont)
      {
        SvcCont += '\0';
        DictService->setData((void*)SvcCont.c_str(),SvcCont.size());
        DictService->updateService();
        OCont = SvcCont;
      }
    }
};
typedef map<string,float> NodePerfMap;
class FarmStatus
{
  public:
    ostringstream osStatus;
    ostringstream osDetails;
    map<string,SFarm *> m_Farms;
    myNodeMap m_Nodes;
    myNodeMap m_AllNodes;
    map<string,DimInfo*> m_infoMap;
    map<string,DimInfo*> m_TaskMap;
    map<DimInfo*,string> m_DimMap;
    StatusInfoHandler *m_InfoHandler;
    TaskSummaryInfoHandler *m_TSInfoHandler;
    int m_nnodes;
    int m_nfiles;
    long m_nfiles2;
    int m_low;
    int m_high;
    DimInfo *m_DefStateInfo;
    DimService *m_StatusService;
    DimService *m_DetailService;
    NodeSet m_enabledFarm;
    NodeSet m_recvNodes;
    NodeSet m_everrecvd;
    NodeSet m_BufferrecvNodes;
    NodeSet m_exclNodes;
    set<string> m_AllpFarms;
    NodePerfMap m_nodePerf;
    Dictionary *m_TaskDict;
    Dictionary *m_ConnDict;
    Dictionary *m_ProjDict;
    FarmStatus();
    void Analyze();
    void Dump();
    void BufferDump();
};
typedef ROMon::Nodeset _MBMSF;

class StatusInfoHandler : public DimInfoHandler
{
  public:
    SFarm *m_subfarm;
    _SFSumm *m_sfstatus;
    int m_bufsiz;
    FarmStatus *m_Equalizer;
    StatusInfoHandler(FarmStatus *e);
    void infoHandler();
};

class TaskSummaryInfoHandler : public DimInfoHandler
{
  public:
    int m_bufsiz;
    FarmStatus *m_Equalizer;
    map<DimInfo*,Cluster*> m_ClusterMap;
    map<string,Cluster*> m_CluMap;
    TaskSummaryInfoHandler(FarmStatus *e);
    void infoHandler();
};



#endif /* FARMSTATUS_H */
