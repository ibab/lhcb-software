// $Id: MCFinderObj.h,v 1.8 2007-06-04 09:57:29 cattanem Exp $
// ============================================================================
#ifndef LOKI_MCFINDEROBJ_H 
#define LOKI_MCFINDEROBJ_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/HashMap.h"
// ============================================================================
// Gaudi Alg
// ============================================================================
#include "GaudiAlg/GaudiTool.h"
#include "GaudiAlg/GaudiAlgorithm.h"
// ============================================================================
// MCInterfaces
// ============================================================================
#include "MCInterfaces/IMCDecayFinder.h"
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/Interface.h"
#include "LoKi/Base.h"
#include "LoKi/Keeper.h"
#include "LoKi/UniqueKeeper.h"
// ============================================================================
// LoKiMC 
// ============================================================================
#include "LoKi/MCTypes.h"
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
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-03-11 
 */
// ============================================================================
namespace LoKi 
{ 
  /** determine if the decay members need to be extracted 
   *  @param decay decay descritor 
   *  @return true if decay descriptor contains special symbols for
   *          decay members extraction 
   *  @date 2004-06-28 
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   */
  inline bool extractMembers ( const std::string& decay )
  { return std::string::npos != decay.find_first_of ( ":^" ) ; } ;  
  /** @class MCFinderObj MCFinderObj.h LoKi/MCFinderObj.h
   *  
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-03-10
   */
  class MCFinderObj : public LoKi::Base 
  {
  public: 
    /** Standard constructor
     *  @param name object name 
     *  @param finder the mc-decay finder itself
     *  @param algo   data provider 
     *  @param reporter error reporter 
     */
    MCFinderObj 
    ( const std::string&    name     , 
      const IMCDecayFinder* finder   , 
      const GaudiAlgorithm* algo     , 
      const IReporter*      reporter ) ;
    /** Standard constructor
     *  @param name object name 
     *  @param finder the mc-decay finder itself
     *  @param algo   data provider 
     *  @param reporter error reporter 
     */
    MCFinderObj 
    ( const std::string&    name     , 
      const IMCDecayFinder* finder   , 
      const GaudiTool*      algo     , 
      const IReporter*      reporter ) ;
  protected:
    /// virtual and protected  destructor 
    virtual ~MCFinderObj(); ///< Destructor
  public:
    /** find MC decays.
     *  
     *  It is just a short cut for very convinient and simple DecayFinder 
     *  tool by Olivier Dormond   
     *
     *  @code 
     *
     *  MCPRange range = findDecays ( "B0 -> (J/psi(1S) -> mu+ mu-) KS0" );
     *  
     *  @endcode 
     * 
     *  @see DecayFinder  
     *  @param decay   decay   formula 
     *  @param address location of container of MC particle
     *  @return range of found MC decays 
     */
    LoKi::Types::MCRange findDecays 
    ( const std::string& decay   , 
      const std::string& address = 
      LHCb::MCParticleLocation::Default ) const ;
    /** find MC decays.
     *  
     *  It is just a short cut for very convinient and simple DecayFinder 
     *  tool by Olivier Dormond   
     *
     *  @code 
     *
     *  MCPRange range = findDecays ( "B0 -> (J/psi(1S) -> mu+ mu-) KS0" );
     *  
     *  @endcode 
     * 
     *  @see DecayFinder  
     *  @param decay   decay   formula 
     *  @param address location of container of MC particle
     *  @return range of found MC decays 
     */
    LoKi::Types::MCRange findDecays 
    ( const std::string&                 decay     , 
      const LHCb::MCParticle::Container* particles ) const ;
    /** find MC decays.
     *  
     *  It is just a short cut for very convinient and simple DecayFinder 
     *  tool by Olivier Dormond   
     *
     *  @code 
     *
     *  MCPRange range = findDecays ( "B0 -> (J/psi(1S) -> mu+ mu-) KS0" );
     *  
     *  @endcode 
     * 
     *  @see DecayFinder  
     *  @param decay   decay   formula 
     *  @param address location of container of MC particle
     *  @return range of found MC decays 
     */
    LoKi::Types::MCRange findDecays 
    ( const std::string&                decay     , 
      const LHCb::MCParticle::Vector& particles ) const ;
    /** find MC decays.
     *  
     *  It is just a short cut for very convinient and simple DecayFinder 
     *  tool by Olivier Dormond   
     *
     *  @code 
     *
     *  MCPRange range = findDecays ( "B0 -> (J/psi(1S) -> mu+ mu-) KS0" );
     *  
     *  @endcode 
     * 
     *  @see DecayFinder  
     *  @param decay   decay   formula 
     *  @param address location of container of MC particle
     *  @return range of found MC decays 
     */
    LoKi::Types::MCRange findDecays 
    ( const std::string&                   decay     , 
      const LHCb::MCParticle::ConstVector& particles ) const ;    
    /** find MC decays.
     *  
     *  It is just a short cut for very convinient and simple DecayFinder 
     *  tool by Olivier Dormond   
     *
     *  @code 
     *
     *  MCPRange range = findDecays ( "B0 -> (J/psi(1S) -> mu+ mu-) KS0" );
     *  
     *  @endcode 
     * 
     *  @see DecayFinder  
     *  @param decay   decay   formula 
     *  @param address location of container of MC particle
     *  @return range of found MC decays 
     */
    LoKi::Types::MCRange findDecays 
    ( const std::string&          decay , 
      const LoKi::Types::MCRange& range ) const ;    
    LoKi::Types::MCRange findDecays
    ( const std::string& decay , 
      const LoKi::Keeper<LHCb::MCParticle>& range ) const ;
    LoKi::Types::MCRange findDecays
    ( const std::string& decay , 
      const LoKi::UniqueKeeper<LHCb::MCParticle>& range ) const ;
    /** find MC decays.
     *  
     *  It is just a short cut for very convinient and simple DecayFinder 
     *  tool by Olivier Dormond   
     *
     *  @code 
     *
     *  MCPRange range = findDecays ( "B0 -> (J/psi(1S) -> mu+ mu-) KS0" );
     *  
     *  @endcode 
     * 
     *  @see DecayFinder  
     *  @param decay   decay   formula 
     *  @param address location of container of MC particle
     *  @return range of found MC decays 
     */
    template <class MCPARTICLE>
    LoKi::Types::MCRange findDecays 
    ( const std::string& decay , 
      MCPARTICLE         first , 
      MCPARTICLE         last  ) const 
    { 
      /// the most trivial case 
      if ( first == last ) { return LoKi::Types::MCRange() ; }
      LoKi::MCTypes::MCContainer vct ;
      vct.reserve( std::distance ( first , last ) ) ;
      for ( ; first != last ; ++first ) 
      {
        const LHCb::MCParticle* mc = *first ;
        vct.push_back( const_cast<LHCb::MCParticle*>( mc ) ) ;
      } ;
      return _findDecays ( decay , vct ) ; 
    } ;    
    /// clear the internal storage of decays
    void clear() ;
  private:
    /** templated decay extractor to eliminate code duplication 
     *  @paran decay decay descriptor
     *  @param source data source 
     *  @return the pseudocontainer of found decays 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-03-11
     */
    template <class SOURCE> 
    inline LoKi::Types::MCRange _findDecays 
    ( const std::string& decay  , 
      const SOURCE&      source ) const ;
  private:
    // the default constructor is disabled 
    MCFinderObj() ;
  private:
    typedef GaudiUtils::HashMap<std::string,LoKi::MCTypes::MCContainer> MCDecays ;
    // "the temporary" storage of decays 
    mutable MCDecays                m_decays ; ///< the storage of decays 
    // the decay finder itself 
    LoKi::Interface<IMCDecayFinder> m_finder ; ///< the decay finder itself 
    // data provider 
    LoKi::Interface<GaudiTool>      m_tool   ; ///< data provider 
    // data provider 
    LoKi::Interface<GaudiAlgorithm> m_algo   ; ///< data provider 
  } ;
}  // end of the namespace LoKi
// ===========================================================================
/** templated decay extractor to eliminate code duplication 
 *  @paran decay decay descriptor
 *  @param source data source 
 *  @return the pseudocontainer of found decays 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-03-11
 */
// ===========================================================================
template <class SOURCE>
inline LoKi::Types::MCRange 
LoKi::MCFinderObj::_findDecays 
( const std::string& decay  , 
  const SOURCE&      source ) const
{
  //
  Assert ( m_finder.validPointer() , "IMCDecayFinder is invalid!") ;
  /// the decay need to be reset ?
  if ( decay != m_finder->decay() )
  {
    StatusCode sc = m_finder->setDecay( decay ) ;
    Assert ( sc.isSuccess() , "Decay Descriptor is not set" , sc ) ;
  }
  LoKi::MCTypes::MCContainer& decays = m_decays[decay];
  decays.clear() ;                                              // CLEAR!!
  // prepare for member extraction 
  const bool members =  extractMembers( decay ) ;
  LHCb::MCParticle::Vector mcVct ;
  /// @see IMCDecayFinder 
  const LHCb::MCParticle* init   = 0 ;
  for ( bool  found = true ; found ; ) 
  {
    found = m_finder->findDecay ( source , init );
    if ( !found || 0 == init ) { continue ; } 
    // extract decay members ? 
    if ( members ) ///< extract decay members 
    {
      mcVct.clear() ;
      m_finder->decayMembers( init , mcVct ) ;
      decays.insert( decays.end() , mcVct.begin() , mcVct.end() ) ;
      mcVct.clear() ;
    } 
    else /// < deal only with heads  
    { decays.push_back( const_cast<LHCb::MCParticle*>( init ) ) ; } 
  }
  //
  return LoKi::Types::MCRange( decays.begin() , decays.end() );
} 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_MCFINDEROBJ_H
// ============================================================================
