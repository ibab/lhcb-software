// $Id: IMuonTell1Config.h,v 1.1 2008-04-02 11:53:39 asatta Exp $
#ifndef MUONDAQ_IMUONTELL1CONFIG_H 
#define MUONDAQ_IMUONTELL1CONFIG_H 1

// Include files
// from STL
#include <string>
#include <ostream>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IMuonTell1Config ( "IMuonTell1Config", 1, 0 );

/** @class IMuonTell1Config IMuonTell1Config.h MuonDAQ/IMuonTell1Config.h
 *  
 *
 *  @author 
 *  @date   2008-02-29
 */
class IMuonTell1Config : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IMuonTell1Config; }

  virtual StatusCode configTell1(int Tell1)=0;
  
protected:

private:

};


#endif // MUONDAQ_IMUONTELL1CONFIG_H
