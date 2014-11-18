#ifndef MATHEMATICAFN_HH_
#define MATHEMATICAFN_HH_

#include "EvtGenBase/EvtComplex.hh"
#include "EvtGenBase/EvtComplexExtras.hh"

#include <cmath>
#include <utility>
#include <vector>
#include <cassert>

/**
 * Module EvtGenBase/EvtMathematicaFn
 * 
 * @author wreece
 * @date September '08
 * 
 * A collection of functions and templated
 * objects consistent with the Mathematica programing 
 * language. Written to aid translating between
 * Mathematica based decay models and C++ based ones
 * in as brain-dead a way as possible, using for
 * example the Mathematica function CForm.
 * 
 */


template<typename T> T Chop(const T value){
	return chop(value);
}

template<typename T> EvtComplex Abs( const T value ){
	const EvtComplex tmp = Chop(value);//will not compile if this conversion is not defined
	return abs(tmp);
}

template<typename T> EvtComplex Exp( const T value ){
	const EvtComplex tmp = Chop(value);//will not compile if this conversion is not defined
	return exp(tmp);
}

template<>
double Chop(const double value);
EvtComplex Complex(const double re, const double im = 0.0);

template<typename T> EvtComplex ArcTan( const T value ){
	const EvtComplex tmp = Chop(value);//will not compile if this conversion is not defined
	return atan(tmp);
}

template<typename T> double Re( const T value ){
	const EvtComplex tmp = Chop(value);//will not compile if this conversion is not defined
	return real(tmp);
}

template<typename T> double Im( const T value ){
	const EvtComplex tmp = Chop(value);//will not compile if this conversion is not defined
	return imag(tmp);
}

template<typename T> EvtComplex Conjugate( const T value ){
	const EvtComplex tmp = Chop(value);//will not compile if this conversion is not defined
	return conj(tmp);
}

template<typename T> unsigned int KroneckerDelta( const T i, const T j){
	return (i == j) ? 1 : 0;
}

template<typename T> EvtComplex Log( const T value ){
	const EvtComplex tmp = Chop(value);//will not compile if this conversion is not defined
	return log(tmp);
}

template<typename T> EvtComplex Power( const T value, const double index){
	const EvtComplex tmp = Chop(value);//will not compile if this conversion is not defined
	return (index == 2) ? tmp*tmp : pow(tmp, index);
}

template<typename T> EvtComplex Power( const T value, const T index){
	const EvtComplex tmp = Chop(value);//will not compile if this conversion is not defined
	return pow(tmp,index);
}

EvtComplex Sqrt(const EvtComplex value);
EvtComplex Sqrt(const double value);

EvtComplex PolyLog(const int n, const EvtComplex value);
EvtComplex PolyLog(const int n, const double value);

template<class T>
class DataBlock{
	/**
	 * Array like class for holding objects and indexing
	 * them in a Mathematica like way.
	 */
	typedef std::vector<std::vector<T>* > vec2D;
	template<class TT> friend const DataBlock<TT> operator+(const DataBlock<TT>&,const DataBlock<TT>&);
public:	
	DataBlock(const unsigned int& _m, const unsigned int& _n):
	data(new vec2D),m(_m),n(_n){
		init();
	};
	
	DataBlock(const DataBlock& other):
	data(new vec2D),m(other.m),n(other.n){
		init();
		//copy by value
		for(unsigned int i = 1; i <= m; ++i){
			for(unsigned int j = 1; j <= n; ++j){
				set(i,j, other(i,j));
			}
		}
	};
	
	virtual ~DataBlock(){
		for(typename vec2D::iterator it = data->begin(); it != data->end(); ++it){
			delete *it;
		}
		delete data;
	};
	
	T operator()(const unsigned int& _m, const unsigned int& _n) const{
		//indexing like mathematicas
		return data->at(_m-1)->at(_n-1);
	};
	
	DataBlock<T>& operator+=(const DataBlock<T>& other){
		assert( (other.m == m) && (other.n == n) );
		for(unsigned int i = 1; i <= m; ++i){
			for(unsigned int j = 1; j <= n; ++j){
				set(i,j, other(i,j) + (*this)(i,j));
			}
		}
		return *this;
	};
	
	DataBlock<T>& operator*=(const double & c){
		for(unsigned int i = 1; i <= m; ++i){
			for(unsigned int j = 1; j <= n; ++j){
				set(i,j, c * (*this)(i,j));
			}
		}
		return *this;
	};
	
	void set(const unsigned int& _m, const unsigned int& _n, const T& val){
		data->at(_m-1)->at(_n-1) = val;
	}

	
private:
	
	void init(){
		data->reserve(m);
		for(unsigned int i = 0; i < m; ++i){
			data->push_back(new std::vector<T>(n,0));
		}
	};
	
	DataBlock& operator=(const DataBlock& other);
	
	vec2D* const data;
	const unsigned int m;
	const unsigned int n;
	
};

template<class TT>
const DataBlock<TT> operator+(const DataBlock<TT>& first, const DataBlock<TT>& second){
	return DataBlock<TT>(first) += second;
}

template<class TT>
const DataBlock<TT> operator*(const double& c, const DataBlock<TT>& block){
	return DataBlock<TT>(block) *= c;
}

template<class T>
class SignedPair{
/**
 * Similar to DataBlock, but stores a pair of objects
 * of the same type as a pair, again with Mathematica
 * style indexing.
 */
public:
	
	SignedPair():
			values(std::pair<T,T>()){
	};
	
	SignedPair(const T& a, const T& b):
		values(std::make_pair(a,b)){
	};
	
	SignedPair(const std::pair<T,T> p):
		values(p){
	};
	
	const T& operator[](const unsigned int index) const{
		assert( (index == 1) || (index == 2));
		return (index == 1) ? values.first : values.second;
	};
	
	void set(const unsigned int index, const T& val){
		if(index == 1){
			values.first = val;
		}else if(index == 2){
			values.second = val;
		}
	};
	
private:
	
	//SignedPair& operator=(const SignedPair& other);
	
	std::pair<T,T> values;
};



#endif /*MATHEMATICAFN_HH_*/
