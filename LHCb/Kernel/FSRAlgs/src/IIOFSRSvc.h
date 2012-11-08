#ifndef GAUDIKERNEL_IIOFSRSVC_H
#define GAUDIKERNEL_IIOFSRSVC_H

// Include files
#include "GaudiKernel/IService.h"


/** @class ICounterSummarySvc ICounterSummarySvc.h
 *
 *  Simple service interface to return/store IOFSRS
 *  
 *
 *  @see IOFSRSvc
 *
 *  @author Rob Lambert
 *  @date   2012-11-26
 */
class GAUDI_API IIOFSRSvc: virtual public IService {

  

public:
  /// InterfaceID
  DeclareInterfaceID(IIOFSRSvc,1,0);
  
  //merge existing IOFSRs into the maps of this service
  virtual StatusCode mergeIOFSRs()=0;
  //remove any IOFSR at the top level. To be called before requesting a new FSR.
  virtual StatusCode cleanTopFSR()=0;
  //Create a new IOFSR, store in the TES
  virtual StatusCode storeIOFSR(const std::string & outputName)=0;
  //Does the accounting all add up OK?
  virtual bool traceCounts()=0;
  
  //print contained information
  virtual void print()=0;
  

};

#endif
