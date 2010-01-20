// $Id: IL0DUConfigProvider.h,v 1.2 2010-01-20 16:06:46 odescham Exp $
#ifndef IL0DUCONFIGPROVIDER_H 
#define IL0DUCONFIGPROVIDER_H 1

// Include files
// from STL
#include <string>
// from Gaudi
#include "GaudiKernel/IAlgTool.h"
//from Event
#include "Event/L0DUConfig.h"

static const InterfaceID IID_IL0DUConfigProvider ( "IL0DUConfigProvider", 2, 0 );

/** @class IL0DUConfigProvider IL0DUConfigProvider.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2007-09-19
 */
class IL0DUConfigProvider : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IL0DUConfigProvider; }

  virtual StatusCode initialize()=0; 
  virtual StatusCode finalize()=0; 
  virtual LHCb::L0DUConfig* config(long tck,std::string slot="T0")=0;
  virtual LHCb::L0DUConfig::Container* configs(std::string slot="T0")=0;
  

protected:

private:



};
#endif // IL0DUCONFIGPROVIDER_H
