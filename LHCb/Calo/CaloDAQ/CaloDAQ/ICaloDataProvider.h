
// $Id: 
#ifndef CALODAQ_ICALODATAPROVIDER_H 
#define CALODAQ_ICALODATAPROVIDER_H 1

// Include files
// from STL
#include <string>


#include "CaloDAQ/ICaloReadoutTool.h"
#include "Event/CaloAdc.h"
#include "Event/CaloDigit.h"
#include "CaloKernel/CaloVector.h"

static const InterfaceID IID_ICaloDataProvider ( "ICaloDataProvider", 3, 0 );

/** @class ICaloDataProvider ICaloDataProvider.h CaloDAQ/ICaloDataProvider.h
 *  Interface to the tool for fast decoding of CaloBanks
 *
 *  @author Olivier Deschamps
 *  @date   2007-02-27
 */
class ICaloDataProvider : virtual public ICaloReadoutTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ICaloDataProvider; }


  virtual int adc(LHCb::CaloCellID id)=0;
  virtual double digit(LHCb::CaloCellID id)=0;
  virtual unsigned int nTell1s()=0;
  virtual const CaloVector<LHCb::CaloAdc>& adcs(int source=-1,bool clean=true)=0;
  virtual const CaloVector<LHCb::CaloDigit>& digits(int source=-1,bool clean=true)=0;
  virtual const CaloVector<LHCb::CaloAdc>&  adcs(std::vector<int> sources,bool clean=true)=0;
  virtual const CaloVector<LHCb::CaloDigit>& digits(std::vector<int> sources,bool clean=true)=0;
  typedef std::pair<LHCb::CaloAdc,LHCb::CaloAdc> CaloAdcPair;
  virtual CaloAdcPair adcRange() = 0;
  virtual CaloAdcPair pinRange() = 0;

protected:

private:

};
#endif // CALODAQ_ICALODATAPROVIDER_H
