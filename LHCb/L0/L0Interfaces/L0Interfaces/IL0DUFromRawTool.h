// $Id: IL0DUFromRawTool.h,v 1.6 2010-01-20 16:06:46 odescham Exp $
#ifndef IL0DUFROMRAWTOOL_H 
#define IL0DUFROMRAWTOOL_H 1

// Include files
// from STL
#include <string>
#include <utility>
// from Gaudi
#include "GaudiKernel/IAlgTool.h"
// from L0Event
#include "Event/L0DUReport.h"
#include "Event/L0ProcessorData.h"

static const InterfaceID IID_IL0DUFromRawTool ( "IL0DUFromRawTool", 1, 3 );

/** @class IL0DUFromRawTool IL0DUFromRawTool.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2007-10-19
 */
class IL0DUFromRawTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IL0DUFromRawTool; }



  virtual bool decodeBank(int iBank=0 )=0;
  virtual unsigned int data(std::string name)=0;
  virtual unsigned int tck()=0;
  virtual unsigned int version()=0;
  virtual unsigned int firmware()=0;
  virtual const std::pair<unsigned int,unsigned int> bcid()=0;
  virtual unsigned int rsda()=0;
  virtual unsigned int muonCleaningPattern()=0;
  virtual unsigned int status()=0;
  virtual LHCb::L0DUReport report()=0;
  virtual LHCb::L0ProcessorDatas* L0ProcessorDatas()=0;
  virtual unsigned int size()=0;
  virtual unsigned long roStatus()=0;
  virtual void fillDataMap(bool fill = true)=0;
  virtual std::string dump()=0;

protected:

private:

};
#endif // IL0DUFROMRAWTOOL_H
