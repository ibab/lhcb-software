// $Id: ICaloTrackIdEval.cpp,v 1.1 2009-10-25 14:40:11 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// Local
// ============================================================================
#include "CaloInterfaces/ICaloTrackIdEval.h"
// ============================================================================
/** @file 
 *  Implementation file for class ICaloTrackIdEval
 *  @see ICaloTrackIdEval
 *  @date 2009-10-25 
 *  @author Vanya Belyaev Ivan.Belyaev@nikhef.nl
 */
// ============================================================================
/*  static interface identification
 *  @see IInterface
 *  @return unique interface identifier
 */
// ============================================================================
const InterfaceID& ICaloTrackIdEval::interfaceID () 
{
  static const InterfaceID s_ID ( "ICaloTrackIdEval" , 3 , 0 );
  return s_ID ;
}
// ============================================================================
// virtual & protected destructor 
// ============================================================================
ICaloTrackIdEval::~ICaloTrackIdEval(){}
// ============================================================================
// The END 
// ============================================================================
