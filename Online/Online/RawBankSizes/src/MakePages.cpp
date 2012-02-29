/*
 * MakePages.cpp
 *
 *  Created on: Aug 10, 2011
 *      Author: beat
 */
#include <map>
#include <string>
#include <vector>
#include <set>
#include "string.h"
#include "OnlineHistDB/OnlineHistDB.h"

typedef std::map<std::string,std::vector<OnlineHistogram*> > bankmap;
int main()
{
  std::set<std::string> banknams;
  bankmap profiles,histos;
  std::string password="HISTEGGIA194";
  OnlineHistDB *HistDB = new OnlineHistDB(password,"HIST_WRITER","HISTDB");
  std::vector<OnlineHistogram*> profs,hists,all;
  int nprofs,nhists;
  nprofs = 0;
  nhists =0;
  int nall=HistDB->getHistogramsByTask("RawSizeONLMon", &all);
  int i;
  for (i = 0;i<nall;i++)
  {
    if (all[i]->hstype() == "P1D")
    {
      profs.push_back(all[i]);
      nprofs++;
    }
    else if (all[i]->hstype() == "H1D")
    {
      hists.push_back(all[i]);
      nhists++;
    }
  }
  size_t fpos1,fpos2,fpos3;
  for (i=0;i<nhists;i++)
  {
    std::string nam;
    nam = hists[i]->hname();
//    printf("%s of type %s ---> ",nam.c_str(),hists[i]->hstype().c_str());
    fpos1 = nam.find("Total_");
    fpos2 = nam.find("Banksp");
    fpos3 = nam.find("HLTRatio");
    if ((fpos1 == std::string::npos) && (fpos2 == std::string::npos) && (fpos3 == std::string::npos))
    {
      std::pair<bankmap::iterator,bool> ins;
      std::string bnam;
      std::vector<OnlineHistogram*> *a;
      bnam = nam.substr(nam.find("h")+1,nam.find("(")-1);
      bankmap::iterator it;
      ins = histos.insert(std::make_pair(bnam,std::vector<OnlineHistogram*>()));
      a = &ins.first->second;
      a->push_back(hists[i]);
      banknams.insert(bnam);
//      printf("fpos1 %d fpos2 %d %s\n",fpos1, fpos2, bnam.c_str());
    }
    else if (fpos1 != std::string::npos)
    {
      std::pair<bankmap::iterator,bool> ins;
      std::string bnam;
      std::vector<OnlineHistogram*> *a;
      bnam = "DET_"+nam.substr(strlen("Total_"),nam.find("(")-strlen("Total_"));
      bankmap::iterator it;
      ins = histos.insert(std::make_pair(bnam,std::vector<OnlineHistogram*>()));
      a = &ins.first->second;
      a->push_back(hists[i]);
      banknams.insert(bnam);
    }
    else
    {
//      printf("fpos1 %d fpos2 %d nothing\n",fpos1, fpos2);
    }
  }
  for (i=0;i<nprofs;i++)
  {
    std::string nam;
    nam = profs[i]->hname();
//    printf("%s of type %s ---> ",nam.c_str(),profs[i]->hstype().c_str());
    fpos1 = nam.find("Total_");
    fpos2 = nam.find("Banksp");
    fpos3 = nam.find("HLTRatio");
    if ((fpos1 == std::string::npos) && (fpos2 == std::string::npos) && (fpos3 == std::string::npos))
    {
      std::pair<bankmap::iterator,bool> ins;
      std::string bnam;
      std::vector<OnlineHistogram*> *a;
      bnam = nam.substr(nam.find("p")+1,nam.find("(")-1);
      bankmap::iterator it;
      ins = profiles.insert(std::make_pair(bnam,std::vector<OnlineHistogram*>()));
      a = &ins.first->second;
      a->push_back(profs[i]);
      banknams.insert(bnam);
//      printf("fpos1 %d fpos2 %d %s\n",fpos1, fpos2, bnam.c_str());
    }
    else
    {
//      printf("fpos1 %d fpos2 %d nothing\n",fpos1, fpos2);
    }
  }
  std::set<std::string>::iterator bit;
  bankmap::iterator hvit;
  for (bit = banknams.begin();bit!=banknams.end();bit++)
  {
    std::vector<OnlineHistogram*> *a;
    std::string rnam,pnam,hnam, histnam, profnam,bnam;
    OnlineHistPage* pg;
    bnam = *bit;
    rnam = "Online/DataSize/";
    hvit = profiles.find(bnam);
    if (hvit != profiles.end())
    {
      pnam = rnam+"Banks/"+bnam+"/Profiles";
      pg=HistDB->getPage(pnam);
      printf("Page %s\n", pnam.c_str());
      a = &hvit->second;
      for (unsigned int j=0;j<a->size();j++)
      {
        OnlineHistogram* h=a->at(j);
//        profnam = (a->at(j))->hname();
        int ist = 111111;
        h->setDisplayOption("STATS",&ist);
//        char* mod = "NONE";
//        h->setDisplayOption("DRAWOPTS",mod);
        h->saveDisplayOptions();
        pg->declareHistogram(h,0.01,0.01+j/3.0,0.99,0.01+(j+1)/3.0);
//        printf("\t%s\n",profnam.c_str());
      }
      pg->save();
      HistDB->commit();
    }
    else
    {
      printf("Funny... Bankname %s in name set but no entry in Profile Bank map\n",bnam.c_str());
    }
    hvit = histos.find(bnam);
    if (hvit != histos.end())
    {
      pnam = rnam+"Banks/"+bnam+"/Histograms";
      if (bnam.find("DET_") != std::string::npos)
      {
        bnam = bnam.substr(strlen("DET_"));
        pnam = rnam+"SubDetectors/"+bnam;
      }
      pg=HistDB->getPage(pnam);
      printf("Page %s\n", pnam.c_str());
      a = &hvit->second;
      for (unsigned int j=0;j<a->size();j++)
      {
        OnlineHistogram* h=a->at(j);
        int ist = 111111;
//        char *mod = "HIST";
        h->setDisplayOption("STATS",&ist);
//        h->setDisplayOption("DRAWOPTS",mod);
        h->saveDisplayOptions();
//        profnam = (a->at(j))->hname();
        pg->declareHistogram(h,0.01,0.01+j/3.0,0.99,0.01+(j+1)/3.0);
//        printf("\t%s\n",profnam.c_str());
      }
      pg->save();
      HistDB->commit();
    }
    else
    {
      printf("Funny... Bankname %s in name set but no entry in Histogram Bank map\n",bnam.c_str());
    }
  }
//  OnlineHistPage* pg=HistDB->getPage("/BJTest/Page1");
//  pg->save();
//  bool status = HistDB->commit();
  return 0;
}
