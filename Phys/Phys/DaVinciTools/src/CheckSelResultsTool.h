// $Id: CheckSelResultsTool.h,v 1.1 2008-10-31 18:41:41 pkoppenb Exp $
#ifndef CHECKSELRESULTSTOOL_H 
#define CHECKSELRESULTSTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/ICheckSelResults.h"            // Interface

/** @class CheckSelResultsTool CheckSelResultsTool.h
 *  
 *  Implementation of ICheckSelResults interface.
 *
 *  Checks if an algorithm has passed.
 *
 *  @author Patrick Koppenburg
 *  @date   2008-10-31
 */
class CheckSelResultsTool : public GaudiTool, virtual public ICheckSelResults {
public: 
  /// Standard constructor
  CheckSelResultsTool( const std::string& type, 
                       const std::string& name,
                       const IInterface* parent);

  virtual ~CheckSelResultsTool( ); ///< Destructor

  /** check if the event has been (pre)selected 
   *  
   *  This method is not implemented as it is not well defined
   *
   *  @return true if the event has been (pre)selected 
   */
  bool isSelected() const {
    Exception("Method isSelected() is not implemented");
    return false ;
  } ;
  
  /** check if the event has been (pre)selected by a certain selection
   *  @param  selection selection name 
   *  @return true if the event has been (pre)selected 
   */
  bool isSelected ( const Selection  & selection          ) const  ;
  
  /** check if the event has been (pre)selected by certain selection(s)
   *  @param  selections vector of selection names 
   *  @param  ANDMode    flag to distinguish AND/OR modes 
   *  @return true if the event has been (pre)selected by *ALL* 
   *    selections (AND-mode) or by at least one preselection (OR-mode)
   */
  bool isSelected ( const Selections & selections, 
                    const bool ANDMode = false ) const  ;

protected :

private : 
  bool m_useSelResults ; ///< Check also in SelResults if algorithm passed
  std::string m_selResults;        ///< Location of selresults container
};
#endif // CHECKSELRESULTSTOOL_H
