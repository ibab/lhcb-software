////////////////////////////////////////////////////////////////////////
#include "TMultiLayerPerceptron.h"
#include "TMLPAnalyzer.h"
#include "TMVA/Reader.h"
#include "TMVA/Factory.h"

Float_t mult, ptB, partP, partPt, IPPV, nndeta, nndphi, nndq;
Float_t ncand, nkrec, vtx_ptmin,vtx_ipsmin,vtx_docamax,vtx_likel,vtx_longfrac,vtx_WeigVch;
TMVA::Reader *reader=0, *reader_mu=0, *reader_e=0, *reader_k=0, 
  *reader_kS=0, *reader_pS=0;
bool initmva_mu, initmva_e, initmva_k, initmva_kS, initmva_pS;
double P0_mu(0), P1_mu(0), P2_mu(0), P3_mu(0);
double P0_e(0),  P1_e(0),  P2_e(0),  P3_e(0);
double P0_k(0),  P1_k(0),  P2_k(0),  P3_k(0);
double P0_kS(0), P1_kS(0), P2_kS(0), P3_kS(0);
double P0_pS(0), P1_pS(0), P2_pS(0), P3_pS(0);

//TMVA method
bool myinit = true;
std::string m_WeightsFile= "weights/TMVAClassification_MLP.weights.xml"; //for bs (kSS) --> GOOD
std::string m_TmvaMethod = "MLP method";
TMVA::Reader *reader_comb=0;


////////////////////////////////////////////////////////////////////////
void normalise(std::vector<double>& par){
  if( par.size() != 10 ) {
    cout<<"ERROR in normalise!"<<endl;
    return;
  }

  if(DBG) {
  cout<<"before  "<<par.at(0)<<"  "<<par.at(2)<<"  "<<par.at(3)
      <<"  "<<par.at(4)<<" "<<par.at(5)<<" "<<par.at(6)<<"  "<<par.at(7)
      <<"  "<<par.at(8)<<"  "<<par.at(9)<<"  "<<par.at(1)<<endl;
  }

  par.at(0) /= 90.; //mult
  par.at(1) /= 25.; //ptB
  par.at(2) = std::min( sqrt(par.at(2))/15., 1.); //partP
  par.at(3) = std::min( sqrt(par.at(3))/2.5, 1.); //partPt
  double tmp=0;
  if(par.at(4)<0) tmp=-sqrt(-par.at(4)); else tmp=sqrt(par.at(4));//IPPV
  tmp /= 10.; 
  if(tmp> 1) tmp= 1; if(tmp<-1) tmp=-1;
  par.at(4)=tmp;
  par.at(5) /= 2.; //nndeta
  par.at(6) /= 3.; //nndphi
  par.at(7) /= 12.;//nndq
  if(par.at(7)>1.) par.at(7) = 1.;
  par.at(8) = (par.at(8)-1)/4.;//krec
  par.at(9) = (par.at(9)-1)/3;//ncands
}

#include "weights/NNmuon.cxx"
double MLP_muon(std::vector<double>& par, double& rnet) {

  normalise(par);

  NNmuon net;
  rnet= net.Value(0, par.at(0),par.at(2),par.at(3),par.at(4),par.at(8),par.at(1));

//   double out= 1-pol(rnet, 9.610658e-01, -9.336494e-01);// <=========NU=1
  double out= 1-pol(rnet,0.974458, -0.967522);
  
  if(DBG) {
    cout<<"mu after  "<<" "<<par.at(0)<<"  "<<par.at(2)<<"  "<<par.at(3)
	<<"  "<<par.at(4)<<"  "<<par.at(8)<<par.at(9)<<"  "<<par.at(1)<<endl;
    cout<<"mu rnet="<<rnet<<"  pn="<<out<<endl;
  }
  return out;
};

#include "weights/NNele.cxx"
double MLP_ele(std::vector<double>& par, double& rnet) {
  normalise(par);
  NNele net;

  rnet= net.Value(0, par.at(0),par.at(2),par.at(3),par.at(4),par.at(8),par.at(1));

//   double out=1-pol(rnet, 7.353325e-01, -6.463841e-01);// <========= NU=1
  double out=1-pol(rnet, 0.865053, -0.813964) ;
 
  if(DBG) {
    cout<<"ele after  "<<" "<<par.at(0)<<"  "<<par.at(2)<<"  "<<par.at(3)<<"  "
	<<par.at(4)<<"  "<<par.at(8)<<"  "<<par.at(9)<<"  "<<par.at(1)<<endl;
    cout<<"ele rnet="<<rnet<<"  pn="<<out<<endl;
  }
  return out;
};
#include "weights/NNkaon.cxx"
double MLP_kaon(std::vector<double>& par, double& rnet) {
  normalise(par);

  NNkaon net;
  rnet= net.Value(0, par.at(0),par.at(2),par.at(3),par.at(4),par.at(8),par.at(1));

//   double out= 1-pol(rnet, 9.149897e-01, -8.646468e-01);// <=========NU=1
  double out= 1-pol(rnet, 0.967369, -0.956133 );

  if(DBG) {
    cout<<"kOppo after  "<<" "<<par.at(0)<<"  "<<par.at(2)<<"  "<<par.at(3)<<"  "
      <<par.at(4)<<"  "<<par.at(8)<<"  "<<par.at(9)<<"  "<<par.at(1)<<endl;
  cout<<"k rnet="<<rnet<<"  pn="<<out<<endl;
  }

  return out;
};
#include "weights/NNkaonS.cxx"
double MLP_kaonS(std::vector<double>& par, double& rnet) {
  normalise(par);

  NNkaonS net;
  rnet= net.value(0, par.at(0),par.at(2),par.at(3),par.at(4),
		  par.at(5),par.at(6),par.at(7),par.at(8),par.at(9),par.at(1) );

  double out=1-pol(rnet, 9.845081e-01, -9.742343e-01);// <=========NU=1
 
  if(DBG) {
  cout<<"kS after  "<<par.at(0)<<"  "<<par.at(2)<<"  "<<par.at(3)<<"  "
      <<par.at(4)<<" "<<par.at(5)<<" "<<par.at(6)<<"  "<<par.at(7)<<"  "
      <<par.at(8)<<"  "<<par.at(9)<<"  "<<par.at(1)<<endl;
  cout<<"kS rnet="<<rnet<<"  pn="<<out<<endl;
  }
  return out;
};
#include "weights/NNpionS.cxx"
double MLP_pionS(std::vector<double>& par, double& rnet) {
  normalise(par);

  NNpionS net;
  rnet= net.Value(0, par.at(0),par.at(2),par.at(3),par.at(4),
		  par.at(5),par.at(6),par.at(7),par.at(8),par.at(1) );

//   double out=1-pol(rnet, 1.222453, -1.369672 );// <=========NU=1
  double out=1-pol(rnet, 1.05709,  -1.09695);

  if(DBG) {
  cout<<"pS after  "<<par.at(0)<<"  "<<par.at(2)<<"  "<<par.at(3)<<"  "
      <<par.at(4)<<" "<<par.at(5)<<" "<<par.at(6)<<"  "<<par.at(7)<<"  "
      <<par.at(8)<<"  "<<par.at(9)<<"  "<<par.at(1)<<endl;
  cout<<"pS rnet="<<rnet<<"  pn="<<out<<endl;
  }
  return  out;
};
#include "weights/NNvtx.cxx"
double MLP_vtx(std::vector<double>& par, double& rnet) {

  NNvtx net;
  rnet= net.Value(0, par.at(0),par.at(1),par.at(2),par.at(3),par.at(4),
		  par.at(5),par.at(6),par.at(7),par.at(8),par.at(9) );

//   double out=1-pol(rnet, 1., -1. );// <=========NU=1
//  double out=1-pol(rnet, 0.229318, 1.46734, -2.30111);//bad trained
  double out=1-pol(rnet, 0.359564, 0.972845, -2.14715, 0.644519);//good trained

  if(DBG) {
    cout<<"vtx after  "<<par.at(0)<<"  "<<par.at(1)<<"  "<<par.at(2)<<"  "
	<<par.at(3)<<" "<<par.at(4)<<" "<<par.at(5)<<"  "<<par.at(6)<<"  "
	<<par.at(7)<<"  "<<par.at(8)<<"  "<<par.at(9)<<endl;
    cout<<"vtx rnet="<<rnet<<"  pn="<<out<<endl;
  }
  return  out;
};

//////////////////////////////////////////////////
void MultiLayerPerceptronTuning(TString NNetTrain){
  //  TApplication app("appKey",0,0);

  TFile* inputFile = TFile::Open( "tag.root" );
  TTree* nntrain   = (TTree*)inputFile->Get("nntrain");

  // for e,mu,K:
  if(NNetTrain=="e" || NNetTrain=="mu" || NNetTrain=="k" ) {
    cout<<"\nWill train NNet for tagger: "<<NNetTrain<<endl;
    Int_t ntrain = 100;
    TMultiLayerPerceptron *mlp = 
      new TMultiLayerPerceptron("mult,partP,partPt,IPPV,nkrec,ptB:10:@iscorrect","1.",
				nntrain,"Entry$%2","(Entry$+1)%2");
    mlp->Train(ntrain, "text,graph,update=5");
    TCanvas* mlpa_canvas = new TCanvas("mlpa_canvas",
				       "Network analysis",1200,0,300,600);
    mlpa_canvas->Divide(1,3);
    TMLPAnalyzer ana(mlp);
    ana.GatherInformations(); //Initialisation
    ana.CheckNetwork(); //output to the console
    mlpa_canvas->cd(1); mlp->Draw(); //shows the network structure
    mlpa_canvas->cd(2); ana.DrawDInputs(); //shows how works each variable 
    mlpa_canvas->cd(3);
    mlpa_canvas->Update();
    mlpa_canvas->WaitPrimitive(); //will return with a double click
 
    ana.DrawNetwork(0,"iscorrect==1","iscorrect==0"); //draws the result
    //write out code
    if(NNetTrain=="mu") mlp->Export("weights/NNmuon");
    if(NNetTrain=="e")  mlp->Export("weights/NNele");
    if(NNetTrain=="k")  mlp->Export("weights/NNkaon");
  }
  // for KS,piS:
  if(NNetTrain=="kS" || NNetTrain=="pS") {
    cout<<"\nWill train NNet for tagger: "<<NNetTrain<<endl;
    Int_t ntrain = 100;
    TMultiLayerPerceptron *mlp = 
      new TMultiLayerPerceptron(
	  "mult,partP,partPt,IPPV,nndeta,nndphi,nndq,nkrec,ptB:10:@iscorrect", "1.",
	  nntrain,"Entry$%2","(Entry$+1)%2");
    mlp->Train(ntrain, "text,graph,update=5");
    TCanvas* mlpa_canvas= new TCanvas("mlpa_canvas",
				      "Network analysis",1200,0,300,600);
    mlpa_canvas->Divide(1,3);
    TMLPAnalyzer ana(mlp);
    ana.GatherInformations(); //Initialisation
    ana.CheckNetwork(); //output to the console
    mlpa_canvas->cd(1); mlp->Draw(); //shows the network structure
    mlpa_canvas->cd(2); ana.DrawDInputs(); //shows how works each variable 
    mlpa_canvas->cd(3);
    mlpa_canvas->Update();
    mlpa_canvas->WaitPrimitive(); //will return with a double click
    ana.DrawNetwork(0,"iscorrect==1","iscorrect==0"); //draws the result
    //write out code
    if(NNetTrain == "kS") mlp->Export("weights/NNkaonS");
    if(NNetTrain == "pS") mlp->Export("weights/NNpionS");
  }
  if(NNetTrain=="vtx") {
    cout<<"\nWill train NNet for tagger: vtx"<<endl;
    Int_t ntrain = 50;
    TMultiLayerPerceptron *mlp = 
      new TMultiLayerPerceptron(
	  "mult,nkrec,ptB,ncand,vtx_ptmin,vtx_ipsmin,vtx_docamax,vtx_likel,vtx_longfrac,vtx_WeigVch:10:@iscorrect", "1.", nntrain,"Entry$%2", "(Entry$+1)%2");

    mlp->Train(ntrain, "text,graph,update=5");
    TCanvas* mlpa_canvas= new TCanvas("mlpa_canvas",
				      "Network analysis",1200,0,300,600);
    mlpa_canvas->Divide(1,3);
    TMLPAnalyzer ana(mlp);
    ana.GatherInformations(); //Initialisation
    ana.CheckNetwork(); //output to the console
    mlpa_canvas->cd(1); mlp->Draw(); //shows the network structure
    mlpa_canvas->cd(2); ana.DrawDInputs(); //shows how works each variable 
    mlpa_canvas->cd(3);
    mlpa_canvas->Update();
    mlpa_canvas->WaitPrimitive(); //will return with a double click
    ana.DrawNetwork(0,"iscorrect==1","iscorrect==0"); //draws the result
    //write out code
    mlp->Export("weights/NNvtx");
  }
  if(NNetTrain!="none") {
    cout<<"\n* 1. Now quit, set NNetTrain=\"none\" and rerun on the same data!"<<endl;
    cout<<"* 2. Then update the fitted values into pn[i]."<<endl;
    cout<<"* 3. Finally rerun and look at your combined performance\n"<<endl;
    return;
  }
}


//////////TMVA Analysis//////////////////////////////////
void TMVATuning(TString NNetTrain){
/////////////////////////////////////////////////////////

  // Create a new root INPUT file.
  TFile* inputFile = TFile::Open( "tag.root" );

  // Create a new root OUTPUT file.
  TFile* outputFile = TFile::Open( "tag.tmva.root", "RECREATE" );
  // The second argument is the output file for the training results
  TMVA::Factory *factory = new TMVA::Factory( "MVAnalysis", outputFile, "" );

  // Get input trees
  TTree* inputTree = (TTree*)inputFile->Get("nntrain");
  TCut signalCut = "iscorrect==1";
  TCut backgrCut = "iscorrect==0";
  factory->SetInputTrees(inputTree, signalCut, backgrCut);

  //--- declare variables to be used for the analysis
  factory->AddVariable( "mult",   'F');
  factory->AddVariable( "ptB",    'F');
  factory->AddVariable( "partP",  'F');
  factory->AddVariable( "partPt", 'F');
  factory->AddVariable( "IPPV",   'F');
  factory->AddVariable( "nkrec",   'F');
  factory->AddVariable( "ncand",   'F');
  if(NNetTrain=="kS" || NNetTrain=="pS") factory->AddVariable( "nndeta", 'F');
  if(NNetTrain=="kS" || NNetTrain=="pS") factory->AddVariable( "nndphi", 'F');
  if(NNetTrain=="kS" || NNetTrain=="pS") factory->AddVariable( "nndq",   'F');
  if(NNetTrain=="vtx") factory->AddVariable( "vtx_ptmin",   'F');
  if(NNetTrain=="vtx") factory->AddVariable( "vtx_ipsmin",   'F');
  if(NNetTrain=="vtx") factory->AddVariable( "vtx_docamax",   'F');
  if(NNetTrain=="vtx") factory->AddVariable( "vtx_longfrac",   'F');


  TCut  presel = "";
  const TString nevet =  "NSigTrain=0:NBkgTrain=0";
  factory->PrepareTrainingAndTestTree( presel, nevet );

  //--- book MVA methods
  factory->BookMethod(TMVA::Types::kMLP, "MLP","!V:NCycles=400:HiddenLayers=N:TestRate=5:NeuronType=sigmoid");
  factory->BookMethod( TMVA::Types::kLikelihood, "LikelihoodD", "!H:!V:!TransformOutput:PDFInterpol=Spline2:NSmoothSig[0]=10:NSmoothBkg[0]=10:NSmooth=5:NAvEvtPerBin=50:VarTransform=Decorrelate" );
  factory->BookMethod( TMVA::Types::kFisher, "Fisher","H:!V:!Normalise:CreateMVAPdfs:Fisher:NbinsMVAPdf=50:NsmoothMVAPdf=1" );
  factory->BookMethod( TMVA::Types::kRuleFit, "RuleFit","H:!V:RuleFitModule=RFTMVA:Model=ModRuleLinear:MinImp=0.001:RuleMinDist=0.001:NTrees=20:fEventsMin=0.01:fEventsMax=0.5:GDTau=-1.0:GDTauPrec=0.01:GDStep=0.01:GDNSteps=10000:GDErrScale=1.02");
  //factory->BookMethod( TMVA::Types::kSVM, "SVM_Gauss", "Sigma=2:C=1:Tol=0.001:Kernel=Gauss" );

  factory->TrainAllMethods();
  factory->TestAllMethods();
  factory->EvaluateAllMethods();

  outputFile->Close();
  std::cout << "==> wrote root file tag.tmva.root" << std::endl<< std::endl;

  TString outname = "weights/NN<tagger>_MLP.weights.txt";
  cout<<" Now launch: root -l TMVAGui.C \n";
  cout<<"  ..if you are happy with the result:  mv "
      <<outname<< (" NN_MLP<tagger>.weights.txt\n\n");

  delete factory;
}

//=====================================================================
void setFitParametersOf(const TString& tagger) {

  string flagstring;
  ifstream in("weights/NN_FitParameters.txt");
  while(!in.eof()) {
    in >> flagstring;
    if(flagstring != tagger) continue;
    if(flagstring == "mu") in >> P0_mu >> P1_mu >> P2_mu >> P3_mu; 
    if(flagstring == "e" ) in >> P0_e  >> P1_e  >> P2_e  >> P3_e;
    if(flagstring == "k" ) in >> P0_k  >> P1_k  >> P2_k  >> P3_k;
    if(flagstring == "kS") in >> P0_kS >> P1_kS >> P2_kS >> P3_kS;
    if(flagstring == "pS") in >> P0_pS >> P1_pS >> P2_pS >> P3_pS;
  }
//   cout<<"\nUse  "<< "mu "<< P0_mu <<" "<< P1_mu <<" "<< P2_mu <<" "<< P3_mu;
//   cout<<"\n     "<< "e  "<< P0_e  <<" "<< P1_e  <<" "<< P2_e  <<" "<< P3_e;
//   cout<<"\n     "<< "k  "<< P0_k  <<" "<< P1_k  <<" "<< P2_k  <<" "<< P3_k;
//   cout<<"\n     "<< "kS "<< P0_kS <<" "<< P1_kS <<" "<< P2_kS <<" "<< P3_kS;
//   cout<<"\n     "<< "pS "<< P0_pS <<" "<< P1_pS <<" "<< P2_pS <<" "<< P3_pS
//       <<"\n";
}

//=====================================================================
double Classify (const TString& type, const TString& tagger,	
		 std::vector<double>& NNinputs, double& rnet){

  if(type=="TMVA") {

    TString method = "none";
    if     (tagger=="mu") method = "Fisher";
    else if(tagger=="e")  method = "Fisher";
    else if(tagger=="k")  method = "LikelihoodD";
    else if(tagger=="kS") method = "Fisher";
    else if(tagger=="pS") method = "LikelihoodD";
    else if(tagger=="finalize") {
      if(reader_mu!=NULL)  delete reader_mu;
      if(reader_e !=NULL)  delete reader_e;
      if(reader_k !=NULL)  delete reader_k;
      if(reader_kS!=NULL)  delete reader_kS;
      if(reader_pS!=NULL)  delete reader_pS;
      return -1;
    } else cout<<"ERROR unknown "<<tagger<<endl;

    //Init phase, done only once:
    if( (initmva_k  and tagger=="k") or (initmva_kS and tagger=="kS") or
	(initmva_pS and tagger=="pS") or (initmva_e and tagger=="e") or 
	(initmva_mu and tagger=="mu") ) {
      reader = new TMVA::Reader();
      reader->AddVariable("mult",   &mult);
      reader->AddVariable("ptB",    &ptB);
      reader->AddVariable("partP",  &partP);
      reader->AddVariable("partPt", &partPt);
      reader->AddVariable("IPPV",   &IPPV);
      reader->AddVariable("IPPV",   &IPPV);
      reader->AddVariable("nkrec",  &nkrec);
      reader->AddVariable("ncand",  &ncand);
      if( tagger=="kS" or tagger=="pS" ) {
	reader->AddVariable("nndeta", &nndeta);
	reader->AddVariable("nndphi", &nndphi);
	reader->AddVariable("nndq",   &nndq);
      }
      TString filename = "weights/NN"+tagger+ "_"+method+".weights.txt";
      //cout<<"..booking "<<method<<"  at: "<<filename<<endl;
      reader->BookMVA( method+" method", filename );
      if     (tagger=="mu")  {reader_mu  = reader; initmva_mu =false;}
      else if(tagger=="e")   {reader_e   = reader; initmva_e  =false;}
      else if(tagger=="k")   {reader_k   = reader; initmva_k  =false;}
      else if(tagger=="kS")  {reader_kS  = reader; initmva_kS =false;}
      else if(tagger=="pS")  {reader_pS  = reader; initmva_pS =false;}

      //set fit parameter to transform rnet into omega:
      setFitParametersOf(tagger); //read from weights/NN_FitParameters.txt
    }

    normalise(NNinputs);
    mult   = NNinputs.at(0);
    ptB    = NNinputs.at(1);
    partP  = NNinputs.at(2);
    partPt = NNinputs.at(3);
    IPPV   = NNinputs.at(4);
    nndeta = NNinputs.at(5);
    nndphi = NNinputs.at(6);      
    nndq   = NNinputs.at(7);
    nkrec  = NNinputs.at(8);
    ncand  = NNinputs.at(9);
    if     (tagger=="mu")  {
      rnet = (double)reader_mu ->EvaluateMVA(method+" method");
      return 1-pol(rnet, P0_mu, P1_mu, P2_mu, P3_mu);
    }
    else if(tagger=="e")   {
      rnet = (double)reader_e  ->EvaluateMVA(method+" method");
      return 1-pol(rnet, P0_e, P1_e, P2_e, P3_e);
    }
    else if(tagger=="k")   {
      rnet = (double)reader_k  ->EvaluateMVA(method+" method");
      return 1-pol(rnet, P0_k, P1_k, P2_k, P3_k);
    }
    else if(tagger=="kS")  {
      rnet = (double)reader_kS ->EvaluateMVA(method+" method");
      return 1-pol(rnet, P0_kS, P1_kS, P2_kS, P3_kS);
    }
    else if(tagger=="pS")  {
      rnet = (double)reader_pS ->EvaluateMVA(method+" method");
      return 1-pol(rnet, P0_pS, P1_pS, P2_pS, P3_pS);
    }

    return -1;

  } else if( type== "MultiLayerPerceptron" ) { 
    if     (tagger=="mu") return MLP_muon ( NNinputs, rnet ); 
    else if(tagger=="e" ) return MLP_ele  ( NNinputs, rnet ); 
    else if(tagger=="k" ) return MLP_kaon ( NNinputs, rnet ); 
    else if(tagger=="kS") return MLP_kaonS( NNinputs, rnet ); 
    else if(tagger=="pS") return MLP_pionS( NNinputs, rnet ); 
    else if(tagger=="vtx")return MLP_vtx  ( NNinputs, rnet ); 
    else cout<<"ERROR unknown "<<tagger<<endl;
    return -1;
  }

  cout<<"WARNING: something wrong in Classify.\n";
  return -1;
}

//=====================================================================                                                                       
double CombineTaggers(const std::vector<int>&    itag,
                      const std::vector<double>& pn ,
                      double& pnsum ) {
  
  double probPlus = 0.5;
  Float_t pmu, pe, pk, pss, pvtx;
  std::vector<double> pnfinal(6);
  for (int ik=1; ik<6; ik++){
    if (itag[ik]<0){
      pnfinal[ik]=1-pn[ik];
    }
    else pnfinal[ik]=pn[ik];
    if (DBGNN) cout<< "ik="<<ik<<" itag="<<itag[ik]<<" pn="<<pn[ik]<<" pplus="<<pnfinal[ik]<<endl;
  }
  pmu=pnfinal[1];
  pe=pnfinal[2];
  pk=pnfinal[3];
  pss=pnfinal[4];
  pvtx=pnfinal[5];
  
  //////////////////////                                                                                                                  
  if (myinit){
    cout<<"The weights file is at "<<m_WeightsFile<<endl;
    cout<<"TMVA Method is "<<m_TmvaMethod<<endl;
    reader_comb = new TMVA::Reader("!Color");
    reader_comb->AddVariable("pmu", &pmu);
    reader_comb->AddVariable("pe", &pe);
    reader_comb->AddVariable("pk", &pk);
    reader_comb->AddVariable("pss", &pss);
    reader_comb->AddVariable("pvtx", &pvtx);
    reader_comb->BookMVA( m_TmvaMethod, m_WeightsFile );
    myinit = false;//only once                                                                                                            
  }
  double probNNcorre = reader_comb -> EvaluateMVA( m_TmvaMethod );
  probPlus = 0.502625 + 0.504699*(probNNcorre);//tuning NN de (-1,1) a (0,1)
  if (probPlus>1.) probPlus=1;
  if (probPlus<0.) probPlus=0;
  if (DBGNN) cout<< "probNN: "<<probNNcorre<<", probPlus="<<probPlus<<endl;

  //return probNNcorre; //For tuning probPlus=a+b*NN
  return probPlus;
}


//OLD likelihood for sec vertex
// double angle( TLorentzVector a, TLorentzVector b) {
//   double ang=0;
//   if(a.Vect().Mag2() && b.Vect().Mag2()) {
//     ang = acos( (a.Vect().Unit()).Dot(b.Vect().Unit()) );
//     if(ang>3.1416) ang=6.2832-ang;
//     if(ang<0) ang=-ang;
//   } else {
//     cout<<"Zero vector(s)! Arguments: "<<a.Vect().Mag2()
//          <<" "<<b.Vect().Mag2()<<endl;
//   }
//   return ang;
// }
// double ipprob(double x) {

//   double m_ipfitpol0= -0.535;
//   double m_ipfitpol1= 0.3351;
//   double m_ipfitpol2= -0.03102;
//   double m_ipfitpol3= 0.001316;
//   double m_ipfitpol4= -0.00002598 ;
//   double m_ipfitpol5= 0.0000001919;

//   if( x > 40. ) return 0.6;
 
//   double r = m_ipfitpol0 + m_ipfitpol1*x + m_ipfitpol2*pow(x,2)
//     + m_ipfitpol3*pow(x,3) + m_ipfitpol4*pow(x,4) + m_ipfitpol5*pow(x,5);
//   if(r<0) r=0;
//   return r;
// }
// double ptprob(double x) {

//   double m_ptfitpol0= 0.04332;
//   double m_ptfitpol1= 0.9493 ;
//   double  m_ptfitpol2= -0.5283 ;
//   double m_ptfitpol3= 0.1296;
//   double  m_ptfitpol4= -0.01094 ;

//   if( x > 5.0 ) return 0.65;
//   double r = m_ptfitpol0 + m_ptfitpol1*x + m_ptfitpol2*pow(x,2)
//     + m_ptfitpol3*pow(x,3) + m_ptfitpol4*pow(x,4);
//   if(r<0) r=0;
//   return r;
// }
// double aprob(double x) {
//   double m_anglepol0= 0.4516 ;
//   double m_anglepol1= 1.033;

//   if( x < 0.02 ) return 0.32;
//   double r = m_anglepol0 + m_anglepol1*x;
//   if(r<0) r=0;
//   return r;
// }
// double calcoldProb(double ipl, double iperrl, double ips, double iperrs, 
// 		   TLorentzVector p1, TLorentzVector p2){

//   //build a likelihood that the combination comes from B ---------
//   double probi1, probi2, probp1, probp2, proba, probs, probb;
//   // impact parameter
//   probi1=ipprob(ipl/iperrl);
//   probi2=ipprob(ips/iperrs);
//   // pt
//   probp1=ptprob(p1.Pt()); 
//   probp2=ptprob(p2.Pt()); 
//   // angle
//   proba= aprob(angle(p1,p2));
//   // total
//   probs=probi1*probi2*probp1*probp2*proba;
//   probb=(1-probi1)*(1-probi2)*(1-probp1)*(1-probp2)*(1-proba);
//   double probf = probs/(probs+probb);
//   if(probf<0.2) return -1;

//   return probf;
// }
