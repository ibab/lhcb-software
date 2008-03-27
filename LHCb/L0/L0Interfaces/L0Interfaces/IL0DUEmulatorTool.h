// $Id: IL0DUEmulatorTool.h,v 1.2 2008-03-27 16:35:05 odescham Exp $
#ifndef IL0DUEMULATORTOOL_H 
#define IL0DUEMULATORTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Event/L0ProcessorData.h"
#include "Event/L0DUConfig.h"
#include "Event/L0DUReport.h"

static const InterfaceID IID_IL0DUEmulatorTool ( "IL0DUEmulatorTool", 1, 1 );

/** @class IL0DUEmulatorTool IL0DUEmulatorTool.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2007-10-10
 */
class IL0DUEmulatorTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IL0DUEmulatorTool; }



  virtual StatusCode process(LHCb::L0DUConfig* config,  LHCb::L0ProcessorDatas* datas)=0;
  virtual StatusCode process(LHCb::L0DUConfig* config,  std::vector<std::string> dataLocs)=0;
  virtual const LHCb::L0DUReport emulatedReport()=0;
  virtual const std::vector<unsigned int> bank(unsigned int version)=0;  

protected:

private:


};
#endif // IL0DUEMULATORTOOL_H
