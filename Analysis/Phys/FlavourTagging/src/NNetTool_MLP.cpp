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

//-----------------------------------------------------------------------------
// Implementation file for class : NNetTool_MLP v1.3
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
DECLARE_TOOL_FACTORY( NNetTool_MLP );

//=============================================================================
NNetTool_MLP::NNetTool_MLP( const std::string& type,
                            const std::string& name,
                            const IInterface* parent ) :
  GaudiTool ( type, name, parent ) { 
  declareInterface<INNetTool>(this);

  declareProperty( "P0_mu_scale", m_P0mu =  9.084315e-01);
  declareProperty( "P1_mu_scale", m_P1mu = -8.764311e-01);
  declareProperty( "P0_e_scale",  m_P0e  =  7.937505e-01);
  declareProperty( "P1_e_scale",  m_P1e  = -7.197138e-01);
  declareProperty( "P0_k_scale",  m_P0k  =  9.839528e-01);
  declareProperty( "P1_k_scale",  m_P1k  = -9.846077e-01);
  declareProperty( "P0_ks_scale", m_P0ks =  1.004943);
  declareProperty( "P1_ks_scale", m_P1ks = -1.017668);
  declareProperty( "P0_ps_scale", m_P0ps =  1.147873);
  declareProperty( "P1_ps_scale", m_P1ps = -1.241866);
    
}
NNetTool_MLP::~NNetTool_MLP(){}
StatusCode NNetTool_MLP::initialize() { return StatusCode::SUCCESS; }
StatusCode NNetTool_MLP::finalize()   { return StatusCode::SUCCESS; }

//=============================================================================
double NNetTool_MLP::SIGMOID(double x){
  if(x >  37.0) return 1.0;
  if(x < -37.0) return 0.0;
  return 1.0/(1.0+exp(-x));
}
double NNetTool_MLP::pol2(double x, double a0, double a1) {
  return a0+a1*x;
}
double NNetTool_MLP::pol3(double x, double a0, double a1, double a2) {
  return a0+a1*x+a2*x*x;
}

//=============================================================================
void NNetTool_MLP::normaliseOS(std::vector<double>& par) { 

  debug()<<"before norm "<<par<<endreq;

  par.at(0) /= 90.; //mult
  par.at(1) /= 20.; //ptB
  par.at(2) = std::min( par.at(2)/80., 1.); //partP
  par.at(3) = std::min( par.at(3)/5., 1.); //partPt
  par.at(4) = par.at(4)/50.; //IPPV
  if(par.at(4)> 1.) par.at(4)= 1.; if(par.at(4)<-1.) par.at(4)=-1.;
}

void NNetTool_MLP::normaliseSS(std::vector<double>& par) { 

  par.at(5) /= 2.; //nndeta
  par.at(6) /= 3.; //nndphi
  par.at(7) /= 12.;//nndq
  if(par.at(7)>1.) par.at(7) = 1.;

}

//=============================================================================
double NNetTool_MLP::MLPm(std::vector<double>& par) { 

  normaliseOS(par);
  NNmuon net;
  double rnet = net.value(0, par.at(0),par.at(2),par.at(3),par.at(4));

  double pn = 1.0-pol2(rnet, m_P0mu, m_P1mu);// <=========

  debug()<<"entering muon: rnet="<<rnet<<" pn="<<pn<<endreq;
  debug()<<"par = "<<par.at(0)<<" "<<par.at(2)<<" "<<par.at(3)
         <<" "<<par.at(4)<<endreq;

  return pn;
}; 

//=============================================================================
double NNetTool_MLP::MLPe(std::vector<double>& par) { 

  normaliseOS( par );
  NNele net;
  double rnet = net.value(0, par.at(0),par.at(2),par.at(3),par.at(4));

  double pn = 1.0-pol2(rnet, m_P0e, m_P1e);// <=========

  debug()<<"entering ele: rnet="<<rnet<<" pn="<<pn<<endreq;
  debug()<<"par = "<<par.at(0)<<" "<<par.at(2)<<" "<<par.at(3)
         <<" "<<par.at(4)<<endreq;

  return pn;
}; 

//=============================================================================
double NNetTool_MLP::MLPk(std::vector<double>& par ) { 

  normaliseOS( par );
  NNkaon net;
  double rnet = net.value(0, par.at(0),par.at(2),par.at(3),par.at(4));

  double pn = 1.0-pol2(rnet, m_P0k, m_P1k);// <=========

  debug()<<"entering k: rnet="<<rnet<<" pn="<<pn<<endreq;
  debug()<<"par = "<<par.at(0)<<" "<<par.at(2)<<" "<<par.at(3)
         <<" "<<par.at(4)<<endreq;

  return pn;
}; 

//=============================================================================
double NNetTool_MLP::MLPkS(std::vector<double>& par) { 

  normaliseOS( par );
  normaliseSS( par );
  NNkaonS net;
  double rnet = net.value(0, par.at(0),par.at(2),par.at(3),par.at(4),
                          par.at(5),par.at(6),par.at(7) );
  
  double pn = 1.0-pol2(rnet, m_P0ks, m_P1ks);// <=========

  debug()<<"entering kS: rnet="<<rnet<<" pn="<<pn<<endreq;
  debug()<<"par = "<<par.at(0)<<" "<<par.at(2)<<" "<<par.at(3)
         <<" "<<par.at(4)<<" "<<par.at(5)<<" "<<par.at(6)
         <<" "<<par.at(7)<<endreq;

  return pn;
}; 

//=============================================================================
double NNetTool_MLP::MLPpS(std::vector<double>& par) { 

  normaliseOS( par );
  normaliseSS( par );
  NNpionS net;
  double rnet = net.value(0, par.at(0),par.at(2),par.at(3),par.at(4),
                          par.at(5),par.at(6),par.at(7) );

  double pn = 1.0-pol2(rnet, m_P0ps, m_P1ps);// <=========

  debug()<<"entering pS: rnet="<<rnet<<" pn="<<pn<<endreq;
  debug()<<"par = "<<par.at(0)<<" "<<par.at(2)<<" "<<par.at(3)
         <<" "<<par.at(4)<<" "<<par.at(5)<<" "<<par.at(6)
         <<" "<<par.at(7)<<endreq;

  return pn;
};
//=============================================================================
