// Include files 
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
// local
#include "NNetTool_TMVA.h"

//-----------------------------------------------------------------------------
// Implementation file for class : NNetTool_TMVA v1.3
//
// 2008-10-10 : Marco Musy 
//
// Neural Net tuned with Root.
// Inputs are:  event-multiplicity,
// tagger p/GeV, pt/GeV, IP-significance,
// Delta-eta between B and tagger,
// Delta-phi, Delta-Q (= mass of B+tagger system - mass of B).
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( NNetTool_TMVA );

//=============================================================================
NNetTool_TMVA::NNetTool_TMVA( const std::string& type,
                              const std::string& name,
                              const IInterface* parent ) :
  GaudiTool ( type, name, parent ) { 
  declareInterface<INNetTool>(this);
  declareProperty( "MuonMethod",     m_MuonMethod   = "Fisher" );
  declareProperty( "ElectronMethod", m_ElectronMethod="Fisher" );
  declareProperty( "KaonOSMethod",   m_KaonOSMethod = "LikelihoodD" );
  declareProperty( "KaonSSMethod",   m_KaonSSMethod = "Fisher" );
  declareProperty( "PionSSMethod",   m_PionSSMethod = "LikelihoodD" );
}

NNetTool_TMVA::~NNetTool_TMVA(){}

//=============================================================================
StatusCode NNetTool_TMVA::initialize() { 
  
  m_reader_mu = new TMVA::Reader();
  m_reader_mu->AddVariable("mult",   &mult);
  m_reader_mu->AddVariable("ptB",    &ptB);
  m_reader_mu->AddVariable("partP",  &partP);
  m_reader_mu->AddVariable("partPt", &partPt);
  m_reader_mu->AddVariable("IPPV",   &IPPV);
  std::string filename = "NeuralNet/weights/NNmu_"+m_MuonMethod+".weights.txt";
  debug()<<"Booking TMVA: "<<filename<<endreq;
  m_reader_mu->BookMVA( m_MuonMethod+" method", filename );

  m_reader_e = new TMVA::Reader();
  m_reader_e->AddVariable("mult",   &mult);
  m_reader_e->AddVariable("ptB",    &ptB);
  m_reader_e->AddVariable("partP",  &partP);
  m_reader_e->AddVariable("partPt", &partPt);
  m_reader_e->AddVariable("IPPV",   &IPPV);
  filename = "NeuralNet/weights/NNe_"+m_ElectronMethod+".weights.txt";
  debug()<<"Booking TMVA: "<<filename<<endreq;
  m_reader_e->BookMVA( m_ElectronMethod+" method", filename );

  m_reader_k = new TMVA::Reader();
  m_reader_k->AddVariable("mult",   &mult);
  m_reader_k->AddVariable("ptB",    &ptB);
  m_reader_k->AddVariable("partP",  &partP);
  m_reader_k->AddVariable("partPt", &partPt);
  m_reader_k->AddVariable("IPPV",   &IPPV);
  filename = "NeuralNet/weights/NNk_"+m_KaonOSMethod+".weights.txt";
  debug()<<"Booking TMVA: "<<filename<<endreq;
  m_reader_k->BookMVA( m_KaonOSMethod+" method", filename );

  m_reader_kS = new TMVA::Reader();
  m_reader_kS->AddVariable("mult",   &mult);
  m_reader_kS->AddVariable("ptB",    &ptB);
  m_reader_kS->AddVariable("partP",  &partP);
  m_reader_kS->AddVariable("partPt", &partPt);
  m_reader_kS->AddVariable("IPPV",   &IPPV);
  m_reader_kS->AddVariable("nndeta", &nndeta);
  m_reader_kS->AddVariable("nndphi", &nndphi);
  m_reader_kS->AddVariable("nndq",   &nndq);
  filename = "NeuralNet/weights/NNkS_"+m_KaonSSMethod+".weights.txt";
  debug()<<"Booking TMVA: "<<filename<<endreq;
  m_reader_kS->BookMVA( m_KaonSSMethod+" method", filename );

  m_reader_pS = new TMVA::Reader();
  m_reader_pS->AddVariable("mult",   &mult);
  m_reader_pS->AddVariable("ptB",    &ptB);
  m_reader_pS->AddVariable("partP",  &partP);
  m_reader_pS->AddVariable("partPt", &partPt);
  m_reader_pS->AddVariable("IPPV",   &IPPV);
  m_reader_pS->AddVariable("nndeta", &nndeta);
  m_reader_pS->AddVariable("nndphi", &nndphi);
  m_reader_pS->AddVariable("nndq",   &nndq);
  filename = "NeuralNet/weights/NNpS_"+m_PionSSMethod+".weights.txt";
  debug()<<"Booking TMVA: "<<filename<<endreq;
  m_reader_pS->BookMVA( m_PionSSMethod+" method", filename );
 
  return StatusCode::SUCCESS; 
}

StatusCode NNetTool_TMVA::finalize()   { 

  if(m_reader_mu != NULL) delete m_reader_mu;
  if(m_reader_e  != NULL) delete m_reader_e;
  if(m_reader_k  != NULL) delete m_reader_k;
  if(m_reader_kS != NULL) delete m_reader_kS;
  if(m_reader_pS != NULL) delete m_reader_pS;

  return StatusCode::SUCCESS; 
}

double NNetTool_TMVA::pol2(double x, double a0, double a1) {
  return a0+a1*x;
}
double NNetTool_TMVA::pol3(double x, double a0, double a1, double a2) {
  return a0+a1*x+a2*x*x;
}

//=============================================================================
void NNetTool_TMVA::normaliseOS(std::vector<double>& par) { 

  debug()<<"before norm "<<par<<endreq;

  par.at(0) /= 90.; //mult
  par.at(1) /= 20.; //ptB
  par.at(2) = std::min( par.at(2)/80., 1.); //partP
  par.at(3) = std::min( par.at(3)/5., 1.); //partPt
  par.at(4) = par.at(4)/50.; //IPPV
  if(par.at(4)> 1.) par.at(4)= 1.; if(par.at(4)<-1.) par.at(4)=-1.;
}

void NNetTool_TMVA::normaliseSS(std::vector<double>& par) { 

  par.at(5) /= 2.; //nndeta
  par.at(6) /= 3.; //nndphi
  par.at(7) /= 12.;//nndq
  if(par.at(7)>1.) par.at(7) = 1.;

}

//=============================================================================
double NNetTool_TMVA::MLPm(std::vector<double>& par) { 

  normaliseOS(par);
  mult   = par.at(0);
  ptB    = par.at(1);
  partP  = par.at(2);
  partPt = par.at(3);
  IPPV   = par.at(4);
  double rnet = m_reader_mu ->EvaluateMVA( m_MuonMethod+" method" );

  double pn   = 1-pol2(rnet, 9.084315e-01, -8.764311e-01);// <=========

  debug()<<"muon: rnet= "<<rnet<<" pn="<<pn<<endreq;
  debug()<<"muon: par = "<<par.at(0)<<" "<<par.at(2)<<" "<<par.at(3)
         <<" "<<par.at(4)<<endreq;

  return pn;
}; 

//=============================================================================
double NNetTool_TMVA::MLPe(std::vector<double>& par) { 

  normaliseOS( par );
  mult   = par.at(0);
  ptB    = par.at(1);
  partP  = par.at(2);
  partPt = par.at(3);
  IPPV   = par.at(4);
  double rnet = m_reader_e ->EvaluateMVA( m_ElectronMethod+" method" );

  double pn   = 1.0-pol2(rnet, 7.937505e-01, -7.197138e-01);// <=========

  debug()<<"ele: rnet= "<<rnet<<" pn="<<pn<<endreq;
  debug()<<"ele: par = "<<par.at(0)<<" "<<par.at(2)<<" "<<par.at(3)
         <<" "<<par.at(4)<<endreq;

  return pn;
}; 

//=============================================================================
double NNetTool_TMVA::MLPk(std::vector<double>& par ) { 

  normaliseOS( par );
  mult   = par.at(0);
  ptB    = par.at(1);
  partP  = par.at(2);
  partPt = par.at(3);
  IPPV   = par.at(4);
  double rnet = m_reader_k ->EvaluateMVA( m_KaonOSMethod+" method" );

  double pn = 1.0-pol2(rnet, 9.839528e-01, -9.846077e-01 );// <=========

  debug()<<"kaonOs: rnet= "<<rnet<<" pn="<<pn<<endreq;
  debug()<<"kaonOs  par = "<<par.at(0)<<" "<<par.at(2)<<" "<<par.at(3)
         <<" "<<par.at(4)<<endreq;

  return pn;
}; 

//=============================================================================
double NNetTool_TMVA::MLPkS(std::vector<double>& par) { 

  normaliseOS( par );
  normaliseSS( par );
  mult   = par.at(0);
  ptB    = par.at(1);
  partP  = par.at(2);
  partPt = par.at(3);
  IPPV   = par.at(4);
  nndeta = par.at(5);
  nndphi = par.at(6);
  nndq   = par.at(7);
  double rnet = m_reader_kS ->EvaluateMVA( m_KaonSSMethod+" method" );
 
  double pn = 1.0-pol2(rnet, 1.004943, -1.017668);// <=========
 
  debug()<<"kaonS: rnet= "<<rnet<<" pn="<<pn<<endreq;
  debug()<<"kaonS: par = "<<par.at(0)<<" "<<par.at(2)<<" "<<par.at(3)
         <<" "<<par.at(4)<<" "<<par.at(5)<<" "<<par.at(6)
         <<" "<<par.at(7)<<endreq;

  return pn;
}; 

//=============================================================================
double NNetTool_TMVA::MLPpS(std::vector<double>& par) { 

  normaliseOS( par );
  normaliseSS( par );
  mult   = par.at(0);
  ptB    = par.at(1);
  partP  = par.at(2);
  partPt = par.at(3);
  IPPV   = par.at(4);
  nndeta = par.at(5);
  nndphi = par.at(6);
  nndq   = par.at(7);
  double rnet = m_reader_pS ->EvaluateMVA( m_PionSSMethod+" method" );

  double pn = 1.0-pol2(rnet, 1.147873, -1.241866 );// <=========

  debug()<<"pionS: rnet= "<<rnet<<" pn="<<pn<<endreq;
  debug()<<"pionS  par = "<<par.at(0)<<" "<<par.at(2)<<" "<<par.at(3)
         <<" "<<par.at(4)<<" "<<par.at(5)<<" "<<par.at(6)
         <<" "<<par.at(7)<<endreq;

  return pn;
};
//=============================================================================
double NNetTool_TMVA::MLPvtx(std::vector<double>& par) { 
  debug()<<par.size()<<endreq;
  return 0;
}
