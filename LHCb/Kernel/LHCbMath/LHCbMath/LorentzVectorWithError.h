// $Id: LorentzVectorWithError.h,v 1.6 2010-06-08 17:51:53 ibelyaev Exp $
// ============================================================================
#ifndef LHCBMATH_LORENTZVECTORWITHERROR_H 
#define LHCBMATH_LORENTZVECTORWITHERROR_H 1
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/SymmetricMatrixTypes.h"
#include "GaudiKernel/Vector4DTypes.h"
#include "GaudiKernel/GenericVectorTypes.h"
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/SVectorWithError.h"
#include "LHCbMath/ValueWithError.h"
// ============================================================================
/** @file 
 *  Collection of useful objects with associated "covarinaces".
 *  The concept has been stollen from Wouter Hulsbergen's lines 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 20090603
 */
namespace Gaudi
{
  // ==========================================================================
  namespace Math 
  {
    // ========================================================================
    /** @class LorentzVectorWithError LHCbMath/LorentzVectorWithError.h
     *  
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-06-12
     */
    class GAUDI_API LorentzVectorWithError : public Gaudi::LorentzVector
    {
    public:
      // ======================================================================
      /// the actual type of the Vector 
      typedef Gaudi::LorentzVector                                 Vector4D   ;
      typedef Gaudi::LorentzVector                                 Value      ;
      /// the actual type of covariance 
      typedef Gaudi::SymMatrix4x4                                  Covariance ;
      // ======================================================================
    public:
      // ======================================================================
      /// the actual type of generic 4-vector 
      typedef Gaudi::Vector4                                       Vector     ;
      /// the actual type of vector with errors  
      typedef Gaudi::Math::SVectorWithError<4,double>              VectorE    ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from lorentz vector and covariance
      LorentzVectorWithError 
      ( const Vector4D&   value  = Vector4D   () , 
        const Covariance& cov2   = Covariance () ) ;  
      // ======================================================================
      /// constructor from lorentz vector and covariance
      LorentzVectorWithError 
      ( const Covariance& cov2                   , 
        const Vector4D&   value  = Vector4D   () ) ;
      /// constructor from generic vector and covariance
      LorentzVectorWithError 
      ( const Vector&     value                  , 
        const Covariance& cov2   = Covariance () ) ;  
      /// constructor from generic vector and covariance
      LorentzVectorWithError 
      ( const VectorE&     value                 ) ;
      // ======================================================================
    public: // trivial accessors 
      // ======================================================================
      const  Vector4D&   vector4d   ( ) const { return *this       ; }
      const  Vector4D&   vector4D   ( ) const { return vector4d () ; }
      const  Covariance& covariance ( ) const { return cov2  ()    ; }      
      inline Vector4D&   vector4d   ( )       { return *this       ; }
      // ======================================================================      
      const  Vector4D&   value      ( ) const { return vector4d () ; }
      const  Covariance& cov2       ( ) const { return m_cov2      ; }
      // ======================================================================      
    public: // setters 
      // ======================================================================
      void setVector4d   ( const Vector4D&   v ) { vector4d() =  v   ; }
      void setVector4D   ( const Vector4D&   v ) { setVector4d ( v ) ; }
      void setVector     ( const Vector4D&   v ) { setVector4d ( v ) ; }
      // ======================================================================
      void setValue      ( const Vector4D&   v ) { setVector4d ( v ) ; }
      void setCovariance ( const Covariance& c ) { m_cov2      = c   ; }      
      // ======================================================================
      void setValue      ( const VectorE&    v ) ;
      void setValue      ( const Vector&     v ) ;
      // ======================================================================
    public: // finally it is just a point + covariance 
      // ======================================================================
      operator const Covariance& () const { return cov2  () ; }        
      operator       Covariance& ()       { return m_cov2   ; }        
      // ======================================================================
    public: // useful accessors to covarinace matrix 
      // ======================================================================
      /// access to elemens of covariance matrix 
      double cov2 ( unsigned int i , unsigned int j ) const 
      { return m_cov2 ( i , j ) ; }
      // ======================================================================
    public: // operators 
      // ======================================================================
      LorentzVectorWithError& operator+= ( const LorentzVectorWithError& right ) ;
      LorentzVectorWithError& operator+= ( const Vector4D&               right ) ;
      LorentzVectorWithError& operator+= ( const VectorE&                right ) ;
      LorentzVectorWithError& operator+= ( const Vector&                 right ) ;
      LorentzVectorWithError& operator-= ( const LorentzVectorWithError& right ) ;
      LorentzVectorWithError& operator-= ( const Vector4D&               right ) ;
      LorentzVectorWithError& operator-= ( const VectorE&                right ) ;
      LorentzVectorWithError& operator-= ( const Vector&                 right ) ;
      // ======================================================================
    public: // scaling
      // ======================================================================
      /// *= 
      LorentzVectorWithError& operator*= ( const double v ) ;            // *= 
      /// /= 
      LorentzVectorWithError& operator/= ( const double v ) ;            // /= 
      // ====================================================================== 
    public:
      // ======================================================================
      /// get generic vector 
      void    asVector ( VectorE& data ) const ;
      /// get generic vector 
      void    asVector ( Vector&  data ) const ;
      /// convert to generic vector with errors:
      VectorE asVector () const ;
      // ======================================================================
    public:
      // ======================================================================
      /// evaluate chi2 distance 
      double chi2 ( const LorentzVectorWithError& right )  const ;
      /// evaluate chi2 distance 
      double chi2 ( const Vector4D&               right )  const ;      
      /// evaluate chi2 distance 
      double chi2 ( const VectorE&                right )  const ;      
      /// evaluate chi2 distance 
      double chi2 ( const Vector&                 right )  const ;      
      // ======================================================================
    public:
      // ======================================================================
      /// get the mass with error 
      Gaudi::Math::ValueWithError mass () const { return massWithError     () ; }
      /// get the mass with error 
      Gaudi::Math::ValueWithError m    () const { return massWithError     () ; }
      /// get the energy with error 
      Gaudi::Math::ValueWithError e    () const { return energyWithError   () ; }
      /// get the momentum with error 
      Gaudi::Math::ValueWithError p    () const { return momentumWithError () ; }
      /// get the transverse momentum with error
      Gaudi::Math::ValueWithError pt   () const { return ptWithError       () ; }
      // ======================================================================
    public:
      // ======================================================================
      /// get the mass with error 
      Gaudi::Math::ValueWithError massWithError     () const ;
      /// get the energy with error 
      Gaudi::Math::ValueWithError energyWithError   () const ;
      /// get the momentum with error 
      Gaudi::Math::ValueWithError momentumWithError () const ;
      /// get the transverse momentum with error 
      Gaudi::Math::ValueWithError ptWithError       () const ;
      // ======================================================================
    public:
      // ======================================================================
      /** evaluate  sigma mass 
       *  @see Gaudi::Math::sigmamass
       */
      double sigmaMass   () const ;                     // evaluate  sigma mass 
      // ======================================================================
      /** evaluate  sigma2 mass 
       *  @see Gaudi::Math::sigma2mass
       */
      double sigma2Mass  () const ;                  //   evaluate  sigma2 mass 
      /** evaluate  sigma2 mass2 
       *  @see Gaudi::Math::sigma2mass
       */
      double sigma2Mass2 () const ;                   // evaluate  sigma2 mass2 
      /** evaluate chi2-mass
       *  @see Gaudi::Math::chi2mass
       */
      double chi2mass ( const double m0 ) const ;         // evaluate chi2-mass
      // ======================================================================
    public:
      // ======================================================================
      /** evaluate sigma^2(p)
       *  @see Gaudi::Math::sigma2p
       */          
      double sigma2p() const ;                           // evaluate sigma^2(p)
      // ======================================================================
      /** evaluate sigma(p)
       *  @see Gaudi::Math::sigmap
       */           
      double sigmap () const ;                           //   evaluate sigma(p)
      // ======================================================================
    public:
      // ======================================================================
      /** evaluate sigma^2(pt)
       *  @see Gaudi::Math::sigma2pt
       */          
      double sigma2pt () const ;                       // evaluate sigma^2(pt)
      // ======================================================================
      /** evaluate sigma(p)
       *  @see Gaudi::Math::sigmap
       */           
      double sigmapt  () const ;                       //   evaluate sigma(pt)
      // ======================================================================
    public: // helper operators for Python
      // ======================================================================
      LorentzVectorWithError __add__   ( const LorentzVectorWithError& right ) const ;
      LorentzVectorWithError __add__   ( const Vector4D&               right ) const ;
      LorentzVectorWithError __sub__   ( const LorentzVectorWithError& right ) const ;
      LorentzVectorWithError __sub__   ( const Vector4D&               right ) const ;
      // ======================================================================
      LorentzVectorWithError __radd__  ( const Vector4D&               right ) const 
      { return __add__ ( right ) ; }  
      LorentzVectorWithError __rsub__  ( const Vector4D&               right ) const ;
      // ======================================================================
      LorentzVectorWithError& __imul__   ( const double v ) ;
      LorentzVectorWithError& __idiv__   ( const double v ) ;
      LorentzVectorWithError  __mul__    ( const double v ) const ;
      LorentzVectorWithError  __div__    ( const double v ) const ;
      LorentzVectorWithError  __rmul__   ( const double v ) const 
      { return __mul__ ( v ) ; }
      // ======================================================================
    public:
      // ======================================================================
      /// printout 
      std::ostream& fillStream ( std::ostream& s ) const ;          // printout 
      /// conversion to the string 
      std::string   toString   () const ;           // conversion to the string 
      // ======================================================================
    private:
      // ======================================================================
      /// the covariance 
      Covariance m_cov2     ;                                 // the covariance 
      // ======================================================================
    };
    // ========================================================================
    inline LorentzVectorWithError
    operator+ 
    ( const LorentzVectorWithError& a ,
      const LorentzVectorWithError& b ){ return a.__add__ ( b ) ; }
    inline LorentzVectorWithError
    operator- 
    ( const LorentzVectorWithError& a ,
      const LorentzVectorWithError& b ){ return a.__sub__ ( b ) ; }
    inline LorentzVectorWithError
    operator+ 
    ( const LorentzVectorWithError& a ,
      const Gaudi::LorentzVector&   b ){ return a.__add__ ( b ) ; }
    inline LorentzVectorWithError
    operator- 
    ( const LorentzVectorWithError& a ,
      const Gaudi::LorentzVector&   b ){ return a.__sub__ ( b ) ; }
    inline LorentzVectorWithError
    operator+ 
    ( const Gaudi::LorentzVector&   b ,
      const LorentzVectorWithError& a ) { return a + b ; }
    inline LorentzVectorWithError
    operator- 
    ( const Gaudi::LorentzVector&   b , 
      const LorentzVectorWithError& a ) { return a.__rsub__ ( b ) ; }
    // ========================================================================
    inline LorentzVectorWithError
    operator* 
    ( const LorentzVectorWithError& a ,
      const double                  b ) { return a.__mul__ ( b ) ; }
    inline LorentzVectorWithError
    operator/ 
    ( const LorentzVectorWithError& a ,
      const double                  b ) { return a.__div__ ( b ) ; }
    inline LorentzVectorWithError
    operator* 
    ( const double                  b , 
      const LorentzVectorWithError& a ) { return a.__mul__ ( b ) ; }
    // ========================================================================
    inline double chi2 
    ( const LorentzVectorWithError& a , 
      const LorentzVectorWithError& b ) { return a.chi2  ( b ) ; }
    inline double chi2 
    ( const LorentzVectorWithError& a , 
      const Gaudi::LorentzVector&   b ) { return a.chi2  ( b ) ; }
    inline double chi2 
    ( const Gaudi::LorentzVector&   b ,
      const LorentzVectorWithError& a ) { return a.chi2  ( b ) ; }
    inline double chi2 
    ( const LorentzVectorWithError&          a , 
      const LorentzVectorWithError::VectorE& b ) { return a.chi2  ( b ) ; }
    inline double chi2 
    ( const LorentzVectorWithError::VectorE& b , 
      const LorentzVectorWithError&          a ) { return a.chi2  ( b ) ; }
    inline double chi2 
    ( const LorentzVectorWithError&          a , 
      const LorentzVectorWithError::Vector&  b ) { return a.chi2  ( b ) ; }
    inline double chi2 
    ( const LorentzVectorWithError::Vector&  b , 
      const LorentzVectorWithError&          a ) { return a.chi2  ( b ) ; }                         
    // ========================================================================
  } //                                             end of namespace Gaudi::Math 
  // ==========================================================================
} //                                                     end of namespace Gaudi
// ============================================================================
// The END 
// ============================================================================
#endif // LHCBMATH_LORENTZVECTORWITHERROR_H
// ============================================================================
