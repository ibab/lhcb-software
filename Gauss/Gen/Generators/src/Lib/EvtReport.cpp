// $Id: EvtReport.cpp,v 1.2 2006-02-17 13:23:35 robbep Exp $
// Overwrite EvtGen output messages

#include "GaudiKernel/MsgStream.h" 
#include "EvtGenBase/EvtReport.hh"

MsgStream * evtgenStream ;
std::ostringstream dummyStr ;

//=============================================================================
// Reimplementation of print facility of EvtGen
//=============================================================================
std::ostream & report( Severity severity , const char * facility ) 
{  
  dummyStr.str("") ;
  if ( severity < WARNING ) {
    if ( 0 != facility[0] ) { 
      (*evtgenStream) << MSG::ERROR << facility << " Error from EvtGen" 
                      << endmsg ; 
    }
    else (*evtgenStream) << MSG::ERROR << "Error from EvtGen" 
                         << endmsg ;
  } else if ( severity < INFO ) {
    if ( 0 != facility[0] ) {
      (*evtgenStream) << MSG::INFO ; 
      if ( evtgenStream -> isActive() ) std::cout << facility << ":" ;
    } else (*evtgenStream) << MSG::INFO ;
  } else {
    if ( 0 != facility[0] ) {
      (*evtgenStream) << MSG::DEBUG ;
      if ( evtgenStream -> isActive() ) std::cout << facility << ":" ;
    } else (*evtgenStream) << MSG::DEBUG ;
  }
  if ( evtgenStream -> isActive() ) return std::cout ;
  else return dummyStr ;
}
