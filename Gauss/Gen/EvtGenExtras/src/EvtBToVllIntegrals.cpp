#include "EvtGenBase/EvtReport.hh"

#include "EvtGenModels/EvtBToVllIntegrals.hh"
#include "EvtGenModels/EvtBToVllQCDUtils.hh"

#include "gsl/gsl_integration.h"
#include "gsl/gsl_sf_dilog.h"
#include "gsl/gsl_sf_expint.h"
#include "gsl/gsl_sf_gegenbauer.h"
#include "gsl/gsl_sf_result.h"


Integrate::Integrate(const double& _e, const double& _MB, const double& _mK,
		const double& _q2, const WilsonCoefficientsD* const _C_mb,
		const WilsonCoefficientsD* const _C_mb3) :
	e(_e), MB(_MB), mK(_mK), q2(_q2), reg(0), C_mb(_C_mb), C_mb3(_C_mb3),
			Xi(SignedPair<double>(1, _mK/_e)), T0(DataBlock<EvtComplex>(2, 2)) {

	const EvtComplex t0_2_2 = -constants::eq * ((4*MB)/constants::mb)
			* ((*C_mb)(3) + 3*(*C_mb)(4));
	T0.set(1, 1, 0);
	T0.set(1, 2, 0);
	T0.set(2, 1, 0);
	T0.set(2, 2, t0_2_2);
	fK = SignedPair<double>(constants::fKperp, constants::fKpar);
}

SignedPair<EvtComplex> Integrate::getCfac() const{
	const EvtComplex Ceff1_7 = (*C_mb)(7);
	const EvtComplex cfac = Ceff1_7*( 3*Log( (constants::mb*constants::mb)/(constants::mu_mb*constants::mu_mb) ) 
		- 4.0 + (4*constants::muf/constants::mb)  );
	DEBUGPRINT("Ceff1[7]: ", Ceff1_7);//V
	DEBUGPRINT("Cfac[1]: ", cfac);//V
	DEBUGPRINT("Cfac[2]: ", -cfac);//V
	return SignedPair<EvtComplex>(std::make_pair(cfac,-cfac));
}

SignedPair<EvtComplex> Integrate::getC1(const EvtComplex& _F_2_7,
		const EvtComplex& _F_8_7, const EvtComplex& _F_1_9,
		const EvtComplex& _F_2_9, const EvtComplex& _F_8_9) const {
	const double CF_factor = 1/constants::CF;
	const EvtComplex part1 = (*C_mb)(2)*_F_2_7;

	const EvtComplex part2 = (*C_mb)(8)*_F_8_7;
	const EvtComplex part3 = ((*C_mb)(2)*_F_2_9) + (2*(*C_mb)(1)*(_F_1_9 + (1
			/6.)*_F_2_9)) + ((*C_mb)(8)*_F_8_9);
	const EvtComplex result1 = CF_factor*(-part1 - part2 - (q2/(2*constants::mb
			*MB))*part3);
	const EvtComplex result2 = CF_factor*(part1 + part2
			+ (MB/(2*constants::mb))*part3);

	const SignedPair<EvtComplex> Cfac = getCfac();
	return SignedPair<EvtComplex>(std::make_pair(Cfac[1] + result1, Cfac[2] + result2));
}

ComplexPair Integrate::get_t_x(const double u, const double mq) const {
	//Eqns 27, 28
	const double preFact = (2*MB)/((1-u)*e);
	if (mq == 0.0) {
		return std::make_pair(EvtComplex(preFact, 0), EvtComplex(preFact, 0));
	}
	return std::make_pair(get_tt1(u, mq), get_tt2(u, mq));
}

EvtComplex Integrate::get_tt1(const double u, const double mq) const {

	const double MB2 = MB*MB;
	const double mq2 = mq*mq;

	//exported from mathematica
	return chop((2*q2*(Sqrt(-1 + (4*mq2)/q2)* ArcTan(1/Sqrt(-1 + (4*mq2)/q2))
			- Sqrt(-1 + (4*mq2)/(MB2 - MB2*u + q2*u))* ArcTan(1/Sqrt(-1 + (4
					*mq2)/(MB2 - MB2*u + q2*u)))) - (2*e*MB*(-MB2 + q2 + MB2*u
			- q2*u + 2*mq2* Log((-1 + Sqrt(1 - (4*mq2)/q2))/(1 + Sqrt(1 - (4
			*mq2)/q2)))* Log(0.5 - Sqrt(0.25 - mq2/q2)) + 2*mq2*Log((1 + Sqrt(1
			- (4*mq2)/q2))/(-1 + Sqrt(1 - (4*mq2)/q2)))* Log(0.5 + Sqrt(0.25
			- mq2/q2)) - 2*mq2
			*Log(0.5 - Sqrt(0.25 + mq2/(MB2*(-1 + u) - q2*u)))* Log((-1 + 2
			*Sqrt(0.25 + mq2/(MB2*(-1 + u) - q2*u)))/(1 + 2*Sqrt(0.25 + mq2
			/(MB2*(-1 + u) - q2*u)))) - 2*mq2*Log(0.5 + Sqrt(0.25 + mq2/(MB2
			*(-1 + u) - q2*u)))* Log((1 + 2*Sqrt(0.25 + mq2/(MB2*(-1 + u) - q2
			*u)))/(-1 + 2*Sqrt(0.25 + mq2/(MB2*(-1 + u) - q2*u)))) +

	2*mq2*PolyLog(2, (-1 + Sqrt(1 - (4*mq2)/q2))/(1 + Sqrt(1 - (4*mq2)/q2)))
			+ 2*mq2*PolyLog(2, (1 + Sqrt(1 - (4*mq2)/q2))/(-1 + Sqrt(1
					- (4*mq2)/q2))) - 2*mq2*PolyLog(2, (-1 + 2*Sqrt(0.25 + mq2
			/(MB2*(-1 + u) - q2*u)))/(1 + 2*Sqrt(0.25 + mq2/(MB2*(-1 + u) - q2
			*u)))) - 2*mq2*PolyLog(2, (1 + 2*Sqrt(0.25 + mq2/(MB2*(-1 + u) - q2
			*u)))/(-1 + 2*Sqrt(0.25 + mq2/(MB2*(-1 + u) - q2*u))))))
			/ (MB2 - q2))/Power(e - e*u, 2));
}

EvtComplex Integrate::get_tt2(const double u, const double mq) const {

	const double MB2 = MB*MB;
	const double mq2 = mq*mq;

	//exported from mathematica
	return chop(((-(MB2*(-1 + u)) + q2*u)*(2*Sqrt(-1 + (4*mq2)/q2)*ArcTan(1
			/Sqrt(-1 + (4*mq2)/q2)) - 2*Sqrt(-1 + (4*mq2)/(MB2 - MB2*u + q2*u))
			* ArcTan(1/Sqrt(-1 + (4*mq2)/(MB2 - MB2*u + q2*u)))) - (2*e*MB
			*(-MB2 + q2 + MB2*u - q2*u + 2*mq2*Log((-1 + Sqrt(1 - (4*mq2)/q2))
					/(1 + Sqrt(1 - (4*mq2)/q2)))
					* Log(0.5 - Sqrt(0.25 - mq2/q2)) + 2*mq2*Log((1 + Sqrt(1
					- (4*mq2)/q2))/(-1 + Sqrt(1 - (4*mq2)/q2)))* Log(0.5
					+ Sqrt(0.25 - mq2/q2)) - 2*mq2*Log(0.5 - Sqrt(0.25 + mq2
					/(MB2*(-1 + u) - q2*u)))* Log((-1 + 2*Sqrt(0.25 + mq2/(MB2
					*(-1 + u) - q2*u)))/ (1 + 2*Sqrt(0.25 + mq2/(MB2*(-1 + u)
					- q2*u)))) - 2*mq2*Log(0.5 + Sqrt(0.25 + mq2/(MB2*(-1 + u)
					- q2*u)))* Log((1 + 2
					*Sqrt(0.25 + mq2/(MB2*(-1 + u) - q2*u)))/ (-1 + 2*Sqrt(0.25
					+ mq2/(MB2*(-1 + u) - q2*u)))) + 2*mq2*PolyLog(2, (-1
					+ Sqrt(1 - (4*mq2)/q2))/(1 + Sqrt(1 - (4*mq2)/q2))) + 2*mq2
					*PolyLog(2, (1 + Sqrt(1 - (4*mq2)/q2))/(-1 + Sqrt(1 - (4
							*mq2)/q2))) - 2*mq2*PolyLog(2, (-1 + 2*Sqrt(0.25
					+ mq2/(MB2*(-1 + u) - q2*u)))/ (1 + 2*Sqrt(0.25 + mq2/(MB2
					*(-1 + u) - q2*u)))) - 2*mq2*PolyLog(2, (1 + 2*Sqrt(0.25
					+ mq2/(MB2*(-1 + u) - q2*u)))/ (-1 + 2*Sqrt(0.25 + mq2/(MB2
					*(-1 + u) - q2*u))))))/(MB2 - q2))/Power(e - e*u, 2));

}

DataBlock<EvtComplex> Integrate::getTnf(const double& u) const {
	//Eqns 23, 24, 25, 26 of Beneke

	ComplexPair tt_0 = get_t_x(u, 0.0);
	ComplexPair tt_mc = get_t_x(u, constants::mc);
	ComplexPair tt_mb = get_t_x(u, constants::mb);

	const EvtComplex tnf_1_1 = chop(-4*constants::ed*((*C_mb3)(8)/(u + (((1-u)
			*q2)/(MB*MB)))) + (MB/(2*constants::mb))*( (constants::eu
			*tt_mc.first*((*C_mb3)(2) + (*C_mb3)(4) - (*C_mb3)(6)))
			+ (constants::ed*tt_mb.first*((*C_mb3)(3) + (*C_mb3)(4)
					- (*C_mb3)(6) - ((4*constants::mb/MB)*(*C_mb3)(5))))
			+ (constants::ed*tt_0.first * (*C_mb3)(3))));

	const EvtComplex tnf_1_2 =
			chop((MB/constants::mb)*( ((constants::eu * tt_mc.second)
					*((*C_mb3)(2) + (*C_mb3)(4) - (*C_mb3)(6)))
					+ ((constants::ed*tt_mb.second)*((*C_mb3)(3) + (*C_mb3)(4)
							- (*C_mb3)(6))) + (constants::ed*tt_0.second
					*(*C_mb3)(3)) ));

	const double ub = 1 - u;

	const EvtComplex h_mc = qcd::h((ub*MB*MB) + (u*q2), constants::mc,
			C_mb3->getScaleValue());
	const EvtComplex h_mb = qcd::h((ub*MB*MB) + (u*q2), constants::mb,
			C_mb3->getScaleValue());
	const EvtComplex h_0 = qcd::h((ub*MB*MB) + (u*q2), 0, C_mb3->getScaleValue());

	const EvtComplex tnf_2_2 = chop(constants::eq * ( ((8*(*C_mb3)(8))/(ub
			+ ((u*q2)/(MB*MB)))) + ( (6*MB/constants::mb)*( (h_mc*((*C_mb3)(2)
			+ (*C_mb3)(4) + (*C_mb3)(6))) + (h_mb*((*C_mb3)(3) + (*C_mb3)(4)
			+ (*C_mb3)(6))) + (h_0
			*((*C_mb3)(3) + 3*(*C_mb3)(4) + 3*(*C_mb3)(6))) - ((8/27.)
			*((*C_mb3)(3) - (*C_mb3)(5) - 15*(*C_mb3)(6)))))));

	DataBlock<EvtComplex> Tnf(2, 2);
	Tnf.set(1, 1, tnf_1_1);
	Tnf.set(1, 2, tnf_1_2);
	Tnf.set(2, 1, 0);
	Tnf.set(2, 2, tnf_2_2);
	return Tnf;

}

DataBlock<EvtComplex> Integrate::getT1(const double u) const{
	return getTnf(u);
}

//Gegenbauer moment
double Integrate::amom(const int& n) {
	return 1/pow( (0.5*n + 1), 3);
}

//Integrated LCDA of B for + and - signs
double Integrate::PhiK_1(const double& u) {
	//Eqn 48			
	const double amom1 = 0.03;
	const double amom2 = 0.08;
	
	const double tmp = -1 + 2*u;
	const double result = 6*(1 - u)*u*(1 + 3*(-1 + 2*u)*amom1 + 
	     (-1.5 + (15*tmp*tmp)/2.)*amom2);
	return result;
}

//Integrated LCDA of B for + and - signs
double Integrate::PhiK_2(const double& u) {
	//Eqn 48			
	const double amom1 = 0.02;
	const double amom2 = 0.08;
	
	const double tmp = -1 + 2*u;
	const double result  = 6*(1 - u)*u*(1 + 3*(tmp)*amom1 + 
		     (-1.5 + (15*tmp*tmp)/2.)*amom2);
	return result;
}

SignedPair<EvtComplex> Integrate::PhiK(const double& u){
	SignedPair<EvtComplex> result(std::make_pair(PhiK_1(u),PhiK_2(u)));
	return result;
}

ComplexPair Integrate::PhiBint() const {
	//see Eqn 54
	const double w0 = constants::LambdaB;
	const EvtComplex PhiBint_1(1/w0);
		EvtComplex PhiBint_2(-gsl_sf_expint_Ei(q2/(MB*w0)), constants::Pi);
	PhiBint_2 *= (pow(constants::E, (-q2/(MB*w0)))/w0);
	return std::pair<const EvtComplex,const EvtComplex>(PhiBint_1,PhiBint_2);
}

EvtComplex Integrate::getT(const int& sign, const int a, const double& u) const{
	DataBlock<EvtComplex> kT0 = ((qcd::alpha_s(C_mb3->getScaleValue(), 5)/(4
			*constants::Pi))*constants::CF*getT1(u));
	return T0(sign, a) + kT0(sign, a);
}

//form dictated by gsl_function
double Integrate::integralFunction1(double u, void* p) {
	struct int_params* params = (struct int_params*)p;

	const int a = params->a;
	const bool re = params->real;
	Integrate cal(params->e, params->MB, params->mK, params->q2, params->C_mb,
			params->C_mb3);
	EvtComplex cResult = cal.PhiK_1(u) * cal.getT(1, a, u) * cal.PhiBint().first;

	double result = 0.0;
	//from playing about in mathematica, it seems I can integrate the Re and Im parts seperately
	if (re) {
		result = real(cResult);
	} else {
		result = imag(cResult);
	}
	return result;
}

//form dictated by gsl_function
double Integrate::integralFunction2(double u, void* p) {
	struct int_params* params = (struct int_params*)p;

	const int a = params->a;
	const bool re = params->real;
	Integrate cal(params->e, params->MB, params->mK, params->q2, params->C_mb,
			params->C_mb3);
	EvtComplex cResult = cal.PhiK_2(u) * cal.getT(2, a, u) * cal.PhiBint().second;

	double result = 0.0;
	//from playing about in mathematica, it seems I can intregrate the Re and Im parts seperately
	if (re) {
		result = real(cResult);
	} else {
		result = imag(cResult);
	}
	return result;
}

EvtComplex Integrate::get_t(const int& a, const SignedPair<double>& xi,
		const SignedPair<EvtComplex>& C1) const {

	gsl_function re_one;
	re_one.function = &Integrate::integralFunction1;
	int_params re_one_params(a, e, MB, mK, q2, true, C_mb, C_mb3);
	re_one.params = &re_one_params;

	gsl_function im_one;
	im_one.function = &Integrate::integralFunction1;
	int_params im_one_params(a, e, MB, mK, q2, false, C_mb, C_mb3);
	im_one.params = &im_one_params;

	gsl_function re_two;
	re_two.function = &Integrate::integralFunction2;
	int_params re_two_params(a, e, MB, mK, q2, true, C_mb, C_mb3);
	re_two.params = &re_two_params;

	gsl_function im_two;
	im_two.function = &Integrate::integralFunction2;
	int_params im_two_params(a, e, MB, mK, q2, false, C_mb, C_mb3);
	im_two.params = &im_two_params;

	const EvtComplex one(doIntegral(re_one), doIntegral(im_one));
	const EvtComplex two(doIntegral(re_two), doIntegral(im_two));
	
	DEBUGPRINT("one: ", one);
	DEBUGPRINT("two: ", two);

	return constants::CF*qcd::as1(C_mb->getScaleValue(), 5)*C1[a]*xi[a] + 
	   ((constants::fB*constants::Pi*constants::Pi*fK[a]*Xi[a])/(MB*constants::Nc))*(one + two);
}

double Integrate::doIntegral(const gsl_function& F) const{
	/** This function actually calls the GSL integration routines.*/

	double result = 0;
	double error = 0;

	const int nDivisions = 100;
	const double accuracyGoal = 1e-4;

	gsl_integration_workspace* w = gsl_integration_workspace_alloc(nDivisions);

	const int intCode = gsl_integration_qag(&F, 0, 1, 0, accuracyGoal,
			nDivisions, GSL_INTEG_GAUSS61, w, &result, &error);
	if (intCode != 0) {
		report(WARNING,"EvtGen") << "Numerical integration did not return cleanly."
				<< std::endl;
		report(WARNING,"EvtGen") << "Return code of the integration was " << intCode
				<< std::endl;
		report(WARNING,"EvtGen") << "Result: " << result << " Error: " << error << std::endl;
	}
	gsl_integration_workspace_free(w);
	return result;

}

IntegrateRight::IntegrateRight(const double& _e, const double& _MB,
		const double& _mK, const double& _q2,
		const WilsonCoefficientsD* const _C_mb,
		const WilsonCoefficientsD* const _C_mb3) :
	Integrate::Integrate(_e, _MB, _mK, _q2, _C_mb, _C_mb3) {
}
/*
 * The following functions are over-ridden to remove the dependence
 * on C(1-6) explicityly. This is needed as the users is allowed to set
 * them, and non-zero values will mess things up. It might be a bit faster
 * also.
 */
DataBlock<EvtComplex> IntegrateRight::getTnf(const double& u) const {

	const double ub = 1 - u;
	const EvtComplex tnf_1_1 = chop(-4*constants::ed*((*C_mb3)(8)/(u + (((1-u)
			*q2)/(MB*MB)))));
	const EvtComplex tnf_2_2 = chop(constants::eq*( ((8*(*C_mb3)(8))/(ub + ((u
			*q2)/(MB*MB))))));

	DataBlock<EvtComplex> Tnf(2, 2);
	Tnf.set(1, 1, tnf_1_1);
	Tnf.set(1, 2, 0.0);
	Tnf.set(2, 1, 0.0);
	Tnf.set(2, 2, tnf_2_2);
	return Tnf;
}

SignedPair<EvtComplex> IntegrateRight::getC1(const EvtComplex& /*_F_2_7*/,
		const EvtComplex& _F_8_7, const EvtComplex& /*_F_1_9*/,
		const EvtComplex& /*_F_2_9*/, const EvtComplex& _F_8_9) const {
	const double CF_factor = 1/constants::CF;
	const EvtComplex part2 = (*C_mb)(8)*_F_8_7;
	const EvtComplex part3 = ((*C_mb)(8)*_F_8_9);
	const EvtComplex result1 = CF_factor*(-part2 - (q2/(2*constants::mb*MB))
			*part3);
	const EvtComplex result2 = CF_factor*(part2 + (MB/(2*constants::mb))*part3);
	return SignedPair<EvtComplex>(std::make_pair(result1, result2));
}


EvtComplex IntegrateRight::getT(const int& sign, const int a, const double& u) const{
	DataBlock<EvtComplex> kT0 = ((qcd::alpha_s(C_mb3->getScaleValue(), 5)/(4
			*constants::Pi))*constants::CF*getT1(u));
	return kT0(sign, a);
}

