#include "EvtGenBase/EvtMathematicaFn.hh"
#include "gsl/gsl_sf_dilog.h"
#include "gsl/gsl_sf_result.h"

#include <cassert>

EvtComplex PolyLog(const int n, const EvtComplex value){
	assert(n == 2);//We can only handle dilogarithms
	if ( n != 2 ) return EvtComplex(0.,0.) ;
	gsl_sf_result re;
	gsl_sf_result im;
	
	gsl_sf_complex_dilog_e(abs(value),arg(value),&re,&im);
	return EvtComplex(re.val,im.val);
}

EvtComplex PolyLog(const int n, const double value){
	assert(n == 2);//We can only handle dilogarithms
	const EvtComplex tmp(value);
	return PolyLog(n, tmp);
}

template<>
double Chop(const double value){
	const double sigma = 1e-10;
	return (fabs(0.0 - value) < sigma) ? 0.0 : value;
}

EvtComplex Sqrt(const EvtComplex value){
	return sqrt(Chop(value));
}

EvtComplex Sqrt(const double value){
	return sqrt_real(Chop(value));
}

EvtComplex Complex(const double re, const double im){
	return EvtComplex(re,im);
}
