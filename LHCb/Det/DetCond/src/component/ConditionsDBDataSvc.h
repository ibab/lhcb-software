//$Id: ConditionsDBDataSvc.h,v 1.5 2001-11-29 10:51:04 andreav Exp $
#ifndef DETCOND_CONDITIONSDBDATASVC_H
#define DETCOND_CONDITIONSDBDATASVC_H 1

/// Base classes
#include "ConditionDataSvc.h"

/// Forward and external declarations
template <class TYPE> class SvcFactory;

///---------------------------------------------------------------------------
/** @class ConditionsDBDataSvc ConditionsDBDataSvc.h Det/DetCond/ConditionsDBDataSvc.h

    A data service for DataObjects corresponding to condition data 
    from the CERN-IT ConditionsDB.

    @author Andrea Valassi 
    @date August 2001
*///--------------------------------------------------------------------------

class ConditionsDBDataSvc : virtual public ConditionDataSvc 
{
  
  /// Only factories can access protected constructors
  friend class SvcFactory<ConditionsDBDataSvc>;

 protected:
  
  /// Constructor
  ConditionsDBDataSvc( const std::string& name, ISvcLocator* svc );
  
  /// Destructor
  virtual ~ConditionsDBDataSvc();

 public:

  // Overloaded DataSvc methods

  /// Initialize the service
  virtual StatusCode initialize();

  /// Finalize the service
  virtual StatusCode finalize();
  
};

#endif    // DETCOND_CONDITIONSDBDATASVC_H


