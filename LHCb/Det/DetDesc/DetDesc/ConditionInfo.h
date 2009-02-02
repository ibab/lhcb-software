//$Id: ConditionInfo.h,v 1.2 2009-02-02 12:20:22 marcocle Exp $
#ifndef DETDESC_CONDITIONINFO_H
#define DETDESC_CONDITIONINFO_H 1

#include <string>

// Base class
#include "DetDesc/Services.h"
#include "DetDesc/IConditionInfo.h"

// Forward declarations
class IDataProviderSvc;

/**
 *  @class ConditionInfo ConditionInfo.h Det/DetDesc/ConditionInfo.h
 *
 *  The simplest implementation of the IConditionInfo abstract interface.
 *
 *  @author Andrea Valassi 
 *  @author Sebastien Ponce
 *
 */

class ConditionInfo : virtual public IConditionInfo
{

 public: 
  
  /// Constructor
  ConditionInfo( IDetectorElement* de,
		 const std::string& condition );

  /// Destructor 
  virtual ~ConditionInfo();
  
 private:
  
  /// Get a pointer to the data service responsible for condition data
  IDataProviderSvc* dataSvc() const;

 public:

  // Implementation of the IConditionInfo interface

  /// Get a pointer to the detector element to which the ConditionInfo belongs
  virtual IDetectorElement* detElem() const;

  /// Get the name of the associated condition
  virtual const std::string& conditionName() const;

  /// Get a pointer to the associated condition
  virtual Condition* condition();

  // Implementation of the IInterface interface

  /// query interface
    StatusCode queryInterface( const InterfaceID& ID , void** ppI ) ;
  /// add reference
  unsigned long addRef();
  /// release 
  unsigned long release();


 private:

  /// Pointer to the detector element to which the ConditionInfo belongs
  IDetectorElement* m_detElem;

  /// Name of the associated condition
  std::string m_conditionName;

  /// Pointer to the associated condition (loaded on demand only)
  Condition* m_condition;

  /// reference count
  unsigned long m_count;

  /// reference to dataSvc
  DetDesc::Services* m_services;

};

#endif // DETDESC_CONDITIONINFO_H
