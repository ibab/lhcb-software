/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.1  2001/08/09 18:13:39  ibelyaev
/// modification for solid factories
/// 
/// ===========================================================================
///@{
/** DetDesc includes */ 
#include "DetDesc/SolidFactory.h"
#include "DetDesc/SolidBox.h"
#include "DetDesc/SolidChild.h"
#include "DetDesc/SolidCons.h"
#include "DetDesc/SolidIntersection.h"
#include "DetDesc/SolidSphere.h"
#include "DetDesc/SolidSubtraction.h"
#include "DetDesc/SolidTrap.h"
#include "DetDesc/SolidTrd.h"
#include "DetDesc/SolidTubs.h"
#include "DetDesc/SolidPolycone.h"
#include "DetDesc/SolidUnion.h"
///@}

// ============================================================================
/** @file SolidFactories.cpp
 * 
 *  Implementation of all concrete solid factories 
 * 
 *  @date    09/08/2001 
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru 
 */
// ============================================================================

// ===========================================================================
/** @defGroup 
 *  SolidBoxFactory Factory business for solids  
 *  @{
 */ 
// ===========================================================================
static const SolidFactory<SolidBox>                  s_FactoryBox       ;
const       ISolidFactory&SolidBoxFactory          = s_FactoryBox       ;
static const SolidFactory<SolidChild>                s_FactoryChild     ;
const       ISolidFactory&SolidChildFactory        = s_FactoryChild     ;
static const SolidFactory<SolidCons>                 s_FactoryCons      ;
const       ISolidFactory&SolidConsFactory         = s_FactoryCons      ;
static const SolidFactory<SolidIntersection>         s_FactoryInt       ;
const       ISolidFactory&SolidIntersectionFactory = s_FactoryInt       ;
static const SolidFactory<SolidSphere>               s_FactorySphere    ;
const       ISolidFactory&SolidSphereFactory       = s_FactorySphere    ;
static const SolidFactory<SolidSubtraction>          s_FactorySub       ;
const       ISolidFactory&SolidSubtractionFactory  = s_FactorySub       ;
static const SolidFactory<SolidTrap>                 s_FactoryTrap      ;
const       ISolidFactory&SolidTrapFactory         = s_FactoryTrap      ;
static const SolidFactory<SolidTrd>                  s_FactoryTrd       ;
const       ISolidFactory&SolidTrdFactory          = s_FactoryTrd       ;
static const SolidFactory<SolidTubs>                 s_FactoryTubs      ;
const       ISolidFactory&SolidTubsFactory         = s_FactoryTubs      ;
static const SolidFactory<SolidUnion>                s_FactoryUnion     ;
const       ISolidFactory&SolidUnionFactory        = s_FactoryUnion     ;
static const SolidFactory<SolidPolycone>             s_FactoryPolycone  ;
const       ISolidFactory&SolidPolyconeFactory     = s_FactoryPolycone  ;
///@} 
// ===========================================================================
