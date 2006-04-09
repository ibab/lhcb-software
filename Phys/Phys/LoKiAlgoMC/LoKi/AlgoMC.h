// $Id: AlgoMC.h,v 1.1.1.1 2006-04-09 08:44:51 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, Version $Revison:$
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef LOKI_ALGOMC_H 
#define LOKI_ALGOMC_H 1
// ============================================================================
// Include files
// ============================================================================
// DavinciKernel
// ============================================================================
#include "Kernel/IMCDecayFinder.h"
// ============================================================================
// LoKiAlgo 
// ============================================================================
#include "LoKi/Algo.h"
// ============================================================================
// LoKiMC 
// ============================================================================
#include "LoKi/MCTypes.h"
#include "LoKi/MCFinder.h"
#include "LoKi/MCFinderObj.h"
// ============================================================================
// LoKiPhysMC 
// ============================================================================
//#include "LoKi/PhysMCTypes.h"
// ============================================================================

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
 *  with the campain of Dr.O.Callot et al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-03-31 
 */
// ============================================================================

namespace LoKi 
{
  /** @class AlgoMC AlgoMC.h LoKi/AlgoMC.h
   *  
   *
   *  @author 
   *  @date   2006-03-31
   */
  class AlgoMC 
    : public LoKi::Algo
  {
  public: 
    /** 'Select' the MC particles to be used in local storage
     *  
     *  - The MC Particles are selected from the TES location
     *
     *  @code
     *
     *  MCRange kaons = mcselect( "Kaons" , 
     *                  "K+" == MCABSID  && MCP > 5 * GeV );
     *
     *  @endcode
     *
     *  - The example illustrate the 'selection'/'filtering of MC particles, 
     *    which are @c K+ or @c K- and have a
     *  momentum in excess of 5GeV/c
     *  - The selected MC particles
     *  are stored inside local LoKi storage under the tag @c "Kaons"
     *  and returned as a sequence of MC particles @c kaons .
     *
     *  @see LHCb::MCParticle 
     *  @see LoKi::Types::MCRange
     *  @see LoKi::Cuts::MCABSID 
     *  @see LoKi::Cuts::MCP
     *  @param name name/tag assigned to the selected particles
     *  @param cut  cut to be applied
     *  @param location TES location of MC-particles
     *  @return selected range of particles
     */
    LoKi::Types::MCRange 
    mcselect 
    ( const std::string&         tag                                          , 
      const LoKi::Types::MCCuts& cut                                          ,
      const std::string&         location = LHCb::MCParticleLocation::Default ) ;

    /** 'Select' the MC particles to be used in local storage
     *  
     *  @code
     *
     *  MCRange kaons = mcselect( "Kaons" , 
     *                  "K+" == MCABSID  && MCP > 5 * GeV );
     *  MCRange kPlus = mcselect( "K+" , kaons , MC3Q > 0 ) ;
     *
     *  @endcode
     *
     *  - The example illustrate the 'selection'/'filtering of MC particles, 
     *    which are @c K+ or @c K- and have a
     *  momentum in excess of 5GeV/c
     *  - The selected MC particles
     *  are stored inside local LoKi storage under the tag @c "Kaons"
     *  and returned as a sequence of MC particles @c kaons .
     *
     *  @see LHCb::MCParticle 
     *  @see LoKi::Types::MCRange
     *  @see LoKi::Cuts::MCABSID 
     *  @see LoKi::Cuts::MCP
     *  @see LoKi::Cuts::MC3Q
     *  @param name  name/tag assigned to the selected particles
     *  @param range input MC-particles 
     *  @param cut   cut to be applied
     *  @return selected range of particles
     */    
    LoKi::Types::MCRange 
    mcselect 
    ( const std::string&          tag   , 
      const LoKi::Types::MCRange& range ,
      const LoKi::Types::MCCuts&  cut   ) 
    {
      return mcselect ( tag , range.begin() , range.end() , cut ) ;
    } ;

    /** 'Select' the MC particles to be used in local storage
     *  
     *  @code
     *
     *  LHCb::MCParticle::Vector kaons = ... ;
     *  MCRange kPlus = mcselect( "K+" , kaons , MC3Q > 0 ) ;
     *
     *  @endcode
     *
     *  @see LHCb::MCParticle 
     *  @see LoKi::Types::MCRange
     *  @see LoKi::Cuts::MC3Q
     *  @param name name/tag assigned to the selected particles
     *  @param range input MC-particles 
     *  @param cut  cut to be applied
     *  @return selected range of particles
     */
//     LoKi::Types::MCRange 
//     mcselect 
//     ( const std::string&              tag   , 
//       const LHCb::MCParticle::Vector& range ,
//       const LoKi::Types::MCCuts&      cut   ) 
//     {
//       return mcselect ( tag , range.begin() , range.end() , cut ) ;
//     } ;
    
    /** 'Select' the MC particles to be used in local storage
     *  
     *  @code
     *
     *  LHCb::MCParticle::ConstVector kaons = ... ;
     *  MCRange kPlus = mcselect( "K+" , kaons , MC3Q > 0 ) ;
     *
     *  @endcode
     *
     *  @see LHCb::MCParticle 
     *  @see LoKi::Types::MCRange
     *  @see LoKi::Cuts::MC3Q
     *  @param name name/tag assigned to the selected particles
     *  @param range input MC-particles 
     *  @param cut  cut to be applied
     *  @return selected range of particles
     */
//     LoKi::Types::MCRange 
//     mcselect 
//     ( const std::string&                   tag   , 
//       const LHCb::MCParticle::ConstVector& range ,
//       const LoKi::Types::MCCuts&           cut   ) 
//     {
//       return mcselect ( tag , range.begin() , range.end() , cut ) ;
//     } ;

    /** 'Select' the MC particles to be used in local storage
     *  
     *  @code
     *
     *  const LHCb::MCParticles* kaons = ... ;
     *  MCRange kPlus = mcselect( "K+" , kaons , MC3Q > 0 ) ;
     *
     *  @endcode
     *
     *  @see LHCb::MCParticle 
     *  @see LoKi::Types::MCRange
     *  @see LoKi::Cuts::MC3Q
     *  @param name name/tag assigned to the selected particles
     *  @param range input MC-particles 
     *  @param cut  cut to be applied
     *  @return selected range of particles
     */
    LoKi::Types::MCRange 
    mcselect 
    ( const std::string&                   tag   , 
      const LHCb::MCParticles*             range ,
      const LoKi::Types::MCCuts&           cut   ) 
    {
      if ( 0 == range ) 
      {
        Error ( "LHCb::MCParticles* points to null, return empty range!" ) ;
        return LoKi::Types::MCRange() ;
      }
      return mcselect ( tag , range->begin() , range->end() , cut ) ;
    } ;
    /** 'Select' the MC particles to be used in local storage
     *  
     *  @code
     *
     *  SEQUENCE kaons = ... ;
     *  MCRange kPlus = mcselect( "K+" , 
     *                 kaons.begin () , 
     *                 kaons.end   () , MC3Q > 0 ) ;
     *
     *  @endcode
     *
     *  @see LHCb::MCParticle 
     *  @see LoKi::Types::MCRange
     *  @see LoKi::Cuts::MC3Q
     *  @param name name/tag assigned to the selected particles
     *  @param first 'begin'-itertor for input  MC-particles 
     *  @param last  'end'-itertor for input  MC-particles 
     *  @param cut  cut to be applied
     *  @return selected range of particles
     */
    template <class MCPARTICLE, class CUT>
    LoKi::Types::MCRange          
    mcselect    
    ( const std::string&   name   , 
      MCPARTICLE           first  , 
      MCPARTICLE           last   , 
      const CUT&           cut    ) 
    {
      return m_mcselected.add ( name , first , last , cut ) ;
    } ;
    
  public:
    
    /** 'Select' the MC vertices to be used in local storage
     *  
     *  - The MC vertices are selected from the TES location
     *
     *  @code
     *
     *  MCVRange primary = mcvselect( "MCPVs" , MCISPRIMARY );
     *
     *  @endcode
     *
     *  @see LHCb::MCVertex
     *  @see LoKi::Types::MCVRange
     *  @see LoKi::Cuts::MCISPRIMARY 
     *  @param name name/tag assigned to the selected particles
     *  @param cut  cut to be applied
     *  @param location TES location of MC-vertices
     *  @return selected range of particles
     */
    LoKi::Types::MCVRange 
    mcvselect 
    ( const std::string&          tag                                          , 
      const LoKi::Types::MCVCuts& cut                                          ,
      const std::string&          location = LHCb::MCVertexLocation::Default ) ;
    
    /** 'Select' the MC vertices to be used in local storage
     *  
     *  @code
     *
     *  MCVRange all     =  ... ;
     *  MCVRange primary = mcvselect( "MCPVs" , all , MCISPRIMARY );
     *
     *  @endcode
     *
     *  @see LHCb::MCVertex
     *  @see LoKi::Types::MCVRange
     *  @see LoKi::Cuts::MCISPRIMARY 
     *  @param name name/tag assigned to the selected particles
     *  @param range input MC-vertices
     *  @param cut  cut to be applied
     *  @return selected range of particles
     */
    LoKi::Types::MCVRange 
    mcvselect 
    ( const std::string&           tag   ,
      const LoKi::Types::MCVRange& range ,
      const LoKi::Types::MCVCuts&  cut   )
    {
      return mcvselect ( tag , range.begin() , range.end()  , cut ) ;
    } ;

    /** 'Select' the MC vertices to be used in local storage
     *  
     *  @code
     *
     *  LHCb::MCVertex::Vector all = ... ;  
     *  MCVRange primary = mcvselect( "MCPVs" , all , MCISPRIMARY );
     *
     *  @endcode
     *
     *  @see LHCb::MCVertex
     *  @see LoKi::Types::MCVRange
     *  @see LoKi::Cuts::MCISPRIMARY 
     *  @param name name/tag assigned to the selected particles
     *  @param range input MC-vertices
     *  @param cut  cut to be applied
     *  @return selected range of particles
     */
//     LoKi::Types::MCVRange 
//     mcvselect 
//     ( const std::string&              tag   ,
//       const LHCb::MCVertex::Vector&   range ,
//       const LoKi::Types::MCVCuts&     cut   )
//     {
//       return mcvselect ( tag , range.begin() , range.end()  , cut ) ;
//     } ;

    /** 'Select' the MC vertices to be used in local storage
     *  
     *  @code
     *
     *  LHCb::MCVertex::ConstVector all = ... ;  
     *  MCVRange primary = mcvselect( "MCPVs" , all , MCISPRIMARY );
     *
     *  @endcode
     *
     *  @see LHCb::MCVertex
     *  @see LoKi::Types::MCVRange
     *  @see LoKi::Cuts::MCISPRIMARY 
     *  @param name name/tag assigned to the selected particles
     *  @param range input MC-vertices
     *  @param cut  cut to be applied
     *  @return selected range of particles
     */
//     LoKi::Types::MCVRange 
//     mcvselect 
//     ( const std::string&                   tag   ,
//       const LHCb::MCVertex::ConstVector&   range ,
//       const LoKi::Types::MCVCuts&          cut   )
//     {
//       return mcvselect ( tag , range.begin() , range.end()  , cut ) ;
//     } ;

    /** 'Select' the MC vertices to be used in local storage
     *  
     *  @code
     *
     *  const LHCb::MCVertices* all = ... ;  
     *  MCVRange primary = mcvselect( "MCPVs" , all , MCISPRIMARY );
     *
     *  @endcode
     *
     *  @see LHCb::MCVertex
     *  @see LoKi::Types::MCVRange
     *  @see LoKi::Cuts::MCISPRIMARY 
     *  @param name name/tag assigned to the selected particles
     *  @param range input MC-vertices
     *  @param cut  cut to be applied
     *  @return selected range of particles
     */
    LoKi::Types::MCVRange 
    mcvselect 
    ( const std::string&                   tag   ,
      const LHCb::MCVertices*              range ,
      const LoKi::Types::MCVCuts&          cut   )
    {
      if ( 0 == range ) 
      {
        Error ( "LHCb::MCVertices* points to null, return empty range!" ) ;
        return LoKi::Types::MCVRange() ;
      }
      return mcvselect ( tag , range->begin() , range->end()  , cut ) ;
    } ;
    
    /** 'Select' the MC particles to be used in local storage
     *  
     *  @code
     *
     *  SEQUENCE all = ... ;
     *  MCVRange primary = mcvselect( "MCPVs" , 
     *                     all.begin () , 
     *                     all.end   () , MCPRIMARY ) ;
     *
     *  @endcode
     *
     *  @see LHCb::MCVertex
     *  @see LoKi::Types::MCVRange
     *  @see LoKi::Cuts::MCPRIMARY
     *  @param name name/tag assigned to the selected particles
     *  @param first 'begin'-itertor for input  MC-particles 
     *  @param last  'end'-itertor for input  MC-particles 
     *  @param cut  cut to be applied
     *  @return selected range of particles
     */
    template <class MCVERTEX, class CUT>
    LoKi::Types::MCVRange          
    mcvselect    
    ( const std::string&   name   , 
      MCVERTEX             first  , 
      MCVERTEX             last   , 
      const CUT&           cut    ) 
    {
      return m_mcvselected.add ( name , first , last , cut ) ;
    } ;

  public:
    
    /** Extract the selected MC-particles from local LoKi storage  
     *   by their name/tag
     *  
     *  @code
     *
     *  MCRange kaons = mcselected( "Kaons");
     *
     *  @endcode
     *
     *  @param name name/tag assigned to the selected vertices
     *  @return selected range of particles
     */
    LoKi::Types::MCRange 
    mcselected 
    ( const std::string& tag ) const { return m_mcselected( tag ) ; } ;
    
    /** Extract the selected MC-vertices from local LoKi storage  
     *  by their name/tag
     *  
     *  @code
     *
     *  MCVRange primaries = mcvselected( "MCPVs");
     *
     *  @endcode
     *
     *  @param name name/tag assigned to the selected vertices
     *  @return selected range of particles
     */
    LoKi::Types::MCVRange 
    mcvselected 
    ( const std::string& tag ) const { return m_mcvselected( tag ) ; } ;

  public:
    
    /// get LoKi::MCFinder object
    LoKi::MCFinder 
    mcFinder ( const std::string& name = "" ) const ;
    
  protected:
    /** standard constructor 
     *  @param name algorithm instance name 
     *  @param pSvc pointer to Service Locator 
     */
    AlgoMC
    ( const std::string& name , 
      ISvcLocator*       pSvc ) ;
    /// virtual and protected destructor 
    virtual ~AlgoMC() ;
  private:
    // the actual storage of selected MC-particles
    LoKi::MCTypes::MCSelected  m_mcselected  ; ///< the actual storage of MC-particles
    // the actual storage of selected MC-vertices
    LoKi::MCTypes::MCVSelected m_mcvselected ; ///< the actual storage of MC-vertices
    // collection  of MCFinder objects
    typedef GaudiUtils::HashMap<std::string,LoKi::MCFinderObj*> MCfinders ;
    mutable MCfinders  m_mcfinders  ;
    // collection  of IMCDecayFinder objects
    typedef GaudiUtils::HashMap<std::string,IMCDecayFinder* > IMCfinders ;
    mutable IMCfinders m_imcfinders  ;
    /// the actual type for collection of TES address
    typedef std::vector<std::string> Addresses ;
    Addresses    m_P2MC  ;
    Addresses    m_P2MCw ;
    Addresses    m_PP2MC ;
    Addresses    m_TR2MC ;
  } ;
  
} ; /// end of namespace LoKi ;

// ============================================================================
/// The END 
// ============================================================================
#endif // LOKI_ALGOMC_H
// ============================================================================
