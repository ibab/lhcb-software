// $Id: ISTAmplifierResponse.h,v 1.1.1.1 2005-12-19 15:43:16 mneedham Exp $
#ifndef _ISTAMPLIFIERRESPONSE_H
#define _ISTAMPLIFIERRESPONSE_H

#include "GaudiKernel/IAlgTool.h"
#include <string>

/** @class ISTAmpliferResponse ISTAmplifierResponse.h ITAlgorithms/ISTAmplifierResponse.h
 *
 *  Interface Class for estimating response of amplifer in ST Channel
 *
 *  @author M.Needham
 *  @date   6/11/2005
 */

// Declaration of the interface ID ( interface id, major version, minor version) 
static const InterfaceID IID_ISTAmplifierResponse("ISTAmplifierResponse", 0 , 0); 

namespace STAmpliferResponseType{

  static const std::string& signal = "signal";
  static const std::string& capCoupling = "capCoupling";
};

class ISTAmplifierResponse : virtual public IAlgTool {

public: 

  typedef struct {
    double capacitance;
    unsigned int vfs;  
    std::string type;
  } Info;

  // Static access to interface id
  static const InterfaceID& interfaceID() { return IID_ISTAmplifierResponse; }
   
  /// calc noise
 virtual double response(const double time) const=0;

 virtual ISTAmplifierResponse::Info validity() const = 0; 

};

#endif // _ISTAMPLIFIERRESPONSE_H



