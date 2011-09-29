#ifndef IFSRNAVIGATOR_H 
#define IFSRNAVIGATOR_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// forward declarations
namespace LHCb {
  class LumiFSR;
  class LumiIntegral;
}

static const InterfaceID IID_IFSRNavigator ( "IFSRNavigator", 1, 0 );

/** @class IFSRNavigator IFSRNavigator.h
 *   
 *
 *  @author Jaap Panman
 *  @date   2010-10-05
 */
class IFSRNavigator : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IFSRNavigator; }

  // return list of addresses found below the address
  virtual std::vector< std::string > navigate(std::string, std::string) = 0;

protected:
  virtual void explore(IRegistry* pObj, std::string tag, std::vector< std::string >& addresses) = 0;

private:

};
#endif // IFSRNAVIGATOR_H
