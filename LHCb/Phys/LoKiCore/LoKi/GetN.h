// ============================================================================
#ifndef LOKI_GETN_H 
#define LOKI_GETN_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Combiner.h"
// ============================================================================
/** @file LoKi/GenN.h
 *
 *  This file is a part of LoKi project -
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas,
 *  contributions and advices from G.Raven, J.van Tilburg,
 *  A.Golutvin, P.Koppenburg have been used in the design.
 * 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 */
namespace LoKi 
{
  // ==========================================================================
  /** @class GetN GetN.h LoKi/GetN.h
   *
   *  Simple wrapper over LoKi::Combiner class 
   *  that allows to get certain elements combinations
   *
   *  @author Vanya Belyaev Ivan.Belyaev@cern.ch
   *  @date   2011-01-26
   */
  template <class CONTAINER, class UNIQUE>
  class GetN_
  {
  public:
    // =======================================================================
    GetN_ ( const          std::size_t                        N     , 
            const typename LoKi::Combiner_<CONTAINER>::Range& range )
      : m_combiner () 
      , m_unique   () 
    {
      for ( std::size_t n = 0 ; n < N ; ++n ) 
      { m_combiner.add( range ) ; }
    }  
    // =======================================================================
    template <class INPUT> 
    GetN_ ( const std::size_t N     , 
            INPUT             first , 
            INPUT             last  ) 
      : m_combiner () 
      , m_unique   () 
    {
      for ( std::size_t n = 0 ; n < N ; ++n ) 
      { m_combiner.add ( typename LoKi::Combiner_<CONTAINER>::Range ( first , last ) ) ; }
    }  
    // =======================================================================
  public:
    // =======================================================================
    /// get the actual combination 
    template <class OUTPUT> 
    std::size_t next ( OUTPUT out ) 
    {
      //
      if ( !m_combiner.valid() ) { return 0 ; }
      //
      if (  m_combiner.valid() && !m_combiner.unique ( m_unique ) ) 
      { 
        ++m_combiner ;
        return next ( out ) ;
      }
      //
      typedef typename LoKi::Combiner_<CONTAINER>::Select   Select   ;
      typedef typename Select::const_iterator               iterator ;
      //
      const Select&  sel = m_combiner.current() ; 
      for ( iterator ic  = sel.begin() ; sel.end() != ic ; ++ic )
      {  *out = **ic ; ++out ; }  
      //
      ++m_combiner ;
      //
      return sel.size() ;
    }
    // =======================================================================
  private :
    // =======================================================================
    /// the default constructor is disabled ;
    GetN_ () ;                        // the default constructor is disabled ;    
    // =======================================================================
  private :
    // =======================================================================
    /// the actual combiner 
    LoKi::Combiner_<CONTAINER> m_combiner ;             // the actual combiner 
    /// the unique selector 
    UNIQUE                     m_unique   ;             // the unique selector
    // =======================================================================
  } ;
  // ==========================================================================  
} //                                                      end of namespace LoKi 
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_GETN_H
// ============================================================================
