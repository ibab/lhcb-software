
#include "KalmanCalculator.h"

#define SLOWBUTSAFE 1
#undef SLOWBUTSAFE
#undef SKIPHIGHACCURACYCORRECTION

namespace DecayTreeFitter
{
  
  inline double fastsymmatrixaccess(double* m, int row, int col)
  {
    return *(m+(row*(row-1))/2+(col-1));
  }

  inline double symmatrixaccess(double* m, int row, int col)
  {
    return (row>=col? fastsymmatrixaccess(m,row,col) : fastsymmatrixaccess(m,col,row)) ;
  }

  ErrCode
  KalmanCalculator::init(const HepVector& value, const HepMatrix& G, 
			 const FitParams& fitparams, const HepSymMatrix* V, 
			 int weight)
  {
    ErrCode status ;
    m_nconstraints = value.num_row() ;  // dimension of the constraint
    m_nparameters  = fitparams.dim() ; // dimension of the state

    int valdim  = value.num_row() ; // dimension of the constraint
    int statdim = fitparams.par().num_row() ; // dimension of the state

#ifdef VTK_BOUNDSCHECKING
    assert( G.num_row() == valdim && G.num_col() == statdim &&
	    (!V || V->num_row()==valdim) ) ;
#endif
    m_value = &value ;
    m_matrixG     = &G ;
    const HepSymMatrix& C = fitparams.cov() ;
    // calculate C*G.T()
#ifdef SLOWBUTSAFE
    m_matrixCGT = C * G.T() ;
#else
    double tmp ;
    m_matrixCGT = HepMatrix(statdim,valdim,0) ;
    for(int col=1; col<=m_nconstraints; ++col)
      for(int k=1; k<=m_nparameters; ++k)
 	if( (tmp=G.HepMatrix::operator()(col,k)) !=0 ) {
	  for(int row=1; row<k; ++row)
	    m_matrixCGT.HepMatrix::operator()(row,col) += C.fast(k,row) * tmp ;
	  for(int row=k; row<=statdim; ++row)
	    m_matrixCGT.HepMatrix::operator()(row,col) += C.fast(row,k) * tmp ;
	}
#endif

    // calculate the error in the predicted residual R = G*C*GT + V
    // slow:
#ifdef SLOWBUTSAFE
    m_matrixRinv = fitparams.cov().similarity(G) ;
    if(V) m_matrixRinv += weight*(*V) ;
#else
    if(V) {
      m_matrixRinv = *V ;
      if(weight!=1) m_matrixRinv *= weight ;
    } else m_matrixRinv = HepSymMatrix(valdim,0) ;
    
    for(int row=1; row<=m_nconstraints; ++row)
      for(int k=1; k<=m_nparameters; ++k)
	if( (tmp=G.HepMatrix::operator()(row,k)) != 0 )
	  for(int col=1; col<=row; ++col)
	    m_matrixRinv.fast(row,col) += tmp*m_matrixCGT.HepMatrix::operator()(k,col) ;
#endif    
    m_matrixR = m_matrixRinv ;
    m_matrixRinv.invert(m_ierr) ;
    if(m_ierr) {
      status |= ErrCode::inversionerror; 
      std::cout << "Error inverting matrix. Vertex fit fails." << std::endl ;
    }
    
    // calculate the gain matrix
    m_matrixK = m_matrixCGT * m_matrixRinv ;
    m_chisq = -1 ;
//     // let's see if we get same results using sparce matrices
//     VtkSparseMatrix Gs(G) ; 
//     VtkSparseMatrix CGT = Gs.transposeAndMultiplyRight(fitparams.cov()) ;
//     HepSymMatrix Rs(value.numrow()) ;
//     Gs.multiplyLeft(CGT,Rs) ;
//     if(V) Rs += (*V) ;
//     Rs.invert(m_ierr) ;
//     VtkSparseMatrix Ks = CGT*Rs ;
    return status ;
  }

  void 
  KalmanCalculator::updatePar(FitParams& fitparams)
  {
    //fitparams.par() -= fitparams.cov() * (G.T() * (R * value) ) ;
    fitparams.par() -= m_matrixK * (*m_value) ;
    m_chisq = m_matrixRinv.similarity(*m_value) ;
  }

  void 
  KalmanCalculator::updatePar(const HepVector& pred, FitParams& fitparams)
  {
    // this is still very, very slow !
    HepVector valueprime = (*m_value) + (*m_matrixG) * (pred-fitparams.par()) ;
    fitparams.par() = pred - m_matrixK*valueprime ;
    m_chisq = m_matrixRinv.similarity( valueprime ) ;
  }
  
  void 
  KalmanCalculator::updateCov(FitParams& fitparams)
  {

#ifdef SLOWBUTSAFE
    HepSymMatrix deltaCov = m_matrixRinv.similarityT(*m_matrixG).similarity(fitparams.cov()) ;
    fitparams.cov() -= deltaCov ;
#else

    // There are two expessions for updating the covariance
    // matrix.
    // slow: deltaCov = - 2*C*GT*KT +  K*R*KT
    // fast: deltaCov = - C*GT*KT
    // The fast expression is very sensitive to machine accuracy. The
    // following piece of code effectively invokes the slow
    // expression. I couldn't write it faster than this.

    double tmp ;
#ifndef SKIPHIGHACCURACYCORRECTION
    // substitute C*GT --> 2*C*GT - K*R. of course, this invalidates
    // C*GT, but we do not need it after this routine.
    
    // we use the fact that _in principle_ C*GT = K*R, such that
    // they have the same zero elements
    for(int row=1; row<=m_nparameters; ++row)
      for(int col=1; col<=m_nconstraints; ++col) 
	if( (tmp =2*m_matrixCGT.HepMatrix::operator()(row,col))!=0 ) {
	  for(int k=1; k<=m_nconstraints; ++k)
	    tmp -= m_matrixK.HepMatrix::operator()(row,k) * m_matrixR.HepSymMatrix::operator()(k,col) ;
	  m_matrixCGT.HepMatrix::operator()(row,col) = tmp ;
	}
#endif
    
//     HepMatrix KR = m_matrixK*m_matrixR ;
//     double tmp ;
//     for(int row=1; row<=m_nparameters; ++row)
//       for(int k=1; k<=m_nconstraints; ++k) 
// 	if( (tmp= (KR(row,k) - 2*m_matrixCGT(row,k))) != 0 )
// 	  for(int col=1; col<=row; ++col) 
// 	    fitparams.cov().fast(row,col) += tmp * m_matrixK(col,k) ;

    // deltaCov = - C*GT*KT 
    for(int row=1; row<=m_nparameters; ++row)
      for(int k=1; k<=m_nconstraints; ++k) 
	if( (tmp = -(m_matrixCGT.HepMatrix::operator()(row,k))) != 0 )  // they have same size, and same 'emptiness'
	  for(int col=1; col<=row; ++col) 
	    fitparams.cov().fast(row,col) += tmp * m_matrixK.HepMatrix::operator()(col,k) ;

#endif
    for(int col=1; col<=m_nconstraints; ++col)
      for(int k=1; k<=m_nparameters; ++k)
 	if( (*m_matrixG).HepMatrix::operator()(col,k) !=0 ) ++(fitparams.nConstraintsVec(k)) ;
  }
}
