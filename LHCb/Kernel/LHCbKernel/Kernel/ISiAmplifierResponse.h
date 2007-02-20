// $Id: ISiAmplifierResponse.h,v 1.2 2007-02-20 16:42:02 cattanem Exp $
#ifndef _ISiAMPLIFIERRESPONSE_H
#define _ISiAMPLIFIERRESPONSE_H

#include "GaudiKernel/IAlgTool.h"
#include <string>

// Declaration of the interface ID ( interface id, major version, minor version) 
static const InterfaceID IID_ISiAmplifierResponse("ISiAmplifierResponse", 0 , 0); 

namespace SiAmpliferResponseType{

  static const std::string& signal = "signal";
  static const std::string& capCoupling = "capCoupling";
};

/** @class ISiAmpliferResponse ISiAmplifierResponse.h Kernel/ISiAmplifierResponse.h
 *
 *  Interface Class for estimating response of amplifier in Si Channel
 *
 *  @author M.Needham
 *  @date   6/11/2005
 */
class ISiAmplifierResponse : virtual public IAlgTool {

public: 

  typedef struct {
    double capacitance;
    unsigned int vfs;  
    std::string type;
  } Info;

  /** Static access to interface id */
  static const InterfaceID& interfaceID() { return IID_ISiAmplifierResponse; }

 /** calculate Beetle response
 * @param time in ns 
 * @return response 
 */
 virtual double response(const double time) const=0;

 /** The response is only valid for a certain capacitance, Vfs, ..
 * This method allows you to find out when the curve is valid
 *
 * @return validity info
 */
 virtual ISiAmplifierResponse::Info validity() const = 0; 

};

#endif // _ISiAMPLIFIERRESPONSE_H



