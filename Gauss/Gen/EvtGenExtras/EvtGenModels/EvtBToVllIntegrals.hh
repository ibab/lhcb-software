#ifndef EVTBTOVLLINTEGRALS_HH_
#define EVTBTOVLLINTEGRALS_HH_

#include "EvtGenBase/EvtMathematicaFn.hh"

#include "EvtGenModels/EvtBToVllConstants.hh"
#include "EvtGenModels/EvtBToKstarllQCDFactorisation.hh"
#include "EvtGenModels/EvtBToVllWC.hh"

#include "gsl/gsl_math.h"

struct int_params {

public:

	const int a;
	const double e;
	const double MB;
	const double mK;
	const double q2;
	const bool real;

	const WilsonCoefficientsD* const C_mb;
	const WilsonCoefficientsD* const C_mb3;

	int_params(const int& _a, const double& _e, const double& _MB,
			const double& _mK, const double& _q2, const bool& _real,
			const WilsonCoefficientsD* const _C_mb,
			const WilsonCoefficientsD* const _C_mb3) :
		a(_a), e(_e), MB(_MB), mK(_mK), q2(_q2), real(_real), C_mb(_C_mb),
				C_mb3(_C_mb3) {
	};

};

class Integrate {

public:

	Integrate(const double& _e, const double& _MB, const double& _mK,
			const double& _q2, const WilsonCoefficientsD* const _C_mb,
			const WilsonCoefficientsD* const _C_mb3);
	virtual ~Integrate() {
	}
	;

	virtual SignedPair<EvtComplex> getCfac() const;
	virtual SignedPair<EvtComplex> getC1(const EvtComplex& _F_2_7,
			const EvtComplex& _F_8_7, const EvtComplex& _F_1_9,
			const EvtComplex& _F_2_9, const EvtComplex& _F_8_9) const;

	ComplexPair get_t_x(const double u, const double mq) const;
	EvtComplex get_tt1(const double u, const double mq) const;
	EvtComplex get_tt2(const double u, const double mq) const;
	virtual DataBlock<EvtComplex> getTnf(const double& u) const;

	DataBlock<EvtComplex> getT1(const double u) const;

	//Gegenbauer moment
	static double amom(const int& n);
	//Integrated LCDA of B for + and - signs
	static double PhiK_1(const double& u);
	static double PhiK_2(const double& u);
	static SignedPair<EvtComplex> PhiK(const double& u);
	ComplexPair PhiBint() const;
	virtual EvtComplex getT(const int& sign, const int a, const double& u) const;

	//form dictated by gsl_function
	static double integralFunction1(double u, void* p);
	//form dictated by gsl_function
	static double integralFunction2(double u, void* p);

	EvtComplex get_t(const int& a, const SignedPair<double>& xi,
			const SignedPair<EvtComplex>& C1) const;
	double doIntegral(const gsl_function& F) const;

protected:

	const double e;
	const double MB;
	const double mK;
	const double q2;
	const double reg;

	const WilsonCoefficientsD* const C_mb;
	const WilsonCoefficientsD* const C_mb3;

	const SignedPair<double> Xi;
	DataBlock<EvtComplex> T0;
	SignedPair<double> fK;

};

//class performs integrations for the right handed operators
class IntegrateRight : public Integrate {

public:
	IntegrateRight(const double& _e, const double& _MB, const double& _mK,
			const double& _q2, const WilsonCoefficientsD* const _C_mb,
			const WilsonCoefficientsD* const _C_mb3);
	/*
	 * The following functions are over-ridden to remove the dependence
	 * on C(1-6) explicityly. This is needed as the users is allowed to set
	 * them, and non-zero values will mess things up. It might be a bit faster
	 * also.
	 */
	virtual DataBlock<EvtComplex> getTnf(const double& u) const;
	virtual SignedPair<EvtComplex> getC1(const EvtComplex& /*_F_2_7*/,
			const EvtComplex& _F_8_7, const EvtComplex& /*_F_1_9*/,
			const EvtComplex& /*_F_2_9*/, const EvtComplex& _F_8_9) const;
	virtual EvtComplex getT(const int& sign, const int a, const double& u) const;

};

#endif /*EVTBTOVLLINTEGRALS_HH_*/
