// Include files 
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
// local
#include "NNetTool_MLP.h"
// MC11 Reco09 
#include "NeuralNet/NNmuon.cxx"
#include "NeuralNet/NNele.cxx"
#include "NeuralNet/NNkaon.cxx"
#include "NeuralNet/NNkaonS.cxx"
#include "NeuralNet/NNpionS.cxx"
#include "NeuralNet/NNvtx.cxx"
// data Reco14
#include "NeuralNet/weights/muon__muonMLPBNN.class.C"
#include "NeuralNet/weights/ele__eleMLPBNN.class.C"
#include "NeuralNet/weights/kaon__kaonMLPBNN.class.C"
#include "NeuralNet/weights/vtx__vtxMLPBNN.class.C"
#include "NeuralNet/weights/charm__charmMLPBNN.class.C"

// MC12 Reco14
#include "NeuralNet/weights/muon__muonMLPBNN_MC.class.C"
#include "NeuralNet/weights/ele__eleMLPBNN_MC.class.C"
#include "NeuralNet/weights/kaon__kaonMLPBNN_MC.class.C"
#include "NeuralNet/weights/vtx__vtxMLPBNN_MC.class.C"
#include "NeuralNet/weights/pionS__pionSMLPBNN_MC.class.C"
#include "NeuralNet/weights/kaonS__kaonSMLPBNN_MC.class.C"



//-----------------------------------------------------------------------------
// Implementation file for class : NNetTool_MLP v1.3
//
// 2008-10-10 : Marco Musy & Marc Grabalosa
//
// Neural Net tuned with Root.
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( NNetTool_MLP )
//=============================================================================
NNetTool_MLP::NNetTool_MLP( const std::string& type,
                            const std::string& name,
                            const IInterface* parent )
: GaudiTool ( type, name, parent )
{
  declareInterface<INNetTool>(this);

  // cuts_Moriond2013 trained on CloneKO, lowCorrelation, newNNPID D,N, CE tanh, Split i%3=0,1
  //mu scaleX=-5.47039 scaleY=1.23885 offsetY=-0.00793716 pivotX=0.647253
  declareProperty( "P0_mu_scale", m_P0mu =  -5.47039);
  declareProperty( "P1_mu_scale", m_P1mu =  1.23885);
  declareProperty( "P2_mu_scale", m_P2mu =  -0.00793716);
  declareProperty( "P3_mu_scale", m_P3mu =  0.647253);

  //e scaleX=-3.04032 scaleY=1.7055 offsetY=-0.136785 pivotX=0.646733
  declareProperty( "P0_e_scale", m_P0e =  -3.04032);
  declareProperty( "P1_e_scale", m_P1e =  1.7055);
  declareProperty( "P2_e_scale", m_P2e =  -0.136785);
  declareProperty( "P3_e_scale", m_P3e =  0.646733);
  //k scaleX=-5.12122 scaleY=1.21427 offsetY=0.088854 pivotX=0.573943
  declareProperty( "P0_k_scale", m_P0k =  -5.12122);
  declareProperty( "P1_k_scale", m_P1k =  1.21427);
  declareProperty( "P2_k_scale", m_P2k =  0.088854);
  declareProperty( "P3_k_scale", m_P3k =  0.573943);
  //vtx scaleX=-5.77134 scaleY=1.11591 offsetY=0.113682 pivotX=0.570684
  declareProperty( "P0_vtx_scale", m_P0vtx =  -5.77134);
  declareProperty( "P1_vtx_scale", m_P1vtx =  1.11591);
  declareProperty( "P2_vtx_scale", m_P2vtx =  0.113682);
  declareProperty( "P3_vtx_scale", m_P3vtx =  0.570684);

  declareProperty( "P0_ks_scale", m_P0ks =  1.22418); //dec2011_v2
  declareProperty( "P1_ks_scale", m_P1ks = -1.63297);
  declareProperty( "P2_ks_scale", m_P2ks =  0.401361);
  declareProperty( "P3_ks_scale", m_P3ks =  0.);

  declareProperty( "P0_ps_scale", m_P0ps =  1.22123);
  declareProperty( "P1_ps_scale", m_P1ps = -1.76027);
  declareProperty( "P2_ps_scale", m_P2ps =  0.651766);
  declareProperty( "P3_ps_scale", m_P3ps =  0.);

  declareProperty( "P0_charm_scale", m_P0charm =  0.3206);
  declareProperty( "P1_charm_scale", m_P1charm =  -0.8247);
  declareProperty( "P2_charm_scale", m_P2charm =  0.6732);
  declareProperty( "P3_charm_scale", m_P3charm =  0.);

  /*
  declareProperty( "XML_dir",          m_XML_dir   = "$FLAVOURTAGGINGROOT/src/NeuralNet/weights/"  );
  declareProperty( "NNetWeights_mu",   m_NNetWeights_mu   = "muon__muonMLPBNN.weights.xml"  );
  declareProperty( "NNetWeights_ele",  m_NNetWeights_ele  = "ele__eleMLPBNN.weights.xml"  );
  declareProperty( "NNetWeights_kaon", m_NNetWeights_kaon = "kaon__kaonMLPBNN.weights.xml"  );
  declareProperty( "NNetWeights_vtx",  m_NNetWeights_vtx  = "vtx__vtxMLPBNN.weights.xml"  );
  */
}

NNetTool_MLP::~NNetTool_MLP(){}
//=============================================================================
StatusCode NNetTool_MLP::initialize() 
{
  StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;
  /*
  std::string dir = m_XML_dir;
  if ( dir.compare(0,1,"$")==0 ) 
  {
    int i = dir.find_first_of("/");
    int len = dir.length();
    const std::string env = dir.substr(1, i-1);
    m_XML_dir = ( std::string(getenv(env.c_str())) + dir.substr(i,len-i+1));
    debug()<< " directory "<< dir << " resolved "<<m_XML_dir<<endreq;
  } else
  {
    debug()<< " directory not starting with and evniromental variable "<<m_XML_dir<<" "<<dir.at(0)<< endreq;
  }
  */
  return sc;
}
//=============================================================================

StatusCode NNetTool_MLP::finalize()   { return GaudiTool::finalize(); }

//=============================================================================
double NNetTool_MLP::pol2(double x, double a0, double a1)
{
  return a0+a1*x;
}
double NNetTool_MLP::pol3(double x, double a0, double a1, double a2)
{
  return a0+a1*x+a2*x*x;
}
double NNetTool_MLP::pol4(double x, double a0, double a1, double a2, double a3)
{
  return a0+a1*x+a2*x*x+a3*x*x*x;
}
double NNetTool_MLP::func(double x, double a0=0, double a1=0., double a2=0., double a3=0.)
{
  // arcTAN = atan((x-[3])*[0])+1.5)*[1]*0.17+[2]
  double arg = ((x-a3)*a0);
  double res = ((std::atan(arg))+1.5)*a1*0.17+a2;
  return res;
}
//=============================================================================

void NNetTool_MLP::normaliseOS(std::vector<double>& par)
{
  if(msgLevel(MSG::DEBUG))
    debug()<<"par = "<<par.at(0)<<" "<<par.at(2)<<" "<<par.at(3)
           <<" "<<par.at(4)<<" "<<par.at(5)<<" "<<par.at(6)<<" "<<par.at(7)
           <<" "<<par.at(8)<<" "<<par.at(9)<<" "<<par.at(1)<<endreq;

  par.at(0) /= 90.; //multiplicity
  par.at(1) /= 25.; //ptB
  par.at(2) = std::min( sqrt(par.at(2))/15., 1.); //partP
  par.at(3) = std::min( sqrt(par.at(3))/2.5, 1.); //partPt
  double tmp=0;
  if(par.at(4)<0) tmp=-sqrt(-par.at(4)); else tmp=sqrt(par.at(4));//IPPV
  tmp /= 10.;
  if(tmp> 1) tmp= 1; if(tmp<-1) tmp=-1;
  par.at(4)=tmp;
  par.at(8) = (par.at(8)-1)/4.;//krec
  par.at(9) = (par.at(9)-1)/3;//ncands
}
void NNetTool_MLP::normaliseSS(std::vector<double>& par)
{
  par.at(9) = (par.at(9) * 3) + 1;//undo ncands
  par.at(9) /= 4.; //nndr
  par.at(5) /= 2.; //nndeta
  par.at(6) /= 3.; //nndphi
  par.at(7) /= 12.;//nndq
  if(par.at(7)>1.) par.at(7) = 1.;
}
void NNetTool_MLP::normalisepSS(std::vector<double>& par)
{
  par.at(5) /= 4.;   //nndr
  par.at(6) /= 3.2;  //nnpartPhi
  par.at(7) /= 12.;  //nndq
  if(par.at(7)>1.) par.at(7) = 1.;
}
void NNetTool_MLP::normaliseVtx(std::vector<double>& par)
{
  if(msgLevel(MSG::DEBUG))
    debug()<<"par vtx = "<<par.at(0)<<" "<<par.at(1)<<" "<<par.at(2)
           <<" "<<par.at(3)<<" "<<par.at(4)<<" "<<par.at(5)<<" "<<par.at(6)
           <<" "<<par.at(7)<<" "<<par.at(8)<<" "<<par.at(9)<<" "<<par.at(10)<<endreq;

  par.at(0) /= 90.;  //vtag size
  par.at(1) = (par.at(1)-1)/4.;  //krec
  par.at(2) /= 25.;  //ptB
  par.at(3) /= 4.;  //ncand
  par.at(4) /= 2.;  //ptmean
  par.at(5) /= 30.;  //ipsmean
  par.at(6) /= 0.1;  //docamax
  par.at(8) /= 10.;  //ptsumM
  par.at(10)/= 6.;  //SVM
}
//=============================================================================
double NNetTool_MLP::MLPm(std::vector<double>& par)
{
  normaliseOS(par);
  NNmuon net;
  const double rnet = net.value(0, par.at(2),par.at(3),par.at(8),
                                par.at(1),par.at(4));

  const double pn = 1.0-pol4(rnet, m_P0mu, m_P1mu, m_P2mu, m_P3mu);// <=========

  if(msgLevel(MSG::DEBUG))
  {
    debug()<<"muon: par = "<<par.at(2)<<" "<<par.at(3)
           <<" "<<par.at(8)<<" "<<par.at(1)<<" "<<par.at(4)<<endreq;
    debug()<<" muon: rnet="<<rnet<<" pn="<<pn<<endreq;
  }

  return pn;
}

//=============================================================================
double NNetTool_MLP::MLPe(std::vector<double>& par)
{

  normaliseOS( par );
  NNele net;
  const double rnet = net.value(0, par.at(2),par.at(3),par.at(8),
                                par.at(1),par.at(4));

  const double pn = 1.0-pol3(rnet, m_P0e, m_P1e, m_P2e);// <=========

  if(msgLevel(MSG::DEBUG))
  {
    debug()<<"ele: par = "<<par.at(2)<<" "<<par.at(3)
           <<" "<<par.at(8)<<" "<<par.at(1)<<" "<<par.at(4)<<endreq;
    debug()<<" ele: rnet="<<rnet<<" pn="<<pn<<endreq;
  }

  return pn;
}

//=============================================================================
double NNetTool_MLP::MLPk(std::vector<double>& par )
{
  normaliseOS( par );
  NNkaon net;
  const double rnet = net.value(0, par.at(0),par.at(2),par.at(3),par.at(8),
                                par.at(1),par.at(4));

  const double pn = 1.0-pol4(rnet, m_P0k, m_P1k, m_P2k, m_P3k);// <=========

  if(msgLevel(MSG::DEBUG))
  {
    debug()<<"kaon: par = "<<par.at(0)<<" "<<par.at(2)<<" "<<par.at(3)
           <<" "<<par.at(8)<<" "<<par.at(1)<<" "<<par.at(4)<<endreq;
    debug()<<" k: rnet="<<rnet<<" pn="<<pn<<endreq;
  }

  return pn;
}

//=============================================================================
double NNetTool_MLP::MLPkS(std::vector<double>& par)
{
  normaliseOS( par );
  normaliseSS( par );
  NNkaonS net;
  const double rnet = net.value(0, par.at(0),par.at(3),par.at(8),par.at(5),
                                par.at(6),par.at(1),par.at(7));

  const double pn = 1.0-pol3(rnet, m_P0ks, m_P1ks, m_P2ks);// <=========

  if(msgLevel(MSG::DEBUG))
  {
    debug()<<"par = "<<par.at(0)<<" "<<par.at(2)<<" "<<par.at(3)
           <<" "<<par.at(4)<<" "<<par.at(5)<<" "<<par.at(6)
           <<" "<<par.at(7)<<" "<<par.at(8)<<" "<<par.at(9)<<" "<<par.at(1)<<endreq;
    debug()<<" kS: rnet="<<rnet<<" pn="<<pn<<endreq;
  }

  return pn;
}

//=============================================================================
double NNetTool_MLP::MLPpS(std::vector<double>& par) 
{
  normaliseOS( par );
  normalisepSS( par );
  NNpionS net;
  const double rnet = net.value(0,par.at(0),par.at(3),par.at(4),
                                par.at(8),par.at(1),par.at(7),par.at(5));

  const double pn = 1.0-pol3(rnet, m_P0ps, m_P1ps, m_P2ps);// <=========

  if(msgLevel(MSG::DEBUG))
  {
    debug()<<"pS: par = "<<par.at(0)<<" "<<par.at(3)<<" "<<par.at(4)
           <<" "<<par.at(8)<<" "<<par.at(1)<<" "<<par.at(7)<<" "<<par.at(5)<<endreq;
    debug()<<" pS: rnet="<<rnet<<" pn="<<pn<<endreq;
  }

  return pn;
}

//=============================================================================
double NNetTool_MLP::MLPvtx(std::vector<double>& par)
{
  normaliseVtx( par );
  NNvtx net;
  const double rnet = net.value(0,par.at(0),par.at(1),par.at(2),par.at(3),par.at(4),
                                par.at(5),par.at(9),par.at(10));

  const double pn = 1.0-pol2(rnet, m_P0vtx, m_P1vtx);// <=========

  if(msgLevel(MSG::DEBUG))
  {
    debug()<<"vtx: par = "<<par.at(0)<<" "<<par.at(1)<<" "<<par.at(2)
           <<" "<<par.at(3)<<" "<<par.at(4)<<" "<<par.at(5)<<" "
           <<par.at(9)<<" "<<par.at(10)<<endreq;
    debug()<<" vtx: rnet="<<rnet<<" pn="<<pn<<endreq;
  }

  return pn;
}

//============================================================================

//=============================================================================
// New NNet trained on DATA 2012  !!!!!!!!!!!
//=============================================================================
double NNetTool_MLP::MLPmTMVA(std::vector <std::string>& inputVars, std::vector<double>& inputVals) 
{
  double rnet = 0.5;
  double pn = 0.5;

  Read_muonMLPBNN *reader = new Read_muonMLPBNN(inputVars);  
  rnet = reader->GetMvaValue(inputVals);

  if (rnet>=0 && rnet<=1) {
    pn = 1.0 -func(rnet, m_P0mu, m_P1mu, m_P2mu, m_P3mu);
  } else {
    debug()<<"**********************BAD TRAINING muon"<<rnet<<endmsg;
    pn = -1.;
  }
  return pn;
}

//=============================================================================
double NNetTool_MLP::MLPeTMVA(std::vector <std::string>& inputVars, std::vector<double>& inputVals) 
{
  double rnet = 0.5;
  double pn = 0.5;

  Read_eleMLPBNN *reader = new Read_eleMLPBNN(inputVars); 
  rnet = reader->GetMvaValue(inputVals);

  if (rnet>=0 && rnet<=1) {
    pn = 1.0 -func(rnet, m_P0e, m_P1e, m_P2e, m_P3e);
  } else {
    debug()<<"**********************BAD TRAINING ele"<<rnet<<endmsg;
    pn = -1.;
  }
  return pn;
}

//=============================================================================
double NNetTool_MLP::MLPkaonTMVA(std::vector <std::string>& inputVars, std::vector<double>& inputVals) 
{
  double rnet = 0.5;
  double pn = 0.5;

  Read_kaonMLPBNN *reader = new Read_kaonMLPBNN(inputVars); 
  rnet = reader->GetMvaValue(inputVals);

  if (rnet>=0 && rnet <=1){
    pn = 1.0 -func(rnet, m_P0k, m_P1k, m_P2k, m_P3k);
  }
  else{
    debug()<<"**********************BAD TRAINING kaon"<<rnet<<endmsg;
    pn = -1.;
  }
  return pn;
}

//=============================================================================
double NNetTool_MLP::MLPvtxTMVA(std::vector <std::string>& inputVars, std::vector<double>& inputVals) 
{
  double rnet = 0.5;
  double pn = 0.5;

  Read_vtxMLPBNN *reader = new Read_vtxMLPBNN(inputVars); 
  rnet = reader->GetMvaValue(inputVals);

  if (rnet>=0 && rnet<=1) {
    pn = 1.0 -func(rnet, m_P0vtx, m_P1vtx, m_P2vtx, m_P3vtx);
  } else {
    debug()<<"**********************BAD TRAINING vtx"<<rnet<<endmsg;
    pn = -1.;
  }
  return pn;
}
//=============================================================================
double NNetTool_MLP::MLPcharmTMVA(std::vector <std::string>& inputVars, std::vector<double>& inputVals) {

  double rnet = 0.5;  
  double pn = 0.5;  

  Read_charmMLPBNN *reader = new Read_charmMLPBNN(inputVars); 
  rnet = reader->GetMvaValue(inputVals);

  if (rnet>=0 && rnet<=1) {
    pn = 1.0 - (m_P0charm + m_P1charm * (rnet - m_P2charm));
    verbose()<<"Using MLPBNN for charm - rnet "<<rnet<<" pn "<<pn<<endmsg;
  } else {
    debug()<<"**********************BAD TRAINING charm"<<rnet<<endmsg;
    pn = -1.;
  }

  return pn;

};
//============================================================================
double NNetTool_MLP::MLPmTMVA_MC(std::vector <std::string>& inputVars, std::vector<double>& inputVals) 
{
  double rnet = 0.5;
  double pn = 0.5;

  Read_muonMLPBNN_MC *reader = new Read_muonMLPBNN_MC(inputVars);  
  rnet = reader->GetMvaValue(inputVals);

  if (rnet>=0 && rnet<=1) {
    pn = 1.0 -func(rnet, m_P0mu, m_P1mu, m_P2mu, m_P3mu);
  } else {
    debug()<<"**********************BAD TRAINING muon"<<rnet<<endmsg;
    pn = -1.;
  }
  return pn;
}

//=============================================================================
double NNetTool_MLP::MLPeTMVA_MC(std::vector <std::string>& inputVars, std::vector<double>& inputVals) 
{
  double rnet = 0.5;
  double pn = 0.5;

  Read_eleMLPBNN_MC *reader = new Read_eleMLPBNN_MC(inputVars); 
  rnet = reader->GetMvaValue(inputVals);

  if (rnet>=0 && rnet<=1) {
    pn = 1.0 -func(rnet, m_P0e, m_P1e, m_P2e, m_P3e);
  } else {
    debug()<<"**********************BAD TRAINING ele"<<rnet<<endmsg;
    pn = -1.;
  }
  return pn;
}

//=============================================================================
double NNetTool_MLP::MLPkaonTMVA_MC(std::vector <std::string>& inputVars, std::vector<double>& inputVals) 
{
  double rnet = 0.5;
  double pn = 0.5;

  Read_kaonMLPBNN_MC *reader = new Read_kaonMLPBNN_MC(inputVars); 
  rnet = reader->GetMvaValue(inputVals);

  if (rnet>=0 && rnet <=1){
    pn = 1.0 -func(rnet, m_P0k, m_P1k, m_P2k, m_P3k);
  }
  else{
    debug()<<"**********************BAD TRAINING kaon"<<rnet<<endmsg;
    pn = -1.;
  }
  return pn;
}

//=============================================================================
double NNetTool_MLP::MLPvtxTMVA_MC(std::vector <std::string>& inputVars, std::vector<double>& inputVals) 
{
  double rnet = 0.5;
  double pn = 0.5;

  Read_vtxMLPBNN_MC *reader = new Read_vtxMLPBNN_MC(inputVars); 
  rnet = reader->GetMvaValue(inputVals);

  if (rnet>=0 && rnet<=1) {
    pn = 1.0 -func(rnet, m_P0vtx, m_P1vtx, m_P2vtx, m_P3vtx);
  } else {
    debug()<<"**********************BAD TRAINING vtx"<<rnet<<endmsg;
    pn = -1.;
  }
  return pn;
}
//============================================================================
double NNetTool_MLP::MLPpSTMVA_MC(std::vector <std::string>& inputVars, std::vector<double>& inputVals) 
{
  double rnet = 0.5;
  double pn = 0.5;

  Read_pionSMLPBNN_MC *reader = new Read_pionSMLPBNN_MC(inputVars); 
  rnet = reader->GetMvaValue(inputVals);

  if (rnet>=0 && rnet<=1) {
    pn = 1.0 -func(rnet, m_P0ps, m_P1ps, m_P2ps, m_P3ps);
  } else {
    debug()<<"**********************BAD TRAINING pSame"<<rnet<<endmsg;
    pn = -1.;
  }
  return pn;
}
//============================================================================
//============================================================================
double NNetTool_MLP::MLPkSTMVA_MC(std::vector <std::string>& inputVars, std::vector<double>& inputVals) 
{
  double rnet = 0.5;
  double pn = 0.5;

  Read_kaonSMLPBNN_MC *reader = new Read_kaonSMLPBNN_MC(inputVars); 
  rnet = reader->GetMvaValue(inputVals);

  if (rnet>=0 && rnet<=1) {
    pn = 1.0 -func(rnet, m_P0ks, m_P1ks, m_P2ks, m_P3ks);
  } else {
    debug()<<"**********************BAD TRAINING kSame"<<rnet<<endmsg;
    pn = -1.;
  }
  return pn;
}
//============================================================================
