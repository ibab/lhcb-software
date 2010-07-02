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
DECLARE_TOOL_FACTORY( NNetTool_MLP );

//=============================================================================
NNetTool_MLP::NNetTool_MLP( const std::string& type,
                            const std::string& name,
                            const IInterface* parent ) :
  GaudiTool ( type, name, parent ) { 
  declareInterface<INNetTool>(this);

  declareProperty( "P0_mu_scale", m_P0mu =  0.994053); //MC10 defaults
  declareProperty( "P1_mu_scale", m_P1mu = -0.990767);
  declareProperty( "P0_e_scale",  m_P0e  =  0.960898);
  declareProperty( "P1_e_scale",  m_P1e  = -0.956197);
  declareProperty( "P0_k_scale",  m_P0k  =  0.910427);
  declareProperty( "P1_k_scale",  m_P1k  = -0.864479);
  declareProperty( "P0_ks_scale", m_P0ks =  1.03063);
  declareProperty( "P1_ks_scale", m_P1ks = -1.04504);
  declareProperty( "P0_ps_scale", m_P0ps =  1.05709);
  declareProperty( "P1_ps_scale", m_P1ps = -1.09695);
  declareProperty( "P0_vtx_scale", m_P0vtx =  2.34711);
  declareProperty( "P1_vtx_scale", m_P1vtx = -7.63911);
  declareProperty( "P2_vtx_scale", m_P2vtx = 10.7425);
  declareProperty( "P3_vtx_scale", m_P3vtx = -5.37049);
  //  declareProperty( "P0_vtx_scale", m_P0vtx =  5.255669e-01);
  //  declareProperty( "P1_vtx_scale", m_P1vtx = -3.251661e-01);
  //  declareProperty( "Gt075_vtx",    m_Gt075 = 0.35);
  //  declareProperty( "wSameSign2",         m_wSameSign2         = 0.4141);
  //  declareProperty( "wSameSignMoreThan2", m_wSameSignMoreThan2 = 0.3250);
  //  declareProperty( "ProbMinVtx",       m_ProbMinVtx     = 0.52);
  //  declareProperty( "MinimumVCharge",   m_MinimumVCharge = 0.15);

}
NNetTool_MLP::~NNetTool_MLP(){}
StatusCode NNetTool_MLP::initialize() { return StatusCode::SUCCESS; }
StatusCode NNetTool_MLP::finalize()   { return StatusCode::SUCCESS; }

//=============================================================================
double NNetTool_MLP::pol2(double x, double a0, double a1) {
  return a0+a1*x;
}
double NNetTool_MLP::pol4(double x, double a0, double a1, double a2, double a3) {
  return a0+a1*x+a2*x*x+a3*x*x*x;
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
void NNetTool_MLP::normaliseVtx(std::vector<double>& par) { 

  debug()<<"par = "<<par.at(0)<<" "<<par.at(2)<<" "<<par.at(3)
         <<" "<<par.at(4)<<" "<<par.at(5)<<" "<<par.at(6)<<" "<<par.at(7)
	 <<" "<<par.at(8)<<" "<<par.at(9)<<" "<<par.at(1)<<endreq;

  par.at(0) /= 90.; //vtag size
  par.at(2) /= 25.; //ptB
  par.at(3) /= 4.;  //ncand
  par.at(4) /= 2.;  //ptmin
  par.at(5) /= 30.; //ipsmin
  par.at(6) /= 0.1; //docamax
}
//=============================================================================
double NNetTool_MLP::MLPm(std::vector<double>& par) { 

  normaliseOS(par);
  NNmuon net;
  double rnet = net.value(0, par.at(0),par.at(2),par.at(3),par.at(4),
                          par.at(8),par.at(1));

  double pn = 1.0-pol2(rnet, m_P0mu, m_P1mu);// <=========

  if(msgLevel(MSG::DEBUG))debug()<<"par = "<<par.at(0)<<" "<<par.at(2)<<" "<<par.at(3)
         <<" "<<par.at(4)<<" "<<par.at(8)<<" ("<<par.at(9)<<") "<<par.at(1)<<endreq;
  if(msgLevel(MSG::DEBUG))debug()<<" muon: rnet="<<rnet<<" pn="<<pn<<endreq;

  return pn;
}; 

//=============================================================================
double NNetTool_MLP::MLPe(std::vector<double>& par) { 

  normaliseOS( par );
  NNele net;
  double rnet = net.value(0, par.at(0),par.at(2),par.at(3),par.at(4),
                          par.at(8),par.at(1));

  double pn = 1.0-pol2(rnet, m_P0e, m_P1e);// <=========

  if(msgLevel(MSG::DEBUG))debug()<<"ele: par = "<<par.at(0)<<" "<<par.at(2)<<" "<<par.at(3)
	 <<" "<<par.at(4)<<" "<<par.at(5)<<" "<<par.at(6)<<" "<<par.at(7)
	 <<" "<<par.at(8)<<" ("<<par.at(9)<<") "<<par.at(1)<<endreq;
  if(msgLevel(MSG::DEBUG))debug()<<" ele: rnet="<<rnet<<" pn="<<pn<<endreq;

  return pn;
}; 

//=============================================================================
double NNetTool_MLP::MLPk(std::vector<double>& par ) { 

  normaliseOS( par );
  NNkaon net;
  double rnet = net.value(0, par.at(0),par.at(2),par.at(3),par.at(4),
                          par.at(8),par.at(1));

  double pn = 1.0-pol2(rnet, m_P0k, m_P1k);// <=========

  if(msgLevel(MSG::DEBUG))debug()<<"par = "<<par.at(0)<<" "<<par.at(2)<<" "<<par.at(3)
	 <<" "<<par.at(4)<<" "<<par.at(8)<<" ("<<par.at(9)<<") "<<par.at(1)<<endreq;
  if(msgLevel(MSG::DEBUG))debug()<<" k: rnet="<<rnet<<" pn="<<pn<<endreq;

  return pn;
}; 

//=============================================================================
double NNetTool_MLP::MLPkS(std::vector<double>& par) { 

  normaliseOS( par );
  normaliseSS( par );
  NNkaonS net;
  double rnet = net.value(0, par.at(0),par.at(2),par.at(3),par.at(4),
                          par.at(5),par.at(6),par.at(7),par.at(8),par.at(1));
  
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
                          par.at(5),par.at(6),par.at(7),par.at(8),par.at(1));

  double pn = 1.0-pol2(rnet, m_P0ps, m_P1ps);// <=========

  if(msgLevel(MSG::DEBUG)) debug()<<"par = "<<par.at(0)<<" "<<par.at(2)<<" "<<par.at(3)
	 <<" "<<par.at(4)<<" "<<par.at(5)<<" "<<par.at(6)
	 <<" "<<par.at(7)<<" "<<par.at(8)<<" ("<<par.at(9)<<") "<<par.at(1)<<endreq;
  if(msgLevel(MSG::DEBUG)) debug()<<" pS: rnet="<<rnet<<" pn="<<pn<<endreq;

  return pn;
};

//=============================================================================
double NNetTool_MLP::MLPvtx(std::vector<double>& par) { 

  normaliseVtx( par );
  NNvtx net;
  double rnet = net.value(0,par.at(0),par.at(1),par.at(2),par.at(3),par.at(4),
                          par.at(5),par.at(6),par.at(7),par.at(8),par.at(9));

  double pn = 1.0-pol4(rnet, m_P0vtx, m_P1vtx, m_P2vtx, m_P3vtx);// <=========

  if(msgLevel(MSG::DEBUG)) debug()<<"par = "<<par.at(0)<<" "<<par.at(1)<<" "<<par.at(2)
	 <<" "<<par.at(3)<<" "<<par.at(4)<<" "<<par.at(5)
	 <<" "<<par.at(6)<<" "<<par.at(7)<<" "<<par.at(8)<<" "<<par.at(9)<<endreq;
  if(msgLevel(MSG::DEBUG)) debug()<<" vtx: rnet="<<rnet<<" pn="<<pn<<endreq;

  return pn;
};

// double NNetTool_MLP::MLPvtx(std::vector<double>& par) { 

//   if ( par.size() != 2 ) {
//     err()<<"Wrong size input parameter. "<<par.size()<<endreq;
//     return 0.5;
//   }

//   double WeigVch = par.at(0);
//   double vflagged= par.at(1);

//   double omega=0.50;
//   if( fabs(WeigVch)<0.75 ) omega = pol2(fabs(WeigVch), m_P0vtx, m_P1vtx);
//   if( fabs(WeigVch)>0.75 ) omega = m_Gt075;
//   if( fabs(WeigVch)>0.99 ) { // tracks have all the same charge
//     if(vflagged==2) omega = m_wSameSign2;
//     if(vflagged >2) omega = m_wSameSignMoreThan2;
//   }
//   if( 1-omega < m_ProbMinVtx || omega > m_ProbMinVtx ) {
//     return 0.50;
//   }
//   return 1 - omega; 
// }
//=============================================================================
