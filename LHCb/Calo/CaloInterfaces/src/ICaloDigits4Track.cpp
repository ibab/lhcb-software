// $Id: ICaloDigits4Track.cpp,v 1.1 2009-10-25 14:40:11 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// local
// ============================================================================
#include "CaloInterfaces/ICaloDigits4Track.h"
// ============================================================================
/** @file 
 *  Implementation file for class ICaloDigits4Track
 *  @see ICaloDigits4Track
 *  @date 2009-10-25 
 *  @author Vanya Belyaev Ivan.Belyaev@nikhef.nl
 */
// ============================================================================
/*  static interface identification
 *  @see IInterface
 *  @return unique interface identifier
 */
// ============================================================================
const InterfaceID& ICaloDigits4Track::interfaceID () 
{
  static const InterfaceID s_ID ( "ICaloDigits4Track" , 1 , 0 ) ;
  return s_ID ;
}
// ============================================================================
// virtual & protected destructor 
// ============================================================================
ICaloDigits4Track::~ICaloDigits4Track(){}
// ============================================================================
// The END 
// ============================================================================
