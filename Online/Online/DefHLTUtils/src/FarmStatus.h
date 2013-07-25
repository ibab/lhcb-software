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
#include "dim/dic.hxx"
#include "dim/dis.hxx"
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
class MyNode
{
  public:
    std::string m_name;
    std::string m_subfarm;
    int m_state;
    int m_badtasks;
    int m_badconns;
    bool m_excl;
    MyNode(std::string n)
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
    std::string m_svcnam;

};
typedef ROMon::SubfarmSummary _SFSumm;
class StatusInfoHandler;
class MBMInfoHandler;
typedef std::map<std::string,MyNode*> myNodeMap;
typedef std::map<std::string,std::list<std::pair<std::string,int> > > myActionMap; //list of nodes per subfarm to execute an action on.
typedef std::set<std::string> NodeSet;
typedef std::map<std::string,float> NodePerfMap;
class FarmStatus
{
  public:
    std::map<std::string,SFarm *> m_Farms;
    myNodeMap m_Nodes;
    myNodeMap m_AllNodes;
    nodemap M_PMap;
    std::map<std::string,DimInfo*> m_infoMap;
    StatusInfoHandler *m_InfoHandler;
    MBMInfoHandler *m_MBMInfoHandler;
    int m_nnodes;
    int m_nfiles;
    long m_nfiles2;
    int m_low;
    int m_high;
    DimInfo *m_DefStateInfo;
    DimService *m_StatusService;
    std::string m_servdat;
    NodeSet m_enabledFarm;
    NodeSet m_recvNodes;
    NodeSet m_everrecvd;
    NodeSet m_BufferrecvNodes;
    NodeSet m_exclNodes;
    std::set<std::string> m_AllpFarms;
    std::set<std::string> m_AllpNodes;
    NodePerfMap m_nodePerf;
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

class LHCb1RunStatus : public DimInfo
{
  public:
    FarmStatus *m_equalizer;
    int m_nolink;
    int m_state;
    LHCb1RunStatus(char *name, int nolink,FarmStatus *e);
    void infoHandler();
};

class ExclInfo : public DimInfo
{
  public:
    NodeSet *m_exclNodes;
    ExclInfo(char *name, NodeSet *nodeset);
    void infoHandler();
};



#endif /* FARMSTATUS_H */
