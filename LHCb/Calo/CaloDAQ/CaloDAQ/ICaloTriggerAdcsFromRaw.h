// $Id: ICaloTriggerAdcsFromRaw.h,v 1.2 2005-12-19 19:29:14 ocallot Exp $
#ifndef CALODAQ_ICALOTRIGGERADCSFROMRAW_H 
#define CALODAQ_ICALOTRIGGERADCSFROMRAW_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Event/L0CaloAdc.h"

static const InterfaceID IID_ICaloTriggerAdcsFromRaw ( "ICaloTriggerAdcsFromRaw", 1, 0 );

/** @class ICaloTriggerAdcsFromRaw ICaloTriggerAdcsFromRaw.h CaloDAQ/ICaloTriggerAdcsFromRaw.h
 *  INterface for trigger adc decoding
 *
 *  @author Olivier Callot
 *  @date   2005-01-05
 */
class ICaloTriggerAdcsFromRaw : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ICaloTriggerAdcsFromRaw; }

  virtual std::vector<LHCb::L0CaloAdc>&    adcs( ) = 0;

protected:

private:

};
#endif // CALODAQ_ICALOTRIGGERADCSFROMRAW_H
