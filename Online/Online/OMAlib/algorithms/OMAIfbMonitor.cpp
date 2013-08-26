#include <TH1F.h>
#include <TFile.h>
#include <TTree.h>
#include <math.h>
#include "OMAlib/OMAAlgorithms.h"
#include "OMAlib/OMAlib.h"
#include <sstream>
#include <stdio.h>
#include <string>

OMAIfbMonitor::OMAIfbMonitor(OMAlib* Env) : 
  OMACheckAlg("IfbMonitor", Env) {
  m_ninput = 2;
  m_npars = 0;
  m_inputNames.push_back("MinTotClusters"); m_inputDefValues.push_back(0.);
  m_inputNames.push_back("MaxIFBClusters"); m_inputDefValues.push_back(60000000.);
  m_doc = "Algorithm for converting 2-D histograms of HPD cluster hits (total and IFB/large) to IDs ";
  m_doc += "saved on an ntuple on disk - IFB calculation is then performed offline. Cuts are used to ";
  m_doc += "filter data deemed eligible for an IFB calculation.";
}

void OMAIfbMonitor::exec(TH1 &Histo,
                         std::vector<float> & warn_thresholds,
                         std::vector<float> & alarm_thresholds,
                         std::vector<float> & input_pars,
                         unsigned int anaID,
                         TH1* ) {
   
   if( input_pars.size() < m_ninput) return;
   warn_thresholds.empty(); // avoid compil. warning
   alarm_thresholds.empty(); // avoid compil. warning
   int min_tot = (int) (input_pars[0]+0.1);
   int max_ifb =  (int) (input_pars[1]+0.1);

  Int_t Clus;
  Int_t Age=anaID;
  Int_t ID;

  std::string fileName=m_OutputDir+"/RICHclusters.root";
  TFile *f1 = new TFile(fileName.c_str(), "UPDATE"); // File name  
  if ( ! f1->IsOpen()) {// check that output file exists 
    f1->Delete();
    return; 
  }
  const char* name = Histo.GetName();
  //std::cout << name[4] << name[5] << " - name" << std::endl;
  std::stringstream s, s_age;
  TString str, str_age;
  s << name[4] << name[5];
  s_age << name[4] << name[5] << "_" << Age;
  str=s.str();
  str_age=s_age.str();
  
  TTree t1(str_age,str_age);
  t1.Branch("ID",&ID,"ID/I");
  t1.Branch("Age",&Age,"Age/I");
  t1.Branch("Clus",&Clus, "Clus/I");
 
  int rich_max = (Histo.GetNbinsX())*(Histo.GetNbinsY());

  //std::cout << "rich_max = " << rich_max << std::endl;

  for(ID=1;ID<rich_max+1;ID++){ //Looping over all HPDs in RICH1 and RICH2

  // Get coordinates from ID number
    int xnum=0, ynum=0;
        
    if(str=="1T" || str=="1I"){
      xnum = ID2Xnum1(ID);
      ynum = ID2Ynum1(ID);
    }
    if(str=="2T" || str=="2I"){
      xnum = ID2Xnum2(ID);
      ynum = ID2Ynum2(ID);
    }
    
    Clus = (Int_t) (Histo.GetBinContent(xnum,ynum) +0.1);
  
  
  if(((str=="1T" || str=="2T") && (Clus < min_tot)) || // Cuts
     ((str=="1I" || str=="2I") && (Clus > max_ifb))){
    Clus = -1;  // All entries with -1 are excluded from IFB calculation
  }

  //std::cout << xnum << "_" << ynum << " " << Clus << std::endl;

  t1.Fill();
  }   //...loop over HPDs

  t1.Write();
   
 
  
  f1->Write();
  f1->Close();
  f1->Delete();

}
 
/* ID -> "POSITION ON HISTOGRAM" CONVERSION */
 
Int_t OMAIfbMonitor::ID2Xnum1(Int_t ID)
{
    Int_t num;
    ID=ID-1;
    if(ID<98) num=14-(ID%14);
    else num=1+(ID%14);
    return num;
}

Int_t OMAIfbMonitor::ID2Xnum2(Int_t ID)
{
    Int_t num;
    ID=ID-1;
    if(ID<144) num = 10+(Int_t)(ID/16);
    else num = 1+(Int_t)((ID-144)/16);
    return num;    
}

Int_t OMAIfbMonitor::ID2Ynum1(Int_t ID)
{
    Int_t num;
    ID=ID-1;
    if(ID<98) num = 14-(Int_t)(ID/14);
    else num = 1+(Int_t)((ID-98)/14);
    return num;
}

Int_t OMAIfbMonitor::ID2Ynum2(Int_t ID)
{
    Int_t num;
    ID=ID-1;
    num=17-(ID%16);
    num=num-1;
    return num;
}
