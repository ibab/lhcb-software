#ifndef VTK_KALMANCALCULATOR_HH
#define VTK_KALMANCALCULATOR_HH

#include <CLHEP/Matrix/Matrix.h>
#include <CLHEP/Matrix/SymMatrix.h>
#include "FitParams.h"
#include "VtxErrCode.h"

namespace DecayTreeFitter
{

  class KalmanCalculator
  {
  public:
    ErrCode init(const CLHEP::HepVector& value, const CLHEP::HepMatrix& G,
		 const FitParams& fitparams, const CLHEP::HepSymMatrix* V=0, int weight=1) ;
    void updatePar(FitParams& fitparams) ;
    void updatePar(const CLHEP::HepVector& prediction, FitParams& fitparams) ;
    void updateCov(FitParams& fitparams) ;
    double chisq() const { return m_chisq ; }
  private:
    int m_nconstraints ; // dimension of the constraint
    int m_nparameters  ; // dimension of the state
    const CLHEP::HepVector* m_value ;
    const CLHEP::HepMatrix* m_matrixG ;
    CLHEP::HepSymMatrix m_matrixR;    // cov of residual
    CLHEP::HepSymMatrix m_matrixRinv; // inverse of cov of residual
    CLHEP::HepMatrix m_matrixK   ;    // kalman gain matrix
    double m_chisq ;
    int m_ierr ;
    // some temporary results
    CLHEP::HepMatrix m_matrixCGT ;
  } ;
}

#endif
