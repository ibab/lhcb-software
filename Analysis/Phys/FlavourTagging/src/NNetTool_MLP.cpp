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
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( NNetTool_MLP );

//=============================================================================
NNetTool_MLP::NNetTool_MLP( const std::string& type,
                            const std::string& name,
                            const IInterface* parent ) :
  GaudiTool ( type, name, parent ) { 
  declareInterface<INNetTool>(this);

  declareProperty( "P0_mu_scale", m_P0mu =  9.610658e-01); //MC09 defaults
  declareProperty( "P1_mu_scale", m_P1mu = -9.336494e-01);
  declareProperty( "P0_e_scale",  m_P0e  =  7.353325e-01);
  declareProperty( "P1_e_scale",  m_P1e  = -6.463841e-01);
  declareProperty( "P0_k_scale",  m_P0k  =  9.149897e-01);
  declareProperty( "P1_k_scale",  m_P1k  = -8.646468e-01);
  declareProperty( "P0_ks_scale", m_P0ks =  9.845081e-01);
  declareProperty( "P1_ks_scale", m_P1ks = -9.742343e-01);
  declareProperty( "P0_ps_scale", m_P0ps =  1.222453);
  declareProperty( "P1_ps_scale", m_P1ps = -1.369672);

  declareProperty( "P0_vtx_scale", m_P0vtx =  5.255669e-01);
  declareProperty( "P1_vtx_scale", m_P1vtx = -3.251661e-01);
  declareProperty( "Gt075_vtx",    m_Gt075 = 0.35);
  declareProperty( "wSameSign2",         m_wSameSign2         = 0.4141);
  declareProperty( "wSameSignMoreThan2", m_wSameSignMoreThan2 = 0.3250);
  declareProperty( "ProbMinVtx",       m_ProbMinVtx     = 0.52);
  declareProperty( "MinimumVCharge",   m_MinimumVCharge = 0.15);

}
NNetTool_MLP::~NNetTool_MLP(){}
StatusCode NNetTool_MLP::initialize() { return StatusCode::SUCCESS; }
StatusCode NNetTool_MLP::finalize()   { return StatusCode::SUCCESS; }

//=============================================================================
double NNetTool_MLP::pol2(double x, double a0, double a1) {
  return a0+a1*x;
}

//=============================================================================
void NNetTool_MLP::normaliseOS(std::vector<double>& par) { 

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
  double rnet = net.value(0, par.at(0),par.at(2),par.at(3),par.at(4),
                          par.at(8),par.at(9),par.at(1));

  double pn = 1.0-pol2(rnet, m_P0mu, m_P1mu);// <=========

  if(msgLevel(MSG::DEBUG))debug()<<"par = "<<par.at(0)<<" "<<par.at(2)<<" "<<par.at(3)
         <<" "<<par.at(4)<<" "<<par.at(8)<<" "<<par.at(9)<<" "<<par.at(1)<<endreq;
  if(msgLevel(MSG::DEBUG))debug()<<" muon: rnet="<<rnet<<" pn="<<pn<<endreq;

  return pn;
}; 

//=============================================================================
double NNetTool_MLP::MLPe(std::vector<double>& par) { 

  normaliseOS( par );
  NNele net;
  double rnet = net.value(0, par.at(0),par.at(2),par.at(3),par.at(4)
                          ,par.at(8),par.at(9),par.at(1));

  double pn = 1.0-pol2(rnet, m_P0e, m_P1e);// <=========

  if(msgLevel(MSG::DEBUG))debug()<<"ele: par = "<<par.at(0)<<" "<<par.at(2)<<" "<<par.at(3)
	 <<" "<<par.at(4)<<" "<<par.at(5)<<" "<<par.at(6)<<" "<<par.at(7)
	 <<" "<<par.at(8)<<" "<<par.at(9)<<" "<<par.at(1)<<endreq;
  if(msgLevel(MSG::DEBUG))debug()<<" ele: rnet="<<rnet<<" pn="<<pn<<endreq;

  return pn;
}; 

//=============================================================================
double NNetTool_MLP::MLPk(std::vector<double>& par ) { 

  normaliseOS( par );
  NNkaon net;
  double rnet = net.value(0, par.at(0),par.at(2),par.at(3),par.at(4),
                          par.at(8),par.at(9),par.at(1));

  double pn = 1.0-pol2(rnet, m_P0k, m_P1k);// <=========

  if(msgLevel(MSG::DEBUG))debug()<<"par = "<<par.at(0)<<" "<<par.at(2)<<" "<<par.at(3)
	 <<" "<<par.at(4)<<" "<<par.at(8)<<" "<<par.at(9)<<" "<<par.at(1)<<endreq;
  if(msgLevel(MSG::DEBUG))debug()<<" k: rnet="<<rnet<<" pn="<<pn<<endreq;

  return pn;
}; 

//=============================================================================
double NNetTool_MLP::MLPkS(std::vector<double>& par) { 

  normaliseOS( par );
  normaliseSS( par );
  NNkaonS net;
  double rnet = net.value(0, par.at(0),par.at(2),par.at(3),par.at(4),
                          par.at(5),par.at(6),par.at(7),par.at(8),par.at(9),par.at(1));
  
  double pn = 1.0-pol2(rnet, m_P0ks, m_P1ks);// <=========

  if(msgLevel(MSG::DEBUG))debug()<<"par = "<<par.at(0)<<" "<<par.at(2)<<" "<<par.at(3)
	 <<" "<<par.at(4)<<" "<<par.at(5)<<" "<<par.at(6)
	 <<" "<<par.at(7)<<" "<<par.at(8)<<" "<<par.at(9)<<" "<<par.at(1)<<endreq;
  if(msgLevel(MSG::DEBUG))debug()<<" kS: rnet="<<rnet<<" pn="<<pn<<endreq;

  return pn;
}; 

//=============================================================================
double NNetTool_MLP::MLPpS(std::vector<double>& par) { 

  normaliseOS( par );
  normaliseSS( par );
  NNpionS net;
  double rnet = net.value(0,par.at(0),par.at(2),par.at(3),par.at(4),
                          par.at(5),par.at(6),par.at(7),par.at(8),par.at(9),par.at(1));

  double pn = 1.0-pol2(rnet, m_P0ps, m_P1ps);// <=========

  if(msgLevel(MSG::DEBUG)) debug()<<"par = "<<par.at(0)<<" "<<par.at(2)<<" "<<par.at(3)
	 <<" "<<par.at(4)<<" "<<par.at(5)<<" "<<par.at(6)
	 <<" "<<par.at(7)<<" "<<par.at(8)<<" "<<par.at(9)<<" "<<par.at(1)<<endreq;
  if(msgLevel(MSG::DEBUG)) debug()<<" pS: rnet="<<rnet<<" pn="<<pn<<endreq;

  return pn;
};
//=============================================================================
double NNetTool_MLP::MLPvtx(std::vector<double>& par) { 

  if ( par.size() != 2 ) {
    err()<<"Wrong size input parameter. "<<par.size()<<endreq;
    return 0.5;
  }

  double WeigVch = par.at(0);
  double vflagged= par.at(1);

  double omega=0.50;
  if( fabs(WeigVch)<0.75 ) omega = pol2(fabs(WeigVch), m_P0vtx, m_P1vtx);
  if( fabs(WeigVch)>0.75 ) omega = m_Gt075;
  if( fabs(WeigVch)>0.99 ) { // tracks have all the same charge
    if(vflagged==2) omega = m_wSameSign2;
    if(vflagged >2) omega = m_wSameSignMoreThan2;
  }
  if( 1-omega < m_ProbMinVtx || omega > m_ProbMinVtx ) {
    return 0.50;
  }
  return 1 - omega; 

}
//=============================================================================
