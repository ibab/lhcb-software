#ifndef EVOLVEWC_H_
#define EVOLVEWC_H_

//from CLHEP
#include "CLHEP/Matrix/DiagMatrix.h"
#include "CLHEP/Matrix/Matrix.h"

#include <memory>
#include "EvtGenBase/EvtComplex.hh"
#include "EvtGenModels/EvtBToVllWC.hh"

namespace qcd{

typedef CLHEP::HepDiagMatrix DiagMatrix;
typedef CLHEP::HepGenMatrix MatrixBaseClass;
typedef CLHEP::HepMatrix StdMatrix;
typedef std::auto_ptr<DiagMatrix> DiagMatrixPtr;
typedef std::auto_ptr<StdMatrix> StdMatrixPtr;

/**
 * Class to evolve the WC using the full 10x10 anomalous
 * dimension matrix.
 */
class EvtBToVllEvolveWC10D : public std::unary_function<double,std::auto_ptr<WilsonCoefficients<WilsonType> > >{
	
public:
	
	EvtBToVllEvolveWC10D(const WilsonCoefficients<WilsonType>&);
	result_type operator()(const argument_type& scale); 
	
	const static unsigned int dimension = 10;

private:
	const WilsonCoefficients<WilsonType>& C;
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
