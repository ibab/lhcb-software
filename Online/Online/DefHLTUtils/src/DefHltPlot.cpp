/*
 * DefHltPlot.cpp
 *
 *  Created on: Aug 21, 2012
 *      Author: Beat Jost
 */

#include "TH1F.h"
#include "TApplication.h"
#include "TRint.h"
#include "dic.hxx"
#include "Gaucho/Utilities.h"
#include <string>
#include <map>

class buffer
{
  public:
    std::string name;
    int prod,seen;
    float prodr,seenr;
    buffer()
    {
      prod=seen =0;
      prodr=seenr=0.0;
    }
    void fill(std::string &n,std::string &p,std::string &s, std::string rp, std::string rs)
    {
      name = n;
      sscanf(p.c_str(),"%d",&prod);
      sscanf(s.c_str(),"%d",&seen);
      sscanf(rp.c_str(),"%f",&prodr);
      sscanf(rs.c_str(),"%f",&seenr);
    }
    buffer(std::string &n,std::string &p,std::string &s, std::string rp, std::string rs)
    {
      fill(n,p,s,rp,rs);
    }
};
class node
{
  public:
    std::string name;
    float perf;
    float perf_noovr;
    node(std::string n)
    {
      name = n;
    }
};

typedef std::map<std::string,node*> NodeMap;
NodeMap Nodes;

class BufferInfo : public DimInfo
{
  public:
    TH1F *m_histo;
    TH1F *m_histo1;
    BufferInfo(char *name,TApplication *);
    void infoHandler();
    buffer Events;
    buffer Overflow;
    TApplication *m_appl;
};

BufferInfo::BufferInfo(char *name, TApplication *a) : DimInfo(name,(char*)"\0")
{
  m_histo = new TH1F("PDistribution","Node Performance Distribution",200,500.0,700.0);
  m_histo1 = new TH1F("PDistribution1","Node Performance Distribution (All)",200,500.0,700.0);
  m_appl = a;
}

void BufferInfo::infoHandler()
{
  char *input = getString();
  int len = this->getSize();
  if (len <=1) return;
  dyn_string *hosts =Strsplit(input, (char *)"|");
  m_histo->Reset();
  m_histo1->Reset();
  NodeMap::iterator nit ;
  for (size_t i = 0;i<hosts->size();i++)
  {
    if (hosts->at(i).size() <=0 ) continue;
    dyn_string *host = Strsplit(hosts->at(i).c_str()," ");
    if (host->size() <=0 ) continue;
    node *nod;
    nit = Nodes.find(host->at(0));
    if (nit == Nodes.end())
    {
      nod = new node(host->at(0));
      Nodes[host->at(0)] = nod;
    }
    else
    {
      nod = (*nit).second;
    }
    dyn_string *buffers = Strsplit(host->at(1).c_str(),",");
    if (buffers->size() <=0 ) continue;
    dyn_string *bvalues = Strsplit(buffers->at(0).c_str(),"/");
    if (bvalues->size() <=0 ) continue;
    if (bvalues->size()<5) continue;
    Events.fill(bvalues->at(0),bvalues->at(1),bvalues->at(2),bvalues->at(3),bvalues->at(4));
    bvalues = Strsplit(buffers->at(1).c_str(),"/");
    if (bvalues->size()<5) continue;
    Overflow.fill(bvalues->at(0),bvalues->at(1),bvalues->at(2),bvalues->at(3),bvalues->at(4));
    if (Overflow.prod >0)
    {
      NodeMap::iterator nit = Nodes.find(host->at(0));
      if (nit == Nodes.end())
      {
        nod = new node(host->at(0));
        Nodes[host->at(0)] = nod;
      }
      else
      {
        nod = (*nit).second;
      }
      float perf = Events.prodr;
      m_histo1->Fill(13*perf);
      if (Overflow.prodr >0.0)
      {
        nod->perf = 13*perf;
        m_histo->Fill(13*perf);
      }
      else
      {
        nod->perf_noovr = 13*perf;
      }
    }
  }
  m_histo->Draw();
  m_histo1->Draw();
  double mean = m_histo->GetMean();
  double rms = m_histo->GetRMS();
  double low = mean-2.5*rms;
  printf ("Average %f, RMS %f, low-level cut off %f\n",mean,rms,low);
  for (nit = Nodes.begin();nit!=Nodes.end();nit++)
  {
    node *nod = (*nit).second;
    if ((nod->perf < low) && (nod->perf >1.0))
    {
      printf("%s %f %f\n",nod->name.c_str(),nod->perf,nod->perf_noovr);
    }
  }
//  m_appl->Run(kTRUE);
}

int main(int,char **)
{
//  TApplication a("Appl",0,0);
  int dum=0;
  TRint rint("Rint",&dum,0);
  BufferInfo b((char *)"HLTFileEqualizer/NodesBuffersEvents",&rint);
//  sleep(100000);
  rint.Run();
  return 0;
}

