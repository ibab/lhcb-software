// $Id: ICaloEnergyFromRaw.h,v 1.1.1.1 2005-01-11 07:51:47 ocallot Exp $
#ifndef CALODAQ_ICALOENERGYFROMRAW_H 
#define CALODAQ_ICALOENERGYFROMRAW_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

#include "Kernel/CaloCellID.h"

static const InterfaceID IID_ICaloEnergyFromRaw ( "ICaloEnergyFromRaw", 1, 0 );

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

  virtual void setScaleAndShift( double scale, double shift ) = 0;

  virtual void prepare( int type ) = 0;
  
  virtual StatusCode nextCell( CaloCellID& id, double& energy ) = 0;

protected:

private:

};
#endif // CALODAQ_ICALOENERGYFROMRAW_H
