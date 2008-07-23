// $Id: STErrorDecoding.cpp,v 1.6 2008-07-23 14:50:44 jluisier Exp $
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
  : STDecodingBaseAlg ( name , pSvcLocator ){
 
  declareProperty("PrintErrorInfo", m_PrintErrorInfo = false);  
  declareProperty("OutputLocation",m_outputLocation=STTELL1BoardErrorBankLocation::TTErrorTELL1 );
  declareProperty("BankType", m_bankTypeString = "TTError"); 
}
//=============================================================================
// Destructor
//=============================================================================
STErrorDecoding::~STErrorDecoding() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode STErrorDecoding::initialize() {

  StatusCode sc = STDecodingBaseAlg::initialize();
  if ( sc.isFailure() ) return sc; 

  debug() << "==> Initialize" << endmsg;
   
  // output location
  STDetSwitch::flip(detType(),m_outputLocation);
 
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode STErrorDecoding::execute() {

 debug() << "==> Execute " << endmsg;

 // get the raw event + odin info
 RawEvent* raw = get<RawEvent>(RawEventLocation::Default );

 // Pick up ITError bank 
 const std::vector<LHCb::RawBank*>& itf = raw->banks(LHCb::RawBank::BankType(bankType())); 
 
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

 debug() << "Starting to decode " << itf.size() << detType() <<"Error bank(s)" <<  endmsg;
  
 for( std::vector<LHCb::RawBank*>::const_iterator itB = itf.begin(); itB != itf.end(); ++itB ) {

   std::string errorBank = "sourceID "+
	boost::lexical_cast<std::string>((*itB)->sourceID());  
   ++counter(errorBank);

   if ((*itB)->magic() != RawBank::MagicPattern) {
     std::string pattern = "wrong magic pattern "+
	boost::lexical_cast<std::string>((*itB)->sourceID());  
     Warning(pattern, StatusCode::SUCCESS); 
     continue;
   }

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

   debug() << "Decoding bank number of type "<< detType() << "Error (TELL1 ID: " <<  (*itB)->sourceID() 
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

     const unsigned int  nOptional = errorInfo->nOptionalWords();

     // we must find the optional words + 2 more control words
     if (bankEnd - w < nOptional + 2 ){
        Warning("Ran out of words to read", StatusCode::SUCCESS);
        break;
      }

     const unsigned int *eInfo;
     eInfo = 0;

     if (errorInfo->hasErrorInfo()){
       //errorInfo->setOptionalErrorWords(p[w], p[w+1], p[w+2], p[w+3], p[w+4]);
       eInfo = &p[w];
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

     if (errorInfo->hasErrorInfo()){
       errorInfo->setOptionalErrorWords(eInfo[0], eInfo[1], eInfo[2], eInfo[3], eInfo[4]);
     } // has error information
      
   } //  loop ip [ppx's]
     
   if (w != bankEnd){
     error() << "read " << w << " words, expected: " << bankEnd << endmsg;
   }  

   if (m_PrintErrorInfo == true) info() << *myData << endmsg;

  }// end of loop over banks of a certain type


  
 return StatusCode::SUCCESS;
} 

