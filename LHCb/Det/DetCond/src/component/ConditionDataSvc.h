//$Id: ConditionDataSvc.h,v 1.4 2001-11-27 18:26:20 andreav Exp $
#ifndef DETCOND_CONDITIONDATASVC_H
#define DETCOND_CONDITIONDATASVC_H 1

// Base classes
#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/IDetDataSvc.h"

// Forward and external declarations
class ITime;
class StatusCode;
class TimePoint;

///---------------------------------------------------------------------------
/** @class ConditionDataSvc ConditionDataSvc.h Det/DetCond/ConditionDataSvc.h

    A DataSvc specialised in condition data
    (i.e. DataObjects implementing IValidity).

    @author Andrea Valassi 
    @date August 2001
*///--------------------------------------------------------------------------

class ConditionDataSvc : public DataSvc,
			 virtual public IDetDataSvc
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

  // Implementation of IDetDataSvc interface

  /// Check if the event time has been set 
  virtual const bool validEventTime ( ) const ;

  /// Get the event time  
  virtual const ITime& eventTime    ( ) const ;

  /// Set the new event time  
  virtual void setEventTime         ( const ITime& time );

 private:

  /// Has the event time been defined?
  bool               m_eventTimeDefined;

  /// Current event time
  ITime*             m_eventTime;

};

#endif    // DETCOND_CONDITIONDATASVC_H


