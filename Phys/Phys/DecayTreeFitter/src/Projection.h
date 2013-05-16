#ifndef PROJECTION_H
#define PROJECTION_H

#include <CLHEP/Matrix/Vector.h>
#include <CLHEP/Matrix/SymMatrix.h>
#include <CLHEP/Matrix/Matrix.h>
#include <string.h> // for memset

namespace DecayTreeFitter
{

  class Projection
  {
  public:
    // constructor
    Projection(int dimP, int dimC) 
      : m_matrixH(dimC,dimP,0),m_r(dimC,0),m_matrixV(dimC,0),
        m_offset(0),m_particle(0) {}

    // accessors to the projection matrix
    const HepMatrix& H() const { return m_matrixH ; }
    //HepMatrix& H() { return m_matrixH ; }
    double& H(int row, int col) {
#ifdef VTK_BOUNDSCHECKING
      assert( m_offset+row >0 && col>0 && 
              m_offset+row <= m_matrixH.num_row() && 
              col <= m_matrixH.num_col() ) ;
#endif
      return m_matrixH(m_offset+row,col) ; }

    // accessors to the residual (or the 'value' of the constraint)
    const HepVector& r() const { return m_r ; }
    HepVector& r() { return m_r ; }
    double& r(const int row) 
    {
#ifdef VTK_BOUNDSCHECKING
      assert( m_offset+row >0  && m_offset+row <= m_r.num_row() ) ;
#endif
      return m_r(m_offset+row) ; 
    }

    // accessors to the covariance matrix
    const HepSymMatrix& V() const { return m_matrixV ; }
    //HepSymMatrix& V() { return m_matrixV ; }
    double& V(const int row, const int col)
    {
      return m_matrixV(m_offset+row,m_offset+col) ;
    }
    double& Vfast(const int row, const int col) 
    {
#ifdef VTK_BOUNDSCHECKING
      assert( m_offset+row >0 && m_offset+col>0 && 
              m_offset+row <= m_matrixV.num_row() && 
              m_offset+col <= m_matrixV.num_col() && row>=col ) ;
#endif
      return m_matrixV.fast(m_offset+row,m_offset+col) ; }

    // reset
    void reset() 
    {
      // fill everything with '0'.  this implementation is kind of
      // tricky, but it is fast.
      const int dimC = m_matrixH.num_row() ;
      const int dimP = m_matrixH.num_col() ;
      memset(&(m_matrixH(1,1)),0,dimP*dimC*sizeof(double));
      memset(&(m_r(1))  ,0,dimC*sizeof(double));
      memset(&(m_matrixV(1,1)),0,dimC*(dimC+1)/2*sizeof(double));
      m_offset = 0 ;
    }

    // globalChisq
    double chiSquare() const 
    {
      HepSymMatrix W = m_matrixV ;
      int ierr(0);
      W.inverse(ierr) ;
      return W.similarity(m_r) ;
    }
    
    void incrementOffset(const unsigned int i) { m_offset += i ; }
    unsigned int offset() const { return m_offset ; }

    void setParticle( const ParticleBase& p ) { m_particle = &p ; }
    const ParticleBase* particle() const { return m_particle ; }

  private:

    HepMatrix m_matrixH ;    ///< projection matrix
    HepVector m_r ;          ///< constraint residual
    HepSymMatrix m_matrixV ; ///< constraint variance (zero for lagrange constraints)
    unsigned int m_offset ;  ///< offset for constraint index. only non-zero for merged constraints.
    const ParticleBase* m_particle ; ///< particle where chi2 should be added

  } ;

}

#endif
