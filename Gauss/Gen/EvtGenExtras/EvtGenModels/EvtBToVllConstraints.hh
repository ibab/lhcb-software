#ifndef EVTBTOVLLCONSTRAINTS_HH_
#define EVTBTOVLLCONSTRAINTS_HH_

#include "EvtGenModels/EvtBToKstarllQCDFactorisation.hh"
#include "EvtGenBase/EvtMathematicaFn.hh"

class EvtBToVllConstraints{

public:	
	
	EvtBToVllConstraints(const QCDFactorisation& _fact);
	double findAFBZero() const;
	double getDstar(const double q2, const std::vector<EvtComplex>& tensors) const;
	double getGammaKstar(const double q2) const;
	double getBrBsToMuMu() const;
	double getBrBToXsGamma() const;
	double getBrBToXsll() const;
	

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
	EvtComplex CQ1;
	EvtComplex CQ2;
	//
	EvtComplex Vub;
	EvtComplex Vts;
	EvtComplex Vtb;
	
	
	double getAFB(const double q2) const;
	
};

#endif /*EVTBTOVLLCONSTRAINTS_HH_*/
