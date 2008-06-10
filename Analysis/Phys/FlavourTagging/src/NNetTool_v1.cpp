// $Id: NNetTool_v1.cpp,v 1.5 2008-06-10 19:01:00 musy Exp $
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

  debug()<<"before norm "<<par<<endreq;

  par.at(0) /= 90.; //mult
  par.at(1) /= 20.; //ptB
  par.at(2) = std::min( par.at(2)/80., 1.); //partP
  par.at(3) = std::min( par.at(3)/5., 1.); //partPt
  par.at(4) = par.at(4)/50.; //IPPV
  if(par.at(4)> 1.) par.at(4)= 1.; if(par.at(4)<-1.) par.at(4)=-1.;
}

void NNetTool_v1::normaliseSS(std::vector<double>& par) { 

  par.at(5) /= 2.; //nndeta
  par.at(6) /= 3.; //nndphi
  par.at(7) /= 12.;//nndq
  if(par.at(7)>1.) par.at(7) = 1.;

}

//=============================================================================
double NNetTool_v1::MLPm(std::vector<double>& par) { 

  normaliseOS(par);
  NNmuon net;
  double rnet = net.value(0, par.at(0),par.at(2),par.at(3),par.at(4));

  double pn = 1.0-pol2(rnet, 9.303798e-01, -8.848241e-01);

  debug()<<"entering muon: rnet="<<rnet<<" pn="<<pn<<endreq;
  debug()<<"par = "<<par.at(0)<<" "<<par.at(2)<<" "<<par.at(3)
	 <<" "<<par.at(4)<<endreq;

  return pn;
}; 

//=============================================================================
double NNetTool_v1::MLPe(std::vector<double>& par) { 

  normaliseOS( par );
  NNele net;
  double rnet = net.value(0, par.at(0),par.at(2),par.at(3),par.at(4));

  double pn = 1.0-pol2(rnet, 8.999370e-01, -8.505584e-01);//1-omega

  debug()<<"entering ele: rnet="<<rnet<<" pn="<<pn<<endreq;
  debug()<<"par = "<<par.at(0)<<" "<<par.at(2)<<" "<<par.at(3)
	 <<" "<<par.at(4)<<endreq;

  return pn;
}; 

//=============================================================================
double NNetTool_v1::MLPk(std::vector<double>& par ) { 

  normaliseOS( par );
  NNkaon net;
  double rnet = net.value(0, par.at(0),par.at(2),par.at(3),par.at(4));

  double pn = 1.0-pol2(rnet, 9.912881e-01,-9.958917e-01);

  debug()<<"entering k: rnet="<<rnet<<" pn="<<pn<<endreq;
  debug()<<"par = "<<par.at(0)<<" "<<par.at(2)<<" "<<par.at(3)
	 <<" "<<par.at(4)<<endreq;

  return pn;
}; 

//=============================================================================
double NNetTool_v1::MLPkS(std::vector<double>& par) { 

  normaliseOS( par );
  normaliseSS( par );
  NNkaonS net;
  double rnet = net.value(0, par.at(0),par.at(2),par.at(3),par.at(4),
			  par.at(5),par.at(6),par.at(7) );
  
  double pn = 1.0-pol2(rnet,  9.943074e-01, -9.933644e-01);

  debug()<<"entering kS: rnet="<<rnet<<" pn="<<pn<<endreq;
  debug()<<"par = "<<par.at(0)<<" "<<par.at(2)<<" "<<par.at(3)
	 <<" "<<par.at(4)<<" "<<par.at(5)<<" "<<par.at(6)
	 <<" "<<par.at(7)<<endreq;

  return pn;
}; 

//=============================================================================
double NNetTool_v1::MLPpS(std::vector<double>& par) { 

  normaliseOS( par );
  normaliseSS( par );
  NNpionS net;
  double rnet = net.value(0, par.at(0),par.at(2),par.at(3),par.at(4),
			  par.at(5),par.at(6),par.at(7) );

  double pn = 1.0-pol2(rnet, 9.943074e-01, -9.933644e-01);

  debug()<<"entering pS: rnet="<<rnet<<" pn="<<pn<<endreq;
  debug()<<"par = "<<par.at(0)<<" "<<par.at(2)<<" "<<par.at(3)
	 <<" "<<par.at(4)<<" "<<par.at(5)<<" "<<par.at(6)
	 <<" "<<par.at(7)<<endreq;

  return pn;
};
//=============================================================================
