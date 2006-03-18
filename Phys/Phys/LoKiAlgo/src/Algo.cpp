// $Id: Algo.cpp,v 1.1.1.1 2006-03-18 10:39:21 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1.1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Include files 
// ============================================================================
#include "LoKi/Algo.h"
// ============================================================================

// ============================================================================
/** @file
 *
 * Implementation file for class LoKi::Algo
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
 *  @date 2006-03-14 
 */
// ============================================================================

// ============================================================================
/** standard constructor 
 *  @param name algorithm instance name 
 *  @param pSvc pointer to Service Locator 
 */
// ============================================================================
LoKi::Algo::Algo 
( const std::string& name , 
  ISvcLocator*       pSvc ) 
  : DVAlgorithm ( name , pSvc )
  // local storage of selected particles 
  , m_selected  ( ) 
  // local storage of selected paricles 
  , m_vselected ( ) 
{};
// ============================================================================
/// virtual and protected destructor 
// ============================================================================
LoKi::Algo::~Algo(){} ;
// ============================================================================

// ============================================================================
/** 'Select' the particles to be used in local storage
 *  
 *  - The Particles are selected from the desktop
 *
 *  @code
 *
 *  Range kaons = select( "Kaons" , ABSID == "K+" && P > 5 * GeV );
 *
 *  @endcode
 *
 *  - The example illustrate the 'selection'/'filtering from
 *  desktop the particles, which are @c K+ or @c K- and have a
 *  momentum in excess of 5GeV/c
 *  - The selected particles
 *  are stored inside local LoKi storage under the tag @c "Kaons"
 *  and returned as a sequence of particles @c kaons .
 *
 *  @see LoKi::Types::Range 
 *  @see LoKi::Types::Cuts 
 *  @see LoKi::Cuts::ABSID 
 *  @see LoKi::Cuts::P
 *  @param name name/tag assigned to the selected particles
 *  @param cut  cut to be applied
 *  @return selected range of particles
 */
// ============================================================================
LoKi::Types::Range 
LoKi::Algo::select    
( const std::string&        name ,
  const LoKi::Types::Cuts&  cut  ) 
{
  // get all particles from desktop
  const LHCb::Particle::ConstVector& particles = 
    desktop()->particles();
  // add the particles to the local storage 
  return select ( name , particles.begin() , particles.end() , cut ) ;
} ;
// ============================================================================
/** 'Select' the vertices to be used in local storage
 *  - Vertices are selected from desktop
 *  @param name name/tag assigned to the selected vertices
 *  @param cut  cut to be applied
 *  @return selected range of vertices
 */
// ============================================================================
LoKi::Types::VRange         
LoKi::Algo::vselect   
( const std::string&         name  ,
  const LoKi::Types::VCuts&  cut   ) 
{
  // get all PRIMARY particles from desktop
  const LHCb::PrimVertex::ConstVector& prims = desktop()->primaryVertices();
  //
  vselect ( name , prims.begin() , prims.end() , cut ) ;
  // get all SECONDARY  particles from desktop
  const LHCb::Vertex::ConstVector&     secs  = desktop()->secondaryVertices();
  // 
  return vselect ( name , secs.begin() , secs.end() , cut ) ;
} ;
// ============================================================================





// ============================================================================
// The END 
// ============================================================================


