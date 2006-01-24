// $Id: GenAlgs.h,v 1.1.1.1 2006-01-24 09:54:23 ibelyaev Exp $
// ========================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1.1.1 $
// ========================================================================
// $Log: not supported by cvs2svn $ 
// ========================================================================
#ifndef LOKI_GENALGS_H 
#define LOKI_GENALGS_H 1
// ========================================================================
// Include files
// ========================================================================
// STD & STL 
// ========================================================================
#include <algorithm>
// ========================================================================
// LoKiGen 
// ========================================================================
#include "LoKi/GenExtract.h"
// ========================================================================

// ============================================================================
/** @file
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot at al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23 
 */
// ============================================================================

namespace LoKi 
{
  /** @namespace  GenAlgs 
   *
   *  Collection of  few helper functions/algoritgms  "a'la STL"
   *  to simplify the operations with HepMC graphs
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-01-23
   */
  namespace GenAlgs 
  {
    /** useful helper function (a'la STL) to count a number of 
     *  (HepMC) particles, which satisfies the certain criteria, 
     *  e.e. count number of leptons in LHCb acceptance :
     * 
     *  @code
     *  
     *  using namespace LoKi::GenAlgs 
     *  using namespace LoKi::Cuts 
     *  
     *  const HepMC::GenEvent* event = ... ;
     * 
     *  const size_t nLeptons = 
     *    count_if ( event , GLEPTON && GTHETA < 400 * mrad ) ;
     *
     *  @endcode 
     * 
     *  @see LoKi::Cuts::GLEPTON
     *  @see LoKi::Cuts::GTHETA 
     *
     *  @param event pointer to HepMC event 
     *  @param cut   the condidion/predicate/criterion 
     *  @return number of elements which satisfy the criterion
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-23
     */
    template <class PREDICATE> 
    inline size_t count_if 
    ( const HepMC::GenEvent* event , 
      PREDICATE              cut   )
    {
      return ( 0 == event ) ?  0 : std::count_if 
        ( event->particles_begin () , event->particles_end () , cut ) ;  
    } ;
    
    /** useful helper function (a'la STL) to count a number of 
     *  (HepMC) particles, which satisfies the certain criteria, 
     *  e.e. count number of muons with large transverse momentum
     * 
     *  @code
     *  
     *  using namespace LoKi::GenAlgs 
     *  using namespace LoKi::Cuts 
     *  
     *  const LHCb::HepMCEvent* event = ... ;
     * 
     *  const size_t nLeptons = 
     *    count_if ( event , "mu+" == GABSID  && GPT > 4 * GeV  ) ;
     *
     *  @endcode 
     * 
     *  @see LoKi::Cuts::GABSID
     *  @see LoKi::Cuts::GPT 
     *
     *  @param event pointer to HepMC event 
     *  @param cut   the condidion/predicate/criterion 
     *  @return number of elements which satisfy the criterion
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-23
     */
    template <class PREDICATE>
    inline size_t count_if 
    ( const LHCb::HepMCEvent* event , 
      PREDICATE               cut   )
    { return ( 0 == event ) ? 0 : count_if ( event -> pGenEvt() , cut ) ; } ;
    
    /** useful helper function (a'la STL) to count a number of 
     *  (HepMC) particles, which satisfy certain criteria, 
     *  e.e. count number of beauty particles 
     * 
     *  @code
     *  
     *  using namespace LoKi::GenAlgs 
     *  using namespace LoKi::Cuts 
     *  
     *  const LHCb::HepMCEvent* events = 
     *   get<LHCb::HepMCEvents> ( LHCb::HepMCEvenLocation::Default ) ;
     * 
     *  const size_t nBeauty  = 
     *    count_if ( events , BEAUTY ) ;
     *
     *  @endcode 
     * 
     *  @see LoKi::Cuts::BEAUTY
     *
     *  @param event pointer to HepMC event 
     *  @param cut   the condidion/predicate/criterion 
     *  @return number of elements which satisfy the criterion
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-23
     */
    template <class PREDICATE>
    inline size_t count_if 
    ( const LHCb::HepMCEvents* events , 
      PREDICATE                cut   )
    {
      if ( 0 == events ) { return 0 ; }
      size_t result = 0  ;
      for ( LHCb::HepMCEvents::const_iterator iev = events->begin() ; 
            events->end() != iev ; ++iev ) 
      { result += count_if ( *iev , cut ) ; }
      return result ;
    } ;
    
    /** usefull helper function (a'la STL) to efficiently check the 
     *  presence of at least one (HepMC) particle, which satisfies the 
     *  certain criteria, e.g. b-buarks from Higgs decays:
     * 
     *  @code
     *  
     *  using namespace LoKi::GenAlgs 
     *  using namespace LoKi::Cuts 
     *  
     *  const HepMC::GenEvent* event = ... ;
     * 
     *  const bool good = 
     *    found ( event , ( "b" == GABSID ) && 
     *       0 < GNINTREE ( "H_10" == GABSID , HepMC::parents ) ) ;
     *
     *  @endcode 
     * 
     *  @see LoKi::Cuts::GABSID 
     *  @see LoKi::Cuts::GNINTREE
     *
     *  @param event pointer to HepMC event 
     *  @param cut   the condidion/predicate/criterion 
     *  @return true if the particle, which satisfies the criterion is found 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-23
     */
    template<class PREDICATE>
    inline bool found 
    ( const HepMC::GenEvent* event , 
      PREDICATE              cut   ) 
    {
      return ( 0 == event ) ? false : 
        event->particles_end() != std::find_if
        ( event->particles_begin () , event->particles_end () , cut ) ;
    } ;
    
    /** useful helper function (a'la STL) to efficiently check the 
     *  presence of at least one (HepMC) particle, which satisfies the 
     *  certain criteria, e.e. muon in LHCb acceptance :
     * 
     *  @code
     *  
     *  @code
     *  
     *  using namespace LoKi::GenAlgs 
     *  using namespace LoKi::Cuts 
     *  
     *  const LHCb::HepMCEvent* event = ... ;
     * 
     *  const bool good = 
     *    found ( event , "mu+" == GABSID && GTHETA < 400 * mrad ) ;
     *
     *  @endcode 
     * 
     *  @see LoKi::Cuts::GABSID
     *  @see LoKi::Cuts::GTHETA 
     *
     *  @param event pointer to HepMC event 
     *  @param cut   the condidion/predicate/criterion 
     *  @return true if the particle, which satisfies the criterion is found 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-23
     */
    template<class PREDICATE>
    inline bool found 
    ( const LHCb::HepMCEvent* event , 
      PREDICATE               cut   ) 
    { return ( 0 == event ) ? false :  found ( event->pGenEvt() , cut ) ; } ;
    
    /** useful helper function (a'la STL) to efficiently check the 
     *  presence of at least one (HepMC) particle, which satisfies the 
     *  certain criteria, e.g. for beauty charm meson 
     * 
     *  @code
     *  
     *  @code
     *  
     *  using namespace LoKi::GenAlgs 
     *  using namespace LoKi::Cuts 
     *  
     *  const LHCb::HepMCEvent* events = 
     *   get<LHCb::HepMCEvents> ( LHCb::HepMCEvenLocation::Default ) ;
     * 
     *  const bool good = found ( events , GBEAUTY && GCHARM && GMESON ) ;
     *
     *  @endcode 
     * 
     *  @see LoKi::Cuts::GBEAUTY
     *  @see LoKi::Cuts::GCHARM
     *  @see LoKi::Cuts::GMESON
     *
     *  @param event pointer to HepMC event 
     *  @param cut   the condidion/predicate/criterion 
     *  @return true if the particle, which satisfies the criterion is found 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-23
     */
    template<class PREDICATE>
    inline bool found 
    ( const LHCb::HepMCEvents* events , 
      PREDICATE                cut   ) 
    {
      if ( 0 == events ) { return false ; }
      for ( LHCb::HepMCEvents::const_iterator iev = events->begin() ; 
            events->end() != iev ; ++iev ) 
      { if ( found ( *iev , cut ) ) { return true ; } }
      return false ;
    } ;    

    //
  } ; // end of namespace LoKi::GenAlgs
  //
} ; // end of namespace LoKi 

// ========================================================================
// The END 
// ========================================================================
#endif // LOKI_GENALGS_H
// ========================================================================
