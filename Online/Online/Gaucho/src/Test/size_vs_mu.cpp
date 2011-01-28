//============================================================================
// Name        : size_vs_mu.cpp
// Author      : Beat
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
#include "TROOT.h"
#include "TApplication.h"
#include "TObject.h"
#include "TProfile.h"
#include "TFile.h"
#include "TList.h"
#include"TCanvas.h"
#include <map>
#include <string>
#include <list>
//#include <pair>
//using namespace std;

int shit()//(int argc, char *argv[])
{
//  TROOT *gROOT;
//  TApplication *app=new TApplication("Shit",&argc,argv);
  std::list<std::string> fillist;
  std::list<std::string> banknames;
  std::list<TProfile*> sizeprofs;
  fillist.clear();
  banknames.clear();
  fillist.push_back(std::string("1262"));
  fillist.push_back(std::string("1364"));
  fillist.push_back(std::string("1453"));
  std::map<std::string,TProfile*> profmap;
  profmap.clear();
  std::list<std::string>::iterator f;
  for (f=fillist.begin();f!=fillist.end();f++)
  {
    sizeprofs.clear();
    std::string fill(*f);
    std::string mufname=std::string("/home/beat/mu-fill-")+fill+".root";
    std::string muoname=std::string("Mu_")+fill;
    std::string sizname=std::string("/home/beat/MyHistos-fill-"+fill+".root");
    TProfile *muprof1;
    TProfile muprof;
    printf("%s %s %s\n", fill.c_str(),mufname.c_str(), muoname.c_str());
    TFile *mutim=new TFile(mufname.c_str());
    muprof1 = (TProfile*)mutim->Get(muoname.c_str());
//    muprof = new TProfile(*muprof1);
    muprof1->Copy((TObject&)muprof);// = new TProfile(*muprof1);
    mutim->Close();
//    printf("%s\n",muprof.GetName());
    TFile *sizfil=new TFile(sizname.c_str());
    TList *klist=sizfil->GetListOfKeys();
    TIter kiter(klist);
    while (TObject *key=kiter())
    {
      std::string nam=std::string(key->GetName());
      int nampos=nam.find("_EVS");
      if ( nampos != (int)std::string::npos)
      {
        TProfile *p=new TProfile();
        TProfile *p1=(TProfile*)sizfil->Get(nam.c_str());
        p1->Copy(*p);
        sizeprofs.push_back(p);
        std::string mupname,bankname;
        mupname=nam.substr(0,nampos);
        printf("%s %s\n",mupname.c_str(),p->GetName());
        if (profmap.find(mupname) == profmap.end())
        {
          std::string ptit=std::string("Size of ")+mupname+" as function of mu";
          TProfile *bprof=new TProfile(mupname.c_str(),ptit.c_str(),25,0.0,2.5);
          profmap.insert(std::pair<std::string,TProfile*>(mupname,bprof));
        }
      }
    }
    std::list<TProfile*>::iterator sizeiter;//(sizeprofs);
    for (sizeiter=sizeprofs.begin();sizeiter!=sizeprofs.end();sizeiter++)
    {
      TProfile *sizk=*sizeiter;
      std::string siznam=std::string(sizk->GetName());
      std::string bname = siznam.substr(0,siznam.find("_EVS"));
      std::map<std::string,TProfile*>::iterator muiter;
      muiter = profmap.find(bname);
      TProfile *mup = muiter->second;
      printf ("%s\n",bname.c_str());
      int i;
      for (i=1;i<sizk->GetNbinsX()-1;i++)
      {
        int sizent = sizk->GetBinEntries(i);
        if (sizent >0)
        {
          double x = sizk->GetBinCenter(i);
          int mubin = muprof.FindBin(x);
          double mu = muprof.GetBinContent(mubin);
          mup->Fill(mu,sizk->GetBinContent(i));
        }
      }
    }
    
  }
  std::map<std::string,TProfile*>::iterator muiter;
  for (muiter=profmap.begin();muiter!=profmap.end();muiter++)
  {
    TProfile *mup = muiter->second;
    if (mup->GetEntries()>0)
    {
      TCanvas *c = new TCanvas(mup->GetName(),mup->GetTitle());
      mup->Draw();
    }
  }
  return 0;
} 



 
