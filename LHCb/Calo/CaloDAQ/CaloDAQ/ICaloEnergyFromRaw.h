// $Id: ICaloEnergyFromRaw.h,v 1.4 2007-02-22 23:39:52 odescham Exp $
#ifndef CALODAQ_ICALOENERGYFROMRAW_H 
#define CALODAQ_ICALOENERGYFROMRAW_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

#include "Event/RawEvent.h"
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

  virtual std::vector<LHCb::CaloAdc>&   adcs( ) = 0;
  virtual std::vector<LHCb::CaloAdc>&   adcs( int source ) = 0;
  virtual std::vector<LHCb::CaloAdc>&   adcs( LHCb::RawBank* bank ) = 0;
  virtual std::vector<LHCb::CaloAdc>&   pinAdcs( ) = 0;
  
  virtual std::vector<LHCb::CaloDigit>& digits( ) = 0;
  virtual std::vector<LHCb::CaloDigit>& digits( int source  ) = 0;
  virtual std::vector<LHCb::CaloDigit>& digits( LHCb::RawBank* bank ) = 0;

  virtual StatusCode  getCaloBanks()=0;
  virtual void setBanks(const std::vector<LHCb::RawBank*>* bank ) = 0;
  

  
  
protected:

private:

};
#endif // CALODAQ_ICALOENERGYFROMRAW_H
