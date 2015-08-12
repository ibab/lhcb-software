// $Id$
// Include files

// from Gaudi
#include "GaudiKernel/SmartIF.h"  // TMP
#include "GaudiKernel/IAlgorithm.h"// TMP
#include "GaudiKernel/IAlgManager.h"// TMP

#include "LoKi/AlgFunctors.h"

// local
#include "CheckSelResultsTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CheckSelResultsTool
//
// 2008-10-31 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( CheckSelResultsTool )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  CheckSelResultsTool::CheckSelResultsTool( const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent )
    : GaudiTool( type, name , parent )
{
  declareInterface<ICheckSelResults>(this);
}

//=============================================================================
/** check if the event has been (pre)selected by a certain selection
 *  @param  selection selection name
 *  @return true if the event has been (pre)selected
 */
//=============================================================================
bool CheckSelResultsTool::isSelected ( const Selection  & selection) const {

  if (msgLevel(MSG::VERBOSE)) verbose() << selection << " " << serviceLocator() << endmsg ;
  // @todo : temporary protection against Executing bombing
  SmartIF<IAlgManager> iam(serviceLocator()) ;
  //  if ( 0==iam ) Exception("No IAlgManager");
  //  else if (msgLevel(MSG::VERBOSE)) verbose() << iam << endmsg ;
  IAlgorithm* _a = 0 ;
  StatusCode sc = iam->getAlgorithm ( selection , _a ) ;
  if (!sc) return false ;
  // end temporary protection

  LoKi::Algorithms::Executed exec(selection) ;
  if ( exec()) {
    LoKi::Algorithms::Passed pass(selection) ;
    if (msgLevel(MSG::DEBUG)) debug() << selection << " ran during this processing. Result is "
                                      << pass() << endmsg ;
    return pass() ;
  }

  if (msgLevel(MSG::DEBUG)) {
    debug() << selection << " has not been run." << endmsg ;
  }
  return false ;
}

//=============================================================================
/** check if the event has been (pre)selected by certain selection(s)
 *  @param  selections vector of selection names
 *  @param  ANDMode    flag to distinguish AND/OR modes
 *  @return true if the event has been (pre)selected by *ALL*
 *    selections (AND-mode) or by at least one preselection (OR-mode)
 */
//=============================================================================
bool CheckSelResultsTool::isSelected ( const Selections & selections ,
                                       const bool ANDMode) const {
  for ( Selections::const_iterator isel = selections.begin() ; isel != selections.end() ; ++isel){
    bool pass = isSelected(*isel);
    if ( ANDMode ) {
      if (!pass) return false ; // ANDmode : if any fails, fail
    } else {
      if ( pass ) return true ;  // !ANDmode : if any passes, pass
    }
  }
  return ANDMode ; // if reached here : ANDmode : all pass -> true , !ANDMode : all fail -> false
}

//=============================================================================
// Destructor
//=============================================================================
CheckSelResultsTool::~CheckSelResultsTool() {}

//=============================================================================
