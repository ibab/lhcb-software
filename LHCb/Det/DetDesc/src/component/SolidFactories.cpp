// $Id: SolidFactories.cpp,v 1.3 2002-05-04 13:09:14 ibelyaev Exp $
// ===========================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ===========================================================================
// $Log: not supported by cvs2svn $
// ===========================================================================
// DetDesc includes
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
//

// ============================================================================
/** @file
 * 
 *  Implementation of all concrete solid factories 
 *  @see ISolidFactory
 *  @see  SolidFactory
 *  @see      IFactory
 *  @see IMPLEMENT_SOLID
 * 
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date    09/08/2001 
 */
// ============================================================================

// primitive solids 
IMPLEMENT_SOLID( SolidBox           ) ;
IMPLEMENT_SOLID( SolidCons          ) ;
IMPLEMENT_SOLID( SolidSphere        ) ;
IMPLEMENT_SOLID( SolidTrap          ) ;
IMPLEMENT_SOLID( SolidTrd           ) ;
IMPLEMENT_SOLID( SolidTubs          ) ;
IMPLEMENT_SOLID( SolidPolycone      ) ;
// boolean solid 
IMPLEMENT_SOLID( SolidUnion         ) ;
IMPLEMENT_SOLID( SolidIntersection  ) ;
IMPLEMENT_SOLID( SolidSubtraction   ) ;
// helper solid 
IMPLEMENT_SOLID( SolidChild         ) ;

// ============================================================================
// The END 
// ============================================================================
