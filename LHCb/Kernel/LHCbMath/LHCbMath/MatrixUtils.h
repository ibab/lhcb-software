// $Id: MatrixUtils.h,v 1.4 2007-11-28 08:54:38 cattanem Exp $
// ============================================================================
#ifndef MATRIXUTILS_H
#define MATRIXUTILS_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <algorithm>
#include <functional>
#include <utility>
#include <cmath>
// ============================================================================
// ROOT
// ============================================================================
#include "Math/SMatrix.h"
#include "Math/SVector.h"
#include "Math/Point3D.h"
#include "Math/Vector4D.h"
#include "Math/Vector3D.h"
// ============================================================================

namespace Gaudi
{
  namespace Math 
  {
    /** fill  Linear Algebra - vector from 3D-point
     *
     *  @code
     *   
     *  const Gaudi::XYZPoint xyzv = ... ;
     *  Gaudi::Vector4 lav ;
     *  
     *  // fill Linear Algebra vector from 3D-point
     *  geo2LA ( xyzv , lav ) ; 
     *
     *  @endcode 
     *  
     *  @param[in]  source 3D-point
     *  @param[out] dest   Linear Algebra vector 
     *  @return linear algebra vector 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-05-24
     */
    template <class C,class T>
    inline ROOT::Math::SVector<T,3>& 
    geo2LA 

    ( const ROOT::Math::PositionVector3D<C>& source , 
      ROOT::Math::SVector<T,3>&              dest   ) 
    {
      dest[0] = source.X () ;
      dest[1] = source.Y () ;
      dest[2] = source.Z () ;
      return dest ;
    } ;
    /** fill  Linear Algebra - vector from 3D-vector 
     *
     *  @code
     *   
     *  const Gaudi::XYZVector xyzv = ... ;
     *  Gaudi::Vector4 lav ;
     *  
     *  // fill Linear Algebra vector from 3D-Vector 
     *  geo2LA ( xyzv , lav ) ; 
     *
     *  @endcode 
     *  
     *  @param source (input)  3D-Vector 
     *  @param dest   (output) Linear Algebra vector 
     *  @return linear algebra vector 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-05-24
     */
    template <class C,class T>
    inline ROOT::Math::SVector<T,3>& 
    geo2LA 
    ( const ROOT::Math::DisplacementVector3D<C>& source , 
      ROOT::Math::SVector<T,3>&                  dest   ) 
    {
      dest[0] = source.X () ;
      dest[1] = source.Y () ;
      dest[2] = source.Z () ;
      return dest ;
    } ;
    /** fill  Linear Algebra - vector from 4D-vector 
     *
     *  @code
     *   
     *  const Gaudi::LorenztVector lorv = ... ;
     *  Gaudi::Vector4 lav ;
     *  
     *  // fill Linear Algebra vector from Lorenz Vector 
     *  geo2LA ( lorv , lav ) ; 
     *
     *  @endcode 
     *  
     *  @param source (input) Lorentz Vector 
     *  @param dest   (output) Linear Algebra vector 
     *  @return linear algebra vector 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-05-24
     */
    template <class C,class T>
    inline ROOT::Math::SVector<T,4>& 
    geo2LA 
    ( const ROOT::Math::LorentzVector<C>& source , 
      ROOT::Math::SVector<T,4>&           dest  ) 
    {
      dest[0] = source.X () ;
      dest[1] = source.Y () ;
      dest[2] = source.Z () ;
      dest[3] = source.E () ;
      return dest ;
    } ;


   

    /** construct similarity("chi2") using 3D-vector 
     *
     *  E.g. one can ask the "chi2"-distance inbetween vertices:
     *
     *  @code 
     *
     *  const LHCb::Vertex* v1 = ... ;
     *  const LHCb::Vertex* v2 = ... ;
     *  
     *  int ifail = 0 ;
     *  // evaluate the chi2 distance 
     *  const double chi2 = Gaudi::Math::Similarity
     *     ( v1->position() - v2.position() , 
     *      ( v1->covMatrix() + v2->covMatrix() ).Sinverse( ifail ) ) ;
     *  if ( 0 != ifail ) { ... error here ... } ;
     *
     *  always() << " Chi2 distance between vertices is " << chi2 << endreq ;
     *
     *  @endcode 
     *
     *  @param matrix (input) symmetric (3x3) matrix used for similarity
     *  @param delta  (input) 3D- vector 
     *  @return reult of v^T*M*v (similarity) operation
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-05-24
     */ 
    template <class C,class T>
    inline T 
    Similarity
    ( const ROOT::Math::DisplacementVector3D<C>&                     delta  , 
      const ROOT::Math::SMatrix<T,3,3,ROOT::Math::MatRepSym<T,3> > & matrix ) 
    {
      ROOT::Math::SVector<T,3> tmp ;
      return ROOT::Math::Similarity ( geo2LA ( delta , tmp )  , matrix ) ;
    } ;
    /** construct similarity("chi2") using 3D-vector 
     *
     *  E.g. one can ask the "chi2"-distance inbetween vertices:
     *
     *  @code 
     *
     *  const LHCb::Vertex* v1 = ... ;
     *  const LHCb::Vertex* v2 = ... ;
     *  
     *  int ifail = 0 ;
     *  // evaluate the chi2 distance 
     *  const double chi2 = Gaudi::Math::Similarity
     *    ( ( v1->covMatrix() + v2->covMatrix() ).Sinverse( ifail ) ,
     *        v1->position() - v2.position() ) ;
     *  if ( 0 != ifail ) { ... error here ... } ;
     *
     *  always() << " Chi2 distance between vertices is " << chi2 << endreq ;
     *
     *  @endcode 
     *
     *  @param matrix (input) symmetric (3x3) matrix used for similarity
     *  @param delta  (input) 3D- vector 
     *  @return result of v^T*M*v (similarity) operation
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-05-24
     */ 
    template <class C,class T>
    inline T 
    Similarity
    ( const ROOT::Math::SMatrix<T,3,3,ROOT::Math::MatRepSym<T,3> > & matrix , 
      const ROOT::Math::DisplacementVector3D<C>&                     delta  ) 
    { return Similarity ( delta , matrix ) ; }
    /** construct similarity("chi2") using 4D-vector 
     *
     *  E.g. one can ask the "chi2"-distance inbetween momenta of particles
     *
     *  @code 
     *
     *  const LHCb::Particle* p1 = ... ;
     *  const LHCb::Particle* p2 = ... ;
     *  
     *  int ifail = 0 ;
     *  // evaluate the chi2 distance 
     *  const double chi2 = Gaudi::Math::Similarity
     *     (  p1->momentum() - p2.momentum() ,
     *     ( p1->momCovMatrix() + p2->momCovMatrix() ).Sinverse( ifail ) ) ;
     *  if ( 0 != ifail ) { ... error here ... } ;
     *
     *  always() << " Chi2 distance in momenta is " << chi2 << endreq ;
     *
     *  @endcode 
     *
     *  @param delta  (input) Lorentz vector 
     *  @param matrix (input) symmetric (4x4) matrix used for similarity
     *  @return result of v^T*M*v (similarity) operation
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-05-24
     */ 
    template <class C,class T>
    inline T 
    Similarity
    ( const ROOT::Math::LorentzVector<C>&                            delta  , 
      const ROOT::Math::SMatrix<T,4,4,ROOT::Math::MatRepSym<T,4> > & matrix ) 
    {
      ROOT::Math::SVector<T,4> tmp ;
      return ROOT::Math::Similarity ( geo2LA( delta , tmp ) , matrix ) ;
    } ;
    /** construct similarity("chi2") using 4D-vector 
     *
     *  E.g. one can ask the "chi2"-distance inbetween momenta of particles
     *
     *  @code 
     *
     *  const LHCb::Particle* p1 = ... ;
     *  const LHCb::Particle* p2 = ... ;
     *  
     *  int ifail = 0 ;
     *  // evaluate the chi2 distance 
     *  const double chi2 = Gaudi::Math::Similarity
     *    * ( p1->momCovMatrix() + p2->momCovMatrix() ).Sinverse( ifail ) , 
     *       p1->momentum() - p2.momentum() ) ;
     *  if ( 0 != ifail ) { ... error here ... } ;
     *
     *  always() << " Chi2 distance in momenta is " << chi2 << endreq ;
     *
     *  @endcode 
     *
     *  @param matrix (input) symmetric (4x4) matrix used for similarity
     *  @param delta  (input) Lorentz vector 
     *  @return result of v^T*M*v (similarity) operation
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-05-24
     */ 
    template <class C,class T>
    inline T 
    Similarity
    ( const ROOT::Math::SMatrix<T,4,4,ROOT::Math::MatRepSym<T,4> > & matrix ,
      const ROOT::Math::LorentzVector<C>&                            delta  ) 
    { return Similarity ( delta , matrix ) ; } ;
    /** increment  LorentzVector with 4-component linear vector 
     *  
     *  @code 
     *
     *  Gaudi::LorentzVector v1 = ... ;
     *  const Gaudi::Vector4       v2 = ... ;
     *  
     *  // update Lorentz vector with LA vector:
     *  Gaudi::Math::add ( v1 , v2 ) ;
     *
     *  @endcode 
     *  
     *  @param v1 (input/output) LorentzVector to be updated 
     *  @param v2 (input) Linear Algebra vector, to be added to LorentzVector 
     *  @return the updated LorenzVector 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-05-24
     */
    template <class C, class T>
    inline ROOT::Math::LorentzVector<C>& 
    add 
    ( ROOT::Math::LorentzVector<C>& v1 , const ROOT::Math::SVector<T,4> & v2 ) 
    { return v1 += ROOT::Math::LorentzVector<C>( v2[0] , v2[1] , v2[2] , v2[3] ) ; }
    
    /** increment the symmetric matrix with "symmetrized" part of other matrix
     * 
     *  @code 
     * 
     *  Gaudi::SymMatrix3x3 matrix = ... ;
     *  const Gaudi::Matrix3x3    other  = ... ;
     * 
     *  // update "matrix" with the upper triangular part of "other"
     *  Gaudi::Math::add ( matrix , other ) ;
     *
     *  @endcode 
     * 
     *  @param matrix symmetric matrix to be updated 
     *  @param other matrix, upper traingel is used for updating
     *  @return the updated symmetric matrix 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-05-24
     */
    template <class T1,class T2,unsigned int D, class R>
    inline 
    ROOT::Math::SMatrix<T1,D,D,ROOT::Math::MatRepSym<T1,D> >& 
    add
    ( ROOT::Math::SMatrix<T1,D,D,ROOT::Math::MatRepSym<T1,D> >& matrix , 
      const ROOT::Math::SMatrix<T2,D,D,R>&                      other  ) 
    {
      for   ( unsigned int i = 0 ; i < D ; ++i ) 
      { for ( unsigned int j = i ; j < D ; ++j ) { matrix(i,j) += other(i,j); } }
      return matrix ;
    } ;
    /** set all elements of vector equal to some scalar value 
     * 
     *  @code 
     *
     *  Gaudi::Vector3      vct   = ... ;
     * 
     *  // set all elements to be equal to 10.0 :
     *  Gaudi::Math::setToScalar( vct  , 10.0 ) ;
     *  // set all elements to be eual to 0 :
     *  Gaudi::Math::setToScalar( vct  ) ;
     * 
     *  @endcode 
     *  
     *  @param m      (input/output)vector  to be modified
     *  @param value  (input) new value for all vector elements 
     *  @return number of modified vector elemenets (for consistency)
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-05-24
     */
    template <class T, unsigned int D>
    inline size_t 
    setToScalar
    ( ROOT::Math::SVector<T,D>& m , const T& value = T() ) 
    { 
      std::fill ( m.begin() , m.end() , value ) ; 
      return D ;
    } ;
    /** set all elements of matrix equal to some scalar value 
     * 
     *  @code 
     *
     *  Gaudi::Matrix4x3    mtrx1 = ... ;
     *  Gaudi::SymMatrix5x5 mtrx2 = ... ; 
     * 
     *  // set all elements to be equal to 10.0 :
     *  Gaudi::Math::setToScalar( mtrx1 , 10.0 ) ;
     *  // set all elements to be eual to 0 :
     *  Gaudi::Math::setToScalar( mtrx2 , 10.0 ) ;
     * 
     *  @endcode 
     *  
     *  @param m      (input/output) matrix to be modified
     *  @param value  (input) new value for all matrix elements 
     *  @return number of modified matrix elemenets 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-05-24
     */
    template <class T, unsigned int D1, unsigned int D2, class R>
    inline size_t 
    setToScalar
    ( ROOT::Math::SMatrix<T,D1,D2,R>& m , const T& value = T() ) 
    { 
      std::fill ( m.begin() , m.end() , value ) ; 
      return m.end() - m.begin() ;
    } ;
    /** set square matrix to be proportional to unit matrix 
     *  
     *  @code
     * 
     *  Gaudi::Matrix4x4    mtrx1 = ... ;
     *  Gaudi::SymMatrix3x3 mtrx2 = ... ;
     * 
     *  // set mtrx1 to be equal unit matrix 
     *  Gaudi::Math::setToUnit ( mtrx1 ) ; 
     *  // set mtrx2 to be equal unit matrix, multiplied by 2  
     *  Gaudi::Math::setToUnit ( mtrx2 , 2.0 ) ;
     *  @endcode 
     * 
     *  @param[in,out] m  matrix to be modified 
     *  @param[in] value  value to be used as diagonal elements
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-05-24
     */  
    template <class T, unsigned int D, class R>
    inline size_t 
    setToUnit
    ( ROOT::Math::SMatrix<T,D,D,R>& m , const T& value = T(1) ) 
    { 
      /// nullify the matrix:
      std::fill ( m.begin() , m.end() , T(0.0) ) ; 
      /// set diagonal elements 
      for ( unsigned int i = 0 ; i < D ; ++i ) { m(i,i) = value ; }
      return m.end() - m.begin() ;
    } ;    
    /** efficient scale all elements of the matrix
     *
     *  @code 
     * 
     *  Gaudi::Matrix4x3    mtrx1 = ...  ;
     *  Gaudi::SymMatrix5x5 mtrx2 = ... ;
     *
     *  // multiply all elements by 100:
     *  Gaudi::Math::scale ( mtrx1 , 100.0   ) ;
     *
     *  // divide all elements by 4 :
     *  Gaudi::Math::scale ( mtrx2 , 1.0/4.0 ) ;
     *  
     *  @endcode  
     * 
     *  @param m      (input/output) matrix to be modified
     *  @param value  (input) scaling coefficient 
     *  @return number of modified matrix elemenets 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-05-24
     */
    template <class T, unsigned int D1, unsigned int D2, class R>
    inline size_t 
    scale 
    ( ROOT::Math::SMatrix<T,D1,D2,R>& m , const T& value ) 
    { 
      typedef typename ROOT::Math::SMatrix<T,D1,D2,R>::iterator iterator ;
      for ( iterator it = m.begin() ;  m.end() != it ; ++it ) { (*it) *= value ; }
      return m.end() - m.begin() ;
    } ;
    /** efficient scale all elements of the vector
     *
     *  @code 
     * 
     *  Gaudi::Vector4      vct = ...  ;
     *
     *  // multiply all elements by 100:
     *  Gaudi::Math::scale ( vct , 100.0   ) ;
     *
     *  @endcode  
     * 
     *  @param m      (input/output) vector to be modified
     *  @param value  (input) scaling coefficient 
     *  @return number of modified vector elemenets  (for consistency) 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-05-24
     */
    template <class T, unsigned int D>
    inline size_t 
    scale 
    ( ROOT::Math::SVector<T,D>& m , const T& value ) 
    { 
      typedef typename ROOT::Math::SVector<T,D>::iterator iterator ;
      for ( iterator it = m.begin() ;  m.end() != it ; ++it ) { (*it) *= value ; }
      return D ;
    } ;
    /** @struct _AbsCompare
     *  The trivial structure for comparison of "numbers" by the absolute value 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-05-25
     */
    template <class T>
    struct _AbsCompare : public std::binary_function<T,T,bool>
    {
      inline bool operator() ( const T v1 , const T v2 ) const 
      { return ::fabs( v1 ) < ::fabs( v2 ) ; }
    } ;


    /** find the maximal element in matrix 
     *  @param m (input) matrix to be studied
     *  @return the maximal element 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-05-24
     */
    template <class T,unsigned int D1,unsigned int D2, class R>
    inline T 
    max_element 
    ( const ROOT::Math::SMatrix<T,D1,D2,R>& m ) 
    { return *std::max_element ( m.begin() , m.end() ) ; }
    /** find the minimal element in matrix 
     *  @param m (input) matrix to be studied
     *  @return the minimal element 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-05-24
     */
    template <class T,unsigned int D1,unsigned int D2, class R>
    inline T 
    min_element 
    ( const ROOT::Math::SMatrix<T,D1,D2,R>& m ) 
    { return *std::min_element ( m.begin() , m.end() ) ; }
    /** find the maximal element in vector 
     *  @param m (input) vector to be studied
     *  @return the maximal element 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-05-24
     */
    template <class T,unsigned int D>
    inline T 
    max_element 
    ( const ROOT::Math::SVector<T,D>& m ) 
    { return *std::max_element ( m.begin() , m.end() ) ; }
    /** find the minimal element in vector 
     *  @param m (input) vector to be studied
     *  @return the minimal element 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-05-24
     */
    template <class T,unsigned int D>
    inline T 
    min_element 
    ( const ROOT::Math::SVector<T,D>& m ) 
    { return *std::min_element ( m.begin() , m.end() ) ; }
    /** find the element in matrix with the maximal absolute value  
     *
     *  @code
     *  
     *  const Gaudi::Matrix4x3 mtrx1 = ... ;
     *  const Gaudi::Matrix4x3 mtrx2 = ... ;
     * 
     *  // find the maximal absolute difference between elements : 
     *  const double diff  = Gaudi::Math::maxabs_element ( mtrx1 - mtrx2 ) ;
     *  if ( diff < tolerance ) 
     *   {
     *      std::cout << " matrices are almost identical " << std::endl ;
     *   }
     *  
     *  @endcode 
     *  
     *  @param m (input) matrix to be studied
     *  @return the element with the maximal absolute value 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-05-24
     */
    template <class T,unsigned int D1,unsigned int D2, class R>
    inline T 
    maxabs_element 
    ( const ROOT::Math::SMatrix<T,D1,D2,R>& m ) 
    { return *std::max_element ( m.begin() , m.end()  , _AbsCompare<T>() ) ; }
    /** find the element in matrix with the minimal absolute value  
     *  @param m (input) matrix to be studied
     *  @return the element with the minimal absolute value 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-05-24
     */
    template <class T,unsigned int D1,unsigned int D2, class R>
    inline T 
    minabs_element 
    ( const ROOT::Math::SMatrix<T,D1,D2,R>& m ) 
    { return *std::min_element ( m.begin() , m.end()  , _AbsCompare<T>() ) ; }
    /** find an index of the  maximal element in matrix 
     *  @param m (input) matrix to be studied
     *  @param cmp comparison criteria
     *  @return the pair (i,j)-index of the maximal element 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-05-24
     */
    template <class T,unsigned int D1,unsigned int D2, class R, class CMP>
    inline std::pair<unsigned int,unsigned int> 
    ind_max_element 
    ( const ROOT::Math::SMatrix<T,D1,D2,R>& m , CMP cmp ) 
    { 
      std::pair<unsigned int,unsigned int> result (0,0) ;
      T tmp = m(0,0) ;
      for ( unsigned int i = 0 ; i < D1 ; ++i ) 
      {
        for ( unsigned int j = 0 ; j < D2 ; ++j ) 
        {
          const T val = m(i,j) ;
          if ( !cmp( tmp , val ) ) { continue ; }
          tmp = val ; 
          result.first  = i ; 
          result.second = j ;
        }
      }
      return result ;
    } ;
    /** find an index of the maximal element in symmetric matrix 
     *  @param m (input) symmetric matrix to be studied
     *  @param cmp comparison criteria
     *  @return the pair (i,j)-index of the maximal element 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-05-24
     */
    template <class T,unsigned int D, class CMP>
    inline std::pair<unsigned int,unsigned int> 
    ind_max_element 
    ( const ROOT::Math::SMatrix<T,D,D,ROOT::Math::MatRepSym<T,D> >& m , CMP cmp ) 
    { 
      std::pair<unsigned int,unsigned int> result (0,0) ;
      T tmp = m(0,0) ;
      for ( unsigned int i = 0 ; i < D ; ++i ) 
      {
        for ( unsigned int j = i ; j < D ; ++j ) 
        {
          const T val = m(i,j) ;
          if ( !cmp ( tmp , val ) ) { continue ; }
          tmp = val ; 
          result.first  = i ; 
          result.second = j ;
        }
      }
      return result ;
    } ;    
    /** find an index of the  maximal element in matrix 
     *  @param m (input) matrix to be studied
     *  @return the pair (i,j)-index of the maximal element 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-05-24
     */
    template <class T,unsigned int D1,unsigned int D2, class R>
    inline std::pair<unsigned int,unsigned int> 
    ind_max_element 
    ( const ROOT::Math::SMatrix<T,D1,D2,R>& m ) 
    { return ind_max_element ( m , std::less<T>() ) ; } ;
    /** find an index of the minimal element in matrix 
     *  @param m (input) matrix to be studied
     *  @param cmp comparison criteria
     *  @return the pair (i,j)-index of the maximal element 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-05-24
     */
    template <class T,unsigned int D1,unsigned int D2, class R, class CMP>
    inline std::pair<unsigned int,unsigned int> 
    ind_min_element 
    ( const ROOT::Math::SMatrix<T,D1,D2,R>& m , CMP cmp ) 
    { 
      std::pair<unsigned int,unsigned int> result (0,0) ;
      T tmp = m(0,0) ;
      for ( unsigned int i = 0 ; i < D1 ; ++i ) 
      {
        for ( unsigned int j = 0 ; j < D2 ; ++j ) 
        {
          const T val = m(i,j) ;
          if ( !cmp( val , tmp ) ) { continue ; }
          tmp = val ; 
          result.first  = i ; 
          result.second = j ;
        }
      }
      return result ;
    } ;
    /** find an index of the minimal element in symmetric matrix 
     *  @param m (input) symmetric matrix to be studied
     *  @param cmp comparison criteria
     *  @return the pair (i,j)-index of the maximal element 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-05-24
     */
    template <class T,unsigned int D, class CMP>
    inline std::pair<unsigned int,unsigned int> 
    ind_min_element 
    ( const ROOT::Math::SMatrix<T,D,D,ROOT::Math::MatRepSym<T,D> >& m , CMP cmp ) 
    { 
      std::pair<unsigned int,unsigned int> result (0,0) ;
      T tmp = m(0,0) ;
      for ( unsigned int i = 0 ; i < D ; ++i ) 
      {
        for ( unsigned int j = i ; j < D ; ++j ) 
        {
          const T val = m(i,j) ;
          if ( !cmp ( tmp , val ) ) { continue ; }
          tmp = val ; 
          result.first  = i ; 
          result.second = j ;
        }
      }
      return result ;
    } ;
    /** find an index of the minimal element in the matrix 
     *
     *  @code 
     *  
     *  const Gaudi::Matrix4x4    mtrx1 = ... ;
     *  const Gaudi::SymMatrix5x5 mtrx2 = ... ;
     * 
     *  typedef std::pair<unsigned int,unsigned int> PAIR ;
     *  
     *  // get the minimal element in mtrx1:
     *  PAIR index1 = Gaudi::Math::ind_max_element ( mtrx1 ) ;
     *
     *  // get the minimal element in mtrx2:
     *  PAIR index2 = Gaudi::Math::ind_max_element ( mtrx2 ) ;
     *
     *  @endcode 
     *
     *  @param m (input) matrix to be studied 
     *  @return the pair of indices for (the first) minimal element 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-05-24
     */
    template <class T, unsigned int D1, unsigned int D2, class R>
    inline std::pair<unsigned int,unsigned int> 
    ind_min_element 
    ( const ROOT::Math::SMatrix<T,D1,D2,R>& m ) 
    { return ind_min_element( m , std::less<T>() ) ; } ;
    /** find an index of the maximal element in the vector 
     *  @param m (input) vector to be studied
     *  @param cmp comparison criteria
     *  @return the index of the maximal element 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-05-24
     */
    template <class T,unsigned int D, class CMP>
    inline unsigned int 
    ind_max_element 
    ( const ROOT::Math::SVector<T,D>& m , CMP cmp )
    { return std::max_element( m.begin() , m.end() , cmp ) - m.begin() ; }    
    /** find an index of the minimal element in the vector 
     *  @param m (input) vector to be studied
     *  @param cmp comparison criteria
     *  @return the index of the minimal  element 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-05-24
     */
    template <class T,unsigned int D, class CMP>
    inline unsigned int 
    ind_min_element 
    ( const ROOT::Math::SVector<T,D>& m , CMP cmp )
    { return std::min_element( m.begin() , m.end() , cmp ) - m.begin() ; }
    /** find an index of the maximal element in the vector 
     *  @param m (input) vector to be studied
     *  @return the index of the maximal element 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-05-24
     */
    template <class T,unsigned int D>
    inline unsigned int 
    ind_max_element 
    ( const ROOT::Math::SVector<T,D>& m )
    { return std::max_element( m.begin() , m.end() ) - m.begin() ; }    
    /** find an index of the minimal element in the vector 
     *  @param m (input) vector to be studied
     *  @return the index of the minimal element 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-05-24
     */
    template <class T,unsigned int D>
    inline unsigned int 
    ind_min_element 
    ( const ROOT::Math::SVector<T,D>& m )
    { return std::min_element ( m.begin() , m.end() ) - m.begin() ; }    
    /** find an index of the element with the maximal absolute value 
     *
     *  @code 
     *  
     *  const Gaudi::Matrix4x4    mtrx1 = ... ;
     *  const Gaudi::SymMatrix5x5 mtrx2 = ... ;
     * 
     *  typedef std::pair<unsigned int,unsigned int> PAIR ;
     *  
     *  // get the element with the maximal absolute value in mtrx1:
     *  PAIR index1 = Gaudi::Math::ind_maxabs_element ( mtrx1 ) ;
     *
     *  // get the element with the maximal absolute value in mtrx2:
     *  PAIR index2 = Gaudi::Math::ind_maxabs_element ( mtrx2 ) ;
     *
     *  @endcode 
     *
     *  @param m (input) matrix to be studied 
     *  @return the pair of indices for the element with 
     *          the maximal absolute value 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-05-24
     */
    template <class T, unsigned int D1, unsigned int D2, class R>
    inline std::pair<unsigned int,unsigned int> 
    ind_maxabs_element 
    ( const ROOT::Math::SMatrix<T,D1,D2,R>& m ) 
    { return ind_max_element( m , _AbsCompare<T>() ) ; } ;
    /** find an index of the element with the minimal absolute value 
     *
     *  @code 
     *  
     *  const Gaudi::Matrix4x4    mtrx1 = ... ;
     *  const Gaudi::SymMatrix5x5 mtrx2 = ... ;
     * 
     *  typedef std::pair<unsigned int,unsigned int> PAIR ;
     *  
     *  // get the element with the minimal absolute value in mtrx1:
     *  PAIR index1 = Gaudi::Math::ind_minabs_element ( mtrx1 ) ;
     *
     *  // get the element with the minimal absolute value in mtrx2:
     *  PAIR index2 = Gaudi::Math::ind_minabs_element ( mtrx2 ) ;
     *
     *  @endcode 
     *
     *  @param m (input) matrix to be studied 
     *  @return the pair of indices for the element with 
     *          the maximal absolute value 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-05-24
     */    
    template <class T, unsigned int D1, unsigned int D2, class R>
    inline std::pair<unsigned int,unsigned int> 
    ind_minabs_element 
    ( const ROOT::Math::SMatrix<T,D1,D2,R>& m ) 
    { return ind_min_element( m , _AbsCompare<T>() ) ; } ;
    /** find an index of the element with maximal absolute value
     *  @param m (input) vector to be studied
     *  @return the index of the element with the maximal absolute value 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-05-24
     */
    template <class T,unsigned int D>
    inline unsigned int 
    ind_maxabs_element 
    ( const ROOT::Math::SVector<T,D>& m )
    { return ind_max_element ( m , _AbsCompare<T>() ) ; }    
    /** find an index of the element with minimal absolute value 
     *  @param m (input) vector to be studied
     *  @return the index of the element with minimal absolute value 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-05-24
     */
    template <class T,unsigned int D>
    inline unsigned int 
    ind_minabs_element 
    ( const ROOT::Math::SVector<T,D>& m )
    { return ind_min_element ( m , _AbsCompare<T>() ) ; }
    /** evaluate the trace (sum of diagonal elements) of the square matrix 
     *
     *  @code 
     *  
     *  const Gaudi::Matrix4x4    mtrx1 = ... ;
     *  const Gaudi::SymMatrix3x3 mtrx2 = ... ;
     * 
     *  // evaluate the trace of mtrx1:
     *  const double trace1 = Gaudi::Math::trace ( mtrx1 ) ;
     *  // evaluate the trace of mtrx2:
     *  const double trace2 = Gaudi::Math::trace ( mtrx2 ) ;
     *
     *  @endcode 
     *
     *  @param m (input) matrix to be studied 
     *  @return trace (sum of diagonal elements) of the matrix 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-05-24
     */
    template <class T, unsigned int D, class R>
    inline T 
    trace 
    ( const ROOT::Math::SMatrix<T,D,D,R>& m ) 
    {
      T result = m(0,0) ;
      for ( unsigned int i = 1 ; i < D ; ++i ) { result += m(i,i) ; }
      return result ;
    } ;
    /** find the minimal diagonal element 
     *  @param   m (input) matrix to be studied 
     *  @param   cmp comparison criteria
     *  @return "min" diagonal element (in the sense of comparison criteria)
     */
    template <class T, unsigned int D, class R, class CMP>
    inline T 
    min_diagonal 
    ( const ROOT::Math::SMatrix<T,D,D,R>& m , CMP cmp ) 
    {
      T result = m(0,0);
      for ( unsigned int i = 1 ; i < D ; ++i ) 
      { 
        const T value = m(i,i) ;  
        if ( cmp ( value , result ) ) { result = value ; }
      }
      return result ;
    }
    /** find the maximal diagonal element 
     *  @param   m (input) square matrix to be studied 
     *  @param   cmp comparison criteria
     *  @return "max" diagonal element (in the sense of comparison criteria)
     */
    template <class T, unsigned int D, class R, class CMP>
    inline T 
    max_diagonal 
    ( const ROOT::Math::SMatrix<T,D,D,R>& m , CMP cmp ) 
    {
      T result = m(0,0);
      for ( unsigned int i = 1 ; i < D ; ++i ) 
      { 
        const T value = m(i,i) ;  
        if ( cmp ( result , value ) ) { result = value ; }
      }
      return result ;
    } ;
    /** find the maximal diagonal element of square matrix 
     *
     *  @code 
     *
     *  const Gaudi::Matrix4x4    mtrx1 = ... ;
     *  const Gaudi::SymMatrix3x3 mtrx2 = ... ;
     *  
     *  // find the maximal diagonal element of mtrx1 
     *  const double m1 = Gaudi::Math::max_diagonal ( mtrx1 ) ;
     *
     *  // find the maximal diagonal element of mtrx2 
     *  const double m2 = Gaudi::Math::max_diagonal ( mtrx2 ) ;
     *
     *  @endcode
     *
     *  @param m (input) square matrix to be studied 
     *  @return the maximal diagonal element 
     *  @author Vanya BELYAEV ibelyaev@physics.cyr.edu
     *  @date 2006-05-24
     */
    template <class T, unsigned int D, class R>
    inline T 
    max_diagonal 
    ( const ROOT::Math::SMatrix<T,D,D,R>& m ) 
    { return max_diagonal( m , std::less<T>() ) ; } ;    
    /** find the maximal diagonal element of the square matrix 
     *
     *  @code 
     *
     *  const LHCb::Vertex* v = ..
     *  const Gaudi::Matrix3x3& covariance = v->covMatrix() ;
     *  
     *  // find the minimal diagonal element of covariance matrix:
     *  const double m1 = Gaudi::Math::min_diagonal ( covariance  ) ;
     *
     *  if ( 0 >= 0 ) 
     *  {
     *     std::err 
     *      << " Invlid covarinace matrix" 
     *      << " Non-positive elements on diagonal << m1 << std::endl ;
     *  }
     *
     *  @endcode
     *
     *  @param m (input) square matrix to be studied 
     *  @return the maximal diagonal element 
     *  @author Vanya BELYAEV ibelyaev@physics.cyr.edu
     *  @date 2006-05-24
     */
    template <class T, unsigned int D, class R>
    inline T 
    min_diagonal 
    ( const ROOT::Math::SMatrix<T,D,D,R>& m ) 
    { return min_diagonal( m , std::less<T>() ) ; } ;    
    /** find the diagonal element of square matrix with maximal absolute value 
     *
     *  @code 
     *
     *  const Gaudi::Matrix4x4    mtrx1 = ... ;
     *  const Gaudi::SymMatrix3x3 mtrx2 = ... ;
     *  
     *  // find the diagonal element of mtrx1 with the maximal absolute value 
     *  const double m1 = Gaudi::Math::max_diagonal ( mtrx1 ) ;
     *
     *  // find the diagonal element of mtrx1 with the maximal absolute value 
     *  const double m2 = Gaudi::Math::max_diagonal ( mtrx2 ) ;
     *
     *  @endcode
     *
     *  @param m (input) square matrix to be studied 
     *  @return the diagonal element withmaximal absolute value 
     *  @author Vanya BELYAEV ibelyaev@physics.cyr.edu
     *  @date 2006-05-24
     */
    template <class T, unsigned int D, class R>
    inline T 
    maxabs_diagonal 
    ( const ROOT::Math::SMatrix<T,D,D,R>& m ) 
    { return max_diagonal( m , _AbsCompare<T>() ) ; } ;
    /** find the diagonal element of the square matrix with 
     *  the minimal absolute value 
     *
     *  @code 
     *
     *  const LHCb::Vertex* v = ..
     *  const Gaudi::Matrix3x3& covariance = v->covMatrix() ;
     *  
     *  // find the diagonal element with minimal absolute value 
     *  const double m1 = Gaudi::Math::min_diagonal ( covariance  ) ;
     *
     *  if ( 0.001 * Gaudi::Units::micrometer ) 
     *  {
     *     std::err 
     *      << " Non-realistic element on diagonal << m1 << std::endl ;
     *  }
     *
     *  @endcode
     *
     *  @param m (input) square matrix to be studied 
     *  @return the diagonal element with the minimal absolute value 
     *  @author Vanya BELYAEV ibelyaev@physics.cyr.edu
     *  @date 2006-05-24
     */
    template <class T, unsigned int D, class R>
    inline T 
    minabs_diagonal 
    ( const ROOT::Math::SMatrix<T,D,D,R>& m ) 
    { return min_diagonal( m , _AbsCompare<T>() ) ; } ;
    /** count the number of elements in matrix, which satisfy the certain criteria
     * 
     *  @code
     * 
     *  const Gaudi::Matrix4x4 matrix = ... ;
     *  
     *  // number of NULL elements:
     *  const size_t nulls = 
     *      Gaudi::Math::count_if ( matrix , 
     *       std::bind2nd( std::equal_to<double>() , 0.0 ) ) ;
     *
     *  // number of elements in excess of 100.0 
     *  const size_t large = 
     *      Gaudi::Math::count_if ( matrix , 
     *       std::bind2nd( std::greater<double>() , 100.0 ) ) ;
     * 
     *  // number of elements which are less then 0.01 in absolute value 
     *  const size_t small = 
     *      Gaudi::Math::count_if ( matrix , 
     *       std::bind2nd( _AbsCompare<double>() , 0.01 ) ) ;
     *  
     * 
     *  @endcode 
     *  
     *  @param m    (input) matrix to be studied 
     *  @param pred (input) predicate to be tested 
     *  @return number of elements for which the predicate is valid 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-04-24
     */
    template <class T, unsigned int D1, unsigned int D2, class R, class P>
    inline size_t 
    count_if 
    ( const ROOT::Math::SMatrix<T,D1,D2,R>& m , P pred )
    { return std::count_if ( m.begin() , m.end() , pred ) ; }
    /** count the number of elements in matrix, which satisfy the certain criteria
     * 
     *  @code
     * 
     *  const Gaudi::SymMatrix4x4 matrix = ... ;
     *  
     *  // number of NULL elements:
     *  const size_t nulls = 
     *      Gaudi::Math::count_if ( matrix , 
     *       std::bind2nd( std::equal_to<double>() , 0.0 ) ) ;
     *
     *  // number of elements in excess of 100.0 
     *  const size_t large = 
     *      Gaudi::Math::count_if ( matrix , 
     *       std::bind2nd( std::greater<double>() , 100.0 ) ) ;
     * 
     *  // number of elements which are less then 0.01 in absolute value 
     *  const size_t small = 
     *      Gaudi::Math::count_if ( matrix , 
     *       std::bind2nd( Gaudi::Math::_AbsCompare<double>() , 0.01 ) ) ;
     *  
     *  @endcode 
     *  
     *  If one needs check the presence of elements ("at least one")
     *  a bit more efficient algorithm 
     *   <c>Gaudi::Math::check_if</c> should be used 
     *  @see Gaudi::Math::check_if 
     *
     *  @param m    (input) symmetric matrix to be studied 
     *  @param pred (input) predicate to be tested 
     *  @return number of elements for which the predicate is valid 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-04-24
     */
    template <class T, unsigned int D, class P>
    inline size_t 
    count_if 
    ( const ROOT::Math::SMatrix<T,D,D,ROOT::Math::MatRepSym<T,D> >& m , P pred )
    { 
      size_t result = 0 ;
      for ( unsigned int i = 0 ; i < D ; ++i ) 
      {
        if ( pred ( m ( i , i ) ) ) { result += 1 ; }
        for ( unsigned int j = i + 1 ; j < D ; ++j ) 
        {
          if ( pred (  m ( i , j ) ) ) { result +=2 ; }  // ATTENTION! 
        }
      }
      return result ;
    } ;
    /** count number of diagonal elements in matrix, 
     *  which satisfy certain criteria
     * 
     *  @code 
     * 
     *  const LHCb::Vertex* v = ... ;
     *  const Gaudi::SymMatrix3x3& covariance = v->covMatrix() ;
     *
     *  // count number of VERY small (and negative) diagonal elements:
     *  const size_t bad = 
     *   Gaudi::Math::cound_diagonal( covariance , 
     *   std::bind2nd( std::less<double>() , 0.01 * Gaudi::Units::micrometer ) ;
     *  if ( 0 != bad ) 
     *   {
     *      std::cerr << " #bad diagonal elements is " << bad << std::endl ;  
     *   }
     *  
     *  @endcode 
     *  
     *  @param m    (input) square matrix to be studied 
     *  @param pred (input) predicate to be tested 
     *  @return number of diagonal elements for which the predicate is valid 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-04-24
     */
    template <class T, unsigned int D, class R, class P>
    inline size_t 
    count_diagonal
    ( const ROOT::Math::SMatrix<T,D,D,R>& m , P pred )
    {
      size_t result = 0 ;
      for ( unsigned int i = 0 ; i < D ; ++i ) 
      { if ( pred ( m ( i , i ) ) ) { result += 1 ; } }
      return result ;
    } ;
    /** check the presence of at least one element which satisfy the 
     *  criteria
     *
     *  @code 
     * 
     *  const LHCb::Vertex* v = ... ;
     *  const Gaudi::SymMatrix3x3& covariance = v->covMatrix() ;
     *
     *  // check for "infinities" 
     *  const bool bad = 
     *   Gaudi::Math::check_if( covariance , 
     *   std::bind2nd( std::greater<double>() , 1 * Gaudi::Units::meter ) ;
     *  if ( bad ) 
     *   {
     *      std::cerr << " bad elements are detected " << std::endl ; 
     *   }
     *  
     *  @endcode 
     *
     *  In general this algorithm is faster than 
     *   <c>Gaudi::Math::count_if</c>
     *  @see Gaudi::Math::count_if 
     *
     *  @param m    (input)    matrix to be checked 
     *  @param pred (input) predicate tobe tested 
     *  @return true if at least one element is in the matrix 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-04-24
     */
    template <class T, unsigned int D1, unsigned int D2, class R, class P>
    inline bool
    check_if 
    ( const ROOT::Math::SMatrix<T,D1,D2,R>& m , P pred )
    { return m.end() != std::find_if ( m.begin() , m.end() , pred ) ; }
    /** check the presence of at least one diagonal element which satisfy the 
     *  criteria
     *
     *  @code 
     * 
     *  const LHCb::Vertex* v = ... ;
     *  const Gaudi::SymMatrix3x3& covariance = v->covMatrix() ;
     *
     *  // check for "almost nulls"
     *  const bool bad = 
     *   Gaudi::Math::check_diagonal( covariance , 
     *   std::bind2nd( std::less<double>() , 0.001 * Gaudi::Units::micrometer ) ;
     *  if ( bad ) 
     *   {
     *      std::cerr << " bad diagonal elements are detected " << std::endl ; 
     *   }
     *  
     *  @endcode 
     *
     *  @param m    (input) square matrix to be checked 
     *  @param pred (input) predicate to be tested 
     *  @return true if at least one element is in the matrix 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-04-24
     */
    template <class T, unsigned int D, class R, class P>
    inline bool
    check_diagonal 
    ( const ROOT::Math::SMatrix<T,D,D,R>& m , P pred )
    { 
      for ( unsigned int i = 0 ; i < D ; ++i ) 
      { if ( pred ( m ( i , i ) ) ) { return true ; } }
      return false ;
    } ;
    /** check the "equality" of the two matrices by checking 
     *  element-by-element: true == pred( m1(i,j) , m2(i,j) ) 
     *  
     *  It is an efficient way to compare the matrices in the different
     *  represenattions, e.g. symmetrical and non symmetrical matrices 
     *  of the same size. Also the "tolerance" coudl be introduced 
     *  for equality
     *
     *  @code 
     * 
     *  const Gaudi::Matrix4x4 m1 = ... ;
     *  const Gaudi::SymMatrix4x4 m2 = ... ;
     *  
     *   // comparison criteria:
     *   struct Equal : publuc std::binary_fuction<double,double,bool>
     *   {
     *     Equal ( const double value ) : m_threshold ( value ) {} ;
     *     bool operator() ( const double v1 , const double v2 ) const 
     *     {
     *         return ::fabs( v1 , v2 ) < m_threshold ;
     *     }
     *    private: 
     *    double m_threshold ;
     *   } ;
     *  
     *   // "compare" the matrices 
     *   const bool eq = 
     *      Gaudi::Math::equal_if ( m1 , m2 , Equal(0.001) ) ;
     *  
     *  @endcode 
     *  @param m1   (input) the first matrix to be checked 
     *  @param m2   (input) the second matrix to be checked 
     *  @param pred (input) predicate to be tested 
     *  @return true if at least once false == pred( m1(i,j) , m2(i,j) )
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-04-24
     */    
    template <class T1 , class T2 ,
              unsigned int D1 , unsigned int D2 ,
              class R1 , class R2 , class P>
    inline bool 
    equal_if  
    ( const ROOT::Math::SMatrix<T1,D1,D2,R1>& m1 , 
      const ROOT::Math::SMatrix<T2,D1,D2,R2>& m2 , P pred )
    { 
      for ( unsigned int i = 0 ; i < D1 ; ++i ) 
      {
        for ( unsigned int j = 0 ; j < D2 ; ++j ) 
        {
          if ( !pred( m1(i,j) , m2(i,j) ) ) { return false ; } // RETURN 
        }
      }
      return true ;
    } ;
    /** check the "equality" of the two matrices by checking 
     *  element-by-element: true == pred( m1(i,j) , m2(i,j) ) 
     *
     *  The specialization for matrices of the same representation.
     *  From the first principles it should be much more efficient. 
     *
     *  @code 
     * 
     *  const Gaudi::Matrix4x4 m1 = ... ;
     *  const Gaudi::Matrix4x4 m2 = ... ;
     *  
     *   // comparison criteria:
     *   struct Equal : publuc std::binary_fuction<double,double,bool>
     *   {
     *     Equal ( const double value ) : m_threshold ( value ) {} ;
     *     bool operator() ( const double v1 , const double v2 ) const 
     *     {
     *         return ::fabs( v1 , v2 ) < m_threshold ;
     *     }
     *    private: 
     *    double m_threshold ;
     *   } ;
     *  
     *   // "compare" the matrices 
     *   const bool eq = 
     *      Gaudi::Math::equal_if ( m1 , m2 , Equal(0.001) ) ;
     *  
     *  @endcode 
     *  @param m1   (input) the first matrix to be checked 
     *  @param m2   (input) the second matrix to be checked 
     *  @param pred (input) predicate to be tested 
     *  @return true if at least once false == pred( m1(i,j) , m2(i,j) )
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-04-24
     */
    template <class T,unsigned int D1, unsigned int D2,class R,class P>
    inline bool 
    equal_if  
    ( const ROOT::Math::SMatrix<T,D1,D2,R>& m1 , 
      const ROOT::Math::SMatrix<T,D1,D2,R>& m2 , P pred )
    { return std::equal ( m1.begin() , m1.end() , m2.begin() , pred ) ; } ;
    
     /** Fill Lorentz vector from 3D displacement vector + Mass
     *     
     *@code
     *
     *geoLA(xyz, mass, lav)
     *
     @endcode
     *
     * @param source (input) Linear Algebra vector3D
     * @param dest   (output) Lorentz Vector 
     * @return  Lorentz Vector
     * @author Sean BRISBANE sean.brisbane@cern.ch
     * @date 2007-11-27
     */
    template<class T, class C, class M>
    inline ROOT::Math::LorentzVector<C>&
    geo2LA 
    ( const ROOT::Math::SVector<T, 3>& source , const M mass,
      ROOT::Math::LorentzVector<C>& dest  ) 
    {
      // first calculate momentum in carthesian coordinates:
      double p = 1/fabs(source(2)) ;
      double n = sqrt( 1 + source(0)*source(0)+source(1)*source(1)) ;
      double px = p*source(0)/n ;
      double py = p*source(1)/n ;
      double pz = p/n ;
      dest=   ROOT::Math::LorentzVector<C>(px,py,pz,std::sqrt(p*p+mass*mass)) ;
      return dest ;
    } ;

  
    /** Compute the jacobian for the transformation of a covariance matrix
     *  with rows representing track parameters TxTyQop and columns in xyz
     *  into a covariance matrix representing the track parameters in 
     *  PxPyPzE and columns xyz
     *
     * @code 
     *
     * ROOT::Math::SMatrix<T, 3 ,3 ,R> covTxTyQoP_xyz = ....  ;
     * ROOT::Math::SVector<C,3>& particleMomentum=...;
     * ROOT::Math::SMatrix<T, 4 , 3, R > covPxPyPzE_xyz;
     * massOfParticle = ...;
     *
     * ROOT::Math::SMatrix<T,4,3,R> Jacob;
     * JacobdP4dMom (particleMomentum, massOfParticle, Jacob) ;
     * covPxPyPzE_xyz = Jacob * covTxTyQoP_xyz;
     *
     * @endcode 
     *
     * param dP4dMom (input) the txtyqop vector of track/perticle momenta
     * mass (input) the particle mass
     * J (output) the Jacobian for the transformation
     * return J
     * @author Sean BRISBANE sean.brisbane@cern.ch
     * @date 2007-11-27
     */

    template <class T,class R, class M >
    inline void  JacobdP4dMom 
    (const ROOT::Math::SVector<T, 3>mom,
     const M mass, 
     ROOT::Math::SMatrix<R, 4, 3>& J)
    {
      double tx = mom(0) ;
      double ty = mom(1) ;
      double qop = mom(2) ;
      double p  = 1/fabs(qop) ;
      double n2 = 1 + tx*tx + ty*ty ;
      double n  = std::sqrt(n2) ;
      double n3 = n2*n ;
      double px = p*tx/n ;
      double py = p*ty/n ;
      double pz = p/n ;
      double E = sqrt(p*p+mass*mass) ;
      
      J(0,0) = p * (1+ty*ty)/n3 ;// dpx/dtx
      J(0,1) = p * tx * -ty/n3  ;// dpx/dty
      J(0,2) = -px/qop ;// dpx/dqop
      J(1,0) = p * ty * -tx/n3  ;// dpy/dtx
      J(1,1) = p * (1+tx*tx)/n3 ;// dpy/dty
      J(1,2) = -py/qop ;// dpy/dqop
      J(2,0) = pz * -tx/n2 ;// dpz/dtx
      J(2,1) = pz * -ty/n2 ;// dpz/dtx
      J(2,2) = -pz/qop ;// dpz/dqop
      J(3,2) = p/E * -p/qop ;// dE/dqop
      return ; 
    };

  } // end of namespace Math  
} // end of namespace Gaudi

// ============================================================================
// The END 
// ============================================================================
#endif // MATRIXUTILS_H
// ============================================================================
