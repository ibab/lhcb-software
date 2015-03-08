#include "rooLib.C"
#include "/users/divers/lhcb/hoballah/April2014/timeSelection.C"
#include "getTreeMC.C"
#include <TMath.h>
#include "RooNumConvPdf.h"
#include "RooMultiVarGaussian.h"
#include "RooAddition.h"
#include "RooMinuit.h"


//TWeighter2D tweighterKst2D("kstWeight2D_Angle2D.root");
TWeighter2D tweighterKst2D("kstWeight2D_Angle2D_sim8f.root");
TWeighter2D tweighterKst2D_DOCAz("kstWeight2D_DOCAz2D.root");
TWeighter2D tweighterPhi2D("phiWeight2D_Angle2D_sim8.root");

TWeighter1D tweighterKst1D_DOCAz("kstWeight1D_Average_DOCAz.root");

//TWeighter1D tweighterKst("kstWeight_Kst_892_0_cosThetaH.root");

double weighter(int flag,double val,double val2=0.){
  double ww = 0.;
  //if( flag == 2)ww=tweighterKst(val);
  if( flag == 2)ww=tweighterKst2D(val,val2);
  if( flag == 8)ww=tweighterPhi2D(val,val2);  
  //  ww=1;
  return ww; 
};


double weighter2(int flag,double val,double val2=0.){
  double ww = 0.;
  //if( flag == 2)ww=tweighterKst(val);
  if( flag == 2)ww=tweighterKst2D_DOCAz(val,val2);  
  //  ww=1;
  return ww; 
};

double weighter3(int flag,double val){
  double ww = 0.;
  //if( flag == 2)ww=tweighterKst(val);
  if( flag == 2)ww=tweighterKst1D_DOCAz(val);  
  //  ww=1;
  return ww; 
};


/* DATA WEIGHTS
TCut _weight1 =  TCut("0.34");
TCut _weight2 =  TCut("weighter(2,Kst_892_0_cosThetaH,Kst_892_0_CosTheta*Kst_892_0_P/1000.)*0.74"); 
TCut _weight4 =  TCut("0.04");
TCut _weight8 =  TCut("weighter(8,phi_1020_cosThetaH,phi_1020_CosTheta*phi_1020_P/1000.)*0.02");
*/

TCut _weight1 =  TCut("1");
TCut _weight2 =  TCut("weighter(2,Kst_892_0_cosThetaH,(Kst_892_0_CosTheta*Kst_892_0_P)/1000.)"); 
//2D weight
//TCut _weight2_DOCAz =  TCut("weighter2(2,log(abs(B_ENDVERTEX_X*Kplus_PY-B_ENDVERTEX_Y*Kplus_PX)/Kplus_PT),log(abs(B_ENDVERTEX_X*piminus_PY-B_ENDVERTEX_Y*piminus_PX)/piminus_PT))");
//inverting DOCAzp and DOCAzm
//TCut _weight2_DOCAz =  TCut("weighter2(2,log(abs(B_ENDVERTEX_X*piminus_PY-B_ENDVERTEX_Y*piminus_PX)/piminus_PT),log(abs(B_ENDVERTEX_X*Kplus_PY-B_ENDVERTEX_Y*Kplus_PX)/Kplus_PT))");

//1D weight
TCut _weight2_DOCAz =  TCut("weighter3(2,(log(abs(B_ENDVERTEX_X*Kplus_PY-B_ENDVERTEX_Y*Kplus_PX)/Kplus_PT)+log(abs(B_ENDVERTEX_X*piminus_PY-B_ENDVERTEX_Y*piminus_PX)/piminus_PT))/2)");


//TCut _weight2 =  TCut("1");
TCut _weight4 =  TCut("1");
TCut _weight8 =  TCut("weighter(8,phi_1020_cosThetaH,phi_1020_CosTheta*phi_1020_P/1000.)");

std::map<int,std::pair<double,int> > weights;

//------- CREATE DATASET/HIST
RooDataSet* createDataSet(int flag,RooRealVar x,TTree* tree, std::string name, double tlow){
  

  double min=x.getMin();
  double max=x.getMax();

  RooRealVar ww("weights","weights",1.,-10,10);
  RooDataSet* dataS = new RooDataSet(name.c_str(),name.c_str(), RooArgSet(x,ww),"weights");

  double time=0;
  double vP=0;
  double vH=0;
  double vT=0;
  double mass=0;
  double B_ENDVERTEX_X,B_ENDVERTEX_Y,B_ENDVERTEX_Z;
  double Kplus_PX,Kplus_PY,Kplus_PZ,Kplus_PT;
  double piminus_PX,piminus_PY,piminus_PZ,piminus_PT; 


  // project tree
  /**/
  std::string vec= "Kst_892_0" ;
  if( flag==4 || flag == 8)vec="phi_1020";
  tree->SetBranchAddress("B_BMassConst_TAU"   ,&time);
  tree->SetBranchAddress(std::string(vec+"_P").c_str()        ,&vP);
  tree->SetBranchAddress(std::string(vec+"_cosThetaH").c_str() ,&vH);
  tree->SetBranchAddress(std::string(vec+"_CosTheta").c_str() ,&vT);
  tree->SetBranchAddress("B_MM"   ,&mass);


  tree->SetBranchAddress("B_ENDVERTEX_X"   ,&B_ENDVERTEX_X);
  tree->SetBranchAddress("B_ENDVERTEX_Y"   ,&B_ENDVERTEX_Y);
  tree->SetBranchAddress("B_ENDVERTEX_Z"   ,&B_ENDVERTEX_Z);
  tree->SetBranchAddress("Kplus_PX"   ,&Kplus_PX);
  tree->SetBranchAddress("Kplus_PY"   ,&Kplus_PY);
  tree->SetBranchAddress("Kplus_PZ"   ,&Kplus_PZ);
  tree->SetBranchAddress("Kplus_PT"   ,&Kplus_PT);


  tree->SetBranchAddress("piminus_PX"   ,&piminus_PX);
  tree->SetBranchAddress("piminus_PY"   ,&piminus_PY);
  tree->SetBranchAddress("piminus_PZ"   ,&piminus_PZ);
  tree->SetBranchAddress("piminus_PT"   ,&piminus_PT);


  Float_t logDOCAz_tp;
  Float_t logDOCAz_tm;

  std::cout << "Creating RooDataSet" << endl;
  int n=tree->GetEntries();
  int sn=0;
  double sw=0.;
  for(int i = 0 ; i < n ;i++){
    tree->GetEntry(i);
    double t=time*1000.;
    if( t < min || t > max)continue;
    //if( flag==1 && t<tlow)continue;
    //if( flag==4 && t<tlow)continue;
    if( flag==2 && abs(mass-5280)>40)continue;
    //if( (flag==1 || flag==4) && (mass<5350 || mass>5400))continue;

    //if( flag==8 && abs(mass-5366)>40)continue;


    logDOCAz_tp=log(abs(B_ENDVERTEX_X*Kplus_PY-B_ENDVERTEX_Y*Kplus_PX)/Kplus_PT);
    logDOCAz_tm=log(abs(B_ENDVERTEX_X*piminus_PY-B_ENDVERTEX_Y*piminus_PX)/piminus_PT);

    
    double wgt= (flag==2 || flag==8) ? weighter(flag,vH,vT*vP/1000.) : 1.;
    //2D DOCAz
    //double wgt_DOCAz= (flag==2) ? weighter2(flag,logDOCAz_tp,logDOCAz_tm) : 1.;
    //1D DOCAz
    double wgt_DOCAz= 1;//(flag==2) ? weighter3(flag,(logDOCAz_tp+logDOCAz_tm)/2) : 1.;

    //
    // Scaling 2 data stat
    /*
    if( flag==1) wgt *= 0.34;
    if( flag==2) wgt *= 0.74;
    if( flag==4) wgt *= 0.04;
    if( flag==8) wgt *= 0.02;
    */

    // bin
    x=t;
    ww=wgt;
    dataS->add(RooArgSet(x,ww),wgt); 
    if( (i%1000) == 0 )std::cout << i<< "/" << n << " flag= " << flag << " time = " << t  << " weight = " <<  wgt << " weight_DOCAz = " <<  wgt_DOCAz << endl;//


    // increment counters :
    sw += wgt;
    sn++;
  }
  weights[flag] = std::make_pair(sw,sn);   
  return dataS;      
};



//===============================================================
void timeFitMC(int mask=1,std::string which_selection="clr",double tlow=0.,bool binned=false,bool trigger=true,std::string sim="sim8",std::string period="all",bool simultaneous=true,std::string dec="all"){
  //timeFitMC(2,"clr",0.,false,true,"sim8","2012",true,"all")

  using namespace RooFit;
  using namespace RooStats ;

  //cout<<"attention weights for Jpsi modes are removed############################################################################"<<endl;

  bool scaling = true;  // Apply selection scaling
  bool output  = true;   // produce output
  bool filter  = true;     // use filtered tuples : MANDATORY FOR UNBINNED FIT - WARNING  SELECTION-DEPENDENT 
  bool minos   = true;
  bool logY    = false;     // log-scale
  int  nCPU    = 7;
  bool w2Error = true;   // MUST BE TRUE - EXCEPT WHEN SCALING ERRORS TO DATA STATs

  // dataset & fitting range
  double min =0.;
  double max =15.;
  int    nbin=300;

  gROOT->Reset();

  std::string pref="./work/new_ProperTime_"; // output prefix
  //std::string suff="MC_"+sim+"_FitAD_antiB0"; // output suffix
  //std::string suff="MC_"+sim+"_FitTauBd2KstG"; // output suffix
  std::string suff="MC_SIM."+sim+".PERIOD."+period; // output suffix
  if(!trigger)suff+="_noTRIG";
  if(!scaling)suff+="_noScaling";

  //=================================
  suff+="_FitAD_ALL";
  //================================
  

  std::cout << "=============== Hb -> (hh)gamma fit =========== " << std::endl;
  std::cout << " Usage :  timeFitMC([binned] ,[mask] , [trigger], [period] ,[simult])" << std::endl;
  std::cout << " - mask    (int)   : define the set of channels to fit - see flags below (default : mask   = 1)" <<std::endl; 
  std::cout << " - binned  (bool)  : perform binned  or unbinned fit (default : binned = true)" << std::endl;
  std::cout << " - period  (string): select data sample '2011','2012' or 'any' (default : period ='all')"<<std::endl;
  std::cout << " - sim     (string): SIM version 'sim6', 'sim8', 'all' (default : sim='all')"<< std::endl;
  std::cout << " - simult  (bool)  : perform simultaneous or independant fits (default : simult = true)"<<std::endl;
  std::cout << " Mask  flags  : " <<std::endl; 

  
  std::cout << "========================================= " << std::endl;
  std::cout << " Fit mask = " << mask << std::endl;  
  if(!output)std::cout << "No output will be stored"<<std::endl;

  if  ( 0 == mask){   
    std::cout << "mask must be > 0" <<std::endl; 
    simultaneous=false; 
  }


  //====== Sanity checks
  if( !binned && !filter){
    std::cout << " === WARNING : filtered tuples must be used to perform unbinned fits - change 'filter' to true " << endl;
    filter=true;
  }

  if( (mask == 1 || mask ==2 || mask == 4 || mask == 8) && simultaneous ){
    std::cout << " No need for simultaneous flag for simple fit - change 'simultaneous' to false" << endl;
    //simultaneous=false;
  }
  


  //==== Set fit flag
  std::string flag = (binned) ? "_Binned_" : "_Unbinned_";
  if( simultaneous){
    std::ostringstream type("");
    type << mask;
    flag += "_simult"+type.str();
  }
  
  //==== Get data
  RooMsgService::instance().setGlobalKillBelow(RooFit::ERROR) ;

  if( minos) std::cout << " ===== MINOS fit is requested =======" <<std::endl;
  else  std::cout << " ===== MINOS fit is disabled =======" <<std::endl;
  

  //----- TFile & TTree
  TTree* tree1 = (mask&1) ? getTreeMC(1,period,sim,filter,dec,which_selection) : NULL; // k*g
  TTree* tree2 = (mask&2) ? getTreeMC(2,period,sim,filter,dec,which_selection) : NULL; // psiK*
  TTree* tree4 = (mask&4) ? getTreeMC(4,period,sim,filter,dec,which_selection) : NULL; // phig
  TTree* tree8 = (mask&8) ? getTreeMC(8,period,sim,filter,dec,which_selection) : NULL; // psiphi  

  
  //----- define TCanvas
  TCanvas *c1=NULL;
  TCanvas *c2=NULL;
  TCanvas *c4=NULL;
  TCanvas *c8=NULL;

  if( mask & 1){c1 = new TCanvas("K*0Gamma",""); prepareCanvas(c1, 1);}
  if( mask & 2){c2 = new TCanvas("K*0Jpsi","" ); prepareCanvas(c2, 1);}
  if( mask & 4){c4 = new TCanvas("PhiGamma",""); prepareCanvas(c4, 1);}
  if( mask & 8){c8 = new TCanvas("PhiPsi  ",""); prepareCanvas(c8, 1);}
  
  //----- Channel selection


  std::string selFlag="v0";
  TCut extra= TCut("(B_BKGCAT==0 || B_BKGCAT==50) "); 
  //extra=TCut("1"); 
  TCut cut1 = (mask&1) ?  timeSelection(1,trigger,scaling)+extra : TCut("0");  // K*g 
  TCut cut2 = (mask&2) ?  timeSelection(2,trigger,scaling)+extra : TCut("0");  // K*Psi 
  TCut cut4 = (mask&4) ?  timeSelection(4,trigger,scaling)+extra : TCut("0");  // phig 
  TCut cut8 = (mask&8) ?  timeSelection(8,trigger,scaling)+extra : TCut("0");  // PhiPsi 
  
  

  cut2 +=TCut("abs(B_MM-5280)<40");


  //TTree* sTree = tree2->CopyTree(cut2);


  TCut weight1 =  _weight1;
  TCut weight2 =  _weight2;
  TCut weight2_DOCAz =_weight2_DOCAz;
  TCut weight4 =  _weight4;
  TCut weight8 =  _weight8;
  
  //TCut weightMCscale= TCut("0.74");

  //----- Fitting & Plotting
  
  std::string ssvar= "B_BMassConst_TAU";
  std::string svar=ssvar+"*1000.";
  double fac=1000.;
  
  std::stringstream varRange("");
  varRange << "("<< svar <<" < " << max << " && " << svar << " > " << min <<")";
  TCut mRange(varRange.str().c_str());
  
  //-- Fit parameters -------------------------------------------------------------------
  
  RooRealVar  M("Propertime","#tau",min,max,"ps");
  M.setBins(nbin);

  RooDataSet* dataS1=NULL;
  RooDataSet* dataS2=NULL;
  RooDataSet* dataS4=NULL;
  RooDataSet* dataS8=NULL;

  RooDataHist* dataH1=NULL;
  RooDataHist* dataH2=NULL;
  RooDataHist* dataH4=NULL;
  RooDataHist* dataH8=NULL;
  


  RooDataSet* dataS11=NULL;
  RooDataSet* dataS22=NULL;
  RooDataSet* dataS44=NULL;









  //================= BUILD  binned dataset ..
  std::stringstream svarRange("");
  svarRange << "("<< ssvar <<" < " << max/fac << " && " << ssvar << " > " << min/fac <<")";
  TCut smRange(svarRange.str().c_str());
  
  //====

  TH1F* th1=NULL; 
  TH1F* th2=NULL; 
  TH1F* th4=NULL;
  TH1F* th8=NULL;
  /**/
  if( binned ){
    std::cout << " ==== PERFORMING BINNED FIT =====" <<std::endl;
    std::cout << " ... Building dataset " << std::endl;
    
    TCut var = TCut(svar.c_str());
    
    if( mask & 1){
      th1 = new TH1F("th1","th1",nbin,min,max);
      th1->Sumw2();
      tree1->Project("th1",var,(cut1 +  mRange)*weight1);
      dataH1 = new RooDataHist("dataH1","tau data1", M, th1);    
      std::cout << *dataH1 << std::endl;
    }
    if( mask & 2){
      th2 = new TH1F("th2","th2",nbin,min,max);
      th2->Sumw2();
      tree2->Project("th2",var,(cut2 + mRange)*weight2*weight2_DOCAz);
      dataH2 = new RooDataHist("dataH2","tau data2", M, th2);    
      std::cout << *dataH2 << std::endl;
    }
    if( mask & 4){
      th4 = new TH1F("th4","th4",nbin,min,max);
      th4->Sumw2();
      tree4->Project("th4",var,(cut4 +  mRange)*weight4);
      dataH4 = new RooDataHist("dataH4","tau data4", M, th4);    
      std::cout << *dataH4 << std::endl;
    }
    if( mask & 8){
      th8 = new TH1F("th8","th8",nbin,min,max);
      th8->Sumw2();
      tree8->Project("th8",var,(cut8 +  mRange)*weight8);
      dataH8 = new RooDataHist("dataH8","tau data8", M, th8);    
      std::cout << *dataH8 << std::endl;
    }
    
  }else{      
    //================= ... or BUILD  unbinned dataset   (TO DO !)
    std::cout << " ==== PERFORMING UNBINNED FIT =====" <<std::endl;
    std::cout << " ... Building dataset ... be patient" << std::endl;


    if( mask & 1){
      dataS1= createDataSet(1,M,tree1,"dataS1",tlow);
      //dataS1=new RooDataSet("dataS1", "dataS1", dataS11, RooArgSet(M),"M>0.6");
      dataH1 = new RooDataHist("dataH1","data1" , RooArgSet(M), *dataS1);    
      std::cout << *dataS1 << " -> " << *dataH1 << std::endl;
    }
    if( mask & 2){
      dataS2= createDataSet(2,M,tree2,"dataS2",0.);
      //dataS2=new RooDataSet("dataS2", "dataS2", dataS22, RooArgSet(M), "M>0.");
      dataH2 = new RooDataHist("dataH2","data2" , RooArgSet(M), *dataS2);    
      std::cout << *dataS2 << " -> " << *dataH2 << std::endl;
    }
    if( mask & 4){
      dataS4= createDataSet(4,M,tree4,"dataS4",tlow);
      //dataS4=new RooDataSet("dataS4", "dataS4", dataS44, RooArgSet(M), "M>0.6");
      dataH4 = new RooDataHist("dataH4","data4" , RooArgSet(M), *dataS4);    
      std::cout << *dataS4 << " -> " << *dataH4 << std::endl;
    }
    if( mask & 8){
      dataS8= createDataSet(8,M,tree8,"dataS8",0.);
      dataH8 = new RooDataHist("dataH8","data8" , RooArgSet(M), *dataS8);    
      std::cout << *dataS8 << " -> " << *dataH8 << std::endl;
    }
  }
  
  //++++++++++++++++++++++++++++++++ FIT MODEL ++++++++++++++++++++++++

  M.setBins(10000,"cache") ; // for convolution
  std::cout << "Preparing the fit ..."<<std::endl;
  
//===== Resolution functions



  RooConstVar th =RooConst(0.05);
  /* FOR RADIATIVE */
  /*RooRealVar mu1  ("#mu_{K^{*0}#gamma}"    ,"mu1"           , 0.00) ;// 0.00374);//0.0) ;
  RooRealVar sig1 ("#sigma^{0}_{K^{*0}#gamma}"   ,"sigma01" , 0.110);//0.111 ); // ± 0.025
  RooRealVar ns1  ("#eta_{K^{*0}#gamma}"         ,"eta1"    , 0.62);//0.46  ); // ± 0.12
  RooRealVar as1  ("#alpha_{K^{*0}#gamma}"       ,"alpha1"  , 1.78);//1.5   ); // ± 0.55
  RooFormulaVar sigma1("sigma_{K^{*0}#gamma}","@0*pow(TMath::Max(@3,@4),@1)/(1+@2*pow(TMath::Max(@3,@4),@1))",RooArgList(sig1,ns1,as1,M,th));
  RooRealVar b1   ("b_{K^{*0}#gamma}"            ,"b1"     ,1.11 ) ;
  RooApollonios res1("res_{K^{*0}#gamma}","res1",M, mu1,sigma1,b1);*/

  RooRealVar mu1  ("#mu_{K^{*0}#gamma}"    ,"mu1"           , 0.0) ;//0.00374);//0.0) ;//
  RooRealVar sig1 ("#sigma^{0}_{K^{*0}#gamma}"   ,"sigma01" , 0.111 ); //0.110);//0.111 ); // ± 0.025
  RooRealVar ns1  ("#eta_{K^{*0}#gamma}"         ,"eta1"    , 0.46  ); //0.62);//0.46  ); // ± 0.12
  RooRealVar as1  ("#alpha_{K^{*0}#gamma}"       ,"alpha1"  , 1.5   ); //1.78);//1.5   ); // ± 0.55
  RooFormulaVar sigma1("sigma_{K^{*0}#gamma}","@0*pow(TMath::Max(@3,@4),@1)/(1+@2*pow(TMath::Max(@3,@4),@1))",RooArgList(sig1,ns1,as1,M,th));
  RooRealVar b1   ("b_{K^{*0}#gamma}"            ,"b1"     ,1.11 ) ;
  RooApollonios res1("res_{K^{*0}#gamma}","res1",M, mu1,sigma1,b1);






  RooRealVar mu4  ("#mu_{#phi#gamma}"    ,"mu4"           , 0.00) ;//0.00438);//0.00) ;
  RooRealVar sig4 ("#sigma^{0}_{#phi#gamma}"   ,"sigma04" , 0.115);//0.089 ); // ± 0.022 
  RooRealVar ns4  ("#eta_{#phi#gamma}"         ,"eta4"    , 0.65);//0.39  ); // ± 0.12
  RooRealVar as4  ("#alpha_{#phi#gamma}"       ,"alpha4"  , 1.75);//0.96   ); // ±0.47
  RooFormulaVar sigma4("sigma_{#phi#gamma}","@0*pow(TMath::Max(@3,@4),@1)/(1+@2*pow(TMath::Max(@3,@4),@1))",RooArgList(sig4,ns4,as4,M,th));
  RooRealVar b4   ("b_{#phi#gamma}"            ,"b4"     ,1.11);//1.12 ) ;
  RooApollonios res4("res_{#phi#gamma}","res4",M, mu4,sigma4,b4);


  /* FOR CC*/
  /*RooRealVar mu2  ("#mu_{K^{*0}J/#psi}"    ,"mu2"           , 0.00) ;//0.0010);//0.) ;
  RooRealVar sig2 ("#sigma^{0}_{K^{*0}J/#psi}"   ,"sigma02" , 0.144);//0.245); // ± 0.052
  RooRealVar ns2  ("#eta_{K^{*0}J/#psi}"         ,"eta2"    , 0.84);//1.30 ); // ± 0.3
  RooRealVar as2  ("#alpha_{K^{*0}J/#psi}"       ,"alpha2"  , 2.5);//5.5  ); // ± 1.3
  RooFormulaVar sigma2("sigma_{K^{*0}J/#psi}","@0*pow(TMath::Max(@3,@4),@1)/(1+@2*pow(TMath::Max(@3,@4),@1))",RooArgList(sig2,ns2,as2,M,th));
  RooRealVar b2   ("b_{K^{*0}J/#psi}"            ,"b2"     ,0.87);//0.82 ) ;
  RooApollonios res2("res_{K^{*0}J/#psi}","res2",M, mu2,sigma2,b2);*/

  RooRealVar mu2  ("#mu_{K^{*0}J/#psi}"    ,"mu2"           , 0.00) ;//0.001);//0.) ;
  RooRealVar sig2 ("#sigma^{0}_{K^{*0}J/#psi}"   ,"sigma02" , 0.245); //0.144);//0.245); // ± 0.052
  RooRealVar ns2  ("#eta_{K^{*0}J/#psi}"         ,"eta2"    , 1.30 ); //0.84);//1.30 ); // ± 0.3
  RooRealVar as2  ("#alpha_{K^{*0}J/#psi}"       ,"alpha2"  , 5.5  ); //2.5);//5.5  ); // ± 1.3
  RooFormulaVar sigma2("sigma_{K^{*0}J/#psi}","@0*pow(TMath::Max(@3,@4),@1)/(1+@2*pow(TMath::Max(@3,@4),@1))",RooArgList(sig2,ns2,as2,M,th));
  RooRealVar b2   ("b_{K^{*0}J/#psi}"            ,"b2"     ,0.82);//0.8 ) ;
  RooApollonios res2("res_{K^{*0}J/#psi}","res2",M, mu2,sigma2,b2);





  RooRealVar mu8  ("#mu_{#phi J/#psi}"    ,"mu8"           , 0.) ;
  RooRealVar sig8 ("#sigma^{0}_{#phi J/#psi}"   ,"sigma08" , 0.181 ); // ± 0.021 0.174);//
  RooRealVar ns8  ("#eta_{#phi J/#psi}"         ,"eta8"    , 1.15  ); // ± 0.2 0.82);//
  RooRealVar as8  ("#alpha_{#phi J/#psi}"       ,"alpha8"  , 3.5   ); // ± 0.5 2.9);//
  RooFormulaVar sigma8("sigma_{#phi J/#psi}","@0*pow(TMath::Max(@3,@4),@1)/(1+@2*pow(TMath::Max(@3,@4),@1))",RooArgList(sig8,ns8,as8,M,th));
  RooRealVar b8   ("b_{#phi J/#psi}"            ,"b8"     ,0.73);//0.89 ) ;
  RooApollonios res8("res_{#phi J/#psi}","res8",M, mu8,sigma8,b8);

  
  // Low-propertime acceptance 

  /*RooRealVar a1("a","a1",1.,0.1,10., "ps^{-1}");
  RooRealVar n1("n","n1",1.,0.1,10.);
  RooRealVar t01("t_{0}(B_{d}#rightarrow K^{*0}#gamma)","t01",0.100,0.05,0.5,"ps");
  RooRealVar t0("t_{0}","t0",0.100,0.05,0.5,"ps");*/

  RooRealVar a1("a","a1",2.,0.5,25., "ps^{-1}");
  RooRealVar n1("n","n1",2.,0.5,5.);
  //RooRealVar t01("t_{0}(B_{d}#rightarrow K^{*0}#gamma)","t01",0.240,0.05,0.5,"ps");
  RooRealVar t0("t_{0}","t0",0.20,0.05,0.5,"ps");

  RooRealVar a2("a_{B_{d}#rightarrow K^{*0}J#psi}","a2",2.,0.5,25., "ps^{-1}");
  RooRealVar n2("n_{B_{d}#rightarrow K^{*0}J#psi}","n2",2.,0.5,5.);
  RooRealVar t02("t_{0}(B_{d}#rightarrow K^{*0}J/#psi)","t02",0.240,0.05,0.5,"ps");

  RooRealVar a4("a_{B_{s}#rightarrow #phi#gamma}","a4",5.,0.5,25., "ps^{-1}");
  RooRealVar n4("n_{B_{s}#rightarrow #phi#gamma}","n4",1.5,0.5,5.);
  //RooRealVar t04("t_{0}(B_{s}#rightarrow #phi#gamma)"  ,"t04",0.100,0.05,0.5,"ps");

  RooRealVar a8("a_{B_{s}#rightarrow #phi J/#psi}","a8",5.,0.5,25., "ps^{-1}");
  RooRealVar n8("n_{B_{s}#rightarrow #phi J/#psi}","n8",1.5,0.5,5.);
  RooRealVar t08("t_{0}(B_{s}#rightarrow #phiJ/#psi)"  ,"t08",0.100,0.05,0.5,"ps");



  RooRealVar dt0_12("#Delta t_{0A}","dt0_12",0.0,-0.500,0.500); 
  RooRealVar dt0_14("#Delta t_{0B}"  ,"dt0_14",0.0,-0.500,0.500);
  RooRealVar dt0_24("#Delta t_{0C}"  ,"dt0_24",0.0,-0.500,0.500);


  RooFormulaVar t01("t_{0}(B_{d}#rightarrow K^{*0}#gamma)","@0+@1",RooArgList(t02,dt0_12));
  //RooFormulaVar t04("t_{0}(B_{s}#rightarrow #phi#gamma)"  ,"@0+@1",RooArgList(t01,dt0_14));
  RooFormulaVar t04("t_{0}(B_{s}#rightarrow #phi#gamma)"  ,"@0+@1",RooArgList(t02,dt0_24));












  // High-propertime acceptance
  //RooRealVar dG1("#delta#Gamma(B_{d}#rightarrow K^{*0}#gamma)","dG1",-0.00,-1.1 , 1.1 , "ps^{-1}");
  RooRealVar dG2("#delta#Gamma(B_{d}#rightarrow K^{*0}J/#psi)","dG2",-0.000,-1. , 1. , "ps^{-1}");
  //RooRealVar dG4("#delta#Gamma(B_{s}#rightarrow #phi#gamma)"  ,"dG4",-0.000,-1.1 , 1.1 , "ps^{-1}");
  RooRealVar dG8("#delta#Gamma(B_{s}#rightarrow #phi J/#psi)" ,"dG8",-0.000,-1.1 , 1.1 , "ps^{-1}");
  //RooRealVar dG8("#delta#Gamma(B_{s}#rightarrow #phi J/#psi)" ,"dG8",-0.020);
  
  RooRealVar dG("#delta#Gamma" ,"dG",-0.000,-1.1 , 1.1 , "ps^{-1}");


  RooRealVar dDg12("#Delta#delta#Gamma_{A}","dDg12",-0.0,-0.100,0.100);  
  RooRealVar dDg28("dDg28","dDg28",0,-0.100,0.100);
  RooRealVar dDg14("#Delta#delta#Gamma_{B}","dDg14",0,-0.100,0.100);
  RooRealVar dDg42("#Delta#delta#Gamma_{C}","dDg42",0,-0.100,0.100);



  RooFormulaVar dG1("#delta#Gamma(B_{d}#rightarrow K^{*0}#gamma)","@0+@1",RooArgList(dG2,dDg12));
  //RooFormulaVar dG4("#delta#Gamma(B_{s}#rightarrow #phi#gamma)"  ,"@0+@1",RooArgList(dG1,dDg14));
  RooFormulaVar dG4("#delta#Gamma(B_{s}#rightarrow #phi#gamma)"  ,"@0+@1",RooArgList(dG2,dDg42));
  //RooFormulaVar dG8("#delta#Gamma(B_{s}#rightarrow #phi J/#psi)" ,"@0+@1",RooArgList(dG2,dDg28));
  //RooFormulaVar dG4("#delta#Gamma(B_{s}#rightarrow #phi#gamma)","@0+@1",RooArgList(dG8,dDg));

  //RooFormulaVar dG2("#delta#Gamma(B_{d}#rightarrow K^{*0}J/#psi)","@0+0.0089",RooArgList(dG1));//,-0.1 , 0. , "ps^{-1}");

  //==== External constraint(s) : dG[rad] - dG[diMu]
  //RooFormulaVar DdG("#Delta[#delta#Gamma","(@0-@1)*1000.",RooArgList(dG1,dG2));
  //RooGaussian constDdG("constDdG","constDdG" ,DdG ,RooConst(-8.9),RooConst(0.5) ) ;
  //RooArgSet constSet=RooArgSet(constDdG);




  
  //c same t0
  TMatrixDSym correlation_gaussian_constraint(2);
  //correlation_gaussian_constraint(0,0)=1.889e-05;
  //correlation_gaussian_constraint(0,1)=-8.024e-06;
  //correlation_gaussian_constraint(1,0)=-8.024e-06;
  //correlation_gaussian_constraint(1,1)=1.583e-05;

  //c^prime
  //correlation_gaussian_constraint(0,0)=2.207e-05;
  //correlation_gaussian_constraint(0,1)=-9.296e-06;
  //correlation_gaussian_constraint(1,0)=-9.296e-06;
  //correlation_gaussian_constraint(1,1)=1.863e-05;


  //c^prime^prime
  /*correlation_gaussian_constraint(0,0)=2.190e-05;
  correlation_gaussian_constraint(0,1)=-9.212e-06;
  correlation_gaussian_constraint(1,0)=-9.212e-06;
  correlation_gaussian_constraint(1,1)=1.806e-05;*/

  
  //a_0
  /*correlation_gaussian_constraint(0,0)=1.849e-05;
  correlation_gaussian_constraint(0,1)=-7.932e-06;
  correlation_gaussian_constraint(1,0)=-7.932e-06;
  correlation_gaussian_constraint(1,1)=1.887e-05;
  RooMultiVarGaussian shift("shift","shift",RooArgList(dt0_14,dDg14),RooArgList(RooConst(-0.00891),RooConst(0.0003)),correlation_gaussian_constraint);//*/


  //b_0
  /*correlation_gaussian_constraint(0,0)=1.954e-05;
  correlation_gaussian_constraint(0,1)=-8.464e-06;
  correlation_gaussian_constraint(1,0)=-8.464e-06;
  correlation_gaussian_constraint(1,1)=2.253e-05;
  RooMultiVarGaussian shift("shift","shift",RooArgList(dt0_24,dDg42),RooArgList(RooConst(-0.03628),RooConst(-0.0177)),correlation_gaussian_constraint);//*/



  //c_0
  TMatrixDSym correlation_gaussian_constraint1(4);
  correlation_gaussian_constraint1(0,0)=2.125e-05;
  correlation_gaussian_constraint1(0,1)=-9.672e-06;
  correlation_gaussian_constraint1(0,2)=-9.385e-06;
  correlation_gaussian_constraint1(0,3)=4.263e-06;
  correlation_gaussian_constraint1(1,0)=-9.672e-06;
  correlation_gaussian_constraint1(1,1)=1.774e-05;
  correlation_gaussian_constraint1(1,2)=4.269e-06;
  correlation_gaussian_constraint1(1,3)=-7.700e-06;
  correlation_gaussian_constraint1(2,0)=-9.385e-06;
  correlation_gaussian_constraint1(2,1)=4.269e-06;
  correlation_gaussian_constraint1(2,2)=2.324e-05;
  correlation_gaussian_constraint1(2,3)=-9.806e-06;
  correlation_gaussian_constraint1(3,0)=4.263e-06;
  correlation_gaussian_constraint1(3,1)=-7.700e-06;
  correlation_gaussian_constraint1(3,2)=-9.806e-06;
  correlation_gaussian_constraint1(3,3)=1.898e-05;

  //RooMultiVarGaussian shift("shift","shift",RooArgList(dt0_12,dt0_14,dDg12,dDg14),RooArgList(RooConst(-0.0276),RooConst(-0.0088),RooConst(-0.0179),RooConst(0.0003)),correlation_gaussian_constraint1);//



  //cout<<"determinant= "<<  correlation_gaussian_constraint.Determinant()<<endl;
	//RooGaussian shift("shift","shift", dDg14, RooConst(-0.0035), RooConst(0.0039));

  RooArgSet constSet=RooArgSet();





  double tbd;
  double tbdjpsikst;
  if(sim=="sim8"){tbd= -1.519;
    tbdjpsikst=-1.519;
  }
  
  else if(sim=="sim6"){tbd=-1.525;
    tbdjpsikst=-1.525;
  }
  
  else {tbd=-1.521;
    tbdjpsikst=-1.52158;
  }
  

 

  double tbs=0;
  if(sim=="sim8")tbs= -1.511;
  else if(sim=="sim6")tbs=-1.473;
  else  tbs=(mask & 4) ? -1.497 : -1.501;


  double dgs=0.;
  if(sim=="sim8")dgs= 0.092;//0.139/fabs(tbs);
  else if(sim=="sim6")dgs=0.060;//0.1/fabs(tbs);
  else dgs=(mask & 4) ? 0.081056 : 0.0835;

  double tbsphipsi;
  if(sim=="sim8")tbsphipsi= -1.462;
  else if(sim=="sim6")tbsphipsi=-1.432;
  else tbsphipsi=-1.454433111;//-1.446;
  double tbsphig;
  if(sim=="sim8")tbsphig= -1.525;      // assume eventID = 01
  else if(sim=="sim6")tbsphig=-1.479;  // assume eventID = 02
  else tbsphig=-1.497;//-1.502;



  double _Rl=0.;
  if(sim=="sim8" && dec=="1")_Rl= 0.751068;
  if(sim=="sim8" && dec=="2")_Rl= 0.840451;
  if(sim=="sim6" && dec=="1")_Rl= 0.751068;
  if(sim=="sim6" && dec=="2")_Rl= 0.840451;
  else _Rl=0.;//-1.446;


  double _Rh=0.;
  if(sim=="sim8" && dec=="1")_Rh= 0.249615;
  if(sim=="sim8" && dec=="2")_Rh= 0.160272;
  if(sim=="sim6" && dec=="1")_Rh= 0.249615;
  if(sim=="sim6" && dec=="2")_Rh= 0.160272;
  else _Rh=0.;//-1.446;



  double _AD=0;
  if(sim=="sim8" && dec=="1")  _AD= (mask & 8) ?0.502 : 0;
  if(sim=="sim8" && dec=="2")  _AD= (mask & 8) ?0.680 : 0;
  if(sim=="sim6" && dec=="2")  _AD= (mask & 8) ?0.680 : 0;
  else _AD= (mask & 8) ? 0.553 : 0.0 ;

  
  //dgs=0.100;
  
   
  RooRealVar     tauBd2kstg("#tau(B_{d}#rightarrow K^{*0}#gamma)"  , "tau1", tbd      ,"ps");
  RooRealVar     tauBd2kstpsi("#tau(B_{d}#rightarrow K^{*0}J#psi)" , "tau2", tbdjpsikst      ,"ps");
  RooRealVar     tauBs2phig("#tau(B_{s}#rightarrow#phi#gamma)"     , "tau4", tbsphig,  "ps");
  RooRealVar     tauBs2phipsi("#tau(B_{s}#rightarrow#phi J#psi)"   , "tau8", tbsphipsi,"ps");

  RooRealVar     taus("#tau_{B_{s}}"      , "tau_Bs", tbs  ,"ps");
  RooRealVar     DGs("#Delta#Gamma_{s}"  ,  "DG_s"  , dgs  ,"ps^{-1}");  
  RooRealVar     AD("A^{#Delta}"  ,  "ADelta"  , _AD);//,-1.5,1.5);
  RooRealVar     AD_phig("A^{#Delta}_{#phi #gamma}"  ,  "AD_phig"  , _AD);//,-1.5,1.5);
  RooRealVar     Rl("Rl"  ,  "Rl"  , _Rl);
  RooRealVar     Rh("Rh"  ,  "Rh"  , _Rh);

  taus.Print();
  
  
  /*
  RooProperTime  time1_("time1_","time1_",M,tauBd2kstg  ,a1,n1,dG1,t01,1);   
  RooProperTime  time2_("time2_","time2_",M,tauBd2kstpsi,a2,n2,dG2,t02,1);   
  RooProperTime  time4_("time4_","time4_",M,tauBs2phig  ,a4,n4,dG4,t04,1);    
  RooProperTime  time8_("time8_","time8_",M,tauBs2phipsi,a8,n8,dG8,t08,1);   
  */

  /**/
  RooProperTime  time1_("time1_","time1_",M,tauBd2kstg  ,a1,n1,dG,t0,1);  
  RooProperTime  time2_("time2_","time2_",M,tauBd2kstpsi,a1,n1,dG,t0,1);   
  //RooProperTime  time4_("time4_","time4_",M,tauBs2phig  ,a4,n4,dG4,t04,1);    
  //RooProperTime  time8_("time8_","time8_",M,tauBs2phipsi,a8,n8,dG8,t08,1);

  
  /**/
  RooBsProperTime  time4_("time4_","time4_",M,taus,DGs,AD_phig,a1,n1,dG,t0,1); 
  RooBsProperTime  time8_("time8_","time8_",M,taus,DGs,AD,a1,n1,dG,t0,1);
  /**/


  // Convolution
  RooFFTConvPdf  time1("time1","time1"  ,M, time1_,res1);
  RooFFTConvPdf  time2("time2","time2"  ,M, time2_,res2);
  RooFFTConvPdf  time4("time4","time4"  ,M, time4_,res4);
  RooFFTConvPdf  time8("time8","time8"  ,M, time8_,res8);
  time1.setBufferFraction(0.95) ;
  time2.setBufferFraction(0.95) ;
  time4.setBufferFraction(0.95) ;
  time8.setBufferFraction(0.95) ;


  //===== Signal amplitudes
  RooRealVar y1 ("N_{B_{d} #rightarrow K^{*0}#gamma}" ,"y1" ,40000. ,0. ,250000. );
  RooRealVar y2 ("N_{B_{d} #rightarrow K^{*0}J/#psi}" ,"y2" ,40000. ,0. ,250000. );
  RooRealVar y4 ("N_{B_{s} #rightarrow #phi#gamma}"   ,"y4" ,10000. ,0. ,250000. );
  RooRealVar y8 ("N_{B_{s} #rightarrow #phi J/#psi}"  ,"y8" ,150000. ,0. ,750000. );
  RooAddPdf PDF1=RooAddPdf("PDF_{B_{d} #rightarrow K^{*0}#gamma}","PDF1",time1,y1);
  RooAddPdf PDF2=RooAddPdf("PDF_{B_{d} #rightarrow K^{*0}J/#psi}","PDF2",time2,y2);
  RooAddPdf PDF4=RooAddPdf("PDF_{B_{s} #rightarrow #phi#gamma}"  ,"PDF4",time4,y4);  
  RooAddPdf PDF8=RooAddPdf("PDF_{B_{s} #rightarrow #phi J/#psi}" ,"PDF8",time8,y8);  
  
  RooArgSet parList(tauBd2kstg,tauBs2phig,y1,y2,y4,y8);
  parList.add(a1);
  parList.add(n1);
  parList.add(t01);
  parList.add(t0);
  parList.add(dG1);
  parList.add(a2);
  parList.add(n2);
  parList.add(t02);
  parList.add(dG2);
  parList.add(a4);
  parList.add(n4);
  parList.add(t04);
  parList.add(dG4);
  parList.add(a8);
  parList.add(n8);
  parList.add(t08);
  parList.add(dG8);
  parList.add(tauBd2kstg);
  parList.add(tauBs2phig);
  parList.add(AD);
  parList.add(AD_phig);
  parList.add(dDg12);
  parList.add(dDg28);
  parList.add(dDg14);
  parList.add(dDg42);
  parList.add(dt0_12);
  parList.add(dt0_14);
  parList.add(dt0_24);
  parList.add(dG);
  //====== Define Minos set
  RooCmdArg myMinos;
  if( minos)
    myMinos =  RooCmdArg(Minos(true));
    //myMinos =  RooCmdArg(Minos(RooArgSet(n1,n2,n4,n8,AD,dG1,dG2,t01)));
  else
    myMinos = RooCmdArg(Minos(false));

  
  //======================== Fitting ===============================//

  RooFitResult* result1=NULL;
  RooFitResult* result2=NULL;
  RooFitResult* result4=NULL;
  RooFitResult* result8=NULL;
  RooFitResult* result = NULL;


  double n11;
  double n22;
  

  
  if( simultaneous){
    //========== SIMULTANEOUS FITS
    std::cout << " ====== PERFORM SIMULTANEOUS FIT ======= " << std::endl;
    RooCategory sample("sample","sample");
    if( mask & 1)sample.defineType("sample1");
    if( mask & 2)sample.defineType("sample2");
    if( mask & 4)sample.defineType("sample4");
    if( mask & 8)sample.defineType("sample8");
    RooSimultaneous simPDF("simPDF","simultaneous PDF",sample);
    if( mask & 1)simPDF.addPdf(PDF1,"sample1");    
    if( mask & 2)simPDF.addPdf(PDF2,"sample2");
    if( mask & 4)simPDF.addPdf(PDF4,"sample4");
    if( mask & 8)simPDF.addPdf(PDF8,"sample8");
    //if( mask & 1)simPDF.addPdf(time1,"sample1");    
    //if( mask & 2)simPDF.addPdf(time2,"sample2");
    //if( mask & 4)simPDF.addPdf(time4,"sample4");
    //if( mask & 8)simPDF.addPdf(time8,"sample8");


    M.setRange("Range_sample1",tlow,15.) ;
    M.setRange("Range_sample2",0.,15.) ; 
    M.setRange("Range_sample4",tlow,15.) ;
    M.setRange("Range_sample8",0.,15.) ; 
    
    //simPDF.fixAddCoefRange("Range");



    std::vector<RooCmdArg> cmds;
    M.setBins(nbin);



    if( binned ){
      if( mask & 1){cmds.push_back(Import("sample1",*dataH1));std::cout << "Import kst0gamma RooDataHist"<<std::endl;} // better to import TH1 than dataHist ??
      if( mask & 2){cmds.push_back(Import("sample2",*dataH2));std::cout << "Import kst0psi RooDataHist"  <<std::endl;} 
      if( mask & 4){cmds.push_back(Import("sample4",*dataH4));std::cout << "Import phigamma RooDataHist" <<std::endl;}
      if( mask & 8){cmds.push_back(Import("sample8",*dataH8));std::cout << "Import phipsi RooDataHist"   <<std::endl;}

      std::cout << " ... Imported " << cmds.size() << " dataHist to the combined RooDataHist"<<std::endl;
      RooDataHist* combData=NULL;
      if( cmds.size() == 1)combData=new RooDataHist("combData","combined data",M,Index(sample),cmds[0]);
      if( cmds.size() == 2)combData=new RooDataHist("combData","combined data",M,Index(sample),cmds[0],cmds[1]);
      if( cmds.size() == 3)combData=new RooDataHist("combData","combined data",M,Index(sample),cmds[0],cmds[1],cmds[2]);
      if( cmds.size() == 4)combData=new RooDataHist("combData","combined data",M,Index(sample),cmds[0],cmds[1],cmds[2],cmds[3]);
      if( cmds.size() == 5)
        combData=new RooDataHist("combData","combined data",M,Index(sample),cmds[0],cmds[1],cmds[2],cmds[3],cmds[4]);
      if( cmds.size() == 6)
        combData=new RooDataHist("combData","combined data",M,Index(sample),cmds[0],cmds[1],cmds[2],cmds[3],cmds[4],cmds[5]);
      std::cout << "Starting simultaneous binned fit ..."<<std::endl;
      result = simPDF.fitTo(*combData,Extended(kTRUE),Save(),myMinos,NumCPU(nCPU),SumW2Error(w2Error),ExternalConstraints(constSet));
      if( cmds.size()>0)delete combData;
    }else{
      if( mask & 1){cmds.push_back(Import("sample1",*dataS1));std::cout << "Import kst0gamma RooDataSet"<<std::endl;}
      if( mask & 2){cmds.push_back(Import("sample2",*dataS2));std::cout << "Import kst0psi RooDataSet"<<std::endl;}
      if( mask & 4){cmds.push_back(Import("sample4",*dataS4));std::cout << "Import phigamma RooDataSet"<<std::endl;}
      if( mask & 8){cmds.push_back(Import("sample8",*dataS8));std::cout << "Import phipsi RooDataSet"<<std::endl;}
      std::cout << "... Imported " << cmds.size() << " dataSet to the combined RooDataSet"<<std::endl;
      RooDataSet* combData=NULL;
      RooRealVar ww("weights","weights",1.,-10,10);
      if( cmds.size() == 1)combData= new RooDataSet("combData","combined data",RooArgSet(M,ww),Index(sample),WeightVar("weights"),cmds[0]);
      if( cmds.size() == 2)combData= new RooDataSet("combData","combined data",RooArgSet(M,ww),Index(sample),WeightVar("weights"),cmds[0],cmds[1]);
      if( cmds.size() == 3)combData= new RooDataSet("combData","combined data",RooArgSet(M,ww),Index(sample),WeightVar("weights"),cmds[0],cmds[1],cmds[2]);
      if( cmds.size() == 4)combData= new RooDataSet("combData","combined data",RooArgSet(M,ww),Index(sample),WeightVar("weights"),cmds[0],cmds[1],cmds[2],cmds[3]);
      std::cout << "Starting simultaneous unbinned fit ..."<<std::endl;
      M.setBins(10000,"cache") ; // for convolution

      std::cout << "combined dataset for simultaneous fit " << *combData <<std::endl;




      /*RooAbsReal* nll1 = PDF1.createNLL(*dataS1,Range("Range_sample1"),ExternalConstraints(constSet)) ;
      RooAbsReal* nll2 = PDF2.createNLL(*dataS2,Range("Range_sample2"),ExternalConstraints(constSet)) ;
      RooAddition nll("nll","nll",RooArgSet(*nll1,*nll2)) ;
      RooMinuit m(nll) ;
      result=m.fit("m");
      //m.migrad() ;
      //m.hesse() ;
      //m.minos();
      */      

      result = simPDF.fitTo(*combData,Extended(kTRUE),Save(),myMinos,NumCPU(nCPU),SumW2Error(w2Error),ExternalConstraints(constSet));//,Extended(kTRUE)





      //m_massPdf->plotOn(massFrame, LineColor(1),  Range(m_range),Normalization(n,RooAbsReal::NumEvent) );*/



      
      if( cmds.size()>0)delete combData;
    }
    result1=result;
    result2=result;
    result4=result;
    result8=result;
    result->Print();

  }
  else{
    //========== SIMPLE FITS
    std::cout << " ====== PERFORM INDEPENDANT FIT(S) ======= " << std::endl;
  }  


  //=================== Plotting =======================//
  double schi2=0.;
  int    ndof=0;
  int    npar=0;






  //----++++++++++++ K*g plot
  if( (mask & 1) ){
    M.SetTitle("t_{B_{d} #rightarrow K^{*0}#gamma}");
    RooPlot* frame1 = M.frame(Title("B^{0} #rightarrow K^{*0}(K^{+}#pi^{-})#gamma")) ;
    if( !simultaneous){
      result1 = (binned) ? 
        PDF1.fitTo(*dataH1,Extended(kTRUE),Save(),myMinos,NumCPU(nCPU),SumW2Error(w2Error),ExternalConstraints(constSet)) :
        PDF1.fitTo(*dataS1,Extended(kTRUE),Save(),myMinos,NumCPU(nCPU),SumW2Error(w2Error),ExternalConstraints(constSet));
    }
    std::string out=pref+"KstgammaFit"+flag+suff;  
    //TCanvas *c11=new TCanvas("K*0Gamma",""); prepareCanvas(c11, 1);
    /*dataS1->plotOn(frame1);
    PDF1.plotOn(frame1, LineColor(1),RooFit::Range("Range_sample1"),RooFit::Normalization(n11,RooAbsReal::NumEvent) );
    frame1->Draw();
    
    c11->Print("c11.pdf");*/
    
    schi2 += rooDisplay(M,dataH1, frame1, c1, PDF1 , result1,out,parList,logY,"Poisson",RooFit::Range(tlow,15.),RooFit::Layout(0.55,0.9,0.9));    
    ndof  += nbin;
    npar  += result1->floatParsFinal().getSize();
  }


  if( (mask & 2) ){
    M.SetTitle("t_{B_{d} #rightarrow K^{*0}J/#psi}");
    RooPlot* frame2 = M.frame(Title("B^{0} #rightarrow K^{*0}(K^{+}#pi^{-})J/#psi")) ;
    if( !simultaneous){
      result2 = (binned) ? 
        PDF2.fitTo(*dataH2,Extended(kTRUE),Save(),myMinos,NumCPU(nCPU),SumW2Error(w2Error),ExternalConstraints(constSet)) :
        PDF2.fitTo(*dataS2,Extended(kTRUE),Save(),myMinos,NumCPU(nCPU),SumW2Error(w2Error),ExternalConstraints(constSet));
    }    
    std::string out=pref+"KstPsiFit"+flag+suff; 
    /*TCanvas *c22=new TCanvas("JpsiK*0",""); prepareCanvas(c22, 1);
    dataS2->plotOn(frame2);
    PDF2.plotOn(frame2, LineColor(1),RooFit::Range("Range_sample2"),RooFit::Normalization(n22,RooAbsReal::NumEvent) );
    frame2->Draw();

    c22->Print("c22.pdf");*/
    

    schi2 += rooDisplay(M,dataH2, frame2, c2, PDF2 , result2,out,parList,logY,"Poisson",RooFit::Range(0.,15.),RooFit::Layout(0.55,0.9,0.9));    
    ndof  += nbin;    
    npar  += result2->floatParsFinal().getSize();
  }
  
  if( (mask & 4) ){
    M.SetTitle("t_{B_{s} #rightarrow #phi#gamma}");
    RooPlot* frame4 = M.frame(Title("B_{s} #rightarrow #phi(K^{+}K^{-})#gamma")) ;
    if( !simultaneous){
      result4 = (binned) ? 
        PDF4.fitTo(*dataH4,Extended(kTRUE),Save(),myMinos,NumCPU(nCPU),SumW2Error(w2Error),ExternalConstraints(constSet)) :
        PDF4.fitTo(*dataS4,Extended(kTRUE),Save(),myMinos,NumCPU(nCPU),SumW2Error(w2Error),ExternalConstraints(constSet));
    }
    std::string out=pref+"PhigammaFit"+flag+suff;
    //dataH4->plotOn(frame4,RooFit::Range(0,0.5));
    //PDF4.plotOn(frame4);
    //frame4->Draw();
    schi2 += rooDisplay(M,dataH4, frame4, c4, PDF4 , result4,out,parList,logY,"Poisson",RooFit::Range(tlow,15.),RooFit::Layout(0.55,0.9,0.9));
    ndof  += nbin;
    npar  += result4->floatParsFinal().getSize();
  }
  if( (mask & 8) ){
    M.SetTitle("t_{B_{s} #rightarrow #phi J/#psi}");
    RooPlot* frame8 = M.frame(Title("B_{s} #rightarrow #phi(K^{+}K^{-})J/#psi")) ;
    if( !simultaneous){
      result8 = (binned) ? 
        PDF8.fitTo(*dataH8,Extended(kTRUE),Save(),myMinos,NumCPU(nCPU),SumW2Error(w2Error),ExternalConstraints(constSet)) :
        PDF8.fitTo(*dataS8,Extended(kTRUE),Save(),myMinos,NumCPU(nCPU),SumW2Error(w2Error),ExternalConstraints(constSet));
    }
    std::string out=pref+"PhiPsiFit"+flag+suff;
    schi2 += rooDisplay(M,dataH8, frame8, c8, PDF8 , result8,out,parList,logY,"Poisson",RooFit::Range(0.,15.),RooFit::Layout(0.55,0.9,0.9));
    ndof  += nbin;
    npar  += result8->floatParsFinal().getSize();
  }
  //=================== Finalization
  if(simultaneous) npar = result->floatParsFinal().getSize();
  ndof -= npar;
  double p= TMath::Prob(schi2,ndof)*100;
  std::cout << "========== ++++ Global chi2/ndof = " << schi2 << "/"<<ndof << " -- p : " << p << " %" <<std::endl;
  

  // Weight counters :
  if( !binned ){
    for( int i=0; i < 4; ++i){
      int fl = 1 << i;
      std::map<int,std::pair<double,int> >::iterator itw = weights.find(fl);
      if( itw != weights.end() && (itw->second).second != 0)
        std::cout << " Average(weight)[" << fl << "] = " << (itw->second).first << " / " << (itw->second).second << " = "  << (itw->second).first / (itw->second).second << endl;
      else 
        std::cout << " mask  "<<fl<< " is not in "<< endl;
    }
  }

  

  
}





