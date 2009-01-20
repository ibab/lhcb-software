// $Id: EvtReport.cpp,v 1.1.1.1 2009-01-20 13:27:50 wreece Exp $
// Overwrite EvtGen output messages

#include "Generators/StreamForGenerator.h" 
#include "EvtGenBase/EvtReport.hh"

std::ostringstream dummyStr ;

//=============================================================================
// Reimplementation of print facility of EvtGen
//=============================================================================
std::ostream & report( Severity severity , const char * facility ) 
{  
  dummyStr.str("") ;
  if ( severity < WARNING ) {
    if ( 0 != facility[0] ) { 
      (*StreamForGenerator::getStream()) << MSG::ERROR << facility << " Error from EvtGen" 
                      << endmsg ; 
    }
    else (*StreamForGenerator::getStream()) << MSG::ERROR << "Error from EvtGen" 
                         << endmsg ;
  } else if ( severity < INFO ) {
    if ( 0 != facility[0] ) {
      (*StreamForGenerator::getStream()) << MSG::INFO ; 
      if ( StreamForGenerator::getStream()->isActive() ) std::cout << facility << ":" ;
    } else (*StreamForGenerator::getStream()) << MSG::INFO ;
  } else {
    if ( 0 != facility[0] ) {
      (*StreamForGenerator::getStream()) << MSG::DEBUG ;
      if ( StreamForGenerator::getStream()->isActive() ) std::cout << facility << ":" ;
    } else ( *StreamForGenerator::getStream() ) << MSG::DEBUG ;
  }
  if ( StreamForGenerator::getStream()->isActive() ) return std::cout ;
  else return dummyStr ;
}
