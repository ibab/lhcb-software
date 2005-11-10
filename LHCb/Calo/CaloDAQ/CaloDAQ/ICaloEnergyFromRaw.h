// $Id: ICaloEnergyFromRaw.h,v 1.2 2005-11-10 16:43:22 ocallot Exp $
#ifndef CALODAQ_ICALOENERGYFROMRAW_H 
#define CALODAQ_ICALOENERGYFROMRAW_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

#include "Event/CaloAdc.h"
#include "Event/CaloDigit.h"

static const InterfaceID IID_ICaloEnergyFromRaw ( "ICaloEnergyFromRaw", 2, 0 );

/** @class ICaloEnergyFromRaw ICaloEnergyFromRaw.h CaloDAQ/ICaloEnergyFromRaw.h
 *  Interface to the tool decoding the (cell+energy) from Raw
 *
 *  @author Olivier Callot
 *  @date   2005-01-10
 */
class ICaloEnergyFromRaw : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ICaloEnergyFromRaw; }

  virtual std::vector<CaloAdc>&   adcs( ) = 0;
  
  virtual std::vector<CaloDigit>& digits( ) = 0;
  
protected:

private:

};
#endif // CALODAQ_ICALOENERGYFROMRAW_H
