// $Id: ICaloTriggerAdcsFromRaw.h,v 1.5 2009-10-12 16:03:53 odescham Exp $
#ifndef CALODAQ_ICALOTRIGGERADCSFROMRAW_H 
#define CALODAQ_ICALOTRIGGERADCSFROMRAW_H 1

// Include files
// from STL
#include <string>

#include "ICaloReadoutTool.h"
#include "Event/L0CaloAdc.h"

static const InterfaceID IID_ICaloTriggerAdcsFromRaw ( "ICaloTriggerAdcsFromRaw", 3, 0 );

/** @class ICaloTriggerAdcsFromRaw ICaloTriggerAdcsFromRaw.h CaloDAQ/ICaloTriggerAdcsFromRaw.h
 *  INterface for trigger adc decoding
 *
 *  @author Olivier Callot
 *  @date   2005-01-05
 */
class ICaloTriggerAdcsFromRaw : virtual public ICaloReadoutTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ICaloTriggerAdcsFromRaw; }

  virtual const std::vector<LHCb::L0CaloAdc>&    adcs( ) = 0;
  virtual const std::vector<LHCb::L0CaloAdc>&    adcs( int source ) = 0;
  virtual const std::vector<LHCb::L0CaloAdc>&    adcs( LHCb::RawBank* bank ) = 0;
  virtual const std::vector<LHCb::L0CaloAdc>&    pinAdcs( ) = 0;

protected:

private:

};
#endif // CALODAQ_ICALOTRIGGERADCSFROMRAW_H
