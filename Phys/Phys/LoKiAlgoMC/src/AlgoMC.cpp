// $Id: AlgoMC.cpp,v 1.1.1.1 2006-04-09 08:44:51 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1.1.1 $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Include files 
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiTool.h"
// ============================================================================
// LoKiMC 
// ============================================================================
#include "LoKi/MCFinder.h"
#include "LoKi/MCFinderObj.h"
// ============================================================================
// LoKiAlgoMC
// ============================================================================
#include "LoKi/AlgoMC.h"
// ============================================================================

// ============================================================================
/** @file
 *
 *  Implementation file for class LoKi::AlgoMC
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

// ============================================================================
/** standard constructor 
 *  @param name algorithm instance name 
 *  @param pSvc pointer to Service Locator 
 */
// ============================================================================
LoKi::AlgoMC::AlgoMC
( const std::string& name , 
  ISvcLocator*       pSvc ) 
  : LoKi::Algo ( name , pSvc ) 
{}
// ============================================================================
/// virtual and protected destructor 
// ============================================================================
LoKi::AlgoMC::~AlgoMC(){} ;
// ============================================================================
/** 'Select' the MC particles to be used in local storage
 *  
 *  - The MC Particles are selected from the location
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
 *  @return selected range of particles
 */
// ============================================================================
LoKi::Types::MCRange 
LoKi::AlgoMC::mcselect 
( const std::string&         tag      , 
  const LoKi::Types::MCCuts& cut      , 
  const std::string&         location ) 
{
  const LHCb::MCParticles* mcps = get<LHCb::MCParticles>( location );
  return mcselect ( tag , mcps , cut ) ;
};
// ============================================================================
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
// ============================================================================
LoKi::Types::MCVRange 
LoKi::AlgoMC::mcvselect 
( const std::string&          tag      , 
  const LoKi::Types::MCVCuts& cut      ,
  const std::string&          location )
{
  const LHCb::MCVertices* mcps = get<LHCb::MCVertices>( location );
  return mcvselect ( tag , mcps , cut ) ;
};
// ============================================================================
/// get LoKi::MCFinder objects
// ============================================================================
LoKi::MCFinder 
LoKi::AlgoMC::mcFinder( const std::string& name ) const 
{
  { // check exisiting tool 
    MCfinders::const_iterator ifind = m_mcfinders.find( name ) ;
    if ( m_mcfinders.end() != ifind ) { return LoKi::MCFinder( ifind->second ) ; }
  }
  // create new finder
  const LoKi::IReporter* rep = reporter ("MCFinder " + name  ) ;
  // get IMCDecayFinder tool 
  IMCDecayFinder* finder = 0 ;
  { // check located tool 
    IMCfinders::const_iterator ifind = m_imcfinders.find( name ) ;
    if ( m_imcfinders.end() == ifind )
    {
      // locate new tool
      finder = tool<IMCDecayFinder>( "MCDecayFinder" , name , this ) ;
      // add the finder into the container 
      m_imcfinders[name] = finder ;
    }
    else { finder = ifind->second ; }
  }
  
  Assert ( 0 != finder , "IMCDecayFinder point to NULL!" );
  
  LoKi::MCFinderObj* obj = 
    new LoKi::MCFinderObj ( name , finder , this , rep ) ;
  
  // add the finder into the contaier 
  m_mcfinders[name] = obj ;
  
  return LoKi::MCFinder ( obj ) ;
} ;
// ============================================================================


  
    
    
    








// ============================================================================
/// The END 
// ============================================================================

