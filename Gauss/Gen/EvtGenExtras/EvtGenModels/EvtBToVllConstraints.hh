#ifndef EVTBTOVLLCONSTRAINTS_HH_
#define EVTBTOVLLCONSTRAINTS_HH_

#include "EvtGenModels/EvtBToKstarllQCDFactorisation.hh"
#include "EvtGenBase/EvtMathematicaFn.hh"

#include "gsl/gsl_errno.h"
#include "gsl/gsl_math.h"

#include <utility>

extern int GSL_ERROR_HANDLER_CALLED;//nastly global variable
void debug_handler(const char* reason, const char* file, int line, int gsl_errno);

struct EvtBdToXsllParameters{
public:
	//values used just for this calculation
	static const double mbp;
	static const double muX;
};


class EvtBToVllConstraints{

public:	
	
	enum SpinAmplitudes {A0L = 0, A0R, APL, APR, ATL, ATR, AT, AS, NUMBER_OF_AMPS};
	
	EvtBToVllConstraints(const QCDFactorisation& _fact);
	double getDstar(const double q2, const std::vector<EvtComplex>& tensors) const;
	double getGammaKstar(const double q2) const;
	double getBrBsToMuMu() const;
	double getBrBToXsGamma() const;
	double getBrBToXsll() const;
	double getSBToKStarGamma() const;
	
	void getSpinAmplitudes(const double q2, std::vector<EvtComplex>* amps, const bool isBbar, const double _scaleFactor = 1e12) const;
	const std::pair<double, double> getS4Zero() const;
	const std::pair<double, double> getS5Zero() const;
	const std::pair<double, double> getS6Zero() const;
	double getAFBIntegral() const;
	double getFLIntegral() const;
	double getS5Integral() const;

private:

	const QCDFactorisation& fact;
	
	EvtComplex Cb1_1;
	EvtComplex Cb1_2;
	EvtComplex Cb1_3;
	EvtComplex Cb1_4;
	EvtComplex Cb1_5;
	EvtComplex Cb1_6;
	//
	EvtComplex C7sm;
	EvtComplex C8sm;
	EvtComplex C9sm;
	EvtComplex C10sm;
	//
	EvtComplex Ceff1_7;
	EvtComplex Ceff1_8;
	EvtComplex Cb1_9;
	EvtComplex Cb1_10;
	//
	EvtComplex Ceffp1_7;
	EvtComplex Ceffp1_8;
	EvtComplex Cbp1_9;
	EvtComplex Cbp1_10;
	//
	EvtComplex C7NP;
	EvtComplex C8NP;
	EvtComplex C9NP;
	EvtComplex C10NP;
	//
	EvtComplex C7R;
	//
	EvtComplex CQ1;
	EvtComplex CQ2;
	
	const double q2min;
	const double q2max;
	
	//useful functions for calculating the K* spin amplitudes
	inline double getBeta(const double q2) const;
	inline double getLambda(const double q2) const;
	inline EvtComplex getN(const double q2, const double beta, const double lambda) const;
	
	double getJ4(const double q2) const;
	double getJ5(const double q2) const;
	double getJ6(const double q2) const;
	double getAFB(const double q2) const;
	double getFL(const double q2) const;
	double getGamma(const double q2) const;
	
	const std::pair<double, double> findZero(gsl_function* F) const;
	double findZeroGradient(gsl_function* F, const double zero) const;
	double findRateIntegral(gsl_function* F, const double q2min, const double q2max) const;
	
};

#endif /*EVTBTOVLLCONSTRAINTS_HH_*/
