/*
 * HLTFileEqualizer.h
 *
 *  Created on: Jun 8, 2012
 *      Author: Beat Jost
 */

#ifndef HLTFILEEQUALIZER_H_
#define HLTFILEEQUALIZER_H_
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
#include "HLTPerfFormatter.h"
using namespace ROMon;
using namespace std;
typedef std::map<int,int> RunMap;
class MBMStat
{
  public:
    std::string name;
    int produced;
    int seen;
    float p_rate;
    float s_rate;
    MBMStat()
    {
      produced = 0;
      seen = 0;
      p_rate = 0.0;
      s_rate = 0.0;
    };
    MBMStat(const MBMStat & c): name(c.name),produced(c.produced),seen(c.seen),p_rate(c.p_rate),s_rate(c.s_rate)
    {
    };
    void calcRate(MBMStat &p,long dtime)
    {
      double delta = dtime;
      if (produced >= p.produced)
      {
        p_rate = 1000.0*float(produced-p.produced)/delta;
      }
      else
      {
        p_rate = 0.0;
      }
      if (seen >=p.seen)
      {
        s_rate = 1000.0*float(seen-p.seen)/delta;
      }
      else
      {
        s_rate = 0.0;
      }
    }
};
class PartMBMs
{
public:
  MBMStat Events;
  MBMStat Overflow;
  MBMStat Send;
  MBMStat ProcPerf;
  MBMStat Events_prev;
  MBMStat Overflow_prev;
  MBMStat Send_prev;
  MBMStat ProcPerf_prev;
};

typedef struct
{
    int m_nofiles;
    int m_state;
    char m_ROC_state;
    RunMap m_runmap;
    bool m_excl;
    long ReadTime;
    long ReadTime_prev;
    float m_nodePerformance;
    bool m_active;
    float m_cfiles;
} NodeQuantities;
class myNode
{
  public:
    std::string m_name;
    std::string m_subfarm;
    NodeQuantities m_DefQ;
    NodeQuantities m_HLT1Q;
    PartMBMs AnyPart;
    PartMBMs LHCb2;
//    MBMStat Events;
//    MBMStat Overflow;
//    MBMStat Send;
//    MBMStat ProcPerf;
//    MBMStat Events_prev;
//    MBMStat Overflow_prev;
//    MBMStat Send_prev;
//    MBMStat ProcPerf_prev;
    myNode(std::string n)
    {
      m_name = n;
      m_DefQ.m_state = 1;
      m_DefQ.m_nofiles = 0;
      m_subfarm = m_name.substr(0,6);
      m_DefQ.m_ROC_state = '?';
      m_DefQ.m_runmap.clear();
      m_DefQ.m_excl = false;
      m_DefQ.ReadTime_prev = 0;
      m_DefQ.ReadTime = 0;
      m_DefQ.m_nodePerformance = 0.0;
      m_DefQ.m_active = false;
      m_DefQ.m_cfiles = 0.0;
      m_HLT1Q.m_state = 1;
      m_HLT1Q.m_nofiles = 0;
      m_HLT1Q.m_ROC_state = '?';
      m_HLT1Q.m_runmap.clear();
      m_HLT1Q.m_excl = false;
      m_HLT1Q.ReadTime_prev = 0;
      m_HLT1Q.ReadTime = 0;
      m_HLT1Q.m_nodePerformance = 0.0;
      m_HLT1Q.m_active = false;
      m_HLT1Q.m_cfiles = 0.0;
    };
};
class SFarm
{
  public:
    std::string m_svcnam;

};
class DefHltInfoHandler;
class HLT1InfoHandler;
class MBMInfoHandler;
typedef std::map<std::string,myNode*> myNodeMap;
typedef std::map<std::string,std::list<std::pair<std::string,int> > > myActionMap; //list of nodes per subfarm to execute an action on.
typedef std::set<std::string> NodeSet;
typedef std::map<std::string,float> NodePerfMap;
class HLTFileEqualizer
{
  public:
    std::map<std::string,SFarm *> m_Farms;
    myNodeMap m_Nodes;
    myNodeMap m_AllNodes;
    nodemap M_PMap;
    std::map<std::string,DimUpdatedInfo*> m_infoMap;
    DefHltInfoHandler *m_DefHandler;
    HLT1InfoHandler *m_HLT1Handler;
    MBMInfoHandler *m_MBMInfoHandler;
    int m_nnodes;
    int m_nfiles;
    long m_nfiles2;
    int m_low;
    int m_high;
    DimInfo *m_DefStateInfo;
    DimService *m_DefNodeList;
    DimService *m_DefNodeListDiff;
    DimService *m_DefNodesRunsFiles;
    DimService *m_DefStatServ;
    DimService *m_NodesBuffersEvents;
    DimService *m_NodesBuffersEvents_LHCb2;
    DimService *m_HLT1NodeList;
    DimService *m_HLT1NodeListDiff;
    DimService *m_HLT1NodesRunsFiles;
    std::string m_Defservdat;
    std::string m_DefservdatDiff;
    std::string m_DefservdatNodesRunsFiles;
    std::string m_HLT1servdat;
    std::string m_HLT1servdatDiff;
    std::string m_HLT1servdatNodesRunsFiles;
    std::string m_DefservdatNodesBuffersEvents;
    std::string m_DefservdatNodesBuffersEvents_LHCb2;
    NodeSet m_enabledFarm;
    NodeSet m_recvNodes;
    NodeSet m_BufferrecvNodes;
    NodeSet m_exclNodes;
    std::set<std::string> m_AllpFarms;
    std::set<std::string> m_AllpNodes;
    NodePerfMap m_nodePerf;
    HLTFileEqualizer();
    void Analyze();
    void Dump();
    void BufferDump();
};
typedef ROMon::DeferredHLTSubfarmStats _DHLTSF;
typedef ROMon::Nodeset _MBMSF;

class DefHltInfoHandler : public DimInfoHandler
{
  public:
    SFarm *m_subfarm;
    _DHLTSF *m_sfstatus;
    int m_bufsiz;
    HLTFileEqualizer *m_Equalizer;
    DefHltInfoHandler(HLTFileEqualizer *e);
    void infoHandler();
};

class HLT1InfoHandler : public DimInfoHandler
{
  public:
    SFarm *m_subfarm;
    _DHLTSF *m_sfstatus;
    int m_bufsiz;
    HLTFileEqualizer *m_Equalizer;
    HLT1InfoHandler(HLTFileEqualizer *e);
    void infoHandler();
};

class LHCb1RunStatus : public DimInfo
{
  public:
    HLTFileEqualizer *m_equalizer;
    int m_nolink;
    int m_state;
    LHCb1RunStatus(char *name, int nolink,HLTFileEqualizer *e);
    void infoHandler();
};

class ExclInfo : public DimInfo
{
  public:
    NodeSet *m_exclNodes;
    ExclInfo(char *name, NodeSet *nodeset);
    void infoHandler();
};

class MBMInfoHandler : public DimInfoHandler
{
  public:
//    SFarm *m_subfarm;
    _MBMSF *m_sfstatus;
    int m_bufsiz;
    HLTFileEqualizer *m_Equalizer;
    MBMInfoHandler(HLTFileEqualizer *e);
    void infoHandler();
};


#endif /* HLTFILEEQUALIZER_H_ */
