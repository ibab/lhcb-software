// $Id: OTCheckOTDigit.cpp,v 1.2 2004-03-25 15:53:37 jnardull Exp $
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
  int Time = 0;
  int NewTime = 0;
  int TimeBis = 0;
  int NewTimeBis = 0;
  int TimeTris = 0;
  int NewTimeTris = 0;
  //Loop over the Digits
  OTDigits::iterator iDigit =  m_otdigit->begin();
  OTDigits::iterator iNewDigit = m_newotdigit->begin();
  while( (iDigit != m_otdigit->end() && iNewDigit != m_newotdigit->end() ) ){
    Channel = (*iDigit)->channel();
    vInt& vTimes = (*iDigit)->tdcTimes();
    int TimeSize = (vTimes.size());
    if( TimeSize > 3){ TimeSize = 3;}
    // Multiple Hit
    if(TimeSize == 1){ Time = vTimes[0];}
    else if(TimeSize == 2){ 
      Time =  vTimes[0];
      TimeBis = vTimes[1];
    } else if(TimeSize == 3){
      Time =  vTimes[0];
      TimeBis = vTimes[1];
      TimeTris = vTimes[2];
    }
    NewChannel = (*iNewDigit)->channel();
    vInt& vNewTimes = (*iNewDigit)->tdcTimes();
    int NewTimeSize = (vNewTimes.size());
    if( NewTimeSize > 3){ NewTimeSize = 3;}
     // Multiple Hit
    if(NewTimeSize == 1){ NewTime = vNewTimes[0];}
    else if(NewTimeSize == 2){ 
      NewTime = vNewTimes[0];
      NewTimeBis = vNewTimes[1];
    } else if(NewTimeSize == 3){
      NewTime = vNewTimes[0];
      NewTimeBis = vNewTimes[1];
      NewTimeTris = vNewTimes[2];
    }
    //Debug
    if ((Channel != NewChannel) || (Time != NewTime) || (TimeBis != NewTimeBis)
        || (TimeTris != NewTimeTris)){
      i += 1;
    }
    iDigit++;
    iNewDigit++;

    //To restart the Loop
    Time = 0;
    TimeBis = 0;
    TimeTris = 0;
    NewTime = 0;
    NewTimeBis = 0;
    NewTimeTris = 0;
  }

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
