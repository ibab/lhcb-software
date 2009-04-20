#ifndef EVOLVEWC_H_
#define EVOLVEWC_H_

//from CLHEP
#include "CLHEP/Matrix/DiagMatrix.h"
#include "CLHEP/Matrix/Matrix.h"

#include <memory>
#include <utility>
#include "EvtGenBase/EvtComplex.hh"
#include "EvtGenModels/EvtBToVllWC.hh"

namespace qcd{

typedef CLHEP::HepDiagMatrix DiagMatrix;
typedef CLHEP::HepGenMatrix MatrixBaseClass;
typedef CLHEP::HepMatrix StdMatrix;
typedef std::auto_ptr<DiagMatrix> DiagMatrixPtr;
typedef std::auto_ptr<StdMatrix> StdMatrixPtr;
typedef std::pair<const WilsonCoefficients<WilsonType>*,const WilsonCoefficients<WilsonType>*> WilsonPair;

/**
 * Class to evolve the WC using the full 10x10 anomalous
 * dimension matrix.
 */

class EvtBToVllEvolveWC10D : public std::unary_function<qcd::Scale,WilsonPair*>{
	
public:
	
	EvtBToVllEvolveWC10D(const WilsonCoefficients<WilsonType>&,
			const WilsonCoefficients<WilsonType>&,
			const WilsonCoefficients<WilsonType>&);
	result_type operator()(const argument_type& scale); 
	
	static WilsonType runC7(const WilsonType C7h, const WilsonType C8h, const argument_type& scale);
	static WilsonType runC8(const WilsonType C8h, const argument_type& scale);
	
	const static unsigned int dimension = 10;

private:
	const WilsonCoefficients<WilsonType>& C;//left-handed coefficients
	const WilsonCoefficients<WilsonType>& CNP;//left-handed NP contributions
	const WilsonCoefficients<WilsonType>& CR;//right-handed coefficients
};

template<int m, int n>
StdMatrixPtr makeMatrix(const double data[m][n]){
	StdMatrixPtr result(new StdMatrix(m,n));
	for(int i = 0; i<m; ++i){
		for(int j = 0; j<n; ++j){
			(*result)[i][j] = data[i][j];
		}
	}
	return result;
}


}
#endif /*EVOLVEWC_H_*/
