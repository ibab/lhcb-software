// $Id: MatrixTransforms.h,v 1.2 2008-01-16 07:22:26 cattanem Exp $
// ============================================================================
#ifndef LHCBMATH_MATRIXTRANSFORMS_H 
#define LHCBMATH_MATRIXTRANSFORMS_H 1
// ============================================================================
// Include files
// ============================================================================
// ROOT
// ============================================================================
#include "Math/SMatrix.h"
#include "Math/SVector.h"
#include "Math/Point3D.h"
#include "Math/Vector4D.h"
#include "Math/Vector3D.h"
// ============================================================================
/** @file 
 *  The collection of useful utils to minimize the conversion from 
 *  geometry&kinematical vectors into linear algebra vectors 
 *   In particular it includes:
 *     - conversion from geometry&kinematical vectors into Linear Algebra vectors
 *     - conversion from Linear Algebra vectors into geometry&kinematical vectors
 *     - evaluation of various "chi2"-like values, like chi2-distance between 
 *       two 3D or 4D-vectors. E.g. "vicinity" of two points, or two momenta 
 *     - conversion form "track" to 4-momenta representation (by Sean BRISBANE)
 *     - the transition matrix for the conversion form "track" to 
 *        4-momenta representation (by Sean BRISBANE)
 *
 *  @todo  The file Patricle2State.cpp needs to be updated to use the jacobian 
 *         from the file MatrixTransforms.h
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2008-01-15
 */
// ============================================================================
namespace Gaudi
{
  namespace Math 
  {   
    // ========================================================================
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
     *  @param  source (input)  3D-point
     *  @param  dest   (output) Linear Algebra vector 
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
    } 
    // ========================================================================
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
    } 
    // ========================================================================
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
    } 
    // ========================================================================


    // ========================================================================
    /** fill  3D-point from Linear Algebra vectgor 
     *
     *  @code
     *   
     *  Gaudi::XYZPoint xyzv = ... ;
     *  const Gaudi::Vector4 lav ;
     *  
     *  // fill 3D-point from Linear Algebra vector 
     *  la2geo ( xyzv , lav ) ; 
     *
     *  @endcode 
     *  
     *  @param source (input)  Linear Algebra vector 
     *  @param dest   (output) 3D-point 
     *  @return linear algebra vector 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-05-24
     */
    template <class C,class T>
    inline ROOT::Math::PositionVector3D<C>&
    la2geo 
    ( const ROOT::Math::SVector<T,3>&  source , 
      ROOT::Math::PositionVector3D<C>& dest   ) 
    {
      dest.SetX ( source[0] ) ;      
      dest.SetY ( source[1] ) ;
      dest.SetZ ( source[2] ) ;
      return dest ;
    } 
    // ========================================================================
    /** fill  3D-vector from Linear Algebra vectgor 
     *
     *  @code
     *   
     *  Gaudi::XYZVector xyzv = ... ;
     *  const Gaudi::Vector4 lav ;
     *  
     *  // fill 3D-Vector from Linear Algebra vector 
     *  la2geo ( xyzv , lav ) ; 
     *
     *  @endcode 
     *  
     *  @param source (input)  Linear Algebra vector 
     *  @param dest   (output) 3D-vector 
     *  @return linear algebra vector 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-05-24
     */
    template <class C,class T>
    inline ROOT::Math::DisplacementVector3D<C>&
    la2geo 
    ( const ROOT::Math::SVector<T,3>&      source , 
      ROOT::Math::DisplacementVector3D<C>& dest   ) 
    {
      dest.SetX ( source[0] ) ;      
      dest.SetY ( source[1] ) ;
      dest.SetZ ( source[2] ) ;
      return dest ;
    } 
    // ========================================================================
    /** fill  Lorentz vector from Linear Algebra vectgor 
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
    inline ROOT::Math::LorentzVector<C>&  
    la2geo 
    ( const ROOT::Math::SVector<T,4>& source , 
      ROOT::Math::LorentzVector<C>&   dest   )  
    {
      dest.SetPx ( source[0] ) ;
      dest.SetPy ( source[1] ) ;
      dest.SetPz ( source[2] ) ;
      dest.SetE  ( source[3] ) ;
      return dest ;
    }
    // ========================================================================



    // ========================================================================
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
    } 
    // ========================================================================
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
    // ========================================================================
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
    }
    // ========================================================================
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
    { return Similarity ( delta , matrix ) ; } 
    // ========================================================================


    // ========================================================================
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
    // ========================================================================    
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
     *  @param other matrix, upper traingle is used for updating
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
    }
    // ========================================================================

    // ========================================================================
    /** Fill Lorentz vector from 3D displacement vector + Mass
     *     
     *  @code
     *
     *  geoLA(xyz, mass, lav)
     *
     *  @endcode
     *
     *  @param  source (input) Linear Algebra vector3D
     *  @param  dest   (output) Lorentz Vector 
     *  @return Lorentz Vector
     *  @author Sean BRISBANE sean.brisbane@cern.ch
     *  @date 2007-11-27
     */
    template<class T, class C, class M>
    inline ROOT::Math::LorentzVector<C>&
    geo2LA 
    ( const ROOT::Math::SVector<T, 3>& source , const M mass,
      ROOT::Math::LorentzVector<C>& dest  ) 
    {
      // first calculate momentum in carthesian coordinates:
      const double p = 1/fabs(source(2)) ;
      const double n = sqrt( 1 + source(0)*source(0)+source(1)*source(1)) ;
      const double pz = p/n          ;
      const double px = pz*source(0) ;
      const double py = pz*source(1) ;
      dest.SetPx ( px ) ;
      dest.SetPy ( py ) ;
      dest.SetPz ( pz ) ;
      dest.SetE  ( std::sqrt ( p*p + mass*mass ) ) ;
      return dest ;
    }
    // ========================================================================
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
     * @param dP4dMom (input)  the txtyqop vector of track/perticle momenta
     * @param mass    (input)  the particle mass
     * @param J       (output) the Jacobian for the transformation
     * @author Sean BRISBANE sean.brisbane@cern.ch
     * @date 2007-11-27
     */
    template <class T,class R, class M >
    inline void  JacobdP4dMom 
    ( const ROOT::Math::SVector<T,3>& mom  ,
      const M                         mass , 
      ROOT::Math::SMatrix<R,4,3>&     J    )
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
      
      J(0,0) = p * (1+ty*ty)/n3 ; // dpx/dtx
      J(0,1) = p * tx * -ty/n3  ; // dpx/dty
      J(0,2) = -px/qop ;          // dpx/dqop

      J(1,0) = p * ty * -tx/n3  ; // dpy/dtx
      J(1,1) = p * (1+tx*tx)/n3 ; // dpy/dty
      J(1,2) = -py/qop ;          // dpy/dqop

      J(2,0) = pz * -tx/n2 ;      // dpz/dtx
      J(2,1) = pz * -ty/n2 ;      // dpz/dtx
      J(2,2) = -pz/qop ;          // dpz/dqop
      
      J(3,0) = 0.0          ;     // dE/dtx 
      J(3,1) = 0.0          ;     // dE/dty 
      J(3,2) = p/E * -p/qop ;     // dE/dqop
      
      return ; 
    }
    // ========================================================================
  } // end of namespace Gaudi::Math
} // end of namespace Gaudi
// ============================================================================
// The END
// ============================================================================
#endif // LHCBMATH_XXX_H
// ============================================================================
