
// $Id: 
#ifndef CALODAQ_ICALOL0DATAPROVIDER_H 
#define CALODAQ_ICALOL0DATAPROVIDER_H 1

// Include files
// from STL
#include <string>


#include "CaloDAQ/ICaloReadoutTool.h"
#include "Event/L0CaloAdc.h"
#include "CaloKernel/CaloVector.h"

static const InterfaceID IID_ICaloL0DataProvider ( "ICaloL0DataProvider", 2, 0 );

/** @class ICaloL0DataProvider ICaloL0DataProvider.h CaloDAQ/ICaloL0DataProvider.h
 *  Interface to the tool for fast decoding of CaloBanks
 *
 *  @author Olivier Deschamps
 *  @date   2007-12-03
 */
class ICaloL0DataProvider : virtual public ICaloReadoutTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ICaloL0DataProvider; }


  virtual int l0Adc(LHCb::CaloCellID id)=0;
  virtual unsigned int nTell1s()=0;
  virtual const CaloVector<LHCb::L0CaloAdc>& l0Adcs(int source=-1,bool clean=true)=0;
  virtual const CaloVector<LHCb::L0CaloAdc>& l0Adcs(std::vector<int> sources,bool clean=true)=0;

protected:

private:

};
#endif // CALODAQ_ICALOL0DATAPROVIDER_H
