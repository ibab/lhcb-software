// $Id: MCCaloDataFunctor.h,v 1.1 2007-04-03 11:22:47 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef EVENT_MCCALODATAFUNCTOR_H 
#define EVENT_MCCALODATAFUNCTOR_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <algorithm>
#include <functional>
// ============================================================================
// (MC)Event 
// ============================================================================
#include "Event/MCCaloHit.h"
#include "Event/MCCaloDigit.h"
// ============================================================================

namespace LHCb 
{
  namespace CaloDataFunctor 
  {
    /** @struct InTime 
     *  The trivial predicate which evaluates for "true" for 
     *  MC-calo hits which are "in-time" with the given BX
     *
     *  @code 
     * 
     *   // count the number of MCHits in the current BX for the given 
     *   size_t num = 
     *     std::count_if ( hits->begin() , hits->end() , InTime(0) ) ;
     *
     *  @endcode 
     *
     *  @see LHCb::MCCaloHit
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-04-03
     */
    struct InTime 
      : public std::unary_function<const LHCb::MCCaloHit*,bool>
    {
    public:
      InTime ( const LHCb::MCCaloHit::Time time = 0 ) : m_time ( time ) {} ;
    public:
      /// the only one essential method 
      inline bool operator () ( const LHCb::MCCaloHit* hit )  const 
      { return ( 0 == hit ) ? false : m_time == hit->time() ; }
    private:
      // index of the time slot 
      LHCb::MCCaloHit::Time m_time ; ///< index of the time slot
    } ;
    
    /** @struct SumActiveEnergy
     *  Simple structure to perform the summation of active energy for 
     *  hits, which satisfy certain criteria 
     *  (e.g. in the certain time window):
     *
     *  
     *  @code 
     *  
     *  double energy = 
     *    std::accumulate ( hits->begin() , // begin of the sequence 
     *                      hits->end() ) , // end of the sequence
     *                      0.0           , // initial value 
     *                      sumActiveEnergy( InTime(0) ) // functor
     *                      ) ;
     *                                       
     *  @endcode 
     *
     *
     *  @see LHCb::MCCaloHit
     *  @see sumActiveEnergy
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-04-03
     */
    template <class PREDICATE,class TYPE=LHCb::MCCaloHit> 
    struct SumActiveEnergy
      : public std::binary_function<double,const TYPE*,double>
    {
    public:
      /// contructor from the predicate 
      SumActiveEnergy ( PREDICATE eval ) : m_eval ( eval ) {}
    public:
      /// the only one essential method 
      inline double operator() ( double& energy , const TYPE* hit ) const 
      {
        // use the evaluator 
        if ( m_eval( hit ) ){ energy += hit->activeE() ; }        
        return energy ;
      } ;
    private:
      PREDICATE m_eval ;
    };
    
    /** helper function for instantiation of the proper functor 
     *  for summing of the active energy
     *
     *  @see LHCb::MCCaloHit
     *  @see SumActiveEnergy
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-04-03
     */  
    template <class PREDICATE> 
    inline SumActiveEnergy<PREDICATE>
    sumActiveEnergy ( PREDICATE pred ) { return SumActiveEnergy<PREDICATE>( pred ) ; }
    
    /** simple structure ( placeholder) for the functor,     
     *  whcih extract the active enegy of the objects, 
     *  accumulated for a certain time-bin
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-04-03
     */  
    template <class TYPE> struct ActiveEnergyInTime ;
    
    /** The full tmeplate specialization of the 
     *  the functor,  which extract the active enegy 
     *  LHCb::MCCaloHit object
     *
     *  @code
     *
     *
     *  const LHCb::MCCaloHit* hit = ... ;
     *
     *  // create the functor:
     *  ActiveEnergyInTime<LHCb::MCCaloHit> evaluator( 0 ) ;
     * 
     *  // use it!
     *  const double energy = evaluator( hit ) ;
     *
     *  @endcode 
     *
     *  @see LHCb::MCCaloHit::Time
     *  @see LHCb::MCCaloHit
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-04-03
     */  
    template <>
    struct ActiveEnergyInTime<LHCb::MCCaloHit>    
      : public std::unary_function<const LHCb::MCCaloHit*,double>
    {
    public:
      /// constructor from the time-bin 
      ActiveEnergyInTime( const LHCb::MCCaloHit::Time time = 0 ) : m_eval ( time ) {}
    public:
      /// the only one essential method 
      inline double operator() ( const LHCb::MCCaloHit* hit ) const
      {
        if ( 0 != hit && m_eval ( hit ) ){ return hit->activeE() ; }
        return 0.0 ;
      } ;
    private:
      // predicate, which select the proper time-bin
      InTime m_eval ; ///< predicate, which select the proper time-bin
    };
    
    
    /** The full template specialization of the 
     *  the functor,  which extract the active enegy 
     *  LHCb::MCCaloDigit object
     *
     *  @code
     *
     *  const LHCb::MCCaloDigit* digit = ... ;
     *
     *  // create the functor:
     *  ActiveEnergyInTime<LHCb::MCCaloDigit> evaluator( 0 ) ;
     * 
     *  // use it!
     *  const double energy = evaluator( digit ) ;
     *
     *  @endcode 
     *
     *  @see LHCb::MCCaloHit::Time
     *  @see LHCb::MCCaloHit
     *  @see LHCb::MCCaloDigit
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-04-03
     */  
    template <>
    struct ActiveEnergyInTime<LHCb::MCCaloDigit>    
      : public std::unary_function<const LHCb::MCCaloDigit*,double>
    {
    public:
      /// contructor from the time-slot 
      ActiveEnergyInTime( const LHCb::MCCaloHit::Time time = 0 ) 
        : m_eval ( sumActiveEnergy( InTime( time ) ) ) {}
    public:
      /// the only one essential method 
      inline double operator() ( const LHCb::MCCaloDigit* digit ) const
      {
        if ( 0 == digit ) { return 0 ;}
        const SmartRefVector<LHCb::MCCaloHit>& hits = digit->hits();
        return std::accumulate ( hits.begin() , hits.end() , 0.0 , m_eval ) ;
      } ;
    private:
      // the actual evaluator 
      SumActiveEnergy<InTime> m_eval ; ///< the actual evaluator 
    } ;
    
    /** The full template specialization of the 
     *  the functor,  which extract the active energy
     *  of LHCb::MCCaloHit::Container object
     *
     *  @code
     *
     *  const LHCb::MCCaloHit::Container* hits = 
     *    get<LHCb::MCCaloHit::Container>( LHCb::MCCaloHitLocation::Ecal ) ;
     *
     *  // create the functor:
     *  ActiveEnergyInTime<LHCb::MCCaloHit::Container> evaluator( 0 ) ;
     * 
     *  // use it!
     *  const double energy = evaluator( hits ) ;
     *
     *  @endcode 
     *
     *  @see LHCb::MCCaloHit::Time
     *  @see LHCb::MCCaloHit
     *  @see LHCb::MCCaloHit::Container
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-04-03
     */  
    template <>
    struct ActiveEnergyInTime<LHCb::MCCaloHit::Container>    
      : public std::unary_function<const LHCb::MCCaloHit::Container*,double>
    {
    public:
      /// contructor from the time-slot 
      ActiveEnergyInTime( const LHCb::MCCaloHit::Time time = 0 )
        : m_eval ( sumActiveEnergy( InTime( time ) ) ) {}
    public:
      /// the only one essential method 
      inline double operator() ( const LHCb::MCCaloHit::Container* hits ) const
      {
        if ( 0 == hits ) { return 0 ; }
        return std::accumulate ( hits->begin() , hits->end() , 0.0 , m_eval ) ;        
      } ;
    private:
      // the actual evaluator
      SumActiveEnergy<InTime> m_eval ; ///< the actual evaluator
    };
    
    /** The full template specialization of the 
     *  the functor,  which extract the active energy
     *  of LHCb::MCCaloDigit::Container object
     *
     *  @code
     *
     *  const LHCb::MCCaloDigit::Container* digits = 
     *    get<LHCb::MCCaloDigit::Container>( LHCb::MCCaloDigitLocation::Ecal ) ;
     *
     *  // create the functor:
     *  ActiveEnergyInTime<LHCb::MCCaloDigit::Container> evaluator( 0 ) ;
     * 
     *  // use it!
     *  const double energy = evaluator( digits ) ;
     *
     *  @endcode 
     *
     *  @see LHCb::MCCaloHit::Time
     *  @see LHCb::MCCaloDigit
     *  @see LHCb::MCCaloDigit::Container
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-04-03
     */  
    template <>
    struct ActiveEnergyInTime<LHCb::MCCaloDigit::Container>    
      : public std::unary_function<const LHCb::MCCaloDigit::Container*,double>
    {
    public:
      /// contructor from the time-slot 
      ActiveEnergyInTime( const LHCb::MCCaloHit::Time time = 0 ) : m_eval ( time ) {}
    public:
      /// the only on eessential method 
      inline double operator() ( const LHCb::MCCaloDigit::Container* digits ) const
      {
        if ( 0 == digits ) { return 0 ; }
        double energy = 0.0 ;
        for ( LHCb::MCCaloDigit::Container::const_iterator idigit = digits->begin() ;
              digits->end() != idigit ; ++idigit ) { energy += m_eval ( *idigit ) ; }
        return energy ;
      } ;
    private:
      // the actual evaluatoe
      ActiveEnergyInTime<LHCb::MCCaloDigit> m_eval ; ///< the actual evaluatoe
    } ;

    /** the function whcih return the active energy of the 
     *  LHCb::MCCaloHit, LHCb::MCCaloHit::Container, 
     *  LHCb::MCCaloDigit, LHCb::MCCaloDigit::Container objects 
     *  for the given time slot.
     *  
     *  @code 
     * 
     *  const LHCb::MCCaloDigit::Container* digits = 
     *    get<LHCb::MCCaloDigit::Container>( LHCb::MCCaloDigitLocation::Ecal ) ;
     *
     *  const double e1 = activeEnergyInTime ( digits , 0 ) ;
     *
     *  const LHCb::MCCaloHit::Container* hits = 
     *    get<LHCb::MCCaloHit::Container>( LHCb::MCCaloHitLocation::Ecal ) ;
     *
     *  const double e2 = activeEnergyInTime ( hits   , 0 ) ;
     *
     *  const LHCb::MCCaloHit*   hit   = ... ;
     *
     *  const double e3 = activeEnergyInTime ( hit   , 0 ) ;
     *
     *  const LHCb::MCCaloDigit* digit = ... ;
     *
     *  const double e4 = activeEnergyInTime ( digit   , 0 ) ;
     *
     *  @endcode 
     *
     *  @see LHCb::MCCaloHit::Time
     *  @see LHCb::MCCaloHit
     *  @see LHCb::MCCaloHit::Container
     *  @see LHCb::MCCaloDigit
     *  @see LHCb::MCCaloDigit::Container
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-04-03
     */
    template <class TYPE>
    inline double activeEnergyInTime 
    ( const TYPE*                 object , 
      const LHCb::MCCaloHit::Time time   )
    {
      if ( 0 == object ) { return 0.0 ; }
      ActiveEnergyInTime<TYPE> evaluator ( time )  ;
      return evaluator ( object ) ;
    } ;
    
    
  } // end of namespace CaloDataFuctor
} // end of namespace LHCb


// ============================================================================
// The END 
// ============================================================================
#endif // EVENT_MCCALODATAFUNCTOR_H
// ============================================================================
