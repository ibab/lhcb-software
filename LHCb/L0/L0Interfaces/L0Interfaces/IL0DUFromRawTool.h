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


/** @class IL0DUFromRawTool IL0DUFromRawTool.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2007-10-19
 */
class IL0DUFromRawTool : public extend_interfaces<IAlgTool> {
public: 
  DeclareInterfaceID( IL0DUFromRawTool, 2, 0 );

  virtual bool decodeBank(int iBank=0 )=0;
  virtual unsigned int data(const std::string& name) const =0;
  virtual unsigned int tck() const =0;
  virtual unsigned int version() const =0;
  virtual unsigned int firmware() const =0;
  virtual const std::pair<unsigned int,unsigned int> bcid() const =0;
  virtual unsigned int rsda() const =0;
  virtual unsigned int muonCleaningPattern() const =0;
  virtual unsigned int status() const =0;
  virtual LHCb::L0DUReport report() const =0;
  virtual LHCb::L0ProcessorDatas* L0ProcessorDatas() =0;
  virtual unsigned int size() const =0;
  virtual unsigned long roStatus() const =0;
  virtual void fillDataMap(bool fill = true)=0;
  virtual std::string dump() const =0;
  virtual const std::map<std::string, std::pair<unsigned int,double> >& datas() const =0;
  virtual StatusCode  _setProperty(const std::string& p,const std::string& v)=0;

};
#endif // IL0DUFROMRAWTOOL_H
