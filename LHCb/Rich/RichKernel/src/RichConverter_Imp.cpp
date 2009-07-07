
//-----------------------------------------------------------------------------
/** @file RichConverter_Imp.cpp
 *
 *  Implementation file for class : Rich::Converter_Imp
 *
 *  CVS Log :-
 *  $Id: RichConverter_Imp.cpp,v 1.1 2009-07-07 16:24:53 jonrob Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2009-07-07
 */
//-----------------------------------------------------------------------------

// gaudi
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IChronoStatSvc.h"

// local
#include "RichKernel/RichConverter_Imp.h"

// print and count the error
StatusCode Rich::Converter_Imp::Error( const std::string& Message ,
                                       const StatusCode & Status ) const
{
  return this->Print( Message , MSG::ERROR  , Status  ) ;
}

// print and count the warning
StatusCode Rich::Converter_Imp::Warning( const std::string& Message ,
                                         const StatusCode & Status ) const
{
  return this->Print( Message , MSG::WARNING , Status ) ;
}

// print the message
StatusCode Rich::Converter_Imp::Print( const std::string& Message ,
                                       const MSG::Level & level   ,
                                       const StatusCode & Status ) const
{
  this->msgStream(level) << Message << endreq ;
  return Status;
}

IToolSvc* Rich::Converter_Imp::toolSvc() const
{
  if ( !m_toolSvc )
  {
    m_toolSvc = this -> svc<IToolSvc>( "ToolSvc" );
  }
  return m_toolSvc;
}

IDataProviderSvc* Rich::Converter_Imp::detSvc() const
{
  if ( !m_detSvc )
  {
    m_detSvc = this -> svc<IDataProviderSvc>( "DetectorDataSvc" );
  }
  return m_detSvc;
}

IChronoStatSvc* Rich::Converter_Imp::chronoSvc() const
{
  if ( !m_chronoSvc )
  {
    m_chronoSvc = this -> svc<IChronoStatSvc>( "ChronoStatSvc" );
  }
  return m_chronoSvc;
}

// initialize
StatusCode Rich::Converter_Imp::initialize()
{
  const StatusCode sc = ::Converter::initialize();
  if ( sc.isFailure() )
  { return Error( "Could not initialize base class Converter", sc); }

  return sc;
}

StatusCode Rich::Converter_Imp::finalize()
{
  // release used services
  if ( 0 != m_detSvc    ) { m_detSvc    ->release() ; m_detSvc    = 0 ; }
  if ( 0 != m_toolSvc   ) { m_toolSvc   ->release() ; m_toolSvc   = 0 ; }
  if ( 0 != m_chronoSvc ) { m_chronoSvc ->release() ; m_chronoSvc = 0 ; }
  // try to finalize the base class
  return ::Converter::finalize();
}
