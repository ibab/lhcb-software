#include "EvtGenModels/EvtBToVllConstants.hh"
#include "EvtGenBase/EvtMathematicaFn.hh"
#include "EvtGenModels/EvtBToVllQCDUtils.hh"

//general
const double constants::E = 2.7182818284590452354;
const double constants::Pi = 3.1415926535897932385;

//physics
const double constants::alphaEM = 1/137.0;
const double constants::alphaMZ = 0.1176;
const double constants::CF = 4/3.;
const double constants::ed = -1/3.;
const double constants::eq = -2/3.;
const double constants::eu = 2/3.;
const double constants::fB = 0.200;
const double constants::fKpar = 0.22;//GeV
const double constants::fKperp = 0.163;//GeV
const double constants::Lqcd = 0.220;//GeV +-0.04 -> nF = 5
const int constants::Nc = 3;
const double constants::mB = 5.27953;//GeV
const double constants::mb = 4.52;//GeV
const double constants::mbp = qcd::mb_pole(constants::mb);
const double constants::mc = 1.6;//GeV
const double constants::mme = 0.000511;//GeV
const double constants::mmu = 0.10566;//GeV
const double constants::mKstar = 0.896;//GeV
const double constants::ms = 0.104;//GeV
const double constants::mt = 162.3;
const double constants::mu_h = 2.2;//GeV
const double constants::mu_mw = 80.4;//GeV
const double constants::MW =  80.4;//GeV
const double constants::MZ =  91.1876;//GeV
const double constants::sw2 = 0.23;//Weak mixing angle
const double constants::muf = 2.0;
const double constants::LambdaB = 0.51;
//used in the Constraints
const double constants::fBs = 0.268;
const double constants::mBs = 5.3663;
const double constants::pstoinvGev = 1e-12/6.582119e-25;
const double constants::GF = 1.16637e-5;

const double constants::Vus = 0.2255;
const double constants::Vcb = 4.12e-2;
const double constants::AbsVub = 3.93e-3;

const double constants::mu_mb = constants::mb;
const double constants::gamma = (77*constants::Pi)/180.;
const double constants::tauBs = 1.425*constants::pstoinvGev;

const EvtComplex constants::I = Complex(0.0,1.0);

const EvtComplex constants::Vub = constants::AbsVub*Exp(-constants::I*constants::gamma);
const EvtComplex constants::Vts = -((AbsVub*Exp(constants::I*constants::gamma)*
        Sqrt(1 - Power(constants::Vcb,2)/(1 - Power(constants::AbsVub,2)))*constants::Vus)/
      Sqrt(1 - Power(constants::AbsVub,2))) - 
      (constants::Vcb*Sqrt(1 - Power(constants::Vus,2)/(1 - Power(constants::AbsVub,2))))/
      Sqrt(1 - Power(constants::AbsVub,2));
const EvtComplex constants::Vtb = Sqrt(1 - Power(constants::AbsVub,2))*Sqrt(1 - Power(constants::Vcb,2)/(1 - Power(constants::AbsVub,2)));
const EvtComplex constants::Vtstb = Vts*Vtb;
