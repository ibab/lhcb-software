// $Id: EvtReport.cpp,v 1.1 2005-11-08 00:05:30 robbep Exp $
// Overwrite EvtGen output messages

#include "GaudiKernel/MsgStream.h" 
#include "EvtGenBase/EvtReport.hh"

MsgStream * evtgenStream ;

//=============================================================================
// Reimplementation of print facility of EvtGen
//=============================================================================
std::ostream & report( Severity severity , const char * facility ) 
{
  if ( severity < WARNING ) {
    (*evtgenStream) << endreq ;
    if ( 0 != facility[0] ) { 
      std::cerr << facility << ":" ;
      (*evtgenStream) << MSG::ERROR << facility << " Error from EvtGen" 
                      << endreq ;
    }
    else (*evtgenStream) << MSG::ERROR << "Error from EvtGen" 
                         << endreq ;
    return ( std::cerr ) ;
  }
  
  if ( severity < INFO ) {
    if ( 0 != facility[0] ) (*evtgenStream) << MSG::INFO << facility << ":" ;
    else (*evtgenStream) << MSG::INFO ;
  } else {
    if ( 0 != facility[0] ) (*evtgenStream) << MSG::DEBUG << facility << ":" ;
    else (*evtgenStream) << MSG::DEBUG ;
  }
  return evtgenStream -> stream() ;
}
