//$Id: ConditionInfo.h,v 1.1 2001-12-13 19:19:15 andreav Exp $
#ifndef DETDESC_CONDITIONINFO_H
#define DETDESC_CONDITIONINFO_H 1

#include <string>

// Base class
#include "DetDesc/IConditionInfo.h"

// Forward declarations
class IDataProviderSvc;

///---------------------------------------------------------------------------
/** @class ConditionInfo ConditionInfo.h Det/DetDesc/ConditionInfo.h

    The simplest implementation of the IConditionInfo abstract interface.

    @author Andrea Valassi 
    @date December 2001
*///--------------------------------------------------------------------------

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
  static IDataProviderSvc* dataSvc();

 public:

  // Implementation of the IConditionInfo interface

  /// Get a pointer to the detector element to which the ConditionInfo belongs
  virtual IDetectorElement* detElem() const;

  /// Get the name of the associated condition
  virtual const std::string conditionName() const;

  /// Get a pointer to the associated condition
  virtual Condition* condition();

 private:

  /// Pointer to the detector element to which the ConditionInfo belongs
  IDetectorElement* m_detElem;

  /// Name of the associated condition
  std::string m_conditionName;

  /// Pointer to the associated condition (loaded on demand only)
  Condition* m_condition;

};

#endif // DETDESC_CONDITIONINFO_H
