// $Id: OTCheckOTDigit.cpp,v 1.3 2004-03-30 12:38:12 jnardull Exp $
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

  MsgStream  msg( msgSvc(), name() );
  msg << MSG::DEBUG << "==> Execute" << endreq;
  
  int Count = 0;
  int NewCount = 0;
  
  //Retrieve OTDigit
  SmartDataPtr<OTDigits> digit( eventSvc(), m_OTDigitLoc );
  if ( !digit ) {
    MsgStream  msg( msgSvc(), name() );
    msg << MSG::ERROR
        << "Unable to retrieve OTDigit " << m_OTDigitLoc << endreq;
    return StatusCode::FAILURE;
  }
  m_otdigit = digit;
  //Let's count the First Digit
  OTDigits::iterator iDigit=m_otdigit->begin();
  for( Count = 0; iDigit != m_otdigit->end(); iDigit++, Count++);
  
  SmartDataPtr<OTDigits> newdigit( eventSvc(),m_Container2);
  if( 0 == newdigit){
    MsgStream  msg( msgSvc(), name() );
    msg << MSG::ERROR << "Unable to retrieve the new  OT Digit" 
        << endreq;
    return StatusCode::FAILURE;
  }
  m_newotdigit = newdigit; 
  // Let' count the retrieved OTDigit
  OTDigits::iterator iNewDigit=m_newotdigit->begin();
  for( NewCount = 0; iNewDigit != m_newotdigit->end(); iNewDigit++, NewCount++);

  // newly reconstructed OTDigits not necessarily in right order (?)
  std::stable_sort(m_newotdigit->begin(),m_newotdigit->end(),
                   OTDataFunctor::Less_by_Channel<const OTDigit*>()); 
 
  int Channel = 0;
  int NewChannel = 0;
  int i = 0;

  //Debug Count Mismatch
  if(Count != NewCount) {
    msg << MSG::ERROR << " Count mismatch Old =  " << Count  << " New = " 
        << NewCount << endreq;
    i ++;
  }
  //Loop over the Digits
  for (
	iDigit = m_otdigit->begin(),
	iNewDigit = m_newotdigit->begin(),
	Count=0;
	iDigit != m_otdigit->end() && iNewDigit != m_newotdigit->end();
	iDigit++, iNewDigit++, Count++ 
  ) {
    Channel = (*iDigit)->channel();
    vInt& vTimes = (*iDigit)->tdcTimes();
    int TimeSize = (vTimes.size());

    NewChannel = (*iNewDigit)->channel();
    vInt& vNewTimes = (*iNewDigit)->tdcTimes();
    int NewTimeSize = (vNewTimes.size());    

    if ( TimeSize > 3 ) {
      msg << MSG::WARNING << " Time Size too large: "  << TimeSize  << endreq ;
      TimeSize = 3 ;
    }
    //Debug
    if( NewTimeSize > 3){
      msg << MSG::WARNING << " Time Size NewChannel too large: "  
          << NewTimeSize  << endreq ;
      NewTimeSize = 3;
    }

    if ( Channel != NewChannel ) {
      msg << MSG::ERROR << " Channel Mismatch: " << Channel << " != "  
          << NewChannel  << endreq;
      i ++;
    }

    if ( TimeSize != NewTimeSize ) {
      msg << MSG::ERROR << " Time Size Mismatch: " 
          << TimeSize << " != "  
          << NewTimeSize  << endreq;
      i ++;
    }

    msg << MSG::DEBUG << " TIMESIZE = (" << TimeSize << "," << NewTimeSize
        << ")" << endreq ;
    
    for (register int c=0; c<TimeSize; c++) {
      int Time = vTimes[c] ;
      int NewTime = vNewTimes[c] ;
      if ( Time != NewTime ) {
        msg << MSG::ERROR << " Drift Time Mismatch: " 
          << Time << " != "  
          << NewTime << endreq;
	      i++ ;
      }
    }
  }
  // How many hits ?
  msg << MSG::DEBUG << " Number of hits : " << Count << endreq;
  // How many errors ?
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
