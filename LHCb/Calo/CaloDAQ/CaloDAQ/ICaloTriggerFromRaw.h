// $Id: ICaloTriggerFromRaw.h,v 1.1.1.1 2005-01-11 07:51:47 ocallot Exp $
#ifndef CALODAQ_ICALOTRIGGERFROMRAW_H 
#define CALODAQ_ICALOTRIGGERFROMRAW_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Kernel/CaloCellID.h"

static const InterfaceID IID_ICaloTriggerFromRaw ( "ICaloTriggerFromRaw", 1, 0 );

/** @class ICaloTriggerFromRaw ICaloTriggerFromRaw.h CaloDAQ/ICaloTriggerFromRaw.h
 *  INterface for trigger adc decoding
 *
 *  @author Olivier Callot
 *  @date   2005-01-05
 */
class ICaloTriggerFromRaw : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ICaloTriggerFromRaw; }

  virtual void prepare( int type ) = 0;
  
  virtual StatusCode nextCell ( CaloCellID& id, int& adc ) = 0;

protected:

private:

};
#endif // CALODAQ_ICALOTRIGGERFROMRAW_H
