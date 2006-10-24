// $Id: NNetTool_v1.cpp,v 1.3 2006-10-24 10:16:44 jpalac Exp $
// Include files 
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
// local
#include "NNetTool_v1.h"

#include "NeuralNet/NNmuon.cxx"
#include "NeuralNet/NNele.cxx"
#include "NeuralNet/NNkaon.cxx"
#include "NeuralNet/NNkaonS.cxx"
#include "NeuralNet/NNpionS.cxx"
//#include <math.h>

//-----------------------------------------------------------------------------
// Implementation file for class : NNetTool_v1 v1.3
//
// 2006-05-15 : Marco Musy 
//
// Neural Net tuned with Root.
// Inputs are:  event-multiplicity,
// tagger p/GeV, pt/GeV, IP-significance,
// Delta-eta between B and tagger,
// Delta-phi, Delta-Q (= mass of B+tagger system - mass of B).
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( NNetTool_v1 );

//=============================================================================
NNetTool_v1::NNetTool_v1( const std::string& type,
			  const std::string& name,
			  const IInterface* parent ) :
  GaudiTool ( type, name, parent ) { 
  declareInterface<INNetTool>(this);
}
NNetTool_v1::~NNetTool_v1(){}
StatusCode NNetTool_v1::initialize() { return StatusCode::SUCCESS; }
StatusCode NNetTool_v1::finalize()   { return StatusCode::SUCCESS; }

//=============================================================================
double NNetTool_v1::SIGMOID(double x){
  if(x >  37.0) return 1.0;
  if(x < -37.0) return 0.0;
  return 1.0/(1.0+exp(-x));
}
double NNetTool_v1::pol2(double x, double a0, double a1) {
  return a0+a1*x;
}
double NNetTool_v1::pol3(double x, double a0, double a1, double a2) {
  return a0+a1*x+a2*x*x;
}

//=============================================================================
void NNetTool_v1::normaliseOS(std::vector<double>& par) { 

  if(par.size()<4) {
    err()<<"normaliseOS too short par vector"<<endreq;
    return;
  }
  double mult  = par.at(2) / 90.;
  double ptB   = par.at(0)*sin(par.at(1)) /20.; //unused
  double partP = std::min( par.at(4) /80., 1.);
  double partPt= std::min( par.at(5) /5. , 1.); 
  double IPPV  = par.at(6) /50.;
  if(IPPV > 1.) IPPV = 1.; if(IPPV<-1.) IPPV = -1.;
 
  par.at(0) = mult;
  par.at(1) = ptB;
  par.at(2) = partP;
  par.at(3) = partPt;
  par.at(4) = IPPV;

}

void NNetTool_v1::normaliseSS(std::vector<double>& par) { 

  if(par.size()<11) {
    err()<<"normaliseSS too short par vector"<<endreq;
    return;
  }
  double deta = par.at(9)  /2.;
  double dphi = par.at(10) /3.;
  double dq   = par.at(11) /12.;
  if(dq>1.) dq= 1.;

  par.at(5) = deta;
  par.at(6) = dphi;
  par.at(7) = dq;

}

//=============================================================================
double NNetTool_v1::MLPm(std::vector<double>& par) { 

  normaliseOS(par);
  NNmuon net;
  double rnet = net.value(0, par.at(0),par.at(2),par.at(3),par.at(4));

  double pn = 1.0-pol2(rnet, 1.042397, -1.089925);//1-omega

  debug()<<"entering ele: rnet="<<rnet<<" pn="<<pn<<endreq;

  return pn;
}; 

//=============================================================================
double NNetTool_v1::MLPe(std::vector<double>& par) { 

  normaliseOS( par );
  NNele net;
  double rnet = net.value(0, par.at(0),par.at(2),par.at(3),par.at(4));

  double pn = 1.0-pol2(rnet, 0.909170, -0.929768);//1-omega

  debug()<<"entering ele: rnet="<<rnet<<" pn="<<pn<<endreq;

  return pn;
}; 

//=============================================================================
double NNetTool_v1::MLPk(std::vector<double>& par ) { 

  normaliseOS( par );
  NNkaon net;
  double rnet = net.value(0, par.at(0),par.at(2),par.at(3),par.at(4));

  double pn = 1.0-pol2(rnet, 0.9, -0.815);//1-omega

  debug()<<"entering k: rnet="<<rnet<<" pn="<<pn<<endreq;

  return pn;
}; 

//=============================================================================
double NNetTool_v1::MLPkS(std::vector<double>& par) { 

  normaliseOS( par );
  normaliseSS( par );
  NNkaonS net;
  double rnet = net.value(0, par.at(0),par.at(2),par.at(3),par.at(4),
			  par.at(5),par.at(6),par.at(7) );
  
  double pn = 1.0-pol2(rnet, 0.984405, -0.988735);//1-omega

  debug()<<"par = "<<par.at(0)<<" "<<par.at(2)<<" "<<par.at(3)
	 <<" "<<par.at(4)<<" "<<par.at(5)<<" "<<par.at(6)
	 <<" "<<par.at(7)<<endreq;
  debug()<<"entering kS: rnet="<<rnet<<" pn="<<pn<<endreq;

  return pn;
}; 

//=============================================================================
double NNetTool_v1::MLPpS(std::vector<double>& par) { 

  normaliseOS( par );
  normaliseSS( par );
  NNpionS net;
  double rnet = net.value(0, par.at(0),par.at(2),par.at(3),par.at(4),
			  par.at(5),par.at(6),par.at(7) );

  double pn = 1.0-pol2(rnet, 0.850815, -0.726422);//1-omega

  debug()<<"entering pS: rnet="<<rnet<<" pn="<<pn<<endreq;

  return pn;
};
//=============================================================================
