// $Id: OTCheckOTDigit.cpp,v 1.1.1.1 2004-02-03 09:49:17 jnardull Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
// GaudiKernel
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/SmartRef.h"
// local
#include "OTDAQ/OTCheckOTDigit.h"

//-----------------------------------------------------------------------------
// Implementation file for class : OTCheckOTDigit
//
// 2004-01-30 : Jacopo Nardulli & Bart Hommels
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<OTCheckOTDigit>          s_factory ;
const        IAlgFactory& OTCheckOTDigitFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
OTCheckOTDigit::OTCheckOTDigit( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : Algorithm ( name , pSvcLocator )
{
  this->declareProperty( "OTDigitLocation", 
                         m_OTDigitLoc = OTDigitLocation::Default );
  this->declareProperty( "OutputLocation",
                         m_Container2 = "/Event/Rec/OT/checkDigits" );
}
//=============================================================================
// Destructor
//=============================================================================
OTCheckOTDigit::~OTCheckOTDigit() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode OTCheckOTDigit::initialize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Initialise" << endreq;

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode OTCheckOTDigit::execute() {

  //Retrieve OTDigit
  SmartDataPtr<OTDigits> digit( eventSvc(), m_OTDigitLoc );
  if ( !digit ) {
    MsgStream  msg( msgSvc(), name() );
    msg << MSG::ERROR
        << "Unable to retrieve OTDigit " << m_OTDigitLoc << endreq;
    return StatusCode::FAILURE;
  }
  m_otdigit = digit;
  
  SmartDataPtr<OTDigits> newdigit( eventSvc(),m_Container2);
  if( 0 == newdigit){
    MsgStream  msg( msgSvc(), name() );
    msg << MSG::ERROR << "Unable to retrieve the new  OT Digit" 
        << endreq;
    return StatusCode::FAILURE;
  }
  m_newotdigit = newdigit; 

  // newly reconstructed OTDigits not necessarily in right order (?)
  std::stable_sort(m_newotdigit->begin(),m_newotdigit->end(),
                   OTDataFunctor::Less_by_Channel<const OTDigit*>()); 
 
  int Channel = 0;
  int NewChannel = 0;
  int i = 0;
  //Loop over the Digits
  OTDigits::iterator iDigit =  m_otdigit->begin();
  OTDigits::iterator iNewDigit = m_newotdigit->begin();
  while( (iDigit != m_otdigit->end() && iNewDigit != m_newotdigit->end() ) ){
    Channel = (*iDigit)->channel();
    vInt& vTimes = (*iDigit)->tdcTimes();
    int Time = (*(vTimes.begin()));
    NewChannel = (*iNewDigit)->channel();
    vInt& vNewTimes = (*iNewDigit)->tdcTimes();
    int NewTime = (*(vNewTimes.begin()));
    if ((Channel != NewChannel) || (Time != NewTime)){
      i = i + 1;
    }
    iDigit++;
    iNewDigit++;
  }
  // How many errors ?
  MsgStream  msg( msgSvc(), name() );  
  msg << MSG::DEBUG << " Number of errors : " << i << endreq;
  
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode OTCheckOTDigit::finalize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Finalize" << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
