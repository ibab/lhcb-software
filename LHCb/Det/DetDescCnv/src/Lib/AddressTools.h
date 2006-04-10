// $Id: AddressTools.h,v 1.2 2006-04-10 16:26:46 marcocle Exp $
#ifndef STREAMS_ADDRESSTOOLS_H 
#define STREAMS_ADDRESSTOOLS_H 1

// Include files
#include <string>

/** @class AddressTools AddressTools.h streams/AddressTools.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2005-06-13
 */

static const std::string environmentKey("$");
static const std::string delims(" \t,.;/#");
static const std::string brackets("{}");
class AddressTools {
public: 

  static bool expandAddress(std::string& address);
  static bool hasEnvironmentVariable(  const std::string& address) 
  {
    return address.find(environmentKey) != std::string::npos;
    
  }
  
private:

  static std::string extractVariable(const std::string& address);
  static std::string cleanEV(const std::string& environmentVariable);
  static bool resolvedEV(std::string& environmentVariable);
  
};
#endif // STREAMS_ADDRESSTOOLS_H
