// $Id: L0TriggerTisTos.h,v 1.1 2010-06-23 22:39:24 tskwarni Exp $
#ifndef L0TRIGGERTISTOS_H 
#define L0TRIGGERTISTOS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/ITriggerTisTos.h"            // Interface

#include "TriggerTisTos.h"

/** @class L0TriggerTisTos L0TriggerTisTos.h
 *  
 *  @author Tomasz Skwarnicki
 *  @date   2010-06-23
 *
 *  Same tool as TriggerTisTos; just different input locations
 */
class L0TriggerTisTos : public TriggerTisTos, virtual public ITriggerTisTos {
public: 
  /// Standard constructor
  L0TriggerTisTos( const std::string& type, 
                   const std::string& name,
                   const IInterface* parent);

  virtual ~L0TriggerTisTos( ); ///< Destructor

  virtual StatusCode         initialize(); 

  
private:


};
#endif // L0TRIGGERTISTOS_H
