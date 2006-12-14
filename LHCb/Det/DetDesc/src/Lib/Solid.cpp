// $Id: Solid.cpp,v 1.6 2006-12-14 13:03:42 ranjard Exp $
/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/** STD & STL includes  */ 
#include <string>
#include <vector>
#include <functional>
#include <algorithm>

/** GaudiKernel includes */ 
#include "Reflex/PluginService.h"

/** DetDesc includes */ 
#include "DetDesc/ISolid.h"
#include "DetDesc/Solid.h"
#include "DetDesc/SolidException.h"

// ============================================================================
/** @file Solid.cpp
 * 
 * Implementation file for methods from namespace Solid 
 * @date  09/08/2001 
 * @author Vanya  Belyaev Ivan.Belyaev@itep.ru 
 */
// ============================================================================

// ============================================================================
/** create (using abstract factory technique) the 
 *  concrete solid of given type 
 *  
 *  @see ISolid
 *  @see ISolidFactory
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date 03/08/2001 
 *
 *  @exception SolidException  wrong solid type or environment 
 *  @param solidType   type of solid 
 *  @return pointer to new created ISolid object 
 */
// ============================================================================
ISolid* Solid::createSolid( const std::string& solidType )
{
  ISolid* solid = ROOT::Reflex::PluginService::Create<ISolid*>(solidType);
  if (!solid) {
    throw SolidException("Solid: no solid factory for type '" + solidType + "' is found!");
  }
  return solid;
};
