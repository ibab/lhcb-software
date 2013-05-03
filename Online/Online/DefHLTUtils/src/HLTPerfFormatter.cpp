/*
 * PerformanceFormatter.cpp
 *
 *  Created on: Aug 22, 2012
 *      Author: Beat Jost
 */

#include "TH1F.h"
#include "TApplication.h"
#include "TRint.h"
#include "TProfile.h"
#include "TCanvas.h"
#include "TSeqCollection.h"
#include "TAxis.h"
#include "TROOT.h"
#include "RTL/strdef.h"
#include "ROMon/Utilities.h"
#include "HLTPerfFormatter.h"
//#include <pair>
#include <stdio.h>
#include <math.h>

typedef std::set<std::string> Farmset;
typedef std::map<std::string,TProfile*> ProfMap;

//class HLTPerfFormatter
//{
//  public:
//    HLTPerfFormatter()
//    {
//    }
//    nodemap &Extract();
//    void Histogram(nodemap&);
//};
void HLTPerfFormatter::Extract(nodemap &Map)//int main(int , char **)
{
  FILE *perf;
  perf = fopen("/home/beat/hltperf.txt","r");
  if (perf == 0)
  {
    printf("Perfomance File not found. Exiting...\n");
    return;
  }
  char line[32768];
  nodemap::iterator nit;
  while (1)
  {
    fgets(line,sizeof(line),perf);
    if (feof(perf)) break;
    dyn_string *nodelist = Strsplit(line,"/");
    dyn_string *nodes;
    if (nodelist->size()>1)
    {
      nodelist->at(1) = RTL::str_lower(nodelist->at(1));
      nodes = Strsplit(nodelist->at(1).c_str(),",");
      for (size_t i=0;i<nodes->size();i++)
      {
        dyn_string *node = Strsplit(nodes->at(i).c_str()," ");
        if (node->size()<2) continue;
        nit = Map.find(node->at(0));
        HLTp_Node *nod;
        if (nit == Map.end())
        {
          nod = new HLTp_Node(node->at(0));
          Map[node->at(0)] = nod;
        }
        else
        {
          nod = (*nit).second;
        }
        nod->fill(node->at(1));
      }
    }
  }
  for (nit=Map.begin();nit!=Map.end();nit++)
  {
    HLTp_Node *nod = (*nit).second;
    nod->calc();
  }
  return;
}

void HLTPerfFormatter::Histogram(nodemap &Map)
{
  TH1F h("PDistribution","Node Performance Distribution",1000,0.0,1000.0);
  int dum=0;
  TRint rint("Rint",&dum,0);
  Farmset sfset;
  nodemap::iterator nit;
  for (nit=Map.begin();nit!=Map.end();nit++)
  {
    HLTp_Node *nod = (*nit).second;
    nod->calc();
    printf("%s %f %f\n",nod->m_name.c_str(),nod->m_perf,nod->m_rms);
    h.Fill(float(nod->m_perf));
    sfset.insert(nod->m_name.substr(0,6));
  }
  ProfMap pmap;
  for (Farmset::iterator fit=sfset.begin();fit!=sfset.end();fit++)
  {
//    float x[2] = {1.0,32.0};
    pmap[*fit] = new TProfile((*fit).c_str(),(std::string("Performance Profile for Farm ")+*fit).c_str(),32,1.0,32.0);
  }
  for (nit=Map.begin();nit!=Map.end();nit++)
  {
    TProfile *p;
    HLTp_Node *nod = (*nit).second;
    p = pmap[nod->m_name.substr(0,6)];
    int nnum;
    sscanf(nod->m_name.substr(6,2).c_str(),"%d",&nnum);
    p->Fill(float(nnum),float(nod->m_perf));
  }
  TCanvas *can;
  for (ProfMap::iterator fit = pmap.begin();fit!=pmap.end();fit++)
  {
    std::string cnam = std::string("C_")+std::string((*fit).second->GetName());
    can = new TCanvas(cnam.c_str());
    can->cd();
//    TCanvas *c;
//    c = (TCanvas*)gROOT->FindObject("c1");
//    TSeqCollection *col = gROOT->GetListOfCanvases();
//    c = (TCanvas*)col->At(0);
//    c->cd();
    TProfile *p = (*fit).second;
    TAxis *ax = p->GetYaxis();
    ax->SetRangeUser(600.0,700.0);
    (*fit).second->Draw();
    can->SaveAs((cnam+".pdf").c_str());
  }
  can = new TCanvas("Perf","Performance Distribution");
  can->cd();
  h.Draw();
  can->SaveAs("perf.pdf");
  rint.Run();
}
