#ifndef MUONDAQ_IMUONTELL1CONFIG_H 
#define MUONDAQ_IMUONTELL1CONFIG_H 1

// from Gaudi
#include "GaudiKernel/IAlgTool.h"


/** @class IMuonTell1Config IMuonTell1Config.h MuonDAQ/IMuonTell1Config.h
 *  
 *
 *  @author 
 *  @date   2008-02-29
 */
class IMuonTell1Config : public extend_interfaces<IAlgTool> {
public: 
  DeclareInterfaceID(  IMuonTell1Config, 2, 0 );

  virtual StatusCode configTell1(int Tell1) const = 0;
};


#endif // MUONDAQ_IMUONTELL1CONFIG_H
