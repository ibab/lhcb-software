//$Id: ConditionDataSvc.h,v 1.2 2001-11-23 17:18:13 andreav Exp $
#ifndef DETCOND_CONDITIONDATASVC_H
#define DETCOND_CONDITIONDATASVC_H 1

// Base classes
#include "GaudiKernel/DataSvc.h"
#include "DetCond/IConditionDataSvc.h"

// Forward and external declarations
class ITime;
class StatusCode;

///---------------------------------------------------------------------------
/** @class ConditionDataSvc ConditionDataSvc.h Det/DetCond/ConditionDataSvc.h

    A DataSvc specialised in ConditionData.

    @author Andrea Valassi 
    @date August 2001
*///--------------------------------------------------------------------------

class ConditionDataSvc : public DataSvc,
			 virtual public IConditionDataSvc
{

  /// Only factories can access protected constructors
  friend class SvcFactory<ConditionDataSvc>;

 protected:
  
  /// Constructor
  ConditionDataSvc( const std::string& name, ISvcLocator* svc );
  
  /// Destructor
  virtual ~ConditionDataSvc();

 public:
  
  // Overloaded DataSvc methods

  /// Initialize the service
  virtual StatusCode initialize();
  
  /// Finalize the service
  virtual StatusCode finalize();
  
  /// Update object
  virtual StatusCode updateObject( DataObject* toUpdate );

 public:
  
  // Reimplemented from IInterface

  /// Query the interface of the service
  virtual StatusCode queryInterface( const IID& riid, 
				     void** ppvInterface );  

 public:

  // Implementation of IConditionDataSvc interface

  /// Set the new event time  
  virtual void setEventTime            ( const ITime& time );

  /// Get the event time  
  virtual const ITime* eventTime       ( );

 private:

  /// Current event time (should be set by the event loop manager)
  ITime*             m_eventTime;       // Current event time

};

#endif    // DETCOND_CONDITIONDATASVC_H


