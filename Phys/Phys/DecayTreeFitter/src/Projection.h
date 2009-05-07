#ifndef _VTK_PROJECTION_HH_
#define _VTK_PROJECTION_HH_

#include <CLHEP/Matrix/Vector.h>
#include <CLHEP/Matrix/SymMatrix.h>
#include <CLHEP/Matrix/Matrix.h>

namespace vtxtreefit
{
  
  class Projection
  {
  public:
    // constructor 
    Projection(int dimP, int dimC) : _matrixH(dimC,dimP,0),_r(dimC,0),_matrixV(dimC,0),_offset(0) {}
    
    // accessors to the projection matrix
    const HepMatrix& H() const { return _matrixH ; } 
    //HepMatrix& H() { return _matrixH ; } 
    double& H(int row, int col) {
#ifdef VTK_BOUNDSCHECKING
      assert( _offset+row >0 && col>0 && _offset+row <= _matrixH.num_row() && col <= _matrixH.num_col() ) ;
#endif
      return _matrixH(_offset+row,col) ; }

    // accessors to the residual (or the 'value' of the constraint)
    const HepVector& r() const { return _r ; }
    HepVector& r() { return _r ; }
    //HepVector& r() { return _r ; }
    double& r(int row) { 
#ifdef VTK_BOUNDSCHECKING
      assert( _offset+row >0  && _offset+row <= _r.num_row() ) ;
#endif
      return _r(_offset+row) ; }
    
    // accessors to the covariance matrix
    const HepSymMatrix& V() const { return _matrixV ; } 
    //HepSymMatrix& V() { return _matrixV ; } 
    double& V(int row, int col) { return _matrixV(_offset+row,_offset+col) ; }
    double& Vfast(int row, int col) { 
#ifdef VTK_BOUNDSCHECKING
      assert( _offset+row >0 && _offset+col>0 && _offset+row <= _matrixV.num_row() && _offset+col <= _matrixV.num_col() && row>=col ) ;
#endif
      return _matrixV.fast(_offset+row,_offset+col) ; }
    
    // reset
    void reset() {
      // fill everything with '0'.  this implementation is kind of
      // tricky, but it is fast.
      int dimC = _matrixH.num_row() ;
      int dimP = _matrixH.num_col() ;
      memset(&(_matrixH(1,1)),0,dimP*dimC*sizeof(double));
      memset(&(_r(1))  ,0,dimC*sizeof(double));
      memset(&(_matrixV(1,1)),0,dimC*(dimC+1)/2*sizeof(double));
      _offset = 0 ;
    }

    // globalChisq
    double chiSquare() const {
      HepSymMatrix W=_matrixV ;
      int ierr;  W.inverse(ierr) ;
      return W.similarity(_r) ;
    }
    
    void incrementOffset(unsigned int i) { _offset+=i ; }
    unsigned int offset() const { return _offset ; }
    
  private:
    HepMatrix _matrixH ;    // projection matrix
    HepVector _r ;    // constraint residual 
    HepSymMatrix _matrixV ; // constraint variance (zero for lagrange constraints)
    unsigned int _offset ; // offset for constraint index. only non-zero for merged constraints.
  } ;
}
#endif
