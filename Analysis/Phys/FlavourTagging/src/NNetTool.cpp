// $Id: NNetTool.cpp,v 1.6 2005-07-08 11:32:06 pkoppenb Exp $
// Include files 
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
// local
#include "NNetTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : NNetTool v1.3
//
// 2004-03-18 : Marco Musy 
//
// Neural Net returns wrong tag fraction for a given tagger.
// Inputs are: B-signal momentum/GeV, B-theta, event-multiplicity,
// tampering flag, tagger p/GeV, pt/GeV, IP-significance,
// IP significance wrt secondary Vtx, Delta-eta between B and tagger,
// Delta-phi, Delta-Q (= mass of B+tagger system - mass of B).
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<NNetTool>          s_factory ;
const        IToolFactory& NNetToolFactory = s_factory ; 

//=============================================================================
NNetTool::NNetTool( const std::string& type,
		    const std::string& name,
		    const IInterface* parent ) :
  GaudiTool ( type, name, parent ) { 
  declareInterface<INNetTool>(this);
}
NNetTool::~NNetTool(){}
StatusCode NNetTool::initialize() { return StatusCode::SUCCESS; }
StatusCode NNetTool::finalize() { return StatusCode::SUCCESS; }

//=============================================================================
double NNetTool::SIGMOID(double x){
  if(x >  37.0) return 1.0;
  if(x < -37.0) return 0.0;
  return 1.0/(1.0+exp(-x));
}

//=============================================================================
void NNetTool::normaliseOS(double& OUT1, double& OUT2, double& OUT3,
			   double& OUT4, double& OUT5, double& OUT6,
			   double& OUT7, double& OUT8) { 
  OUT1 = std::min(OUT1/600., 1.);
  OUT2 = std::min(OUT2/0.25, 1.);
  OUT3 = OUT3/90.;
  if(OUT4 ==  10) OUT4=0.;
  else if(OUT4 == 100) OUT4=0.33;
  else if(OUT4 ==  11) OUT4=0.66;
  else if(OUT4 ==   1) OUT4=0.99;

  OUT5 = std::min(OUT5/150., 1.);
  OUT6 = std::min(OUT6/10., 1.);
  OUT7 = std::min(std::max(OUT7/100.,-1.),1.);
  OUT8 = std::min(std::max(OUT8/12.,-1.),1.);
}
void NNetTool::normaliseSS(double& OUT1, double& OUT2, double& OUT3,
			   double& OUT4, double& OUT5, double& OUT6,
			   double& OUT7, double& OUT8, double& OUT10, 
			   double& OUT11, double& OUT12) { 
  OUT1 = std::min(OUT1/600., 1.);
  OUT2 = std::min(OUT2/0.25, 1.);
  OUT3 = OUT3/90.;
  if(OUT4 ==  10) OUT4=0.;
  else if(OUT4 == 100) OUT4=0.33;
  else if(OUT4 ==  11) OUT4=0.66;
  else if(OUT4 ==   1) OUT4=0.99;

  OUT5 = std::min(OUT5/150., 1.);
  OUT6 = std::min(OUT6/10., 1.);
  OUT7 = std::min(std::max(OUT7/100.,-1.),1.);
  OUT8 = std::min(std::max(OUT8/12.,-1.),1.);
  OUT10= OUT10/3.5;
  OUT11= OUT11/3.2;
  OUT12= std::min(OUT12/12.0, 1.);
}

//=============================================================================
double NNetTool::MLPm(double OUT1, double OUT2, double OUT3,
		      double OUT4, double OUT5, double OUT6,
		      double OUT7, double OUT8) { 

  normaliseOS( OUT1,  OUT2,  OUT3,
	       OUT4,  OUT5,  OUT6, OUT7,  OUT8 );
  double OUT9 = 0.;

  double RIN1 = 3.983911e-01
    +(-7.026953e-01) * OUT1
    +(-3.774790e-01) * OUT2
    +(-1.885221e+00) * OUT3
    +(3.752778e-01) * OUT4
    +(1.054646e+00) * OUT5
    +(1.810602e+00) * OUT6
    +(5.463738e-01) * OUT7
    +(-3.098921e-01) * OUT8
    +(6.636706e-02) * OUT9;

  OUT1 = SIGMOID(RIN1);

  return 3.334001e-01
    +(5.451372e-01) * OUT1;
}; 

//=============================================================================
double NNetTool::MLPe(double OUT1, double OUT2, double OUT3,
		      double OUT4, double OUT5, double OUT6,
		      double OUT7, double OUT8) { 

  normaliseOS( OUT1,  OUT2,  OUT3,
	       OUT4,  OUT5,  OUT6, OUT7,  OUT8 );
  double OUT9 = 0.;

  double RIN1 = 1.645051e+00
    +(5.659825e-01) * OUT1
    +(1.514887e+00) * OUT2
    +(8.109475e-01) * OUT3
    +(7.774106e-01) * OUT4
    +(1.434648e+00) * OUT5
    +(5.218562e-02) * OUT6
    +(-2.601974e+00) * OUT7
    +(9.479127e-01) * OUT8
    +(3.571042e+00) * OUT9;
  double RIN2 = 6.992168e-02
    +(5.187529e-01) * OUT1
    +(5.686077e-01) * OUT2
    +(-6.081721e-01) * OUT3
    +(1.137005e+00) * OUT4
    +(3.420101e-01) * OUT5
    +(3.229865e+00) * OUT6
    +(-2.142025e+00) * OUT7
    +(3.548180e-01) * OUT8
    +(1.462764e+00) * OUT9;
  double RIN3 = -1.341696e+00
    +(-6.984814e-01) * OUT1
    +(-1.241993e+00) * OUT2
    +(3.622794e-01) * OUT3
    +(-3.990370e-01) * OUT4
    +(-1.061339e-01) * OUT5
    +(-2.995072e+00) * OUT6
    +(1.136272e+00) * OUT7
    +(4.595882e-01) * OUT8
    +(-1.349517e+00) * OUT9;

  OUT1 = SIGMOID(RIN1);
  OUT2 = SIGMOID(RIN2);
  OUT3 = SIGMOID(RIN3);

  return 1.339276e+00
    +(-1.705970e+00) * OUT1
    +(1.236691e+00) * OUT2
    +(-1.525095e+00) * OUT3;
}; 

//=============================================================================
double NNetTool::MLPk(double OUT1, double OUT2, double OUT3,
		      double OUT4, double OUT5, double OUT6,
		      double OUT7, double OUT8) { 

  normaliseOS( OUT1,  OUT2,  OUT3,
	       OUT4,  OUT5,  OUT6, OUT7,  OUT8 );
  double OUT9 = 0.;

  double  RIN1 = -2.548925e-01
    +(7.836971e-02) * OUT1
    +(-7.366728e-01) * OUT2
    +(-2.791295e+00) * OUT3
    +(5.307588e-01) * OUT4
    +(-1.701194e+00) * OUT5
    +(1.576454e+01) * OUT6
    +(2.416985e-01) * OUT7
    +(4.765683e-01) * OUT8
    +(1.182657e+00) * OUT9;

  OUT1 = SIGMOID(RIN1);

  double mlpk = 4.675920e-01
    +(2.884447e-01) * OUT1;

  return (mlpk-0.45)*3.0;
}; 

//=============================================================================
double NNetTool::MLPkS(double OUT1, double OUT2, double OUT3,
		       double OUT4, double OUT5, double OUT6,
		       double OUT7, double OUT8,
		       double OUT10, double OUT11, double OUT12) { 

  normaliseSS( OUT1,  OUT2,  OUT3, OUT4,  OUT5,  OUT6,
	       OUT7,  OUT8,  OUT10, OUT11,  OUT12 );
  double OUT9 = 0.;

  double RIN1 = 5.042735e-01
    +(-2.163759e-01) * OUT1
    +(7.990202e-01) * OUT2
    +(-1.043584e-01) * OUT3
    +(5.357348e-01) * OUT4
    +(-2.232234e-01) * OUT5
    +(-7.417259e-01) * OUT6
    +(-9.801387e-02) * OUT7
    +(-1.300400e+00) * OUT8
    +(-2.602109e-01) * OUT9
    +(1.778235e-01) * OUT10
    +(3.284732e-02) * OUT11
    +(2.242044e+00) * OUT12;
  double RIN2 = -2.527002e-01
    +(2.858413e-01) * OUT1
    +(2.087666e-01) * OUT2
    +(-7.325422e-01) * OUT3
    +(1.223245e-01) * OUT4
    +(-2.545524e-01) * OUT5
    +(2.126947e+00) * OUT6
    +(5.791655e-01) * OUT7
    +(-4.434437e-01) * OUT8
    +(5.016979e-01) * OUT9
    +(-9.971176e-03) * OUT10
    +(-5.568749e-01) * OUT11
    +(-4.862657e+00) * OUT12;

  OUT1 = SIGMOID(RIN1);
  OUT2 = SIGMOID(RIN2);

  return 5.250314e-01
    +(-6.236218e-01) * OUT1
    +(1.547721e+00) * OUT2;
}; 

//=============================================================================
double NNetTool::MLPpS(double OUT1, double OUT2, double OUT3,
		       double OUT4, double OUT5, double OUT6,
		       double OUT7, double OUT8,
		       double OUT10, double OUT11, double OUT12) { 

  normaliseSS( OUT1,  OUT2,  OUT3, OUT4,  OUT5,  OUT6,
	       OUT7,  OUT8,  OUT10, OUT11,  OUT12 );
  double OUT9 = 0.;

  double RIN1 = 6.777766e-01
    +(6.243266e-02) * OUT1
    +(7.589884e-01) * OUT2
    +(2.262079e-01) * OUT3
    +(3.697471e-02) * OUT4
    +(-1.080178e-01) * OUT5
    +(5.405635e-02) * OUT6
    +(-6.573137e-01) * OUT7
    +(1.128789e-01) * OUT8
    +(2.411596e-01) * OUT9
    +(-5.859959e-01) * OUT10
    +(4.889772e-01) * OUT11
    +(3.739446e+00) * OUT12;
  double RIN2 = -1.564735e+00
    +(-1.479021e-01) * OUT1
    +(-5.512829e-01) * OUT2
    +(-4.911871e-01) * OUT3
    +(1.979046e-02) * OUT4
    +(3.790361e-01) * OUT5
    +(1.360383e+00) * OUT6
    +(3.004154e-01) * OUT7
    +(-1.259163e-01) * OUT8
    +(-1.379657e+00) * OUT9
    +(8.204520e-01) * OUT10
    +(-1.008796e+00) * OUT11
    +(-9.722511e+00) * OUT12;

  OUT1 = SIGMOID(RIN1);
  OUT2 = SIGMOID(RIN2);
 
  return -9.122275e-01
    +(1.538327e+00) * OUT1
    +(3.393626e+00) * OUT2;
};
//=============================================================================
