// $Id$
// ============================================================================
#ifndef LHCBMATH_PARTICLEPARAMS_H 
#define LHCBMATH_PARTICLEPARAMS_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/SymmetricMatrixTypes.h"
#include "GaudiKernel/GenericMatrixTypes.h"
#include "GaudiKernel/GenericVectorTypes.h"
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/ValueWithError.h"
#include "LHCbMath/Point3DWithError.h"
#include "LHCbMath/LorentzVectorWithError.h"
#include "LHCbMath/SVectorWithError.h"
// ============================================================================
namespace Gaudi
{
  // ==========================================================================
  namespace Math
  {
    // ========================================================================
    /** @class ParticleParams Kernel/ParticleParams.h
     *
     *  Helper class to keep the particle-related represetation of 
     *  fit-parameters
     *
     *  The actual lines have been stollen from Wouter Hulsbergen 
     *
     *  @author Vanya Belyaev Ivan.Belyaev@nikhef.nl
     *  @date   2010-05-20
     */
    class GAUDI_API ParticleParams 
    {
    public: 
      // ======================================================================
      /// constructor from a full parameter set
      ParticleParams 
      ( const Gaudi::XYZPoint&                     position , 
        const Gaudi::LorentzVector&                momentum ,
        const double                               length   ,
        const Gaudi::SymMatrix8x8&                 cov8     ) ;
      /// constructor from a full parameter set
      ParticleParams
      ( const Gaudi::XYZPoint&                     position , 
        const Gaudi::LorentzVector&                momentum ,
        const Gaudi::SymMatrix7x7&                 cov7     ) ;
      /// constructor from a full parameter set
      ParticleParams
      ( const Gaudi::Math::Point3DWithError&       position , 
        const Gaudi::Math::LorentzVectorWithError& momentum ,
        const Gaudi::Math::ValueWithError&         length   , 
        const Gaudi::Matrix4x3&                    posmom   , 
        const Gaudi::Vector3&                      lenpos   , 
        const Gaudi::Vector4&                      lenmom   ) ;
      /// constructor from a full parameter set
      ParticleParams
      ( const Gaudi::XYZPoint&                     position , 
        const Gaudi::LorentzVector&                momentum ,
        const double                               length   , 
        const Gaudi::SymMatrix3x3&                 poscov   , 
        const Gaudi::SymMatrix4x4&                 momcov   , 
        const double                               lencov   ,
        const Gaudi::Matrix4x3&                    posmom   , 
        const Gaudi::Vector3&                      lenpos   , 
        const Gaudi::Vector4&                      lenmom   ) ;
      /// 
      /// constructor from a full parameter set
      ParticleParams ( const Gaudi::Math::SVectorWithError<8,double>& params ) ;
      /// constructor from a full parameter set
      ParticleParams ( const Gaudi::Vector8&                          params , 
                       const Gaudi::SymMatrix8x8&                     cov8   ) ;
      /// default constructor 
      ParticleParams () ;
      /// virtual destructor 
      virtual ~ParticleParams () ; 
      // ======================================================================
    public: // basic accessors  / diagonals 
      // ======================================================================
      // access to position+covariance
      const Gaudi::Math::Point3DWithError&       position     () const 
      { return m_position ; }
      const Gaudi::SymMatrix3x3&                 posCovMatrix () const 
      { return m_position ; }
      // access to 4-momentum+covariance 
      const Gaudi::Math::LorentzVectorWithError& momentum     () const
      { return m_momentum ; }
      const Gaudi::SymMatrix4x4&                 momCovMatrix () const 
      { return m_momentum ; }
      // access to decay length
      const Gaudi::Math::ValueWithError&         decayLength  () const 
      { return m_length   ; }
      // ======================================================================
    public: // covarinaces / non-diagonal 
      // ======================================================================
      ///  Matrix with correlation errors between momentum and position 
      const Gaudi::Matrix4x3& momPosCov () const { return m_momPosCovMatrix ; }
      /// "Matrix" with correlation errors between position and decay length
      const Gaudi::Vector3&   lenPosCov () const { return m_lenPosCovMatrix ; }
      /// "Matrix" with correlation errors between momentum and decay length
      const Gaudi::Vector4&   lenMomCov () const { return m_lenMomCovMatrix ; }
      // ======================================================================
    public: // derived accessors  
      // ======================================================================
      // calculate c*tau 
      Gaudi::Math::ValueWithError ctau () const ;
      // ======================================================================
    public:
      // ======================================================================
      /// get all parameters 
      Gaudi::Math::SVectorWithError<8,double> all() const ;
      /// get all parameters 
      Gaudi::Vector8      par8 () const ;
      /// get all covariances  
      Gaudi::SymMatrix8x8 cov8 () const ;
      /// get all parameters 
      void all  ( Gaudi::Math::SVectorWithError<8,double>& a ) const ;
      /// get all parameters 
      void par8 ( Gaudi::Vector8&      c )  const ;
      /// get all covariances 
      void cov8 ( Gaudi::SymMatrix8x8& c )  const ;      
      // ======================================================================
    private:
      // ======================================================================
      /// the decay length 
      Gaudi::Math::ValueWithError         m_length   ;    //   the decay length 
      /// reference posiion
      Gaudi::Math::Point3DWithError       m_position ;    // reference position
      /// 4-momentum 
      Gaudi::Math::LorentzVectorWithError m_momentum ;    //         4-momentum 
      ///  Matrix with correlation errors between momentum and position 
      Gaudi::Matrix4x3                    m_momPosCovMatrix ; 
      /// "Matrix" with correlation errors between position and decay length
      Gaudi::Vector3                      m_lenPosCovMatrix ; 
      /// "Matrix" with correlation errors between momentum and decay length
      Gaudi::Vector4                      m_lenMomCovMatrix ;
      // ======================================================================
    };
    // ========================================================================
    /** @class FitMass 
     *  helper class to perform amss-constrained fit 
     *  The actual code has been stollen from Wouter Hulsbergen 
     *  @author Vanya Belyaev Ivan.Belyaev@nikhef.nl
     *  @date   2010-05-20
     */
    class GAUDI_API FitMass 
    {
    public:
      // ======================================================================
      /** apply mass-constrained fit to the particle 
       * 
       *  @code
       *
       *     const Gaudi::Math::ParticleParams& params = .... ;
       *
       *     Gaudi::Math::ParticleParams fitted ;
       *     const double mass_B = 5.279 * GeV ;
       *
       *     Gaudi::Math::FitMass fitter ;
       *     const double chi2 = fitter.fit( params , mass_B , fitted ) ;
       *  
       *  @code 
       *
       *  @param  input    (INPUT)  the particle to be constrained 
       *  @param  mass     (INPUT)  the mass 
       *  @param  output  (OUTPUT) the constrained particle 
       *  @return the chi2 of mass-constraiend fit 
       *  The actual code has been stollen from Wouter Hulsbergen 
       *  @author Vanya Belyaev Ivan.Belyaev@nikhef.nl
       *  @date   2010-05-20
       */
      static 
      double fit
      ( const Gaudi::Math::ParticleParams& input  , 
        const double                       mass   ,
        Gaudi::Math::ParticleParams&       output ) ;
      // ======================================================================
      /** apply mass-constrained fit to the particle 
       * 
       *  @code
       *
       *     const Gaudi::Math::ParticleParams& params = .... ;
       *
       *
       *     const double mass_B = 5.279 * GeV ;
       *     double       chi2 = 0.0 
       *
       *     Gaudi::Math::FitMass fitter ;
       *     Gaudi::Math::ParticleParams fitted  = .... ;
       *         fitter.fit( params , mass_B , chi2 ) ;
       *  
       *  @code 
       *
       *  @param  input    (INPUT)  the particle to be constrained 
       *  @param  mass     (INPUT)  the mass 
       *  @param  output  (OUTPUT) the constrained particle 
       *  @return the chi2 of mass-constraiend fit 
       *  The actual code has been stollen from Wouter Hulsbergen 
       *  @author Vanya Belyaev Ivan.Belyaev@nikhef.nl
       *  @date   2010-05-20
       */
      static 
      Gaudi::Math::ParticleParams fit
      ( const Gaudi::Math::ParticleParams& input , 
        const double                       mass  , 
        double&                            chi2  ) ;
      // ======================================================================
      /** apply mass-constrained fit to the particle 
       * 
       *  @code
       *
       *     Gaudi::Math::ParticleParams params = .... ;
       *  
       *     const double mass_B = 5.279 * GeV ;
       *
       *     Gaudi::Math::FitMass fitter ;
       *     const double chi2 = fitter.fit ( mass_B , params ) ;
       *  
       *  @code 
       *
       *  @attention the function changes the parameters!
       * 
       *  @param  mass     (INPUT)  the mass 
       *  @param  particle (UPDATE) the particle to be constrained 
       *  @return the chi2 of mass-constrained fit
       *  The actual code has been stollen from Wouter Hulsbergen 
       *  @author Vanya Belyaev Ivan.Belyaev@nikhef.nl
       *  @date   2010-05-20
       */
      static double fit
      ( const double                       mass   ,
        Gaudi::Math::ParticleParams&       output ) ;
      // ======================================================================
    } ;  
    // ========================================================================
  } //                                             end of namespace Gaudi::Math
  // ==========================================================================
} //                                                      end of namesace Gaudi
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LHCBMATH_PARTICLEPARAMS_H
// ============================================================================
