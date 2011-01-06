// $Id: CaloCellIDKeyTraits.cpp,v 1.1 2005-12-14 15:13:30 cattanem Exp $
// ============================================================================
// Include files
#include "GaudiKernel/GaudiException.h"
// local
#include "Kernel/CaloCellIDKeyTraits.h"


// ============================================================================
/** @file CaloCellIDKeyTraits.cpp 
 * 
 *  Implementation file for methods from Containers namespace
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 20/03/2002 
 */
// ============================================================================

// ============================================================================
/** 
 *  function to be called at each attempt of automatic creation of CaloCellID
 *  object as a KEY for KeyedObjects
 *  
 *  @exception GaudiException 
 *  @see GaudiException 
 *  @see Containers 
 *  @see CaloCellID 
 *  @see CaloCellIDKeyTraits
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 20/03/2002  
 */
// ============================================================================
void Containers::errorMakeCaloCellIDKey()
{
  throw 
    GaudiException("No automatic key creation for KEY=LHCb::CaloCellID !", 
                   "KeyedObject<LHCb::CaloCellID>" , StatusCode::FAILURE );
}

// ============================================================================
// The End 
// ============================================================================
