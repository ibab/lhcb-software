//$Id: ConditionData.cpp,v 1.3 2001-11-23 17:12:03 andreav Exp $
#include <string> 

#include "GaudiKernel/TimePoint.h" 
#include "GaudiKernel/StatusCode.h"

#include "DetCond/ConditionData.h"

//---------------------------------------------------------------------------

/// Default constructor (private)
ConditionData::ConditionData( )
  : DataObject        (                           )
  , m_cd_validSince   (                         0 ) 
  , m_cd_validTill    (                         0 ) 
{
  m_cd_validSince = new TimePoint( time_absolutepast   ) ;
  m_cd_validTill  = new TimePoint( time_absolutefuture ) ;
}; 

//---------------------------------------------------------------------------

/// Preferred constructor
ConditionData::ConditionData( const ITime&       validSince  , 
			      const ITime&       validTill   )
  : DataObject        (            )
  , m_cd_validSince   (          0 ) 
  , m_cd_validTill    (          0 ) 
{
  m_cd_validSince = new TimePoint( validSince ) ;
  m_cd_validTill  = new TimePoint( validTill  ) ;
}; 

//---------------------------------------------------------------------------

/// Copy constructor
ConditionData::ConditionData( ConditionData& icdata )
  : DataObject        (                     )
  , m_cd_validSince   (                   0 ) 
  , m_cd_validTill    (                   0 ) 
{
  m_cd_validSince = new TimePoint( icdata.validSince() ) ;
  m_cd_validTill  = new TimePoint( icdata.validTill()  ) ;
}; 

//---------------------------------------------------------------------------

/// Copy operator.
/// Overloaded from DataObject to point to new TimePoint objects.
ConditionData& ConditionData::operator= ( ConditionData& icdata )
{
  DataObject::operator= (icdata);
  this->m_cd_validSince  = new TimePoint( icdata.validSince() ) ;
  this->m_cd_validTill   = new TimePoint( icdata.validTill()  ) ;
  return *this;
}; 

//---------------------------------------------------------------------------

/// Destructor
ConditionData::~ConditionData() 
{
  delete m_cd_validSince;
  delete m_cd_validTill;
};

//---------------------------------------------------------------------------

/// DUMMY
bool ConditionData::isValid ( ) {
  return true;
};

//---------------------------------------------------------------------------

bool ConditionData::isValid ( const ITime& t ) {
  return validSince() < t &&  t < validTill();
};

//---------------------------------------------------------------------------

const ITime& ConditionData::validSince () { return *m_cd_validSince; };

//---------------------------------------------------------------------------

const ITime& ConditionData::validTill  () { return *m_cd_validTill;  };

//---------------------------------------------------------------------------

void ConditionData::setValidity ( const ITime& Since, const ITime& Till )
{
  setValiditySince( Since );
  setValidityTill ( Till  );
};

//---------------------------------------------------------------------------

void ConditionData::setValiditySince  ( const ITime& Since ) 
{
  delete m_cd_validSince;
  m_cd_validSince = new TimePoint( Since );
};

//---------------------------------------------------------------------------

void ConditionData::setValidityTill  ( const ITime& Till ) 
{
  delete m_cd_validTill;
  m_cd_validTill = new TimePoint( Till );
};

//---------------------------------------------------------------------------

/// DUMMY
StatusCode ConditionData::updateValidity () 
{
  return StatusCode::SUCCESS;
};

//---------------------------------------------------------------------------

/// Overloaded MsgStream printout
MsgStream& operator<<( MsgStream& os , const ConditionData* pco ) {
 
  return ( ( 0 == pco                              ) ? 
	   ( os << "ConditionData* points to NULL" ) : 
	   ( pco->printOut(os)                     ) );
  
};

//---------------------------------------------------------------------------

/// Printout to a message stream
MsgStream& ConditionData::printOut( MsgStream& os ) const {
  os << "This is a ConditionData object. Its address in memory is ";
  os.MsgStream::operator<<( this ); // Print address using base class operator
  os << ". ";
  return os;
};

//---------------------------------------------------------------------------

