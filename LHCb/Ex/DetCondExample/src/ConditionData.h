//$Id: ConditionData.h,v 1.1 2001-11-26 19:18:10 andreav Exp $
#ifndef DETCONDEXAMPLE_CONDITIONDATA_H
#define DETCONDEXAMPLE_CONDITIONDATA_H 1

#include <string>

// Base classes
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IValidity.h"

// Modified classes
#include "GaudiKernel/MsgStream.h"

// Unique ID of the class
static const CLID CLID_ConditionData = 197000;

// Forward declarations
class ITime;
class StatusCode;

///---------------------------------------------------------------------------
/** @class ConditionData ConditionData.h Det/DetCond/ConditionData.h

    A simple DataObject implementing the IValidity interface.

    @author Andrea Valassi 
    @date February 2001
*///--------------------------------------------------------------------------

class ConditionData : public DataObject, 
		      virtual public IValidity

{

 public: 
  
  /// Default constructor
  ConditionData( );

  /// Constructor 
  ConditionData( const ITime&       validSince      , 
		 const ITime&       validTill       );
  
  /// Copy constructor 
  ConditionData( ConditionData& icdata );
  
  /// Overloaded copy operator
  ConditionData& operator= ( ConditionData& icdata );

  /// Destructor 
  virtual ~ConditionData();
  
 public:

  // Re-implemented from DataObject

  /// Class ID of this instance
  inline virtual const CLID& clID   () const { return classID(); } 

  /// Class ID of this class
  inline static  const CLID& classID()       { return CLID_ConditionData; };

 public:

  /// Printout to a message stream
  virtual MsgStream& printOut( MsgStream& os ) const;

 public:

  // Implementation of IValidity

  /// DUMMY
  bool          isValid          (              )  ;

  /// Check if the condition is valid at the specified time
  bool          isValid          ( const ITime& )  ;

  /// Get start of validity
  const ITime&  validSince       (              )  ;

  /// Get end of validity
  const ITime&  validTill        (              )  ;

  /// Set validity range
  void          setValidity      ( const ITime& , 
				   const ITime& )  ;  
  /// Set start of validity
  void          setValiditySince ( const ITime& )  ;  

  /// Set end of validity
  void          setValidityTill  ( const ITime& )  ;   
  
  /// DUMMY
  StatusCode    updateValidity   (              )  ;
 
 private:

  // IValidity data

  /// Start of validity
  ITime*                       m_cd_validSince;

  /// End of validity
  ITime*                       m_cd_validTill;
  
};

/// Overloaded MsgStream printout
MsgStream& operator<<( MsgStream& os , const ConditionData* pco );

#endif // DETCONDEXAMPLE_CONDITIONDATA_H
