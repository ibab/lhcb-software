// $Id: STErrorDecoding.cpp,v 1.1 2008-06-25 06:55:15 mneedham Exp $
// Include files 


// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "STErrorDecoding.h"

// Event
#include "Event/RawEvent.h"
#include "Event/STTELL1BoardErrorBank.h"
#include "Event/STTELL1Error.h"
#include "Event/RawBank.h"
#include "Event/ODIN.h"

// Kernel
#include "Kernel/STDetSwitch.h"
#include "Kernel/STRawBankMap.h"
#include "Kernel/STDAQDefinitions.h"

#include <bitset>

// boost
#include "boost/lexical_cast.hpp"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : STErrorDecoding
//
// 2007-11-29: Mathias Knecht
// Update 2008 M Needham
//-----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( STErrorDecoding );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
STErrorDecoding::STErrorDecoding( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ){
 
  declareProperty("PrintErrorInfo", m_PrintErrorInfo = false);  
  declareProperty("OutputLocation",m_outputLocation=STTELL1BoardErrorBankLocation::TTErrorTELL1 );
  declareProperty("ErrorLocation",m_ErrorLocation=STTELL1ErrorLocation::TTError );
  declareProperty("DetType", m_detType = "TT");   
  declareProperty("BankType", m_typeString = "TTError"); 
}
//=============================================================================
// Destructor
//=============================================================================
STErrorDecoding::~STErrorDecoding() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode STErrorDecoding::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize();
  if ( sc.isFailure() ) return sc; 

  debug() << "==> Initialize" << endmsg;
   
  // output location
  STDetSwitch::flip(m_detType,m_outputLocation);
  STDetSwitch::flip(m_detType,m_ErrorLocation);
  STDetSwitch::flip(m_detType, m_typeString); 
  m_bankType = STRawBankMap::stringToType(m_typeString);
  if (m_bankType ==  LHCb::RawBank::Velo){
   fatal() << "Wrong detector type: only IT or TT !"<< endmsg;
   return StatusCode::FAILURE; 
  } 
 
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode STErrorDecoding::execute() {

 debug() << "==> Execute " << endmsg;

 // get the raw event + odin info
 RawEvent* raw = get<RawEvent>(RawEventLocation::Default );
 ODIN* odin = get<ODIN>(ODINLocation::Default);  
 const int L0EvtID = odin->eventNumber(); 

 // Pick up ITError bank 
 const std::vector<LHCb::RawBank*>& itf = raw->banks(LHCb::RawBank::BankType(m_bankType)); 
 
 if (itf.size() == 0u){
   debug() <<"event has no error banks " << endmsg;
   return StatusCode::SUCCESS;
 }
 else {
    ++counter("events with error banks");
    counter("total # error banks") += itf.size(); 
 }

 STTELL1BoardErrorBanks* outputErrors = new STTELL1BoardErrorBanks();
 
 put(outputErrors, m_outputLocation);

 debug() << "Starting to decode " << itf.size() << m_detType <<"Error bank(s)" <<  endmsg;
  
 for( std::vector<LHCb::RawBank*>::const_iterator itB = itf.begin(); itB != itf.end(); ++itB ) {


   std::string errorBank = "sourceID "+
	boost::lexical_cast<std::string>((*itB)->sourceID());  
   ++counter(errorBank);

   const unsigned int* p = (*itB)->data();
   unsigned int w=0;
   const unsigned int bankEnd = (*itB)->size()/sizeof(unsigned int);

   // bank has to be at least 28 words 
   if (bankEnd < STDAQ::minErrorBankWords){
     Warning("Error bank too short --> not decoded", StatusCode::SUCCESS);
     continue;
   }

   // and less than 52 words
   if (bankEnd > STDAQ::maxErrorBankWords){
     Warning("Error bank too long --> not decoded", StatusCode::SUCCESS);
     continue;
   }

   debug() << "Decoding bank number of type "<< m_detType << "Error (TELL1 ID: " <<  (*itB)->sourceID() 
           << ", Size: " <<  (*itB)->size() << " bytes)" << endmsg;

       
   // make an empty tell1 data object
   STTELL1BoardErrorBank* myData = new STTELL1BoardErrorBank();
   outputErrors->insert(myData, (*itB)->sourceID()); 
   
   for (unsigned int ipp = 0; ipp < STDAQ::npp && w != bankEnd ; ++ipp ){

     debug() << "#######  Parsing now data from PP " << ipp << " #####################"<< endmsg; 

     // we must find 5 words
     if (bankEnd - w < 5 ){
       Warning("Ran out of words to read", StatusCode::SUCCESS);
       break;
     }
 
     STTELL1Error* errorInfo = new STTELL1Error(p[w], p[w+1], p[w+2], p[w+3], p[w+4]);
     myData->addToErrorInfo(errorInfo); 
     w +=5; // read 5 first words

     // check L0 id against Odin
     if( errorInfo->L0EvtID()  != L0EvtID ){
       if (m_PrintErrorInfo == 1) error() <<  "L0-Event ID: " << errorInfo->L0EvtID() << " expected: " << L0EvtID << endmsg;	     
     }

     const unsigned int  nOptional = errorInfo->nOptionalWords();

     // we must find the optional words + 2 more control words
     if (bankEnd - w < nOptional + 2 ){
        Warning("Ran out of words to read", StatusCode::SUCCESS);
        break;
      }

     if (errorInfo->hasErrorInfo()){
       errorInfo->setOptionalErrorWords(p[w], p[w+1], p[w+2], p[w+3], p[w+4]);
       w+= 5;
     } // has error information

     errorInfo->setWord10(p[w]); ++w;
     errorInfo->setWord11(p[w]); ++w;
 
     // then some more optional stuff
     if (errorInfo->hasNZS() == true){
       errorInfo->setWord12(p[w]); ++w;
     }  // nsz info...

     // then some more optional stuff
     if (errorInfo->hasPed() == true){
       errorInfo->setWord13(p[w]); ++w;
     }
      
     if (m_PrintErrorInfo == true) info() << *errorInfo << endmsg;

   } //  loop ip [ppx's]
     
   if (w != bankEnd){
     error() << "read " << w << " words, expected: " << bankEnd << endmsg;
   }  
  }// end of loop over banks of a certain type
  
 return StatusCode::SUCCESS;
} 

