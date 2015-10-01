// $Id$
// ============================================================================
#ifndef LHCBMATH_NSPHERE_H 
#define LHCBMATH_NSPHERE_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL
// ============================================================================
#include <vector>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
// ============================================================================
namespace Gaudi 
{
  // ==========================================================================
  namespace Math 
  {
    // ==========================================================================
    /** @class NSphere NSphere.h LHCbMath/NSphere.h
     *  "N-sphere" of parameters 
     *  @author Vanya Belyaev  Ivan.Belyaev@itep.ru
     *  @date   2014-01-21
     */
    class GAUDI_API NSphere 
    {
    public: 
      // ======================================================================
      /** Standard constructor
       *  @param nPhases  dimensionality of N-sphere 
       *  @param bias     use the rotated sphere? 
       */
      NSphere ( const unsigned short nPhases = 1    ,
                const bool           rotated = true ) ;
      // =======================================================================
      /** Standard constructor
       *  @param nPhases  dimensionality of N-sphere 
       *  @param bias     use the rotated sphere? 
       */
      NSphere ( const std::vector<double>& phases         ,
                const bool                 rotated = true ) ;
      /// copy
      NSphere ( const NSphere&  right ) ;
      /// move
      NSphere (       NSphere&& right ) ;
      /// destructor 
      ~NSphere() ; 
      // ======================================================================
    public:
      // ======================================================================
      unsigned int nX      () const { return nPhi() + 1       ; } 
      unsigned int nPhi    () const { return m_sin_phi.size() ; } 
      bool         rotated () const { return m_rotated        ; }
      // ======================================================================
    public:
      // ======================================================================
      /// get x_i coefficient:               0 <= i < nX  
      inline double x        ( const unsigned short index ) const ;      
      /// get x_i coefficient squared        0 <= i < nX 
      inline double x2       ( const unsigned short index ) const ;
      inline double xsquared ( const unsigned short index ) const 
      { return x2 ( index ) ; }
      // ======================================================================
    public:
      // ======================================================================
      double sin_phi   ( const unsigned short index ) const 
      { return index < nPhi() ? m_sin_phi [index] : 0.0 ; }
      // ======================================================================
      double cos_phi   ( const unsigned short index ) const 
      { return index < nPhi() ? m_cos_phi [index] : 0.0 ; }
      // ======================================================================
    public:
      // ======================================================================
      double phase     ( const unsigned short index ) const 
      { return index < nPhi() ? m_phases  [index] : 0.0 ; }
      // ======================================================================
      double par       ( const unsigned short index ) const 
      { return phase ( index ) ; }
      /// get all phases 
      const std::vector<double>& phases  () const { return m_phases  ; }
      /// get all phases 
      const std::vector<double>& pars    () const { return phases()  ; }
      // get all   sines 
      const std::vector<double>& sines   () const { return m_sin_phi ; }
      // get all cosines 
      const std::vector<double>& cosines () const { return m_cos_phi ; }
      // get all deltas 
      const std::vector<double>& delta   () const { return m_delta   ; }
      // ======================================================================
    public:
      // ======================================================================
      /** set new value for phi(i)      
       *  @param index (input) the index (0 <= index < nPhi)
       *  @param valeu new value to be set 
       *  @return true is new valeu is really set 
       */
      bool setPhase   ( const unsigned short index , 
                        const double         value ) ;
      // ======================================================================
    public:
      // ======================================================================
      /// get biases to equalize the data 
      double delta    ( const unsigned short index ) const  
      { return m_rotated && ( index < nPhi() ) ? m_delta [index] : 0.0 ; }
      // ======================================================================
    public:
      // ======================================================================
      /// copy assignement 
      NSphere& operator=( const NSphere&  right ) ;
      /// move assignement 
      NSphere& operator=(       NSphere&& right ) ;
      // ======================================================================
    public:
      // ======================================================================
      /// swap two spheres 
      void swap ( NSphere& right ) ; // swap two spheres 
      // ======================================================================
    private:
      // ======================================================================
      /// bias to equalize the x_i 
      bool                m_rotated ; // rotated sphere ?
      /// the phase biases for rotated sphere 
      std::vector<double> m_delta   ; // the phase biases for rotated sphere 
      /// the phases  
      std::vector<double> m_phases  ; // the phases 
      /// vector of sin(phi)
      std::vector<double> m_sin_phi ; // vector of sin(phi)
      /// vector of cos(phi)
      std::vector<double> m_cos_phi ; // vector of cos(phi)
      // ======================================================================
    };
    // ========================================================================
    /// swap two speheres 
    inline void swap ( NSphere& a , NSphere& b ) { a.swap ( b ) ; }
    // ========================================================================
  } //                                         The end of namespace Gaudi::Math
  // ==========================================================================
} //                                                 The end of namespace Gaudi
// ============================================================================
// get x_i coefficient:               0 <= i < nX  
// ============================================================================
inline double Gaudi::Math::NSphere::x 
( const unsigned short index ) const 
{
  if      ( nX () <= index ) { return 0                ; } // invalid 
  else if ( nX () == 1     ) { return 1                ; } // trivial 
  else if ( 0     == index ) { return m_cos_phi[index] ; } // x_0 
  //
  // get index as phi 
  //
  double xi = 1.0 ;
  for  ( unsigned short j = 0 ; j < index ; ++j ) { xi *= m_sin_phi[j] ; }
  //
  if   ( index != nPhi ()  ) { xi *= m_cos_phi[index] ; }
  //
  return xi ;  
}
// ============================================================================
// get x_i coefficient squared        0 <= i < nX 
// ============================================================================
inline double Gaudi::Math::NSphere::x2 
( const unsigned short index ) const  
{
  const double xi = x ( index ) ;
  return xi * xi ;
}
// ============================================================================
// The END 
// ============================================================================
#endif // LHCBMATH_NSPHERE_H
// ============================================================================
