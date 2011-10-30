// $Id$
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

  bool isSelected() const {
    Exception("Method isSelected() is not implemented");
    return false ;
  } ;

  bool isSelected ( const Selection  & selection          ) const  ;

  bool isSelected ( const Selections & selections, 
                    const bool ANDMode = false ) const  ;

};
#endif // CHECKSELRESULTSTOOL_H
