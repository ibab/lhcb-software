// Include files 
#include "NNetTool_MLP.h"

//=============================================================================
NNetTool_MLP::NNetTool_MLP( ) {

  declareProperty( "P0_mu_scale", m_P0mu =  0.827472); //MC10 defaults
  declareProperty( "P1_mu_scale", m_P1mu = -0.750251);

  declareProperty( "P0_e_scale",  m_P0e  =  0.832737);
  declareProperty( "P1_e_scale",  m_P1e  = -0.789312);

  declareProperty( "P0_k_scale",  m_P0k  =  0.916836);
  declareProperty( "P1_k_scale",  m_P1k  = -0.847595);

  declareProperty( "P0_ks_scale", m_P0ks =  1.0012);
  declareProperty( "P1_ks_scale", m_P1ks = -0.988425);

  declareProperty( "P0_ps_scale", m_P0ps =  1.05709);
  declareProperty( "P1_ps_scale", m_P1ps = -1.09695);

  declareProperty( "P0_vtx_scale", m_P0vtx =  1.38505 );
  declareProperty( "P1_vtx_scale", m_P1vtx = -1.62705 );
 
  m_rnet_mu= m_rnet_ele= m_rnet_k= m_rnet_kS= m_rnet_pS= m_rnet_vtx=0;
 
}

//=============================================================================
void NNetTool_MLP::normaliseOS(std::vector<double>& par) { 

  if(DBGLEVEL>2)debug()<<"par = "<<par.at(0)<<" "<<par.at(2)<<" "<<par.at(3)
                       <<" "<<par.at(4)<<" "<<par.at(5)<<" "<<par.at(6)<<" "
                       <<par.at(7)<<" "<<par.at(8)<<" "<<par.at(9)
                       <<" "<<par.at(1)<<endreq;

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

  if(DBGLEVEL>2)debug()<<"par = "<<par.at(0)<<" "<<par.at(1)<<" "<<par.at(2)<<" "<<par.at(3)
                       <<" "<<par.at(4)<<" "<<par.at(5)<<" "<<par.at(6)
                       <<" "<<par.at(7)<<" "<<par.at(8)<<" "<<par.at(9)
                       <<" "<<par.at(10)<<endreq;

  par.at(0) /= 90.; //vtag size
  par.at(2) /= 25.; //ptB
  par.at(3) /= 4.;  //ncand
  par.at(4) /= 2.;  //ptmin
  par.at(5) /= 30.; //ipsmin
  par.at(6) /= 0.1; //docamax
  par.at(10)/= 5.;  //SVtau
}
//=============================================================================
double NNetTool_MLP::MLPm(std::vector<double>& par) { 

  normaliseOS( par );
  NNmuon net;
  double rnet = net.value(0, par.at(0),par.at(2),par.at(3),par.at(4),
                          par.at(8),par.at(1));

  double pn = 1.0-pol(rnet, m_P0mu, m_P1mu);// <=========

  print(par, rnet, pn);

  m_rnet_mu = rnet;

  return pn;
}; 

//=============================================================================
double NNetTool_MLP::MLPe(std::vector<double>& par) { 

  normaliseOS( par );
  NNele net;
  double rnet = net.value(0, par.at(0),par.at(2),par.at(3),par.at(4),
                          par.at(8),par.at(1));

  double pn = 1.0-pol(rnet, m_P0e, m_P1e);// <=========

  print(par, rnet, pn);

  m_rnet_ele = rnet;

  return pn;
}; 

//=============================================================================
double NNetTool_MLP::MLPk(std::vector<double>& par ) { 

  normaliseOS( par );
  NNkaon net;
  double rnet = net.value(0, par.at(0),par.at(2),par.at(3),par.at(4),
                          par.at(8),par.at(1));

  double pn = 1.0-pol(rnet, m_P0k, m_P1k);// <=========

  print(par, rnet, pn);

  m_rnet_k = rnet;

  return pn;
}; 

//=============================================================================
double NNetTool_MLP::MLPkS(std::vector<double>& par) { 

  normaliseOS( par );
  normaliseSS( par );
  NNkaonS net;
  double rnet = net.value(0, par.at(0),par.at(2),par.at(3),par.at(4),
                          par.at(5),par.at(6),par.at(7),par.at(8),par.at(1));
  
  double pn = 1.0-pol(rnet, m_P0ks, m_P1ks);// <=========

  print(par, rnet, pn);

  m_rnet_kS = rnet;

  return pn;
}; 

//=============================================================================
double NNetTool_MLP::MLPpS(std::vector<double>& par) { 

  normaliseOS( par );
  normaliseSS( par );
  NNpionS net;
  double rnet = net.value(0,par.at(0),par.at(2),par.at(3),par.at(4),
			  par.at(5),par.at(6),par.at(7),par.at(8),par.at(1));

  double pn = 1.0-pol(rnet, m_P0ps, m_P1ps);// <=========

  print(par, rnet, pn);

  m_rnet_pS = rnet;

  return pn;
};

//=============================================================================
double NNetTool_MLP::MLPvtx(std::vector<double>& par) { 

  normaliseVtx( par );
  NNvtx net;
  double rnet = net.value(0,par.at(0),par.at(1),par.at(2),par.at(3),par.at(4),
			  par.at(5),par.at(6),par.at(7),par.at(8),par.at(9),par.at(10));

  double pn = 1.0-pol(rnet, m_P0vtx, m_P1vtx);// <=========

  print(par, rnet, pn);

  m_rnet_vtx = rnet;

  return pn;
};

//=============================================================================
