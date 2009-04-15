#ifndef EVTBTOVLLCONSTRAINTS_HH_
#define EVTBTOVLLCONSTRAINTS_HH_

#include "EvtGenModels/EvtBToKstarllQCDFactorisation.hh"

class EvtBToVllConstraints{

public:	
	
	EvtBToVllConstraints(const QCDFactorisation& _fact):
		fact(_fact){
	}
	double findAFBZero() const;
	double getDstar(const double q2, const std::vector<EvtComplex>& tensors) const;
	double getGammaKstar(const double q2) const;
	

private:

	const QCDFactorisation& fact;
	
	double getAFB(const double q2) const;
	
};

#endif /*EVTBTOVLLCONSTRAINTS_HH_*/
