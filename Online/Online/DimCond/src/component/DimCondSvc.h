// $Id: DimCondSvc.h,v 1.1.1.1 2005-08-23 10:00:41 marcocle Exp $
#ifndef COMPONENT_DIMCONDSVC_H 
#define COMPONENT_DIMCONDSVC_H 1

// Include files
#include "GaudiKernel/Service.h"

class DimCondCommand;
template <class TYPE> class SvcFactory;

/** @class DimCondSvc DimCondSvc.h component/DimCondSvc.h
 *  
 *  Dummy service that instantiate a DimCondCommand.
 *
 *  @author Marco Clemencic
 *  @date   2005-06-21
 */
class DimCondSvc: public Service {
public: 
  /// Standard constructor
  DimCondSvc(const std::string &name, ISvcLocator *svcloc); 

  virtual ~DimCondSvc(); ///< Destructor
  
  virtual StatusCode initialize();
 	
  virtual StatusCode finalize();

protected:

private:
  
  /// DIM command instantiated.
  DimCondCommand     *m_cmd;

  /// Pointer to the UpdateManagerSvc
  IUpdateManagerSvc  *m_ums;
  
  /// Pointer to the CondDBAccessSvc
  ICondDBAccessSvc   *m_db;
  
};
#endif // COMPONENT_DIMCONDSVC_H
