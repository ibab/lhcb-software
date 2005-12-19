// $Id: ICaloTriggerBitsFromRaw.h,v 1.2 2005-12-19 19:29:14 ocallot Exp $
#ifndef CALODAQ_ICALOTRIGGERBITSFROMRAW_H 
#define CALODAQ_ICALOTRIGGERBITSFROMRAW_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Kernel/CaloCellID.h"

static const InterfaceID IID_ICaloTriggerBitsFromRaw ( "ICaloTriggerBitsFromRaw", 1, 0 );

/** @class ICaloTriggerBitsFromRaw ICaloTriggerBitsFromRaw.h CaloDAQ/ICaloTriggerBitsFromRaw.h
 *  Interface to the Prs/SPD bit decoding
 *
 *  @author Olivier Callot
 *  @date   2005-11-07
 */
class ICaloTriggerBitsFromRaw : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ICaloTriggerBitsFromRaw; }

  virtual std::vector<LHCb::CaloCellID>& firedCells( bool isPrs ) = 0;

protected:

private:

};
#endif // CALODAQ_ICALOTRIGGERBITSFROMRAW_H
