#ifndef WILSONCOEFFICIENTS_HH_
#define WILSONCOEFFICIENTS_HH_

#include "CLHEP/Matrix/GenMatrix.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/Vector.h"
#include "EvtGenBase/EvtComplex.hh"
#include "EvtGenModels/EvtBToVllConstants.hh"
#include "EvtGenModels/EvtBToVllPoleMass.hh"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <memory>
#include <vector>
#include <functional>

namespace qcd{

typedef CLHEP::HepMatrix Matrix;
typedef CLHEP::HepVector Vector;
typedef EvtComplex WilsonType;

enum Scale {MU_MW = 0, MU_MB = 1, MU_H = 3, MU_MBP = 4};


/**
 * Simple class for holding the value of the Wilson Coefficients at a
 * particular value of mu.
 */
template<typename T = WilsonType>
class WilsonCoefficients{
	
public:
	
	explicit WilsonCoefficients(const Scale& _scale, const unsigned int _operatorBasis, const unsigned int _dimension);
	WilsonCoefficients(const WilsonCoefficients<T>& other);

	static std::auto_ptr<WilsonCoefficients<T> > conjugate(const WilsonCoefficients<T>& other);
	
	T& operator()(const size_t index) const;
	T& operator()(const size_t index);

	unsigned int getOperatorBasis() const;
	void setOperatorBasis(const unsigned int newBasis);
	
	unsigned int getDimension() const;
	
	Scale getScale() const;
	static double getScaleValue(const Scale _scale);
	double getScaleValue() const;

private:
	
	WilsonCoefficients<T>& operator=(const WilsonCoefficients<T>&);
	
	const Scale scale;
	std::auto_ptr<std::vector<T> > wc;
	unsigned int operatorBasis;
	unsigned int dimension;
	
	template<typename TT>
	friend WilsonCoefficients<TT> operator*(const Matrix &m1, const WilsonCoefficients<TT>& m2);

	static EvtComplex doConj(const T& number);
	

};
typedef const WilsonCoefficients<WilsonType>* WCPtrNaked;
typedef std::auto_ptr<const WilsonCoefficients<WilsonType> > WCPtr;

template<typename T>
WilsonCoefficients<T>::WilsonCoefficients(const Scale& _scale, const unsigned int _operatorBasis, const unsigned int _dimension):
	scale(_scale), wc(new std::vector<T>(_operatorBasis)),operatorBasis(_operatorBasis),dimension(_dimension)
{
}

template<typename T>
WilsonCoefficients<T>::WilsonCoefficients(const WilsonCoefficients<T>& other):
	scale(other.scale), wc(new std::vector<T>(other.operatorBasis)),
		operatorBasis(other.operatorBasis),dimension(other.dimension)
{
	//copy data rather than transfer ownership
	std::copy(other.wc->begin(),other.wc->end(),wc->begin());
}

/**
 * Index goes from 1, like the Wilson Coefficients
 * @throws std::out_of_range
 */
template<typename T>
T& WilsonCoefficients<T>::operator ()(const size_t index) const{
	return wc->at(index - 1);
}

/**
 * Index goes from 1, like the Wilson Coefficients
 * @throws std::out_of_range
 */
template<typename T>
T& WilsonCoefficients<T>::operator ()(const size_t index){
	return wc->at(index - 1);
}

template<typename T>
unsigned int WilsonCoefficients<T>::getOperatorBasis() const{
	return operatorBasis;
}

template<typename T>
void WilsonCoefficients<T>::setOperatorBasis(const unsigned int newBasis){
	operatorBasis = newBasis;
}

template<typename T>
unsigned int WilsonCoefficients<T>::getDimension() const{
	return dimension;
}

/**
 * The renormalisation scale being used.
 */
template<typename T>
Scale WilsonCoefficients<T>::getScale() const{
	return scale;
}

/**
 * The renormalisation scale being used.
 */
template<typename T>
double WilsonCoefficients<T>::getScaleValue(const Scale _scale){
	switch(_scale){
	case MU_MW:
		return constants::mu_mw;
	case MU_MB:
		return constants::mu_mb;
	case MU_MBP:
		//value of mbp when evaluated at the pole mass scale
		return qcd::mb_pole(constants::mb, constants::mbp);//value at scale mbp
	case MU_H:
		return constants::mu_h;
	default:
		return constants::mu_mw;
	}
}

/**
 * The renormalisation scale being used.
 */
template<typename T>
double WilsonCoefficients<T>::getScaleValue() const{
	return WilsonCoefficients<T>::getScaleValue(scale);
}

/**
 * Returns a copy of the WilsonCoefficients object
 * where a complex conjugate has been made.
 */
template<typename T>
std::auto_ptr<WilsonCoefficients<T> > WilsonCoefficients<T>::conjugate(const WilsonCoefficients<T>& other){
	WilsonCoefficients<T>* result = new WilsonCoefficients<T>(other);
	std::transform(result->wc->begin(),result->wc->end(),result->wc->begin(),WilsonCoefficients<T>::doConj);
	return std::auto_ptr< WilsonCoefficients<T> >(result);
}

template<typename T>
std::ostream& operator<<(std::ostream& s, const WilsonCoefficients<T>& C){
	int basis = C.getOperatorBasis();
	s << "{";
	for(int i = 0; i < basis; ++i){
		s << C(i+1);
		if((i+1) != basis){
			s << ", ";
		}
	}
	s << "}";
	return s;
}

template<typename T>
WilsonCoefficients<T> operator*(const Matrix &m1, const  WilsonCoefficients<T>& m2){
	
	WilsonCoefficients<T> result(m2.getScale(),m2.getOperatorBasis(),m2.getDimension());
	
	Vector mRe(m1.num_row());
	Vector mIm(m1.num_row());
	
	//The idea here is to do the re and im multiplications seperately,
	//and then combine them at the end. Only works as m1 is real.
	for(unsigned int i = 1; i <= m2.getOperatorBasis(); i++){
		mRe(i) = real(m2(i));
		mIm(i) = imag(m2(i));
	}
	Vector mReResult = m1*mRe;
	Vector mImResult = m1*mIm;
	for(unsigned int i = 1; i <= result.getOperatorBasis(); i++){
		result(i) = T(mReResult(i),mImResult(i));
	}
	return result;
}

//private function to perform the conjugate of things which can be converted to an EvtComplex
template<typename T>
EvtComplex WilsonCoefficients<T>::doConj(const T& number){
	EvtComplex c = number;
	return conj(c);
}

}
#endif /*WILSONCOEFFICIENTS_HH_*/
