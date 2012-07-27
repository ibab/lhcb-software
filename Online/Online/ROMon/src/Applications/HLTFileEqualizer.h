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
#include "dim/dic.hxx"
#include "dim/dis.hxx"
#include "ROMon/CPUMon.h"
#include "stdio.h"
#include "time.h"
using namespace ROMon;
using namespace std;
typedef ROMon::DeferredHLTSubfarmStats _S;
typedef _S::Nodes               _N;
typedef _S::Node::Runs          _R;

class myNode
{
  public:
    std::string m_name;
    std::string m_subfarm;
    int m_nofiles;
    int m_state;
    char m_ROC_state;
    myNode(std::string n)
    {
      m_name = n;
      m_state = 1;
      m_nofiles = 0;
      m_subfarm = m_name.substr(0,6);
      m_ROC_state = '?';
    };
};
class SFarm
{
  public:
    std::string m_svcnam;

};
class DefHltInfoHandler;
typedef std::map<std::string,myNode*> myNodeMap;
typedef std::map<std::string,std::list<std::pair<std::string,int> > > myActionMap; //list of nodes per subfarm to execute an action on.

class HLTFileEqualizer
{
  public:
    std::map<std::string,SFarm *> m_Farms;
    myNodeMap m_Nodes;
    myNodeMap m_AllNodes;
    std::map<std::string,DimUpdatedInfo*> m_infoMap;
    DefHltInfoHandler *m_InfoHandler;
    int m_nnodes;
    int m_nfiles;
    long m_nfiles2;
    int m_low;
    int m_high;
    DimInfo *m_DefStateInfo;
    DimService *m_NodeList;
    DimService *m_NodeListDiff;
    std::string m_servdat;
    std::string m_servdatDiff;
    std::set<std::string> m_enabledFarm;
    HLTFileEqualizer();
    void Analyze();
    void Dump();
};

class DefHltInfoHandler : public DimInfoHandler
{
  public:
    SFarm *m_subfarm;
    _S *m_sfstatus;
    int m_bufsiz;
    HLTFileEqualizer *m_Equalizer;
    DefHltInfoHandler(HLTFileEqualizer *e);
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

#endif /* HLTFILEEQUALIZER_H_ */
