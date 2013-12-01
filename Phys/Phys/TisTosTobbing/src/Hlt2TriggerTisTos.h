// $Id: $
#ifndef HLT2TRIGGERTISTOS_H 
#define HLT2TRIGGERTISTOS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/ITriggerTisTos.h"            // Interface

#include "TriggerTisTos.h"

/** @class Hlt2TriggerTisTos Hlt2TriggerTisTos.h
 *  
 *  @author Tomasz Skwarnicki
 *  @date   2013-11-30
 *
 *  Same tool as TriggerTisTos; just different input locations
 */
class Hlt2TriggerTisTos : public TriggerTisTos, virtual public ITriggerTisTos {
public: 
  /// Standard constructor
  Hlt2TriggerTisTos( const std::string& type, 
                   const std::string& name,
                   const IInterface* parent);

  virtual ~Hlt2TriggerTisTos( ); ///< Destructor

  virtual StatusCode         initialize(); 

  
private:


};
#endif // HLT2TRIGGERTISTOS_H
