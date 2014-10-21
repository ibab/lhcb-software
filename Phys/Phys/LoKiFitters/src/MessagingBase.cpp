
// STD
#include <algorithm>

// local
#include "MessagingBase.h"

// GaudiKernel
#include "GaudiKernel/IAlgorithm.h"
#include "GaudiKernel/SmartIF.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MessagingBase
//
// 2014-10-21 : Chris Jones
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
LoKi::MessagingBase::MessagingBase( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : GaudiTool ( type , name , parent )
{ 
  declareProperty ( "PrintMyAlg"        ,
                    m_printMyAlg = true ,
                    "Print the name of ``associated'' algorithm" ) ;
  declareProperty ( "MaxPrints"         ,
                    m_prints = 2        ,
                    "Maximum number of prints" ) ;
  if ( parent )
  {
    SmartIF<IAlgorithm> alg ( const_cast<IInterface*>(parent) ) ;
    if ( alg.isValid() ) { m_myAlg = alg->name() ; }
  }
}

//=============================================================================
// Destructor
//=============================================================================
LoKi::MessagingBase::~MessagingBase() {}

//=============================================================================
// Initialize
//=============================================================================
StatusCode LoKi::MessagingBase::initialize()
{
  /// initialize the base
  const StatusCode sc = GaudiTool::initialize () ;
  if ( sc.isFailure() ) { return sc ; }           

  if ( msgLevel ( MSG::DEBUG ) &&  0 == m_prints )
  {
    m_prints = 10 ;
    warning () << "Redefine 'MaxPrints' property to " << m_prints << endmsg ;
  }
  
  if ( parent() )
  {
    SmartIF<IAlgorithm> alg ( const_cast<IInterface*> ( parent() ) ) ;
    if ( alg.isValid() ) { m_myAlg = alg->name() ; }
  }

  return sc;
}

// ========================================================================
// Warning message
// ========================================================================
StatusCode
LoKi::MessagingBase::_Warning( const std::string& msg,
                               const StatusCode&  code,
                               const unsigned int prints ) const
{
  code.setChecked () ;
  //
  if   ( errorsPrint() && m_printMyAlg  ) { getMyAlg      () ; }
  else                                    { m_myAlg.clear () ; }
  //
  if       ( m_printMyAlg && errorsPrint() )
  { return Warning ( msg + m_myAlg , code , std::min(prints,m_prints) ) ; }
  else if  ( errorsPrint() )
  { return Warning ( msg           , code , std::min(prints,m_prints) ) ; }
  //
  if ( msgLevel ( MSG::DEBUG ) )
  { warning ()
      << "'"       << msg  << "' " << m_myAlg
      << " Code =" << code << endmsg ; }
  //
  return code ;
}

// ========================================================================
// Error message
// ========================================================================
StatusCode
LoKi::MessagingBase::_Error( const std::string& msg,
                             const StatusCode&  code,
                             const unsigned int prints ) const
{
  code.setChecked () ;
  //
  if   ( errorsPrint() && m_printMyAlg  ) { getMyAlg      () ; }
  else                                    { m_myAlg.clear () ; }
  //
  if       ( m_printMyAlg && errorsPrint() )
  { return Error   ( msg + m_myAlg , code , std::min(prints,m_prints) ) ; }
  else if  ( errorsPrint() )
  { return Error   ( msg           , code , std::min(prints,m_prints) ) ; }
  //
  if ( msgLevel ( MSG::DEBUG ) )
  { error ()
      << "'"       << msg  << "' " << m_myAlg
      << " Code =" << code << endmsg ; }
  //
  return code ;
}

// ============================================================================
// get the correct algorithm context
// ============================================================================
bool LoKi::MessagingBase::getMyAlg () const
{
  m_myAlg = "" ;
  //
  const IAlgContextSvc* asvc =  contextSvc () ;
  if ( NULL == asvc    ) { return false ; }
  //
  const IAlgorithm* current = asvc->currentAlg() ;
  if ( NULL == current ) { return false ; }
  //
  m_myAlg = " [" + current->name() + "]" ;
  //
  return true ;
}

//=============================================================================
