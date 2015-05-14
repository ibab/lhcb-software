#ifndef FITPARAMS_H
#define FITPARAMS_H

#include <vector>
#include <map>
#include <CLHEP/Matrix/Vector.h>
#include <CLHEP/Matrix/SymMatrix.h>
#include <CLHEP/Matrix/DiagMatrix.h>
#include "DecayTreeFitter/ChiSquare.h"

namespace DecayTreeFitter
{
  class ParticleBase ;

  class FitParams
  {
  public:
    // Class that contains the parameters and covariance for the
    // vertex fit.
    FitParams(int dim) ;
    ~FitParams() ;

    CLHEP::HepSymMatrix& cov() { return m_cov ; }
    CLHEP::HepVector& par() { return m_par ; }
    double& par(int row) { return m_par(row) ; }
    double cov(int row) const { return m_cov.fast(row,row) ; }

    CLHEP::HepSymMatrix cov(const std::vector<int>& indexVec) const ;
    CLHEP::HepVector par(const std::vector<int>& indexVec) const ;

    const CLHEP::HepSymMatrix& cov() const { return m_cov ; }
    const CLHEP::HepVector& par() const { return m_par ; }
    const double& par(int row) const { return m_par(row) ; }

    CLHEP::HepDiagMatrix& scale() { return m_scale ; }

    int& nConstraintsVec(int row) { return m_nConstraintsVec[row-1] ; }

    //int dim() const { return m_par.num_row() ; }
    int dim() const { return m_dim ; }
    double chiSquare() const { return m_chiSquare ; }

    int nConstraints() const { return m_nConstraints ; }
    int nDof() const { return nConstraints() - dim() ; }
    double err(int row) const { return sqrt(m_cov(row,row)) ; }

    void resize(int newdim) ;
    void resetPar() ;
    void resetCov(double scale=100) ;
    void print() const ;
    bool testCov() const ;

    void addChiSquare( double chi2, int nconstraints, const ParticleBase* p) ;
    ChiSquare chiSquare( const ParticleBase& p) const ;

  protected:
    FitParams() {}
  private:
    int m_dim ;
    CLHEP::HepVector    m_par ;
    CLHEP::HepSymMatrix m_cov ;
    CLHEP::HepDiagMatrix m_scale ;
    double m_chiSquare ;
    int    m_nConstraints ;
    std::vector<int> m_nConstraintsVec ;    // vector with number of constraints per parameter
    std::map<const ParticleBase*, ChiSquare> m_chiSquareMap ;
  } ;
}

#endif
