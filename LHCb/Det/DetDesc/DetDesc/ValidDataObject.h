//$Id: ValidDataObject.h,v 1.1 2001-12-11 10:02:21 sponce Exp $
#ifndef DETDESC_VALIDDATAOBJECT_H
#define DETDESC_VALIDDATAOBJECT_H 1

#include <string>

// Base classes
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IValidity.h"

// Modified classes
#include "GaudiKernel/MsgStream.h"

// Unique ID of the class
#include "DetDesc/CLIDValidDataObject.h"

// Forward declarations
class ITime;
class StatusCode;

///---------------------------------------------------------------------------
/** @class ValidDataObject ValidDataObject.h Det/DetCond/ValidDataObject.h

    A simple DataObject implementing the IValidity interface.

    @author Andrea Valassi 
    @date February 2001
*///--------------------------------------------------------------------------

class ValidDataObject : public DataObject, 
			virtual public IValidity

{

 public: 
  
  /// Default constructor
  ValidDataObject();

  /// Constructor 
  ValidDataObject( const ITime& since, const ITime& till );
  
  /// Copy constructor 
  ValidDataObject( ValidDataObject& obj );
  
  /// Overloaded copy operator
  virtual ValidDataObject& operator= ( ValidDataObject& obj );

  /// Destructor 
  virtual ~ValidDataObject();
  
 public:

  // Re-implemented from DataObject

  /// Class ID of this instance
  inline virtual const CLID& clID() const { return classID(); } 

  /// Class ID of this class
  inline static  const CLID& classID() { return CLID_ValidDataObject; };

 public:

  // Overloaded from DataObject

  /// Printout to a message stream
  virtual MsgStream& printOut( MsgStream& os ) const;

 public:

  // Implementation of IValidity

  /// Check if the data object has a well defined validity range
  virtual bool isValid();

  /// Check if the data object is valid at the specified time
  virtual bool isValid( const ITime& t );

  /// Get start of validity
  virtual const ITime& validSince();

  /// Get end of validity
  virtual const ITime& validTill();

  /// Set validity range
  virtual void setValidity( const ITime& since, const ITime& till );  

  /// Set start of validity
  virtual void setValiditySince( const ITime& since );  

  /// Set end of validity
  virtual void setValidityTill( const ITime& till );   
  
  /// Update the validity range (foreseen for tree-like structures)
  virtual StatusCode updateValidity();
 
 private:

  // IValidity data

  /// Start of validity
  ITime* m_validSince;

  /// End of validity
  ITime* m_validTill;
  
};

/// Overloaded MsgStream printout
MsgStream& operator<<( MsgStream& os, const ValidDataObject* pObj );

#endif // DETDESC_VALIDDATAOBJECT_H
