#include <algorithm>
#include <iostream>
#include <iomanip>
#include <locale>
#include <math.h>
#include <string>
#include <sstream>

#include "EvtGenBase/EvtComplex.hh"
#include "EvtGenBase/EvtComplexExtras.hh"

#include "gsl/gsl_complex.h"
#include "gsl/gsl_complex_math.h"

EvtComplex atan(const EvtComplex& cmp){
	gsl_complex c = gsl_complex_rect(real(cmp), imag(cmp));
	c = gsl_complex_arctan(c);
	return EvtComplex(GSL_REAL(c),GSL_IMAG(c));
}

EvtComplex chop(const EvtComplex& cmp){
	//implements Mathematica's chop function for complex numbers
	class inner {
	public:
		static double chop(const double& val) {
			return (fabs(val) > 1E-10) ? val : 0.0;
		}
	};
	return EvtComplex(inner::chop(real(cmp)),inner::chop(imag(cmp)));
}

EvtComplex log(const EvtComplex& cmp){
	gsl_complex c = gsl_complex_rect(real(cmp), imag(cmp));
	c = gsl_complex_log(c);
	return EvtComplex(GSL_REAL(c),GSL_IMAG(c));
}

EvtComplex pow(const EvtComplex& cmp, const double index){
	gsl_complex c = gsl_complex_rect(real(cmp), imag(cmp));
	c = gsl_complex_pow_real(c,index);
	return EvtComplex(GSL_REAL(c),GSL_IMAG(c));
}

EvtComplex pow(const EvtComplex& cmp, const EvtComplex index){
	gsl_complex z = gsl_complex_rect(real(cmp), imag(cmp));
	gsl_complex a = gsl_complex_rect(real(index), imag(index));
	gsl_complex c =  gsl_complex_pow(z,a);
	return EvtComplex(GSL_REAL(c),GSL_IMAG(c));
}

EvtComplex sqrt(const EvtComplex& cmp){
	gsl_complex c = gsl_complex_rect(real(cmp), imag(cmp));
	c = gsl_complex_sqrt(c);
	return EvtComplex(GSL_REAL(c),GSL_IMAG(c));
}

EvtComplex sqrt_real(const double value){
	const gsl_complex c = gsl_complex_sqrt_real(value);
	return EvtComplex(GSL_REAL(c),GSL_IMAG(c));
}

std::ostream& operator<<(std::ostream& s, const EvtComplex& c){

	s<< "(" << std::setprecision(5) << real(c) << "," << std::setprecision(5) << imag(c) << ")";
	return s;
}

std::istream& operator>>(std::istream& s, EvtComplex& c){

	std::string cmp;
	s >> cmp;
	
	if(!cmp.size()){
		s.setstate(std::ios::failbit);
		return s;
	}
	
	//convert to lower case
	std::transform(cmp.begin(), cmp.end(), cmp.begin(), 
			(int(*)(int)) tolower);

	std::ostringstream valueString;
	double real = 0.0;
	double imag = 0.0;
	
	if( (cmp.find('i') == std::string::npos) && (cmp.find('j') == std::string::npos) ){
		std::istringstream(cmp) >> real;
	}else{
		
		char op = '+';
		char realOp = op;
		unsigned int count = 0;
		
		//see whether to add or subtract imag part
		int factor = 1;
		for (std::string::const_iterator it = cmp.begin(); it != cmp.end(); ++it) {
			const char c = *it;

			switch (c) {
			case 'i':
			case 'j':
			{
				if(valueString.str() != ""){
					std::istringstream(realOp + valueString.str()) >> real;
					
				}
				valueString.str("");
			}
			case ' '://ignore whitespace
				break;
			case '+':
			case '-':
				if(!count) realOp = c;
				op = c;
				break;
			default:
				valueString << c;
			};
			count++;
		}
		
		if(valueString.str() != ""){
			std::istringstream(op + valueString.str()) >> imag;
			imag *= factor;
		}
	}
	//finally set the components
	c = EvtComplex(real,imag);
	return s;
}
