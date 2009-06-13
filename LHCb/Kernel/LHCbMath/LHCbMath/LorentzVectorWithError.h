// $Id: LorentzVectorWithError.h,v 1.2 2009-06-13 18:30:12 ibelyaev Exp $
// ============================================================================
#ifndef LHCBMATH_LORENTZVECTORWITHERROR_H 
#define LHCBMATH_LORENTZVECTORWITHERROR_H 1
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/SymmetricMatrixTypes.h"
#include "GaudiKernel/Vector4DTypes.h"
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
    class LorentzVectorWithError : public Gaudi::LorentzVector
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
      /// constructor for lorent vector and convariance
      LorentzVectorWithError 
      ( const Vector4D&   value  = Vector4D   () , 
        const Covariance& cov2   = Covariance () ) ;  
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
    public: // finally it is just a point + covariance 
      // ======================================================================
      operator const Covariance& () const { return cov2  () ; }        
      operator       Covariance& ()       { return m_cov2   ; }        
      // ======================================================================
    public: // operators 
      // ======================================================================
      LorentzVectorWithError& operator+= ( const LorentzVectorWithError& right ) ;
      LorentzVectorWithError& operator-= ( const LorentzVectorWithError& right ) ;
      LorentzVectorWithError& operator+= ( const Vector4D&               right ) ;
      LorentzVectorWithError& operator-= ( const Vector4D&               right ) ;
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
      /// evaluate chi2 distance 
      double chi2 ( const LorentzVectorWithError& right )  const ;
      /// evaluate chi2 distance 
      double chi2 ( const Vector4D&               right )  const ;      
      // ======================================================================
    public:
      // ======================================================================
      /** evaluate  sigma mass 
       *  @see Gaudi::Math::sigmamass
       */
      double sigmaMass() const ;                        // evaluate  sigma mass 
      /** evaluate chi2-mass
       *  @see Gaudi::Math::chi2mass
       */
      double chi2mass ( const double m0 ) const ;         // evaluate chi2-mass
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
  } //                                             end of namespace Gaudi::Math 
  // ==========================================================================
} //                                                     end of namespace Gaudi
// ============================================================================
// The END 
// ============================================================================
#endif // LHCBMATH_LORENTZVECTORWITHERROR_H
// ============================================================================
