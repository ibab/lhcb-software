// $Id: $
#ifndef HLT1TRIGGERTISTOS_H 
#define HLT1TRIGGERTISTOS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/ITriggerTisTos.h"            // Interface

#include "TriggerTisTos.h"

/** @class Hlt1TriggerTisTos Hlt1TriggerTisTos.h
 *  
 *  @author Tomasz Skwarnicki
 *  @date   2013-11-30
 *
 *  Same tool as TriggerTisTos; just different input locations
 */
class Hlt1TriggerTisTos : public TriggerTisTos, virtual public ITriggerTisTos {
public: 
  /// Standard constructor
  Hlt1TriggerTisTos( const std::string& type, 
                   const std::string& name,
                   const IInterface* parent);

  virtual ~Hlt1TriggerTisTos( ); ///< Destructor

  virtual StatusCode         initialize(); 

  
private:


};
#endif // HLT1TRIGGERTISTOS_H
