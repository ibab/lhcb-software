// $Id: CheckSelResultsTool.h,v 1.2 2008-11-03 17:01:14 jpalac Exp $
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
 *  <b>Configuration properties:</b>
 *
 *  UseSelResults: bool controlling whether SelResults from
 *  LHCb::SelResultLocation::Default are used to determine if algorithm passed.
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

  bool isSelected() const {
    Exception("Method isSelected() is not implemented");
    return false ;
  } ;

  bool isSelected ( const Selection  & selection          ) const  ;

  bool isSelected ( const Selections & selections, 
                    const bool ANDMode = false ) const  ;

private : 
  bool m_useSelResults ; ///< Check also in SelResults if algorithm passed
  std::string m_selResults;        ///< Location of selresults container
};
#endif // CHECKSELRESULTSTOOL_H
