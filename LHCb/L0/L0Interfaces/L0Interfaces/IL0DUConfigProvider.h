// $Id: IL0DUConfigProvider.h,v 1.1.1.1 2007-10-31 13:38:04 odescham Exp $
#ifndef IL0DUCONFIGPROVIDER_H 
#define IL0DUCONFIGPROVIDER_H 1

// Include files
// from STL
#include <string>
// from Gaudi
#include "GaudiKernel/IAlgTool.h"
//from Event
#include "Event/L0DUConfig.h"

static const InterfaceID IID_IL0DUConfigProvider ( "IL0DUConfigProvider", 1, 0 );

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
  virtual LHCb::L0DUConfig* config(long tck)=0;
  virtual LHCb::L0DUConfig::Container configs()=0;
  

protected:

private:



};
#endif // IL0DUCONFIGPROVIDER_H
