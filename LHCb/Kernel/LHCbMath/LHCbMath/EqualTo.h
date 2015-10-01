// $Id$
// ============================================================================
#ifndef LHCBMATH_EQUALTO_H 
#define LHCBMATH_EQUALTO_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Vector4DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Point3DTypes.h"
// ============================================================================
// Root 
// ============================================================================
#include "Math/SVector.h"
#include "Math/SMatrix.h"
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/LHCbMath.h"
// ============================================================================
namespace LHCb
{
  // ==========================================================================
  namespace Math
  {
    // ========================================================================
    /** specialisation for 4D-vectors 
     *  @see Gaudi::LorentzVector
     *  @see LHCb::Math::mULPS_double
     *  @see Gaudi::Math::Lomont
     *  @see Gaudi::Math::Lomont<double>
     */
    template <>
    struct Equal_To<Gaudi::LorentzVector> 
    {
    public:
      // ======================================================================
      /** constructor
       *  @see LHCb::Math::mULPS_double
       */
      Equal_To ( const unsigned int eps  = mULPS_double ) : m_cmp ( eps ) {}
      // ======================================================================
      /// comparison:
      inline bool operator() ( const Gaudi::LorentzVector& v1 , 
                               const Gaudi::LorentzVector& v2 ) const
      {
        return &v1 == &v2 || 
          ( m_cmp ( v1.Px () , v2.Px () ) &&
            m_cmp ( v1.Py () , v2.Py () ) &&
            m_cmp ( v1.Pz () , v2.Pz () ) &&
            m_cmp ( v1.E  () , v2.E  () ) ) ;
      }
      // ======================================================================
    private:
      // ======================================================================
      /// the evaluator 
      Equal_To<double> m_cmp ;                                 // the evaluator 
      // ======================================================================
      } ;  
    // ========================================================================
    /** specialisation for 3D-vectors 
     *  @see Gaudi::XYZVector
     *  @see LHCb::Math::mULPS_double
     *  @see Gaudi::Math::Lomont
     *  @see Gaudi::Math::Lomont<double>
     */
    template <>
    struct Equal_To<Gaudi::XYZVector> 
    {
    public:
      // ======================================================================
      /** constructor
       *  @see LHCb::Math::mULPS_double
       */
      Equal_To ( const unsigned int eps  = mULPS_double ) : m_cmp ( eps ) {}
      // ======================================================================
      /// comparison:
      inline bool operator() ( const Gaudi::XYZVector& v1 , 
                               const Gaudi::XYZVector& v2 ) const
      {
        return &v1 == &v2 || 
          ( m_cmp ( v1.X () , v2.X () ) &&
            m_cmp ( v1.Y () , v2.Y () ) &&
            m_cmp ( v1.Z () , v2.Z () ) ) ;
      }
      // ======================================================================
    private:
      // ======================================================================
      /// the evaluator 
      Equal_To<double> m_cmp ;                                 // the evaluator 
      // ======================================================================
    } ;  
    // ========================================================================
    /** specialisation for 3D-points
     *  @see Gaudi::XYZVector
     *  @see LHCb::Math::mULPS_double
     *  @see Gaudi::Math::Lomont
     *  @see Gaudi::Math::Lomont<double>
     */
    template <>
    struct Equal_To<Gaudi::XYZPoint> 
    {
    public:
      // ======================================================================
      /** constructor
       *  @see LHCb::Math::mULPS_double
       */
      Equal_To ( const unsigned int eps  = mULPS_double ) : m_cmp ( eps ) {}
      // ======================================================================
      /// comparison:
      inline bool operator() ( const Gaudi::XYZPoint& v1 , 
                               const Gaudi::XYZPoint& v2 ) const
      {
        return &v1 == &v2 || 
          ( m_cmp ( v1.X () , v2.X () ) &&
            m_cmp ( v1.Y () , v2.Y () ) &&
            m_cmp ( v1.Z () , v2.Z () ) ) ;
      }
      // ======================================================================
    private:
      // ======================================================================
      /// the evaluator 
      Equal_To<double> m_cmp ;                                 // the evaluator 
      // ======================================================================
    } ;  
    // ========================================================================
    /// specialisation for linear algebra vectors 
    template <class T, unsigned int D>
    struct Equal_To<ROOT::Math::SVector<T,D> > 
    {
    public:
      // ======================================================================
      /** constructor
       *  @see LHCb::Math::mULPS_double
       */
      Equal_To ( const unsigned int eps  = mULPS_double ) : m_cmp ( eps ) {}
      // ======================================================================
      /// comparison:
      inline bool operator()
      ( const ROOT::Math::SVector<T,D>& v1 , 
        const ROOT::Math::SVector<T,D>& v2 ) const  
      {
        return &v1 == &v2 || 
          std::equal ( v1.begin() , v1.end() , v2.begin() , m_cmp ) ;
      }
      // ======================================================================
    private:
      // ======================================================================
      /// the evaluator 
      Equal_To<T> m_cmp ;                                 // the evaluator 
      // ======================================================================
    } ;  
    // ========================================================================
    /// specialisation for matrices 
    template <class T, unsigned int D1, unsigned int D2, class R>
    struct Equal_To<ROOT::Math::SMatrix<T,D1,D2,R> > 
    {
    public:
      // ======================================================================
      /** constructor
       *  @see LHCb::Math::mULPS_double
       */
      Equal_To ( const unsigned int eps  = mULPS_double ) : m_cmp ( eps ) {}
      // ======================================================================
      /// comparison:
      inline bool operator()
      ( const ROOT::Math::SMatrix<T,D1,D2,R>& v1 , 
        const ROOT::Math::SMatrix<T,D1,D2,R>& v2 ) const  
      {
        return &v1 == &v2 || 
          std::equal ( v1.begin() , v1.end() , v2.begin() , m_cmp ) ;
      }
      // ======================================================================
    private:
      // ======================================================================
      /// the evaluator 
      Equal_To<T> m_cmp ;                                 // the evaluator 
      // ======================================================================
    } ;  
    // ========================================================================
  } //                                             end of namespace LHCb::Math 
  // ==========================================================================
} //                                                     end of namespace Gaudi
// ============================================================================
// The END 
// ============================================================================
#endif // LHCBMATH_EQUALTO_H
// ============================================================================
