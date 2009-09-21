#ifndef EVTBTOVLLCONSTANTS_H_
#define EVTBTOVLLCONSTANTS_H_

//#define WRDEBUG
#ifdef WRDEBUG
#define DEBUGPRINT(L,W) std::cout << __FILE__ << ":" << __LINE__ << " " <<(L) << (W) << std::endl
#else
#define DEBUGPRINT(L,W)
#endif

#include "EvtGenBase/EvtComplex.hh"

/**
 * These input constants are taken from Table 2 of
 * M. Beneke, Th. Feldmann and D. Seidel. hep-ph/0106067v2
 * and also Altmannshoffer et al, hep-ph/0811.1214.
 */

class constants{
public:
	//general
	static const double E;
	static const double Pi;

	//physics
	static const double alphaEM;
	static const double alphaMZ;
	static const double CF;
	static const double ed;
	static const double eq;
	static const double eu;
	static const double fB;
	static const double fKpar;//GeV
	static const double fKperp;//GeV
	static const double Lqcd;//GeV +-0.04 -> nF = 5
	static const int Nc;
	static const double mB;//GeV
	static const double mb;//GeV
	static const double mbp;//GeV
	static const double mc;//GeV
	static const double mme;//GeV
	static const double mmu;//GeV
	static const double mKstar;//GeV
	static const double ms;//GeV
	static const double mt;
	static const double mu_h;//GeV
	static const double mu_mb;//GeV
	static const double mu_mw;//GeV
	static const double MW;//GeV
	static const double MZ;//GeV
	static const double sw2;//Weak mixing angle
	static const double muf;
	static const double LambdaB;
	//used in the Constraints
	static const double fBs;
	static const double mBs;
	static const double pstoinvGev;
	static const double tauBs;
	static const double gamma;
	static const double GF;

	static const double Vus;
	static const double Vcb;
	static const double AbsVub;
	static const EvtComplex I;
	static const EvtComplex Vub;
	static const EvtComplex Vts;
	static const EvtComplex Vtb;
	static const EvtComplex Vtstb;

};

#endif /*EVTBTOVLLCONSTANTS_H_*/
