// Include files 
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
// local
#include "NNetTool_MLP.h"

#include "NeuralNet/NNmuon.cxx"
#include "NeuralNet/NNele.cxx"
#include "NeuralNet/NNkaon.cxx"
#include "NeuralNet/NNkaonS.cxx"
#include "NeuralNet/NNpionS.cxx"
#include "NeuralNet/NNvtx.cxx"

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
                            const IInterface* parent ) :
  GaudiTool ( type, name, parent ) {
  declareInterface<INNetTool>(this);

  declareProperty( "P0_mu_scale", m_P0mu =  -1.73872);
  declareProperty( "P1_mu_scale", m_P1mu =  12.1473);
  declareProperty( "P2_mu_scale", m_P2mu =  -20.966);
  declareProperty( "P3_mu_scale", m_P3mu =  10.9829);

  declareProperty( "P0_e_scale",  m_P0e  =  1.32902);
  declareProperty( "P1_e_scale",  m_P1e  =  -2.32922);
  declareProperty( "P2_e_scale",  m_P2e  =  1.17439);

  declareProperty( "P0_k_scale",  m_P0k  = -3.66218);
  declareProperty( "P1_k_scale",  m_P1k  = 20.5552);
  declareProperty( "P2_k_scale",  m_P2k  = -32.958);
  declareProperty( "P3_k_scale",  m_P3k  = 16.7165);

  //  declareProperty( "P0_ks_scale", m_P0ks =  0.8562); //dec2011_v1
  //  declareProperty( "P1_ks_scale", m_P1ks = -0.6897);
  //  declareProperty( "P2_ks_scale", m_P2ks = -0.1932);
  declareProperty( "P0_ks_scale", m_P0ks =  1.22418); //dec2011_v2
  declareProperty( "P1_ks_scale", m_P1ks = -1.63297);
  declareProperty( "P2_ks_scale", m_P2ks =  0.401361);

  declareProperty( "P0_ps_scale", m_P0ps =  1.22123);
  declareProperty( "P1_ps_scale", m_P1ps = -1.76027);
  declareProperty( "P2_ps_scale", m_P2ps =  0.651766);

  declareProperty( "P0_vtx_scale", m_P0vtx =  0.945773 );
  declareProperty( "P1_vtx_scale", m_P1vtx = -0.93411 );

}

NNetTool_MLP::~NNetTool_MLP(){}

//=============================================================================
double NNetTool_MLP::pol2(double x, double a0, double a1) {
  return a0+a1*x;
}
double NNetTool_MLP::pol3(double x, double a0, double a1, double a2) {
  return a0+a1*x+a2*x*x;
}
double NNetTool_MLP::pol4(double x, double a0, double a1, double a2, double a3) {
  return a0+a1*x+a2*x*x+a3*x*x*x;
}

//=============================================================================
void NNetTool_MLP::normaliseOS(std::vector<double>& par) {

  if(msgLevel(MSG::DEBUG)) debug()<<"par = "<<par.at(0)<<" "<<par.at(2)<<" "<<par.at(3)
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
void NNetTool_MLP::normaliseSS(std::vector<double>& par) {
  par.at(9) = (par.at(9) * 3) + 1;//undo ncands
  par.at(9) /= 4.; //nndr
  par.at(5) /= 2.; //nndeta
  par.at(6) /= 3.; //nndphi
  par.at(7) /= 12.;//nndq
  if(par.at(7)>1.) par.at(7) = 1.;
}
void NNetTool_MLP::normalisepSS(std::vector<double>& par) {
  par.at(5) /= 4.;   //nndr
  par.at(6) /= 3.2;  //nnpartPhi
  par.at(7) /= 12.;  //nndq
  if(par.at(7)>1.) par.at(7) = 1.;
}
void NNetTool_MLP::normaliseVtx(std::vector<double>& par) {

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
double NNetTool_MLP::MLPm(std::vector<double>& par) {

  normaliseOS(par);
  NNmuon net;
  double rnet = net.value(0, par.at(2),par.at(3),par.at(8),
                          par.at(1),par.at(4));

  double pn = 1.0-pol4(rnet, m_P0mu, m_P1mu, m_P2mu, m_P3mu);// <=========

  if(msgLevel(MSG::DEBUG))debug()<<"muon: par = "<<par.at(2)<<" "<<par.at(3)
                                 <<" "<<par.at(8)<<" "<<par.at(1)<<" "<<par.at(4)<<endreq;
  if(msgLevel(MSG::DEBUG))debug()<<" muon: rnet="<<rnet<<" pn="<<pn<<endreq;

  return pn;
}

//=============================================================================
double NNetTool_MLP::MLPe(std::vector<double>& par) {

  normaliseOS( par );
  NNele net;
  double rnet = net.value(0, par.at(2),par.at(3),par.at(8),
                          par.at(1),par.at(4));

  double pn = 1.0-pol3(rnet, m_P0e, m_P1e, m_P2e);// <=========

  if(msgLevel(MSG::DEBUG))debug()<<"ele: par = "<<par.at(2)<<" "<<par.at(3)
                                 <<" "<<par.at(8)<<" "<<par.at(1)<<" "<<par.at(4)<<endreq;
  if(msgLevel(MSG::DEBUG))debug()<<" ele: rnet="<<rnet<<" pn="<<pn<<endreq;

  return pn;
}

//=============================================================================
double NNetTool_MLP::MLPk(std::vector<double>& par ) {

  normaliseOS( par );
  NNkaon net;
  double rnet = net.value(0, par.at(0),par.at(2),par.at(3),par.at(8),
                          par.at(1),par.at(4));

  double pn = 1.0-pol4(rnet, m_P0k, m_P1k, m_P2k, m_P3k);// <=========

  if(msgLevel(MSG::DEBUG))debug()<<"kaon: par = "<<par.at(0)<<" "<<par.at(2)<<" "<<par.at(3)
                                 <<" "<<par.at(8)<<" "<<par.at(1)<<" "<<par.at(4)<<endreq;
  if(msgLevel(MSG::DEBUG))debug()<<" k: rnet="<<rnet<<" pn="<<pn<<endreq;

  return pn;
}

//=============================================================================
double NNetTool_MLP::MLPkS(std::vector<double>& par) {

  normaliseOS( par );
  normaliseSS( par );
  NNkaonS net;
  double rnet = net.value(0, par.at(0),par.at(3),par.at(8),par.at(5),
                          par.at(6),par.at(1),par.at(7));

  double pn = 1.0-pol3(rnet, m_P0ks, m_P1ks, m_P2ks);// <=========

  if(msgLevel(MSG::DEBUG))debug()<<"par = "<<par.at(0)<<" "<<par.at(2)<<" "<<par.at(3)
                                 <<" "<<par.at(4)<<" "<<par.at(5)<<" "<<par.at(6)
                                 <<" "<<par.at(7)<<" "<<par.at(8)<<" "<<par.at(9)<<" "<<par.at(1)<<endreq;
  if(msgLevel(MSG::DEBUG))debug()<<" kS: rnet="<<rnet<<" pn="<<pn<<endreq;

  return pn;
}

//=============================================================================
double NNetTool_MLP::MLPpS(std::vector<double>& par) {

  normaliseOS( par );
  normalisepSS( par );
  NNpionS net;
  double rnet = net.value(0,par.at(0),par.at(3),par.at(4),
                          par.at(8),par.at(1),par.at(7),par.at(5));

  double pn = 1.0-pol3(rnet, m_P0ps, m_P1ps, m_P2ps);// <=========

  if(msgLevel(MSG::DEBUG)) debug()<<"pS: par = "<<par.at(0)<<" "<<par.at(3)<<" "<<par.at(4)
                                  <<" "<<par.at(8)<<" "<<par.at(1)<<" "<<par.at(7)<<" "<<par.at(5)<<endreq;
  if(msgLevel(MSG::DEBUG)) debug()<<" pS: rnet="<<rnet<<" pn="<<pn<<endreq;

  return pn;
}

//=============================================================================
double NNetTool_MLP::MLPvtx(std::vector<double>& par) {

  normaliseVtx( par );
  NNvtx net;
  double rnet = net.value(0,par.at(0),par.at(1),par.at(2),par.at(3),par.at(4),
                          par.at(5),par.at(9),par.at(10));

  double pn = 1.0-pol2(rnet, m_P0vtx, m_P1vtx);// <=========

  if(msgLevel(MSG::DEBUG)) debug()<<"vtx: par = "<<par.at(0)<<" "<<par.at(1)<<" "<<par.at(2)
                                  <<" "<<par.at(3)<<" "<<par.at(4)<<" "<<par.at(5)<<" "
                                  <<par.at(9)<<" "<<par.at(10)<<endreq;
  if(msgLevel(MSG::DEBUG)) debug()<<" vtx: rnet="<<rnet<<" pn="<<pn<<endreq;

  return pn;
}

//============================================================================
