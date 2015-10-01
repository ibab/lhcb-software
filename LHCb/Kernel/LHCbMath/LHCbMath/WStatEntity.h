// $Id$ 
// ============================================================================
#ifndef ANALYSIS_WSTATENTITY_H 
#define ANALYSIS_WSTATENTITY_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/StatEntity.h"
// ============================================================================
namespace Gaudi
{
  // ==========================================================================
  namespace Math
  {
    // ========================================================================
    /** @class WStatEntity  LHCbMath/WStatEntity.h
     *  Statistic with weight
     *  @see StatEntity 
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2014-04-07
     */
    class GAUDI_API WStatEntity 
    {
    public:
      // ======================================================================
      /// empty constructor 
      WStatEntity () ;
      /// constructor from StatEntity of values 
      WStatEntity ( const StatEntity& values ) ;
      // ======================================================================
    public:
      // ======================================================================
      /// the only one important method: add value with the given weight 
      WStatEntity& add   
        ( const double value      ,  
          const double weight = 1 ) ;
      /// ditto
      WStatEntity& update 
        ( const double value      ,  
          const double weight = 1 ) { return add ( value , weight ) ; }
      // ======================================================================
      WStatEntity& operator+= ( const double value ) { return add ( value ) ; }
      // ======================================================================
      // reset statistic 
      void reset() ;
      // ======================================================================      
    public: // the basic getters 
      // ======================================================================
      /// get the mean-value 
      double        mean       () const ;  
      double        meanErr    () const ;  
      double        dispersion () const ;
      double        rms        () const ;
      /// get the effective number of entries 
      double        nEff       () const ;
      /// get the actual number of entries 
      unsigned long nEntries   () const { return m_weights.nEntries() ; }
      // ======================================================================
      /// sum_i weight_i*value_i
      double        sum  () const { return m_sum  ; } // sum_i weight_i*value_i
      /// sum_i weight_i*value_i**2
      double        sum2 () const { return m_sum2 ; } // sum_i weight_i*value_i**2
      // ======================================================================
    public:
      // ======================================================================
      /// get the statistic of weights 
      const StatEntity& weights () const { return m_weights ; }
      /// get the statistic of values with non-zero weight 
      const StatEntity&  values () const { return m_values  ; }
      // ======================================================================
    public: // printout 
      // ======================================================================
      std::ostream& fillStream ( std::ostream& s ) const ;      
      std::ostream& print      ( std::ostream& s ) const { return fillStream ( s ) ; }
      std::string   toString   () const ;
      // ======================================================================
    private: /// the basic statistic 
      // ======================================================================
      /// sum_i weight_i*value_i
      long double m_sum    ;  // sum_i weight_i*value_i
      /// sum_i weight_i*value_i**2 
      long double m_sum2   ;  // sum_i weight_i*value_i**2 
      // ======================================================================     
    private: /// helper statistics 
      // ======================================================================
      /// statistic of values with non-zero weight 
      StatEntity m_values  ;
      /// statistic of weights 
      StatEntity m_weights ;
      // ======================================================================
    };
    // ========================================================================
    inline std::ostream& operator<<( std::ostream& s, const WStatEntity& e )
    { return e.fillStream ( s ) ; }
    // ========================================================================
  } //                                         The end of namespace Gaudi::Math
  // ==========================================================================
} //                                                 The end of namespace Gaudi 
// ============================================================================
// The END 
// ============================================================================
#endif // ANALYSIS_WSTATENTITY_H
// ============================================================================
