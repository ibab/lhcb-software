// $Id: DetDesc_load.cpp,v 1.2 2006-12-14 13:03:41 ranjard Exp $
// Include files
#include "GaudiKernel/ObjectFactory.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
// DetDesc
#include "DetDesc/ISolidFactory.h"
// Objects from DetDesc package to be implemented:
#include "DetDesc/LVolume.h"
#include "DetDesc/LAssembly.h"
#include "DetDesc/DetectorElement.h"
#include "DetDesc/Isotope.h"
#include "DetDesc/Element.h"
#include "DetDesc/Mixture.h"
#include "DetDesc/TabulatedProperty.h"
#include "DetDesc/Surface.h"

// ============================================================================
/** @file 
 *  Implementation of Data Object Factories and declaration of ALL 
 *  Factories from DetDesc package 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date xx/xx/xxxx 
 */
// ============================================================================

// ============================================================================
/** @def IMPLEMENT_OBJECT(x) 
 *  Implement Data Object Factory
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 06 May 2002 
 */

// ============================================================================
DECLARE_DATAOBJECT_FACTORY( DetectorElement    ) ;
DECLARE_DATAOBJECT_FACTORY( Isotope           ) ;
DECLARE_DATAOBJECT_FACTORY( Mixture           ) ;
DECLARE_DATAOBJECT_FACTORY( Element           ) ;
DECLARE_DATAOBJECT_FACTORY( LVolume           ) ;
DECLARE_DATAOBJECT_FACTORY( LAssembly         ) ;
DECLARE_DATAOBJECT_FACTORY( Surface           ) ;
DECLARE_DATAOBJECT_FACTORY( TabulatedProperty ) ;

// ============================================================================
// The END 
// ============================================================================
