#include "rooLib.C"
#include "/users/divers/lhcb/hoballah/April2014/timeSelection.C"
#include "getTree.C"
#include <TMath.h>
#include "RooMultiVarGaussian.h"
#include "RooUnblindUniform.h"


TWeighter2D tweighterKst2D("kstWeight2D_Angle2D_sim8f.root");
TWeighter2D tweighterKst2D_DOCAz("kstWeight2D_DOCAz2D.root");
TWeighter2D tweighterPhi2D("phiWeight2D_Angle2D.root");
//TWeighter1D tweighterKst("kstWeight_Kst_892_0_cosThetaH.root");

double weighter(int flag,double val,double val2=0.,double val3=0.){
  double ww = 0.;
  if( flag == 2)ww=tweighterKst2D(val,val2);
  if( flag == 8)ww=tweighterPhi2D(val,val2);
  return ww; 
};


double weighter2(int flag,double val,double val2=0.){
  double ww = 0.;
  //if( flag == 2)ww=tweighterKst(val);
  if( flag == 2)ww=tweighterKst2D_DOCAz(val,val2);  
  //  ww=1;
  return ww; 
};


TCut _weight1 =  TCut("1");
TCut _weight2 =  TCut("weighter(2,Kst_892_0_cosThetaH,Kst_892_0_CosTheta*Kst_892_0_P/1000.)"); 
TCut _weight2_DOCAz =  TCut("weighter2(2,log(abs(B_ENDVERTEX_X*Kplus_PY-B_ENDVERTEX_Y*Kplus_PX)/Kplus_PT),log(abs(B_ENDVERTEX_X*piminus_PY-B_ENDVERTEX_Y*piminus_PX)/piminus_PT))");
TCut _weight4 =  TCut("1");
TCut _weight8 =  TCut("weighter(8,phi_1020_cosThetaH,phi_1020_CosTheta*phi_1020_P/1000.)");

//==
RooDataSet* projectToDataHist(int flag,RooRealVar x,RooRealVar y,TTree* tree, std::string name,int nbin){

  std::string sName=name+"S";
  double min=x.getMin();
  double max=x.getMax();
  double step=(max-min)/double(nbin);

  RooRealVar ww("weights","weights",1.,-100,100);
  RooRealVar sweights("sweights","sweights",-10,10);
  RooDataSet* dataS = new RooDataSet(sName.c_str(),sName.c_str(), RooArgSet(x,ww,sweights,y),"weights");
  //RooDataHist* dataH = new RooDataHist(name.c_str(),name.c_str(),RooArgSet(x));


  double sWeight=0;
  double time=0;
  double vP=0;
  double vH=0;
  double vT=0;
  double mass=0;
  double B_ENDVERTEX_X,B_ENDVERTEX_Y,B_ENDVERTEX_Z;
  double Kplus_PX,Kplus_PY,Kplus_PZ,Kplus_PT;
  double piminus_PX,piminus_PY,piminus_PZ,piminus_PT;
  double dira,piminus_IPCHI2_OWNPV,Kplus_IPCHI2_OWNPV;
  
  x.setBins(nbin);


  // project tree

  if(flag==1){
    tree->SetBranchStatus("*",0); // deactivate all branches
    tree->SetBranchStatus("B_BMassConst_TAU"   ,1);
    tree->SetBranchStatus("B_MM"   ,1);
    tree->SetBranchStatus("SIGYield_peeking_sw"         ,1);
    tree->SetBranchStatus("Kst_892_0_cosThetaH",1);
    // reactivate needed branches
    tree->SetBranchAddress("B_BMassConst_TAU"   ,&time);
    tree->SetBranchAddress("SIGYield_peeking_sw"         ,&sWeight);
    tree->SetBranchAddress("B_MM"         ,&mass);
    tree->SetBranchAddress("Kst_892_0_CosTheta" ,&vT);
  }
  if(flag==2){
    tree->SetBranchStatus("*",0); // deactivate all branches
    tree->SetBranchStatus("Kst_892_0_P"        ,1);// activate what is needed
    tree->SetBranchStatus("Kst_892_0_cosThetaH",1);
    tree->SetBranchStatus("Kst_892_0_CosTheta" ,1);
    tree->SetBranchStatus("B_BMassConst_TAU"   ,1);
    tree->SetBranchStatus("B_MM"   ,1);
    tree->SetBranchStatus("B_DiraAngle"   ,1);
    tree->SetBranchStatus("Kplus_IPCHI2_OWNPV"   ,1);
    tree->SetBranchStatus("piminus_IPCHI2_OWNPV"   ,1);

    //tree->SetBranchStatus("nsigWeight"         ,1);

    tree->SetBranchStatus("B_ENDVERTEX_X"   ,1);//&B_ENDVERTEX_X);
    tree->SetBranchStatus("B_ENDVERTEX_Y"   ,1);//&B_ENDVERTEX_Y);
    tree->SetBranchStatus("B_ENDVERTEX_Z"   ,1);//&B_ENDVERTEX_Z);
    tree->SetBranchStatus("Kplus_PX"   ,1);//&Kplus_PX);
    tree->SetBranchStatus("Kplus_PY"   ,1);//&Kplus_PY);
    tree->SetBranchStatus("Kplus_PZ"   ,1);//&Kplus_PZ);
    tree->SetBranchStatus("Kplus_PT"   ,1);//&Kplus_PT);
    tree->SetBranchStatus("piminus_PX"   ,1);//&piminus_PX);
    tree->SetBranchStatus("piminus_PY"   ,1);//&piminus_PY);
    tree->SetBranchStatus("piminus_PZ"   ,1);//&piminus_PZ);
    tree->SetBranchStatus("piminus_PT"   ,1);//&piminus_PT);
    

    // reactivate needed branches
    tree->SetBranchAddress("Kst_892_0_P"        ,&vP);
    tree->SetBranchAddress("Kst_892_0_cosThetaH",&vH);
    tree->SetBranchAddress("Kst_892_0_CosTheta" ,&vT);
    tree->SetBranchAddress("B_BMassConst_TAU"   ,&time);
    //tree->SetBranchAddress("nsigWeight"         ,&sWeight);
    tree->SetBranchAddress("B_MM"         ,&mass);
    
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
    tree->SetBranchAddress("B_DiraAngle"   ,&dira);

    tree->SetBranchAddress("Kplus_IPCHI2_OWNPV"   ,&Kplus_IPCHI2_OWNPV);
    tree->SetBranchAddress("piminus_IPCHI2_OWNPV"   ,&piminus_IPCHI2_OWNPV);


    /**/
  }


 if(flag==4){
    tree->SetBranchStatus("*",0); // deactivate all branches
    tree->SetBranchStatus("B_BMassConst_TAU"   ,1);
    tree->SetBranchStatus("B_MM"   ,1);
    tree->SetBranchStatus("SIGYield_peeking_sw"         ,1);

    // reactivate needed branches
    tree->SetBranchAddress("B_BMassConst_TAU"   ,&time);
    tree->SetBranchAddress("SIGYield_peeking_sw"         ,&sWeight);
    tree->SetBranchAddress("B_MM"         ,&mass);

  }

  else if( flag==8){
    tree->SetBranchStatus("*",0); // deactivate all branches
    tree->SetBranchStatus("phi_1020_P"        ,1);// activate what is needed
    tree->SetBranchStatus("phi_1020_cosThetaH",1);
    tree->SetBranchStatus("phi_1020_CosTheta" ,1);
    tree->SetBranchStatus("B_BMassConst_TAU"   ,1);
    tree->SetBranchStatus("B_MM"   ,1);
    tree->SetBranchStatus("n2Weight"         ,1);
    // reactivate needed branches
    tree->SetBranchAddress("phi_1020_P"        ,&vP);
    tree->SetBranchAddress("phi_1020_cosThetaH",&vH);
    tree->SetBranchAddress("phi_1020_CosTheta" ,&vT);
    tree->SetBranchAddress("B_BMassConst_TAU"   ,&time);
    tree->SetBranchAddress("n2Weight"         ,&sWeight);
    tree->SetBranchAddress("B_MM"         ,&mass);
  }
  
  std::cout << "Creating RooDataSet" << endl;
  int n=tree->GetEntries();
  for(int i = 0 ; i < n ;i++){
    tree->GetEntry(i);
    double t=time*1000.;
    if( t < min || t > max)continue;
    if( flag==2 && abs(mass-5280)>40)continue;
    //if( flag==2 && dira>0.019)continue;
    //if( flag==2 && piminus_IPCHI2_OWNPV<61.1)continue;
    //if( flag==2 && Kplus_IPCHI2_OWNPV<56.1)continue;
    //if( flag==1 && abs(mass)<5460)continue;
    if( flag==8 && abs(mass-5366)>40)continue;
    double wgt= (flag==2 || flag==8) ? weighter(flag,vH,vT*vP/1000.) : 1.;

    double wgt_DOCAz=1;// (flag==2) ? weighter2(flag,log(abs(B_ENDVERTEX_X*Kplus_PY-B_ENDVERTEX_Y*Kplus_PX)/Kplus_PT),log(abs(B_ENDVERTEX_X*piminus_PY-B_ENDVERTEX_Y*piminus_PX)/piminus_PT)) : 1.;//



    double sw =1;// (flag==1 || flag==4) ? sWeight : 1.;
    //if(sw<0){sw=0;}
    // bin
    x=t;
    ww= wgt*wgt_DOCAz;
    y=wgt;
    sweights=sw;
    dataS->add(RooArgSet(x,ww,sweights),wgt*sw*wgt_DOCAz); 
    if( (i%1000) == 0 )std::cout << i<< "/" << n << " flag= " << flag << " time = " << t  << " weight = " <<  wgt <<" sweight = " << sw << " weight_DOCAz = " <<  wgt_DOCAz <<endl;//
  }
  return dataS;
      
}


TChain* jpsikstDataSelected(std::string name,std::string period,std::string path="/LhcbDisk/data3/scratch/hoballah/data_newSelected"){
  TChain* chain= new TChain( name.c_str(),"");
  /*if(period=="all"){
    chain->AddFile(std::string(path+"/KstPsiFitunbinnedall_sWeightsSel.root").c_str()); //#0
    }*/
  if(period=="all" || period=="2011"){
    chain->AddFile(std::string(path+"/jpsikst_DATA2011_all.root").c_str()); //#0
  }
  if(period=="all" || period=="2012"){
    chain->AddFile(std::string(path+"/jpsikst_DATA2012_all.root").c_str()); //#0
  }
  return chain;
}

TChain* jpsiphiDataSelected(std::string name,std::string path="/users/divers/lhcb/hoballah/April2014/Mostafa"){
  TChain* chain= new TChain( name.c_str(),"");
  chain->AddFile(std::string(path+"/PhiJpsiFitunbinnedall_sWeightsSel.root").c_str()); //#0
  return chain;
}


TChain* kstgammaDataSelected(std::string name,std::string which_selection="hlt1clr",std::string period="all",std::string path="/users/divers/lhcb/hoballah/October2014/massfit_Bd/output"){
  TChain* chain= new TChain( name.c_str(),"");
  if(which_selection=="clr" && period=="all"){
    chain->AddFile(std::string(path+"/sWeights_kstGamma_signalPluspeaking_allfreeBKGcomponents_optimized.root").c_str()); //#0
  }
  if(which_selection=="clr" && period=="2011"){
    chain->AddFile(std::string(path+"/sWeights_kstGamma_signalPluspeaking_allfreeBKGcomponents_optimized2011.root").c_str()); //#0
  }
  if(which_selection=="clr" && period=="2012"){
    chain->AddFile(std::string(path+"/sWeights_kstGamma_signalPluspeaking_allfreeBKGcomponents_optimized2012.root").c_str()); //#0
  }
  if(which_selection=="hlt1clr"){
    chain->AddFile(std::string(path+"/sWeights_signalPluspeaking_allfreeBKGcomponents_with_hlt1photon.root").c_str()); //#0
  }
  if(which_selection=="vlct"){
    chain->AddFile(std::string(path+"/sWeights_signalPluspeaking_allfreeBKGcomponents_valenciatight.root").c_str()); //#0
  }
  if(which_selection=="vlcl"){
    chain->AddFile(std::string(path+"/sWeights_signalPluspeaking_allfreeBKGcomponents_valencialoose.root").c_str()); //#0
  }


  return chain;
}


TChain* phigammaDataSelected(std::string name,std::string which_selection="hlt1clr",std::string path="/users/divers/lhcb/hoballah/October2014/massfit_Bd/output"){
  TChain* chain= new TChain( name.c_str(),"");
  if(which_selection=="clr"){
    chain->AddFile(std::string(path+"/sWeights_phiGamma_peekingInSignalPDF_allfreeBKGcomponents_optimized.root").c_str()); //#0
  }
  if(which_selection=="hlt1clr"){
    chain->AddFile(std::string(path+"/sWeights_phiGamma_peekingInSignalPDF_allfreeBKGcomponents_hlt1photon_clermont.root").c_str()); //#0
  }
  if(which_selection=="vlct"){
    chain->AddFile(std::string(path+"/sWeights_phiGamma_peekingInSignalPDF_allfreeBKGcomponents_valenciatight.root").c_str()); //#0
  }
  if(which_selection=="vlcl"){
    chain->AddFile(std::string(path+"/sWeights_phiGamma_peekingInSignalPDF_allfreeBKGcomponents_valencialoose.root").c_str()); //#0
  }
  return chain;
}


//===============================================================
void timeFitData(int mask=5,std::string which_selection="clr",bool binned=false,bool trigger=true,std::string period="all",bool simultaneous=true){//
  using namespace RooFit;
  using namespace RooStats ;

  bool scaling = true;  // Apply selection scaling
  bool output  = true; // produce output
  bool minos   = true;
  bool logY    = false;     // log-scale
  int  nCPU    = 7;
  bool w2Error = true;

  // dataset & fitting range
  double min =0.3;
  double max =15.;
  int    nbin=147;

  gROOT->Reset();

  std::string pref="./work/ProperTime_"; // output prefix
  std::string suff="DATA_"; // output suffix
  if(!trigger)suff+="_noTRIG";

  std::cout << "=============== Hb -> (hh)gamma fit =========== " << std::endl;
  std::cout << " Usage :  timeFit([binned] ,[mask] , [trigger], [period] ,[simult])" << std::endl;
  std::cout << " - mask    (int)   : define the set of channels to fit - see flags below (default : mask   = 1)" <<std::endl; 
  std::cout << " - binned  (bool)  : perform binned  or unbinned fit (default : binned = true)" << std::endl;
  std::cout << " - period  (string): select data sample '2011','2012' or 'any' (default : period ='all')"<<std::endl;
  std::cout << " - simult  (bool)  : perform simultaneous or independant fits (default : simult = true)"<<std::endl;
  std::cout << " Mask  flags  : " <<std::endl; 

  
  std::cout << "========================================= " << std::endl;
  std::cout << " Fit mask = " << mask << std::endl;  
  if(!output)std::cout << "No output will be stored"<<std::endl;

  if  ( 0 == mask){   
    std::cout << "mask must be > 0" <<std::endl; 
    simultaneous=false; 
  }


  //==== Set fit flag
  std::string flag = (binned) ? "binned" : "unbinned";
  if( simultaneous){
    std::ostringstream type("");
    type << mask;
    flag += "_simult"+type.str();
  }
  
  //==== Get data
  RooMsgService::instance().setGlobalKillBelow(RooFit::ERROR) ;

  if( minos) std::cout << " ===== MINOS fit is requested =======" <<std::endl;
  else  std::cout << " ===== MINOS fit is disabled =======" <<std::endl;
  
  TChain* chain1 =  kstgammaDataSelected("merged",which_selection,period);
  TChain* chain2 = jpsikstDataSelected("DecayTree",period);
  TChain* chain4 =  phigammaDataSelected("merged",which_selection);
  TChain* chain8 = jpsiphiDataSelected("DecayTree");
  
  //----- TFile & TTree
  TTree* tree1 = (mask&1) ? chain1 : NULL; // k*g
  TTree* tree2 = (mask&2) ? chain2 : NULL; // psiK*
  TTree* tree4 = (mask&4) ? chain4 : NULL; // phig
  TTree* tree8 = (mask&8) ? chain8 : NULL; // psiphi
  

  suff+=period;
  
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



  TCut extra=TCut("1");
  TCut cut1 = (mask&1) ?  timeSelection(1,trigger,scaling)+extra : TCut("0");  // K*g
  TCut cut2 = (mask&2) ?  timeSelection(2,trigger,scaling)+extra : TCut("0");  // K*Psi
  TCut cut4 = (mask&4) ?  timeSelection(4,trigger,scaling)+extra : TCut("0");  // phig
  TCut cut8 = (mask&8) ?  timeSelection(8,trigger,scaling)+extra : TCut("0");  // PhiPsi
  
  


  TCut weight1 =  _weight1;
  TCut weight2 =  _weight2;
  TCut weight4 =  _weight4;
  TCut weight8 =  _weight8;

  TCut sWeight1 =  TCut("(SIGYield_peeking_sw)");
  TCut sWeight2 =  TCut("(nsigWeight)");
  //TCut sWeight2 =  TCut("1");
  TCut sWeight4 =  TCut("1");
  TCut sWeight8 =  TCut("(n2Weight)");



  
  //----- Fitting & Plotting
  
  std::string ssvar= "B_BMassConst_TAU";
  std::string svar=ssvar+"*1000.";
  double fac=1000;
  
  std::stringstream varRange("");
  varRange << "("<< svar <<" < " << max << " && " << svar << " > " << min <<")";
  TCut mRange(varRange.str().c_str());
  
  //-- Fit parameters -------------------------------------------------------------------

  
  RooRealVar  M("Propertime","#tau",min,max,"ps");
  RooRealVar www("www","www",-100,100);
  M.setBins(nbin);

  RooDataSet* dataS1t=NULL;
  RooDataSet* dataS2t=NULL;
  RooDataSet* dataS4t=NULL;
  RooDataSet* dataS8t=NULL;

  RooDataSet* dataS1=NULL;
  RooDataSet* dataS2=NULL;
  RooDataSet* dataS4=NULL;
  RooDataSet* dataS8=NULL;

  RooDataHist* dataH1=NULL;
  RooDataHist* dataH2=NULL;
  RooDataHist* dataH4=NULL;
  RooDataHist* dataH8=NULL;
  
  TTree* selTree1=NULL;
  TTree* selTree2=NULL;
  TTree* selTree4=NULL;
  TTree* selTree8=NULL;
  
  
  //================= BUILD  binned dataset ...


  if( mask & 1){
    dataS1t = projectToDataHist(1,M,www,tree1,"dataS1t",nbin);
    dataS1 = new RooDataSet("dataS1","dataS1",dataS1t,*dataS1t->get(),0,"sweights") ;
    dataH1 = new RooDataHist("dataH1","B->Kst #gamma data" , RooArgSet(M), *dataS1);   
    std::cout << *dataS1 << " -> " << *dataH1 << std::endl;
  }

  if( mask & 2){
    dataS2 = projectToDataHist(2,M,www,tree2,"dataS2",nbin);
    dataH2 = new RooDataHist("dataH2","B->Kst J/#psi data" , RooArgSet(M), *dataS2);    
    std::cout << *dataS2 << " -> " << *dataH2 << std::endl;
  }

  if( mask & 4){
    dataS4t = projectToDataHist(4,M,www,tree4,"dataS4t",nbin);
    dataS4 = new RooDataSet("dataS4","dataS4",dataS4t,*dataS4t->get(),0,"sweights") ;
    dataH4 = new RooDataHist("dataH4","B->#phi #gamma data" , RooArgSet(M), *dataS4);   
    std::cout << *dataS4 << " -> " << *dataH4 << std::endl;
  }


    if( mask & 8){
      dataS8 = projectToDataHist(8,M,www,tree8,"dataS8",nbin);
    dataH8 = new RooDataHist("dataH2","B->#phi J/#psi data" , RooArgSet(M), *dataS8);    
    std::cout << *dataS8 << " -> " << *dataH8 << std::endl;
  }


    RooRealVar SIGYield_peeking_sw("SIGYield_peeking_sw","SIGYield_peeking_sw",-10,10);


  //++++++++++++++++++++++++++++++++ FIT MODEL ++++++++++++++++++++++++

  M.setBins(10000,"cache") ; // for convolution
  std::cout << "Preparing the fit ..."<<std::endl;
  
//===== Resolution functions 




  RooConstVar th =RooConst(0.05);
  /* FOR RADIATIVE */
  //new test
  RooRealVar mu1  ("#mu_{K^{*0}#gamma}"    ,"mu1"           , 0.0) ;//0.00374);//0.0) ;//
  RooRealVar sig1 ("#sigma^{0}_{K^{*0}#gamma}"   ,"sigma01" , 0.110);//0.111 ); // ± 0.025
  RooRealVar ns1  ("#eta_{K^{*0}#gamma}"         ,"eta1"    , 0.62);//0.46  ); // ± 0.12
  RooRealVar as1  ("#alpha_{K^{*0}#gamma}"       ,"alpha1"  , 1.78);//1.5   ); // ± 0.55
  RooFormulaVar sigma1("sigma_{K^{*0}#gamma}","@0*pow(TMath::Max(@3,@4),@1)/(1+@2*pow(TMath::Max(@3,@4),@1))",RooArgList(sig1,ns1,as1,M,th));
  RooRealVar b1   ("b_{K^{*0}#gamma}"            ,"b1"     ,1.11 ) ;
  RooApollonios res1("res_{K^{*0}#gamma}","res1",M, mu1,sigma1,b1);
  
  /*
  RooRealVar mu1  ("#mu_{K^{*0}#gamma}"    ,"mu1"           , 0.0) ;//0.00374);//0.0) ;//
  RooRealVar sig1 ("#sigma^{0}_{K^{*0}#gamma}"   ,"sigma01" , 0.111 ); //0.110);//0.111 ); // ± 0.025
  RooRealVar ns1  ("#eta_{K^{*0}#gamma}"         ,"eta1"    , 0.46  ); //0.62);//0.46  ); // ± 0.12
  RooRealVar as1  ("#alpha_{K^{*0}#gamma}"       ,"alpha1"  , 1.5   ); //1.78);//1.5   ); // ± 0.55
  RooFormulaVar sigma1("sigma_{K^{*0}#gamma}","@0*pow(TMath::Max(@3,@4),@1)/(1+@2*pow(TMath::Max(@3,@4),@1))",RooArgList(sig1,ns1,as1,M,th));
  RooRealVar b1   ("b_{K^{*0}#gamma}"            ,"b1"     ,1.11 ) ;
  RooApollonios res1("res_{K^{*0}#gamma}","res1",M, mu1,sigma1,b1);
  */


  RooRealVar mu4  ("#mu_{#phi#gamma}"    ,"mu4"           , 0.00) ;//0.00438);//0.00) ;
  RooRealVar sig4 ("#sigma^{0}_{#phi#gamma}"   ,"sigma04" , 0.089 ); //0.115);//0.089 ); // ± 0.022 
  RooRealVar ns4  ("#eta_{#phi#gamma}"         ,"eta4"    , 0.39  ); //0.65);//0.39  ); // ± 0.12
  RooRealVar as4  ("#alpha_{#phi#gamma}"       ,"alpha4"  , 0.96   ); //1.75);//0.96   ); // ±0.47
  RooFormulaVar sigma4("sigma_{#phi#gamma}","@0*pow(TMath::Max(@3,@4),@1)/(1+@2*pow(TMath::Max(@3,@4),@1))",RooArgList(sig4,ns4,as4,M,th));
  RooRealVar b4   ("b_{#phi#gamma}"            ,"b4"     ,1.12 ) ;//1.11);//1.12 ) ;
  RooApollonios res4("res_{#phi#gamma}","res4",M, mu4,sigma4,b4);


  /* FOR CC*/
//new test
  RooRealVar mu2  ("#mu_{K^{*0}J/#psi}"    ,"mu2"           , 0.00) ;//0.001);//0.) ;
  RooRealVar sig2 ("#sigma^{0}_{K^{*0}J/#psi}"   ,"sigma02" , 0.144);//0.245); // ± 0.052
  RooRealVar ns2  ("#eta_{K^{*0}J/#psi}"         ,"eta2"    , 0.84);//1.30 ); // ± 0.3
  RooRealVar as2  ("#alpha_{K^{*0}J/#psi}"       ,"alpha2"  , 2.5);//5.5  ); // ± 1.3
  RooFormulaVar sigma2("sigma_{K^{*0}J/#psi}","@0*pow(TMath::Max(@3,@4),@1)/(1+@2*pow(TMath::Max(@3,@4),@1))",RooArgList(sig2,ns2,as2,M,th));
  RooRealVar b2   ("b_{K^{*0}J/#psi}"            ,"b2"     ,0.84);//0.82 ) ;
  RooApollonios res2("res_{K^{*0}J/#psi}","res2",M, mu2,sigma2,b2);


    /* 
  RooRealVar mu2  ("#mu_{K^{*0}J/#psi}"    ,"mu2"           , 0.00) ;//0.001);//0.) ;
  RooRealVar sig2 ("#sigma^{0}_{K^{*0}J/#psi}"   ,"sigma02" , 0.245); //0.144);//0.245); // ± 0.052
  RooRealVar ns2  ("#eta_{K^{*0}J/#psi}"         ,"eta2"    , 1.30 ); //0.84);//1.30 ); // ± 0.3
  RooRealVar as2  ("#alpha_{K^{*0}J/#psi}"       ,"alpha2"  , 5.5  ); //2.5);//5.5  ); // ± 1.3
  RooFormulaVar sigma2("sigma_{K^{*0}J/#psi}","@0*pow(TMath::Max(@3,@4),@1)/(1+@2*pow(TMath::Max(@3,@4),@1))",RooArgList(sig2,ns2,as2,M,th));
  RooRealVar b2   ("b_{K^{*0}J/#psi}"            ,"b2"     ,0.82);//0.8 ) ;
  RooApollonios res2("res_{K^{*0}J/#psi}","res2",M, mu2,sigma2,b2);
    */


  //new test
  RooRealVar mu8  ("#mu_{#phi J/#psi}"    ,"mu8"           , 0.) ;
  RooRealVar sig8 ("#sigma^{0}_{#phi J/#psi}"   ,"sigma08" , 0.174);//0.181 ); // ± 0.021
  RooRealVar ns8  ("#eta_{#phi J/#psi}"         ,"eta8"    , 0.82);//1.15  ); // ± 0.2
  RooRealVar as8  ("#alpha_{#phi J/#psi}"       ,"alpha8"  , 2.9);//3.5   ); // ± 0.5
  RooFormulaVar sigma8("sigma_{#phi J/#psi}","@0*pow(TMath::Max(@3,@4),@1)/(1+@2*pow(TMath::Max(@3,@4),@1))",RooArgList(sig8,ns8,as8,M,th));
  RooRealVar b8   ("b_{#phi J/#psi}"            ,"b8"     ,0.89);//0.73 ) ;
  RooApollonios res8("res_{#phi J/#psi}","res8",M, mu8,sigma8,b8);


  /*
  RooRealVar mu8  ("#mu_{#phi J/#psi}"    ,"mu8"           , 0.) ;//0.00262);
  RooRealVar sig8 ("#sigma^{0}_{#phi J/#psi}"   ,"sigma08" , 0.181 );//0.174 // ± 0.021
  RooRealVar ns8  ("#eta_{#phi J/#psi}"         ,"eta8"    , 1.15  );//0.82 // ± 0.2
  RooRealVar as8  ("#alpha_{#phi J/#psi}"       ,"alpha8"  , 3.5   );//2.9 // ± 0.5
  RooFormulaVar sigma8("sigma_{#phi J/#psi}","@0*pow(TMath::Max(@3,@4),@1)/(1+@2*pow(TMath::Max(@3,@4),@1))",RooArgList(sig8,ns8,as8,M,th));
  RooRealVar b8   ("b_{#phi J/#psi}"            ,"b8"     ,0.73 ) ;//0.89);
  RooApollonios res8("res_{#phi J/#psi}","res8",M, mu8,sigma8,b8);
  */
  
  // Low-propertime acceptance 
  RooRealVar a1("a","a1",1.0,0.1,100., "ps^{-1}");
  RooRealVar n1("n","n1",1.0,-10.,10.);
  //RooRealVar t01("t_{0}(B_{d}#rightarrow K^{*0}#gamma)","t01",0.100,0.05,0.5,"ps");
  RooRealVar t0("t_{0}","t0",0.1,-0.5,0.5,"ps");

  RooRealVar a2("a_{B_{d}#rightarrow K^{*0}J#psi}","a2",5.,0.5,10., "ps^{-1}");
  RooRealVar n2("n_{B_{d}#rightarrow K^{*0}J#psi}","n2",2.1,0.5,5.);
  RooRealVar t02("t_{0}(B_{d}#rightarrow K^{*0}J/#psi)","t02",0.100,0.05,0.5,"ps");




  RooRealVar a4("a_{B_{s}#rightarrow #phi#gamma}","a4",5.,0.5,10., "ps^{-1}");
  RooRealVar n4("n_{B_{s}#rightarrow #phi#gamma}","n4",1.5,0.5,5.);
  //RooRealVar t04("t_{0}(B_{s}#rightarrow #phi#gamma)"  ,"t04",0.100,0.05,0.5,"ps");

  RooRealVar a8("a_{B_{s}#rightarrow #phi J/#psi}","a8",5.,0.5,10., "ps^{-1}");
  RooRealVar n8("n_{B_{s}#rightarrow #phi J/#psi}","n8",1.5,0.5,5.);
  RooRealVar t08("t_{0}(B_{s}#rightarrow #phiJ/#psi)"  ,"t08",0.100,0.05,0.5,"ps");






  RooRealVar dt0_12("#Delta t_{0A}","dt0_12",0.0,-0.500,0.500); 
  RooRealVar dt0_14("#Delta t_{0B}"  ,"dt0_14",0.0,-0.500,0.500);
  RooRealVar dt0_24("#Delta t_{0C}"  ,"dt0_24",0.0,-0.500,0.500);

  RooFormulaVar t01("t_{0}(B_{d}#rightarrow K^{*0}#gamma)","@0+@1",RooArgList(t02,dt0_12));
  RooFormulaVar t04("t_{0}(B_{s}#rightarrow #phi#gamma)"  ,"@0+@1",RooArgList(t01,dt0_14));
  //RooFormulaVar t04("t_{0}(B_{s}#rightarrow #phi#gamma)"  ,"@0+@1",RooArgList(t02,dt0_24));







  // High-propertime acceptance
  RooRealVar dG1("#delta#Gamma(B_{d}#rightarrow K^{*0}#gamma)","dG1",0.0,-1.0 , 1.0 , "ps^{-1}");
  RooRealVar dG2("#delta#Gamma(B_{d}#rightarrow K^{*0}J/#psi)","dG2",-0.0,-0.1 , 0.1 , "ps^{-1}");
  //RooRealVar dG4("#delta#Gamma(B_{s}#rightarrow #phi#gamma)"  ,"dG4",-0.00,-0.2 , 0.1 , "ps^{-1}");
  RooRealVar dG8("#delta#Gamma(B_{s}#rightarrow #phi J/#psi)" ,"dG8",-0.00,-0.2 , 0.1 , "ps^{-1}");
  //RooRealVar dG8("#delta#Gamma(B_{s}#rightarrow #phi J/#psi)" ,"dG8",-0.020);

  RooRealVar dG("#delta#Gamma","dG",0.0,-1.0 , 1.0 , "ps^{-1}");


  //RooFormulaVar DdG("#Delta[#delta#Gamma]","(@0-0.0208)",RooArgList(dG2));
  //RooFormulaVar DdG("#Delta[#delta#Gamma]","(@0-@1)*1000",RooArgList(dG1,dG2));
  //RooRealVar dDg("dDg","dDg",0,-0.100,0.100);  


  RooRealVar dDg12("#Delta#delta#Gamma_{A}","dDg12",-0.030,-0.100,0.100);  
  RooRealVar dDg28("dDg28","dDg28",0,-0.100,0.100);
  RooRealVar dDg14("#Delta#delta#Gamma_{B}","dDg14",0.003,-0.100,0.100);
  RooRealVar dDg42("#Delta#delta#Gamma_{C}","dDg42",0,-0.100,0.100);

  RooRealVar dDg22("dDg22","dDg22",0,-0.100,0.100);
  RooFormulaVar dG2prime("#delta#Gamma(B_{s}#rightarrow #phi#gamma)prime"  ,"@0+@1",RooArgList(dG2,dDg22));

  //a with same t0 new mc
  RooGaussian shift("shift","shift",dDg14,RooConst(-0.0031),RooConst(0.0029));
 
  //b with same t0 new mc
  //RooGaussian shift("shift","shift",dDg42,RooConst(-0.036),RooConst(0.0029));

  //tau_bd same t0 new mc
  //RooGaussian shift("shift","shift",dDg12,RooConst(-0.0331),RooConst(0.0029));

  //RooFormulaVar dG1("#delta#Gamma(B_{d}#rightarrow K^{*0}#gamma)","@0+@1",RooArgList(dG2,dDg12));
  RooFormulaVar dG4("#delta#Gamma(B_{s}#rightarrow #phi#gamma)"  ,"@0+@1",RooArgList(dG1,dDg14));
  //RooFormulaVar dG4("#delta#Gamma(B_{s}#rightarrow #phi#gamma)"  ,"@0+@1",RooArgList(dG2,dDg42));
  //RooFormulaVar dG8("#delta#Gamma(B_{s}#rightarrow #phi J/#psi)" ,"@0+@1",RooArgList(dG2,dDg28));
  //RooFormulaVar dG4("#delta#Gamma(B_{s}#rightarrow #phi#gamma)","@0+@1",RooArgList(dG8,dDg));



  
  //constraint from t>0
  /*correlation_gaussian_constraint(0,0)=1.478e-05;
  correlation_gaussian_constraint(0,1)=-6.597e-06;
  correlation_gaussian_constraint(1,0)=-6.597e-06;
  correlation_gaussian_constraint(1,1)=2.143e-05;*/

  //RooMultiVarGaussian shift("shift","shift",RooArgList(dt0_12,dDg),RooArgList(RooConst(-0.0241),RooConst(-0.0193)),correlation_gaussian_constraint);//

  //constraint from t>0.3 for tau bd fit new mc
  TMatrixDSym correlation_gaussian_constraint(2);
  /*correlation_gaussian_constraint(0,0)=3.672e-06;
  correlation_gaussian_constraint(0,1)=-1.569e-06;
  correlation_gaussian_constraint(1,0)=-1.569e-06;
  correlation_gaussian_constraint(1,1)=5.471e-06;

  RooMultiVarGaussian shift("shift","shift",RooArgList(dt0_12,dDg12),RooArgList(RooConst(-0.0195),RooConst(-0.0248)),correlation_gaussian_constraint);//*/






  //c2 same t0 new mc
  /*correlation_gaussian_constraint(0,0)=8.847e-06;
  correlation_gaussian_constraint(0,1)=-4.543e-06;
  correlation_gaussian_constraint(1,0)=-4.543e-06;
  correlation_gaussian_constraint(1,1)=8.700e-06;
  RooMultiVarGaussian shift("shift","shift",RooArgList(dDg12,dDg14),RooArgList(RooConst(-0.0328),RooConst(-0.003)),correlation_gaussian_constraint);//*/


  //a_0 new mc
  /*correlation_gaussian_constraint(0,0)=1.387e-05;
  correlation_gaussian_constraint(0,1)=-6.376e-06;
  correlation_gaussian_constraint(1,0)=-6.376e-06;
  correlation_gaussian_constraint(1,1)=1.149e-05;
  RooMultiVarGaussian shift("shift","shift",RooArgList(dt0_14,dDg14),RooArgList(RooConst(-0.0051),RooConst(0.00088)),correlation_gaussian_constraint);//*/


  //b_0 new mc
  /*correlation_gaussian_constraint(0,0)=9.324e-06;
  correlation_gaussian_constraint(0,1)=-3.995e-06;
  correlation_gaussian_constraint(1,0)=-3.995e-06;
  correlation_gaussian_constraint(1,1)=1.023e-05;
  RooMultiVarGaussian shift("shift","shift",RooArgList(dt0_24,dDg42),RooArgList(RooConst(-0.02439),RooConst(-0.0257)),correlation_gaussian_constraint);//*/



  //c_0 different t0 c2 new mc
  /*TMatrixDSym correlation_gaussian_constraint1(4);
  correlation_gaussian_constraint1(0,0)=1.027e-05;
  correlation_gaussian_constraint1(0,1)=-5.398e-06;
  correlation_gaussian_constraint1(0,2)=-4.519e-06;
  correlation_gaussian_constraint1(0,3)=2.425e-06;
  correlation_gaussian_constraint1(1,0)=-5.398e-06;
  correlation_gaussian_constraint1(1,1)=9.912e-06;
  correlation_gaussian_constraint1(1,2)=2.428e-06;
  correlation_gaussian_constraint1(1,3)=-4.333e-06;
  correlation_gaussian_constraint1(2,0)=-4.519e-06;
  correlation_gaussian_constraint1(2,1)=2.428e-06;
  correlation_gaussian_constraint1(2,2)=1.084e-05;
  correlation_gaussian_constraint1(2,3)=-5.579e-06;
  correlation_gaussian_constraint1(3,0)=2.425e-06;
  correlation_gaussian_constraint1(3,1)=-4.333e-06;
  correlation_gaussian_constraint1(3,2)=-5.579e-06;
  correlation_gaussian_constraint1(3,3)=1.051e-05;

  RooMultiVarGaussian shift("shift","shift",RooArgList(dt0_12,dt0_14,dDg12,dDg14),RooArgList(RooConst(-0.0196),RooConst(-0.00499),RooConst(-0.0247),RooConst(0.00088)),correlation_gaussian_constraint1);//*/



  //c same t0 c1 new mc
  //RooGaussian shift("shift","shift",dDg14,RooConst(-0.003),RooConst(0.0029));


  //c different t0 c1 new mc
  /*TMatrixDSym correlation_gaussian_constraint2(3);
  correlation_gaussian_constraint2(0,0)=1.019e-05;
  correlation_gaussian_constraint2(0,1)=-5.361e-06;
  correlation_gaussian_constraint2(0,2)=2.346e-06;
  correlation_gaussian_constraint2(1,0)=-5.361e-06;
  correlation_gaussian_constraint2(1,1)=9.897e-06;
  correlation_gaussian_constraint2(1,2)=-4.303e-06;
  correlation_gaussian_constraint2(2,0)=2.346e-06;
  correlation_gaussian_constraint2(2,1)=-4.303e-06;
  correlation_gaussian_constraint2(2,2)=1.044e-05;

  RooMultiVarGaussian shift("shift","shift",RooArgList(dt0_12,dt0_14,dDg14),RooArgList(RooConst(-0.0196),RooConst(-0.00499),RooConst(-0.00089)),correlation_gaussian_constraint2);//*/




  //****************************Clermot with HLT1Photon

  //A-1 clermont

  //RooGaussian shift("shift","shift",dDg14,RooConst(-0.001),RooConst(0.0036));

  //A-2 clermont
  /*TMatrixDSym correlation_gaussian_constraint(2);
  correlation_gaussian_constraint(0,0)=1.509e-05;
  correlation_gaussian_constraint(0,1)=-6.361e-06;
  correlation_gaussian_constraint(1,0)=-6.361e-06;
  correlation_gaussian_constraint(1,1)=1.555e-05;
  RooMultiVarGaussian shift("shift","shift",RooArgList(dt0_14,dDg14),RooArgList(RooConst(-0.0108),RooConst(0.0036)),correlation_gaussian_constraint);//*/




  //*******************************Valencia selection tight

  //A-1
  //RooGaussian shift("shift","shift",dDg14,RooConst(0.0144),RooConst(0.0041));

  //A-2
  /*TMatrixDSym correlation_gaussian_constraint(2);
  correlation_gaussian_constraint(0,0)=2.095e-05;
  correlation_gaussian_constraint(0,1)=-8.712e-06;
  correlation_gaussian_constraint(1,0)=-8.712e-06;
  correlation_gaussian_constraint(1,1)=2.064e-05;
  RooMultiVarGaussian shift("shift","shift",RooArgList(dt0_14,dDg14),RooArgList(RooConst(0.0020),RooConst(0.0135)),correlation_gaussian_constraint);//*/


  //*******************************Valencia selection loose

  //A-1
  //RooGaussian shift("shift","shift",dDg14,RooConst(0.0099),RooConst(0.0036));

  //A-2
  /*TMatrixDSym correlation_gaussian_constraint(2);
  correlation_gaussian_constraint(0,0)=1.738e-05;
  correlation_gaussian_constraint(0,1)=-7.347e-06;
  correlation_gaussian_constraint(1,0)=-7.347e-06;
  correlation_gaussian_constraint(1,1)=1.583e-05;
  RooMultiVarGaussian shift("shift","shift",RooArgList(dt0_14,dDg14),RooArgList(RooConst(-0.00315),RooConst(0.0112)),correlation_gaussian_constraint);//*/




  //ss********************test Jpsi kst
  /*TMatrixDSym correlation_gaussian_constraint1(4);
  correlation_gaussian_constraint1(0,0)=2.838e-05;
  correlation_gaussian_constraint1(0,1)=1.097e-04;
  correlation_gaussian_constraint1(0,2)=3.420e-04;
  correlation_gaussian_constraint1(0,3)=-2.155e-05;
  correlation_gaussian_constraint1(1,0)=1.097e-04;
  correlation_gaussian_constraint1(1,1)=2.097e-03;
  correlation_gaussian_constraint1(1,2)=-2.642e-04;
  correlation_gaussian_constraint1(1,3)=1.923e-04;
  correlation_gaussian_constraint1(2,0)=3.420e-04;
  correlation_gaussian_constraint1(2,1)=-2.642e-04;
  correlation_gaussian_constraint1(2,2)=1.202e-02;
  correlation_gaussian_constraint1(2,3)=-9.670e-04;
  correlation_gaussian_constraint1(3,0)=-2.155e-05;
  correlation_gaussian_constraint1(3,1)=1.923e-04;
  correlation_gaussian_constraint1(3,2)=-9.670e-04;
  correlation_gaussian_constraint1(3,3)=1.002e-04;

  RooMultiVarGaussian shift("shift","shift",RooArgList(dG2,a1,n1,t0),RooArgList(RooConst(-0.0177),RooConst(1.962),RooConst(2.26),RooConst(0.2037)),correlation_gaussian_constraint1);//*/








  /*TMatrixDSym correlation_gaussian_constraint_a_n_t0(3);
  correlation_gaussian_constraint_a_n_t0(0,0)=1.437e-03;
  correlation_gaussian_constraint_a_n_t0(0,1)=-5.567e-04;
  correlation_gaussian_constraint_a_n_t0(0,2)=1.669e-04;
  correlation_gaussian_constraint_a_n_t0(1,0)=-5.567e-04;
  correlation_gaussian_constraint_a_n_t0(1,1)=1.029e-02;
  correlation_gaussian_constraint_a_n_t0(1,2)=-9.835e-04;
  correlation_gaussian_constraint_a_n_t0(2,0)=1.669e-04;
  correlation_gaussian_constraint_a_n_t0(2,1)=-9.835e-04;
  correlation_gaussian_constraint_a_n_t0(2,2)=1.085e-04;*/


  //RooMultiVarGaussian shift("shift","shift",RooArgList(a1,n1,t0),RooArgList(RooConst(1.915),RooConst(2.12),RooConst(0.201)),correlation_gaussian_constraint_a_n_t0);//





















  //RooFormulaVar dG1("#delta#Gamma(B_{d}#rightarrow K^{*0}#gamma)","@0+@1",RooArgList(dG2,meanconstraint));
  //RooFormulaVar dG1("#delta#Gamma(B_{d}#rightarrow K^{*0}#gamma)","@0+@1",RooArgList(dG2,shift));
  //RooFormulaVar dG1("#delta#Gamma(B_{d}#rightarrow K^{*0}#gamma)","@0+@1",RooArgList(dG2,dDg));

  double tbd= -1.520;  // LHCb results  (2014)  1.524 ± 0.006 ± 0.04 arXiv 1402.2554  (LHCb-PAPER 2013-065)
  double tbs= -1.509;  //  PDG 2013                      (1.516 ± 0.011)  . 10^{-12}s
  double dgs=  0.081;   // ""  (0.081 ± 0.011) . 10^{-12}s
  double tbsphipsi  = -1.480 ;  // LHCb results (2014)  1.480 ± 0.011 ± 0.05 arXiv 1402.2554  (LHCb-PAPER 2013-065)
  double tbsphig  = tbs;  //  DUMMY VALUE - THIS IS OUR MEASUREMENT !
  double _AD=0.433; //LHCb-PAPER-2013-065, Régis calculations
  
   
  RooRealVar     tauBd2kstg("#tau(B_{d}#rightarrow K^{*0}#gamma)"  , "tau1", tbd  ,"ps");//,-10., -0.1   ,"ps");//
  RooRealVar     tauBd2kstpsi("#tau(B_{d}#rightarrow K^{*0}J#psi)" , "tau2", tbd  ,"ps");//,-1.544, -1.444   ,"ps");//
  RooRealVar     tauBs2phig("#tau(B_{s}#rightarrow#phi#gamma)"     , "tau4", tbsphig  ,"ps");
  RooRealVar     tauBs2phipsi("#tau(B_{s}#rightarrow#phi#psi)"     , "tau8", tbsphipsi,"ps");

  RooRealVar     taus("#tau_{B_{s}}"      , "tau_Bs", tbs ,-10., -0.1 ,"ps");//
  RooRealVar     DGs("#Delta#Gamma_{s}"  ,  "DG_s" , dgs  ,0.02,0.15,"ps^{-1}");//  
  RooRealVar     AD("A^{#Delta}_{blind}"  ,  "AD"  , 0.,-10.,10.);






  TMatrixDSym correlation_gaussian_constraint_external(2);
  correlation_gaussian_constraint_external(0,0)=1.6e-05;
  correlation_gaussian_constraint_external(0,1)=6.504e-06;
  correlation_gaussian_constraint_external(1,0)=6.504e-06;
  correlation_gaussian_constraint_external(1,1)=3.6e-05;

  RooMultiVarGaussian shift_external("shift_external","shift_external",RooArgList(taus,DGs),RooArgList(RooConst(-1.509),RooConst(0.081)),correlation_gaussian_constraint_external);//



  RooGaussian shift_externaltbd("shift_externaltbd","shift_externaltbd",tauBd2kstg,RooConst(-1.520),RooConst(0.004));



  RooArgSet constSet=RooArgSet();//shift,shift_external,shift_externaltbd);




  /*blinding A delta*/
  RooUnblindUniform *AD_unblinded = new RooUnblindUniform("AD_unblinded", "AD_unblinded", "bs2phigammarocks",2, AD);
  /*blinding A delta*/

  RooRealVar     AD_psiphi("A^{#Delta}_{J/#psi#phi}"  ,  "AD_psiphi"  , _AD);//,-1.,1.);

  /*
  RooProperTime  time1_("time1_","time1_",M,tauBd2kstg  ,a1,n1,dG1,t01,1); 
  RooProperTime  time2_("time2_","time2_",M,tauBd2kstpsi,a2,n2,dG2,t02,1); 
  RooProperTime  time4_("time4_","time4_",M,tauBs2phig  ,a4,n4,dG4,t04,1);  
  //RooBsProperTime  time4_("time4_","time4_",M,taus,DGs,AD,a4,n4,dG1,t04,1); 
  RooProperTime  time8_("time8_","time8_",M,tauBs2phipsi,a8,n8,dG8,t08,1); 
  */

  /* == ACCEPTANCE CONSTRAINTS == */
  std::cout << " === APPLY ACCEPTANCE CONSTRAINTS ==== " << endl;
  
  RooProperTime  time1_("time1_","time1_",M,tauBd2kstg  ,a1,n1,dG,t0,1); 
  //RooBsProperTime  time1_("time1_","time1_",M,tauBd2kstg,DGs,AD,a1,n1,dG1,t01,1);
  RooProperTime  time2_("time2_","time2_",M,tauBd2kstg,a1,n1,dG2,t0,1);
  //RooProperTime  time8_("time8_","time8_",M,tauBs2phipsi,a1,n1,dG8,t08,1);  
  //RooProperTime  time4_("time4_","time4_",M,tauBs2phig  ,a4,n4,dG4,t04,1);    
  /* == */

  /**/
  RooBsProperTime  time4_("time4_","time4_",M,taus,DGs,*AD_unblinded,a1,n1,dG4,t0,1); 
  RooBsProperTime  time8_("time8_","time8_",M,taus,DGs,AD_psiphi,a1,n1,dG2,t01,1);
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
  RooRealVar y1 ("N_{B_{d} #rightarrow K^{*0}#gamma}" ,"y1" ,20000. ,0. ,75000. );
  RooRealVar y2 ("N_{B_{d} #rightarrow K^{*0}J/#psi}" ,"y2" ,30000. ,0. ,75000. );
  RooRealVar y4 ("N_{B_{s} #rightarrow #phi#gamma}"   ,"y4" ,5000.  ,0. ,25000. );
  RooRealVar y8 ("N_{B_{s} #rightarrow #phi J/#psi}"  ,"y8" ,5000.  ,0. ,25000. );
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
  parList.add(tauBd2kstpsi);
  parList.add(AD);
  parList.add(AD_psiphi);
  parList.add(dDg12);
  parList.add(dDg14);
  parList.add(dDg42);
  parList.add(dt0_12);
  parList.add(dt0_14);
  parList.add(dt0_24);
  parList.add(dG);
  parList.add(taus);
  parList.add(DGs);
  //====== Define Minos set
  RooCmdArg myMinos;
  if( minos)
    //myMinos =  RooCmdArg(Minos(parList));
    myMinos =  RooCmdArg(Minos(true));
  else
    myMinos = RooCmdArg(Minos(false));

  
  //======================== Fitting ===============================//

  RooFitResult* result1=NULL;
  RooFitResult* result2=NULL;
  RooFitResult* result4=NULL;
  RooFitResult* result8=NULL;
  RooFitResult* result = NULL;

  M.setRange("Range_sample1",0.,15.) ;
  M.setRange("Range_sample2",0.,15.) ; 
  M.setRange("Range_sample4",0.3,15.) ;
  M.setRange("Range_sample8",0.,15.) ;


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



    M.setBins(nbin);
 

    std::vector<RooCmdArg> cmds;
    if( binned ){
      if( mask & 1){cmds.push_back(Import("sample1",*dataH1));std::cout << "Import kst0gamma RooDataHist"<<std::endl;}
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
      M.setBins(10000,"cache") ; // for convolution
      //result = simPDF.fitTo(*combData,Extended(kTRUE),Save(),myMinos,NumCPU(nCPU),SumW2Error(w2Error));
      result = simPDF.fitTo(*combData,Extended(kTRUE),Range("Range"),SplitRange(true),Save(),myMinos,NumCPU(nCPU),SumW2Error(w2Error),ExternalConstraints(constSet)); //
      if( cmds.size()>0)delete combData;
    }else{
      if( mask & 1){cmds.push_back(Import("sample1",*dataS1));std::cout << "Import kst0gamma RooDataSet"<<std::endl;}
      if( mask & 2){cmds.push_back(Import("sample2",*dataS2));std::cout << "Import kst0psi RooDataSet"<<std::endl;}
      if( mask & 4){cmds.push_back(Import("sample4",*dataS4));std::cout << "Import phigamma RooDataSet"<<std::endl;}
      if( mask & 8){cmds.push_back(Import("sample8",*dataS8));std::cout << "Import phipsi RooDataSet"<<std::endl;}
      std::cout << "... Imported " << cmds.size() << " dataSet to the combined RooDataSet"<<std::endl;
      RooDataSet* combData=NULL;
      if( cmds.size() == 1)combData= new RooDataSet("combData","combined data",RooArgSet(M,SIGYield_peeking_sw,www),Index(sample),cmds[0],WeightVar("SIGYield_peeking_sw"),WeightVar("www"));
      if( cmds.size() == 2)combData= new RooDataSet("combData","combined data",RooArgSet(M,SIGYield_peeking_sw,www),Index(sample),cmds[0],WeightVar("SIGYield_peeking_sw"),cmds[1],WeightVar("www"));//
      if( cmds.size() == 3)combData= new RooDataSet("combData","combined data",RooArgSet(M,SIGYield_peeking_sw,www),Index(sample),cmds[0],WeightVar("SIGYield_peeking_sw"),cmds[1],cmds[2],WeightVar("www"));
      if( cmds.size() == 4)combData= new RooDataSet("combData","combined data",RooArgSet(M,SIGYield_peeking_sw,www),Index(sample),cmds[0],WeightVar("SIGYield_peeking_sw"),cmds[1],cmds[2],cmds[3],WeightVar("www"));
      if( cmds.size() == 5)
        combData= new RooDataSet("combData","combined data",M,Index(sample),cmds[0],cmds[1],cmds[2],cmds[3],cmds[4]);
      if( cmds.size() == 6)
        combData= new RooDataSet("combData","combined data",M,Index(sample),cmds[0],cmds[1],cmds[2],cmds[3],cmds[4],cmds[5]);
      std::cout << "Starting simultaneous unbinned fit ..."<<std::endl;

      std::cout << *combData<<std::endl;

      M.setBins(10000,"cache") ; // for convolution
      //result = simPDF.fitTo(*combData,Extended(kTRUE),Save(),myMinos,NumCPU(nCPU),SumW2Error(w2Error)); 
      result = simPDF.fitTo(*combData,Extended(kTRUE),Save(),myMinos,NumCPU(nCPU),SumW2Error(w2Error),ExternalConstraints(constSet)); //
      if( cmds.size()>0)delete combData;
    }
    result1=result;
    result2=result;
    result4=result;
    result8=result;
    result->Print();
    dG1.Print();
    dG2.Print();
    //std::cout << "delta gamma" << dG1 << std::endl;
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

    /*
    dataH1->plotOn(frame1,RooFit::Range(0,0.5));
    PDF1.plotOn(frame1);
    frame1->Draw();
    */
    schi2 += rooDisplay(M,dataH1, frame1, c1, PDF1 , result1,out,parList,logY,"",Range("Range_sample1"),RooFit::Layout(0.55,0.9,0.9));
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
    schi2 += rooDisplay(M,dataH2, frame2, c2, PDF2 , result2,out,parList,logY,"Poisson",Range("Range_sample2"),RooFit::Layout(0.55,0.9,0.9));
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
    schi2 += rooDisplay(M,dataH4, frame4, c4, PDF4 , result4,out,parList,logY,"Poisson",Range("Range_sample4"),RooFit::Layout(0.55,0.9,0.9));
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
    schi2 += rooDisplay(M,dataH8, frame8, c8, PDF8 , result8,out,parList,logY,"Poisson",Range("Range_sample8"),RooFit::Layout(0.55,0.9,0.9));
    ndof  += nbin;
    npar  += result8->floatParsFinal().getSize();
  }
  /* //=================== Finalization
  if(simultaneous) npar = result->floatParsFinal().getSize();
  ndof -= npar;
  double p= TMath::Prob(schi2,ndof)*100;
  std::cout << "========== ++++ Global chi2/ndof = " << schi2 << "/"<<ndof << " -- p : " << p << " %" <<std::endl;*/
}





