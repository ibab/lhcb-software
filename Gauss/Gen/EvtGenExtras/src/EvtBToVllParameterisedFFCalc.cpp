#include "EvtGenModels/EvtBToVllParameterisedFFCalc.hh"
#include <cmath>
#include <iostream>

//The base class functions...
double bdkszmm::FFCalc::getV(const double q2) const{
	return calc(getCoeffs(V),q2);
}
double bdkszmm::FFCalc::getA0(const double q2) const{
	return calc(getCoeffs(A0),q2);
}
double bdkszmm::FFCalc::getA1(const double q2) const{
	return calc(getCoeffs(A1),q2);
}
double bdkszmm::FFCalc::getA2(const double q2) const{
	return calc(getCoeffs(A2),q2);
}
double bdkszmm::FFCalc::getT1(const double q2) const{
	return calc(getCoeffs(T1),q2);
}
double bdkszmm::FFCalc::getT2(const double q2) const{
	return calc(getCoeffs(T2),q2);
}
double bdkszmm::FFCalc::getT3(const double q2) const{
	return calc(getCoeffs(T3),q2);
}

//numbers extracted from a fit to the full calc
const double bdkszmm::Ball07Private::_VC[7] = {0.308613037 , 1.24434076 , 0.781283233 , -0.106459915 , -0.157785631 , 0.996507118 , 2.21715496 };
const double bdkszmm::Ball07Private::_A0C[7] = {0.348247688 , 1.31444723 , 0.789307752 , -0.0701231434 , -0.160690431 , 0.810712337 , 2.10226014 };
const double bdkszmm::Ball07Private::_A1C[7] = {0.233722582 , 0.370589260 , 0.278847031 , -0.117717326 , -0.341083520 , 0.315868724 , 3.06659841 };
const double bdkszmm::Ball07Private::_A2C[7] = {0.193409879 , 0.889395292 , 0.441308446 , -0.375224251 , -0.304719786 , 1.42102900 , 3.38962482 };
const double bdkszmm::Ball07Private::_T1C[7] = {0.267385259 , 1.26459589 , 0.816772940 , -0.154005979 , -0.188955734 , 1.14740143 , 1.94079694 };
const double bdkszmm::Ball07Private::_T2C[7] = {0.266983988 , 0.332704908 , 0.200876952 , -0.138399519 , -0.293161818 , 0.341493269 , 2.81861912 };
const double bdkszmm::Ball07Private::_T3C[7] = {0.158790458 , 0.885398001 , 0.198777952 , -0.199312925 , 0.416681957 , 1.40571882 , 0.303206554 };
const double bdkszmm::Ball07Private::_XiLC[7] = {0.116634132 , 2.04650825 , 1.15893980 , 2.06944482 , -3.06350857 , -1.56650754 , 8.72125423 };
const double bdkszmm::Ball07Private::_XiTC[7] = {0.264046220 , 1.24054803 , 0.804703644 , -0.150709536 , -0.186733482 , 1.16278922 , 2.09047872 };

//Parameters from the Ball 07 FF calc (private comunication)
double bdkszmm::Ball07Private::calc(const double* coeffs, const double q2) const{
	
	const double F0 = coeffs[0];
	const double c1 = coeffs[1];
	const double c2 = coeffs[2];
	const double c3 = coeffs[3];
	const double c4 = coeffs[4];
	const double c5 = coeffs[5];
	const double c6 = coeffs[6];

	const double shat = q2/(mB*mB);
	
	return F0*exp(c1*shat + c2*shat*shat + c3*shat*shat*shat + 
			c4*shat*shat*shat*shat + c5*shat*shat*shat*shat*shat + 
			c6*shat*shat*shat*shat*shat*shat);
}

const double* bdkszmm::Ball07Private::getCoeffs(const FFLABELS index) const{
	switch(index){
	case bdkszmm::V:
		return _VC;
	case bdkszmm::A0:
		return _A0C;
	case bdkszmm::A1:
		return _A1C;
	case bdkszmm::A2:
		return _A2C;
	case bdkszmm::T1:
		return _T1C;
	case bdkszmm::T2:
		return _T2C;	
	case bdkszmm::T3:
		return _T3C;
	case bdkszmm::XIL:
		return _XiLC;
	case bdkszmm::XIT:
		return _XiTC;
	default:
		assert(false);
		return 0;//segfault...
	};
}

double bdkszmm::Ball07Private::getXiL(const double q2) const{
	return calc(getCoeffs(XIL),q2);
}
double bdkszmm::Ball07Private::getXiT(const double q2) const{
	return (mB/(mB + mKstar))*calc(getCoeffs(V),q2);
}


const double bdkszmm::AliBall00::_VC[3] = {0.457,	1.482,	1.015};
const double bdkszmm::AliBall00::_A0C[3] = {0.471,	1.505,	0.710};
const double bdkszmm::AliBall00::_A1C[3] = {0.337,	0.602,	0.258};
const double bdkszmm::AliBall00::_A2C[3] = {0.282,	1.172,	0.567};
const double bdkszmm::AliBall00::_T1C[3] = {0.379,	1.519,	1.030};
const double bdkszmm::AliBall00::_T2C[3] = {0.379,	0.517,	0.426};
const double bdkszmm::AliBall00::_T3C[3] = {0.260,	1.129,	1.128};

//Parameters from the Ball 07 FF calc (private comunication)
double bdkszmm::AliBall00::calc(const double* coeffs, const double q2) const{
	const double F0 = coeffs[0];
	const double c1 = coeffs[1];
	const double c2 = coeffs[2];

	const double shat = q2/(mB*mB);
	//eqn 3.7
	return F0*exp( c1*shat + c2*shat*shat );
}

const double* bdkszmm::AliBall00::getCoeffs(const FFLABELS index) const{
	switch(index){
	case bdkszmm::V:
		return _VC;
	case bdkszmm::A0:
		return _A0C;
	case bdkszmm::A1:
		return _A1C;
	case bdkszmm::A2:
		return _A2C;
	case bdkszmm::T1:
		return _T1C;
	case bdkszmm::T2:
		return _T2C;	
	case bdkszmm::T3:
		return _T3C;
	case bdkszmm::XIL:
		assert(false);
		return 0;//segfault...
	case bdkszmm::XIT:
		assert(false);
		return 0;//segfault...
	default:
		assert(false);
		return 0;//segfault...
	};
}

//from Table 2 of BFS 05
const double bdkszmm::BFS::_xiT0 = 0.26;
const double bdkszmm::BFS::_xiL0 = 0.159;

const double bdkszmm::BFS::_VC[3] = {0.424, 1.55,0.575};
const double bdkszmm::BFS::_A0C[3] = {0.470,1.55,0.680};
const double bdkszmm::BFS::_A1C[3] = {0.337,0.60,-0.023};
const double bdkszmm::BFS::_A2C[3] = {0.283,1.18,0.281};
const double bdkszmm::BFS::_T1C[3] = {0.379,1.59,0.615};
const double bdkszmm::BFS::_T2C[3] = {0.379,0.49,-0.241};
const double bdkszmm::BFS::_T3C[3] = {0.261,1.20,0.098};

//the following are from arXiv:0807.2589 and are valid in the range 1-6 GeV^2
double bdkszmm::BFS::getV(const double q2) const{
	return getF(q2,_VC);
}
double bdkszmm::BFS::getA0(const double q2) const{
	return getF(q2,_A0C);
}
double bdkszmm::BFS::getA1(const double q2) const{
	return getF(q2,_A1C);
}
double bdkszmm::BFS::getA2(const double q2) const{
	return getF(q2,_A2C);
}
double bdkszmm::BFS::getT1(const double q2) const{
	return getF(q2,_T1C);
}
double bdkszmm::BFS::getT2(const double q2) const{
	return getF(q2,_T2C);
}
double bdkszmm::BFS::getT3(const double q2) const{
	return getF(q2,_T3C);
}

//Numbers from table VIII of Ball and Zwicky
const double bdkszmm::Ball05::_VC[5] = {0.0, 0.923, 28.3024, -0.511, 49.40};
const double bdkszmm::Ball05::_A0C[5] = {0.0, 1.364, 31.6969, -0.990, 36.78};
const double bdkszmm::Ball05::_A1C[5] = {2.0, 0.0, 0.0, 0.290, 40.38};
const double bdkszmm::Ball05::_A2C[5] = {1.0, -0.084, 0.0, 0.342, 52.00};
const double bdkszmm::Ball05::_T1C[5] = {0.0, 0.823, 28.3024, -0.491, 46.31};
const double bdkszmm::Ball05::_T2C[5] = {2.0, 0.0, 0.0, 0.333, 41.41};
const double bdkszmm::Ball05::_T3C[5] = {1.0, -0.036, 0.0, 0.368, 48.10};

double bdkszmm::Ball05::calc(const double* coeffs, const double q2) const{
	
	int model = (int)(coeffs[0] + 0.5);
	double result = 0.0;
	switch(model){
	case 0:
		result = calc_0(coeffs, q2);
		break;
	case 1:
		result = calc_1(coeffs, q2);
		break;
	case 2:
		result = calc_2(coeffs, q2);
		break;
	};
	return result;
}

const double* bdkszmm::Ball05::getCoeffs(const FFLABELS index) const{
	switch(index){
	case bdkszmm::V:
		return _VC;
	case bdkszmm::A0:
		return _A0C;
	case bdkszmm::A1:
		return _A1C;
	case bdkszmm::A2:
		return _A2C;
	case bdkszmm::T1:
		return _T1C;
	case bdkszmm::T2:
		return _T2C;	
	case bdkszmm::T3:
		return _T3C;
	case bdkszmm::XIL:
		assert(false);
		return 0;//segfault...
	case bdkszmm::XIT:
		assert(false);
		return 0;//segfault...
	default:
		assert(false);
		return 0;//segfault...
	};
}

std::auto_ptr<bdkszmm::FFCalc> bdkszmm::getFFModel(const bdkszmm::PARAMETERIZATIONS model){
	switch(model){
	case BALL07PRIVATE:
		return std::auto_ptr<bdkszmm::FFCalc>(new bdkszmm::Ball07Private);
	case ALIBALL00:
		return std::auto_ptr<bdkszmm::FFCalc>(new bdkszmm::AliBall00);
	case BALL05:
			return std::auto_ptr<bdkszmm::FFCalc>(new bdkszmm::Ball05);
	case BFS05:
		return std::auto_ptr<bdkszmm::FFCalc>(new bdkszmm::BFS);
	default:
		return std::auto_ptr<bdkszmm::FFCalc>(new bdkszmm::Ball07Private);
	};
}

