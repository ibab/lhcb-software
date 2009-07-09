#ifndef EVTBTOVLLCONSTANTS_H_
#define EVTBTOVLLCONSTANTS_H_

//#define WRDEBUG
#ifdef WRDEBUG
#define DEBUGPRINT(L,W) std::cout << __FILE__ << ":" << __LINE__ << " " <<(L) << (W) << std::endl
#else
#define DEBUGPRINT(L,W)
#endif

/**
 * These input constants are taken from Table 2 of
 * M. Beneke, Th. Feldmann and D. Seidel. hep-ph/0106067v2
 * and also Altmannshoffer et al, hep-ph/0811.1214.
 */

namespace constants{

	//general
	static const double E = 2.7182818284590452354;
	static const double Pi = 3.1415926535897932385;

	//physics
	static const double alphaEM = 1/137.0;
	static const double alphaMZ = 0.1176;
	static const double CF = 4/3.;
	static const double ed = -1/3.;
	static const double eq = -2/3.;
	static const double eu = 2/3.;
	static const double fB = 0.200;
	static const double fKpar = 0.22;//GeV
	static const double fKperp = 0.163;//GeV
	static const double Lqcd = 0.220;//GeV +-0.04 -> nF = 5
	static const int Nc = 3;
	static const double mB = 5.27953;//GeV
	static const double mb = 4.52;//GeV
	static const double mc = 1.75;//GeV
	static const double mmu = 0.10566;//GeV
	static const double mKstar = 0.896;//GeV
	static const double ms = 0.104;//GeV
	static const double mt = 162.3;
	static const double mu_h = 2.2;//GeV
	static const double mu_mb = mb;//GeV
	static const double mu_mw = 80.4;//GeV
	static const double MW =  80.4;//GeV
	static const double MZ =  91.1876;//GeV
	static const double sw2 = 0.23;//Weak mixing angle
	static const double Vts = -0.0416310;//+-0.003
	static const double Vus = 0.2258;
	static const double Vcb = 4.12e-2;
	static const double AbsVub = 3.93e-3;
	static const double Vtstb = -0.0407;
	static const double muf = 2.0;
	static const double LambdaB = 0.51;
	//used in the Constraints
	static const double fBs = 0.268;
	static const double mBs = 5.3663;
	static const double pstoinvGev = 1e-12/6.582119e-25;
	static const double tauBs = 1.425*pstoinvGev;
	static const double gamma = (70*Pi)/180.;
	static const double GF = 1.16637e-5;
	
	

}

#endif /*EVTBTOVLLCONSTANTS_H_*/
