// $Id: 
#ifndef CALODAQ_ICALODATAPROVIDER_H 
#define CALODAQ_ICALODATAPROVIDER_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

#include "Event/RawEvent.h"
#include "Event/CaloAdc.h"
#include "Event/CaloDigit.h"
#include "CaloKernel/CaloVector.h"

static const InterfaceID IID_ICaloDataProvider ( "ICaloDataProvider", 1, 0 );

/** @class ICaloDataProvider ICaloDataProvider.h CaloDAQ/ICaloDataProvider.h
 *  Interface to the tool for fast decoding of CaloBanks
 *
 *  @author Olivier Deschamps
 *  @date   2007-02-27
 */
class ICaloDataProvider : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ICaloDataProvider; }


  virtual StatusCode setBank()=0;
  virtual void clear()=0;
  virtual int adc(LHCb::CaloCellID id)=0;
  virtual double digit(LHCb::CaloCellID id)=0;
  virtual unsigned int nTell1s()=0;
  virtual CaloVector<LHCb::CaloAdc>& adcs(int source=-1)=0;
  virtual CaloVector<LHCb::CaloDigit>& digits(int source=-1)=0;
  virtual std::string rawRoot()=0;

protected:

private:

};
#endif // CALODAQ_ICALODATAPROVIDER_H
