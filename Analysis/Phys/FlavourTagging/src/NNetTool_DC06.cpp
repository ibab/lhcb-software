// Include files 
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
// local
#include "NNetTool_DC06.h"

#include "NeuralNet/DC06_NNmuon.cxx"
#include "NeuralNet/DC06_NNele.cxx"
#include "NeuralNet/DC06_NNkaon.cxx"
#include "NeuralNet/DC06_NNkaonS.cxx"
#include "NeuralNet/DC06_NNpionS.cxx"

//-----------------------------------------------------------------------------
// Implementation file for class : NNetTool_DC06 v1.3
//
// 2009-11-30 : Marco Musy 
//
// Neural Net tuned with Root.
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( NNetTool_DC06 );

//=============================================================================
NNetTool_DC06::NNetTool_DC06( const std::string& type,
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
NNetTool_DC06::~NNetTool_DC06(){}
StatusCode NNetTool_DC06::initialize() { return StatusCode::SUCCESS; }
StatusCode NNetTool_DC06::finalize()   { return StatusCode::SUCCESS; }

//=============================================================================
double NNetTool_DC06::SIGMOID(double x){
  if(x >  37.0) return 1.0;
  if(x < -37.0) return 0.0;
  return 1.0/(1.0+exp(-x));
}
double NNetTool_DC06::pol2(double x, double a0, double a1) {
  return a0+a1*x;
}
double NNetTool_DC06::pol3(double x, double a0, double a1, double a2) {
  return a0+a1*x+a2*x*x;
}

//=============================================================================
void NNetTool_DC06::normaliseOS(std::vector<double>& par) { 

  debug()<<"before norm "<<par<<endreq;

  par.at(0) /= 90.; //mult
  par.at(1) /= 20.; //ptB
  par.at(2) = std::min( par.at(2)/80., 1.); //partP
  par.at(3) = std::min( par.at(3)/5., 1.); //partPt
  par.at(4) = par.at(4)/50.; //IPPV
  if(par.at(4)> 1.) par.at(4)= 1.; if(par.at(4)<-1.) par.at(4)=-1.;
}

void NNetTool_DC06::normaliseSS(std::vector<double>& par) { 

  par.at(5) /= 2.; //nndeta
  par.at(6) /= 3.; //nndphi
  par.at(7) /= 12.;//nndq
  if(par.at(7)>1.) par.at(7) = 1.;

}

//=============================================================================
double NNetTool_DC06::MLPm(std::vector<double>& par) { 

  normaliseOS(par);
  DC06_NNmuon net;
  double rnet = net.value(0, par.at(0),par.at(2),par.at(3),par.at(4));

  double pn = 1.0-pol2(rnet, m_P0mu, m_P1mu);// <=========

  debug()<<"entering muon: rnet="<<rnet<<" pn="<<pn<<endreq;
  debug()<<"par = "<<par.at(0)<<" "<<par.at(2)<<" "<<par.at(3)
         <<" "<<par.at(4)<<endreq;

  return pn;
}; 

//=============================================================================
double NNetTool_DC06::MLPe(std::vector<double>& par) { 

  normaliseOS( par );
  DC06_NNele net;
  double rnet = net.value(0, par.at(0),par.at(2),par.at(3),par.at(4));

  double pn = 1.0-pol2(rnet, m_P0e, m_P1e);// <=========

  debug()<<"entering ele: rnet="<<rnet<<" pn="<<pn<<endreq;
  debug()<<"par = "<<par.at(0)<<" "<<par.at(2)<<" "<<par.at(3)
         <<" "<<par.at(4)<<endreq;

  return pn;
}; 

//=============================================================================
double NNetTool_DC06::MLPk(std::vector<double>& par ) { 

  normaliseOS( par );
  DC06_NNkaon net;
  double rnet = net.value(0, par.at(0),par.at(2),par.at(3),par.at(4));

  double pn = 1.0-pol2(rnet, m_P0k, m_P1k);// <=========

  debug()<<"entering k: rnet="<<rnet<<" pn="<<pn<<endreq;
  debug()<<"par = "<<par.at(0)<<" "<<par.at(2)<<" "<<par.at(3)
         <<" "<<par.at(4)<<endreq;

  return pn;
}; 

//=============================================================================
double NNetTool_DC06::MLPkS(std::vector<double>& par) { 

  normaliseOS( par );
  normaliseSS( par );
  DC06_NNkaonS net;
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
double NNetTool_DC06::MLPpS(std::vector<double>& par) { 

  normaliseOS( par );
  normaliseSS( par );
  DC06_NNpionS net;
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
