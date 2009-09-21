#include "EvtGenBase/EvtMathematicaFn.hh"
#include "EvtGenModels/EvtBToVllQCDUtils.hh"

#include <cmath>
#include <ctime>
#include <iostream>

using qcd::alpha_s;
using qcd::B0;
using qcd::B1;
using qcd::B2;
using qcd::h;
using qcd::Y;

double qcd::B0(const int& nflav) {
	return 11 - (2*nflav)/3.0;
}

double qcd::B1(const int& nflav) {
	return 2*(51 - (19/3.0)*nflav);
}

double qcd::B2(const int& nflav) {
	return (2857 - (5033/9.0)*nflav + (325/27.0)*nflav*nflav);
}

double qcd::alpha_s(const double& mu1, const int& nflav) {
	//We assume that alpha_s is always real, which is not true
	//mathematically, but I guess must be physically

	const double _B0 = qcd::B0(nflav);
	const double _B1 = qcd::B1(nflav);
	const double _B2 = qcd::B2(nflav);
	const double lnF = log((mu1*mu1)/(constants::Lqcd*constants::Lqcd));

	return (4*constants::Pi)/(_B0*lnF) * (1 - (_B1*log(lnF))/(_B0*_B0*lnF)
			+ ((_B1*_B1) /(pow(_B0, 4)*lnF*lnF))* (pow((log(lnF) - 0.5), 2)
					+ (_B0*_B2)/(2*_B1 *_B1) - 1.25) );
}

double qcd::as1(const double mu, const int nflav){
	return alpha_s(mu, nflav)/(4*constants::Pi);
}

double qcd::mb_pole(const double mb, const double scale){
	const double _scale = (scale < 0) ? mb : scale;//use mb for unphysical negative scales
	//see eqn 45 of Beneke
	return mb + ((4*alpha_s(_scale,5))/(3*constants::Pi)*constants::muf);
}

EvtComplex qcd::h(const double& s, const double& mq, const double& mu){

	//eqn 11 of hep-ph/0106067
	const double z = (4*mq*mq)/s;//V
	EvtComplex result = 0;
	
	if(fabs(0.0 - mq) < 1e-6){//compare delta rather than number directly
		result = (8/27.) - (4/9.)*(Log(s/(mu*mu)) - EvtComplex(0,constants::Pi));
		return result;
	}
	const EvtComplex prefactor1 = -(4/9.)*(Log((mq*mq)/(mu*mu)) - (2/3.) - z);
	const EvtComplex prefactor2 = ((4/9.)*(2+z)*Sqrt(fabs(z-1)));
	if(z > 1){
		result = prefactor1 - (prefactor2 * ArcTan(1/Sqrt(z - 1)));
	}else if(z <= 1){
		result = prefactor1 - (prefactor2 * ((Log(((1 + Sqrt(1-z))/Sqrt(z))) - EvtComplex(0,0.5*constants::Pi))));
	}
	return result;
	
}

//Used for C9_eff
EvtComplex qcd::Y(const double& s, const WilsonCoefficients<EvtComplex>& C, const double mb, const double mc){
	const double mu = C.getScaleValue();
	//eqn 10 of hep-ph/0106067
	const double mbp = mb_pole(mb);
	return (h(s,mc, mu)*( (3*C(1)) + C(2) + (3*C(3)) + C(4) + (3*C(5)) + C(6))) - 
		(0.5*h(s,mbp,mu)*((4*(C(3) + C(4))) + (3*C(5)) + C(6))) - (0.5*h(s,0.0,mu)*(C(3) + (3*C(4)))) + 
		((2/9.)*(((2/3.)*C(3)) + (2*C(4)) + ((16/3.)*C(5)) ));
	
}

//Used for C9,C10. Should be optimised by the compiler
double qcd::xt(const int& nfl){
	const double gam0 = 6*constants::CF;
	const double gam1 = constants::CF*(3*constants::CF + (97/3.)*constants::Nc - (10/3.)*nfl);
	
	const double xt = ((constants::mt*constants::mt)*pow(alpha_s(constants::MW,nfl)/alpha_s(constants::mt,nfl),gam0/B0(5))*
			pow(1 + ((-alpha_s(constants::mt,nfl) + alpha_s(constants::MW,nfl))*
					(gam1/(2.*B0(nfl)) - (gam0*B1(nfl))/(2.*pow(B0(nfl),2))))/(4.*constants::Pi),2))/(constants::MW*constants::MW);
	return xt;
}

void qcd::printTime(const std::string& msg){
	time_t tm = time(NULL);
	std::cout << msg << ": " << ctime(&tm);
}

