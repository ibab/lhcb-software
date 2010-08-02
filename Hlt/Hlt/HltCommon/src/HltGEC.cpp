// $Id: HltGEC.cpp,v 1.1 2010-08-02 15:55:59 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// local
// ============================================================================
#include "HltGEC.h"
// ============================================================================
/** @file 
 *  Implementation file for class Hlt::GEC
 *  @see Hlt::GEC 
 *  @author Jose Angel Hernando Morata
 *  @author Vanya Belyaev@nikhef.nl
 
 *  @date 2008-04-08
 */
// ============================================================================
/*  Standard constructor
 *  @param type the tol type (???)
 *  @param name the tool instance name 
 *  @param parent the tool parent 
 */
// ============================================================================
Hlt::GEC::GEC 
( const std::string& type   ,   // the tool type  
  const std::string& name   ,   // the tool instance name 
  const IInterface*  parent )   // the tool parent 
  : base_class  ( type , name , parent )
  , m_maxOTHits ( 10000 )
{
  declareProperty
    ( "MaxOTHits" , 
      m_maxOTHits , 
      "Maximalnumber of OT-hits" ) ;
}
// ============================================================================
// virtual & protected destructor 
// ============================================================================
Hlt::GEC::~GEC (){}
// ============================================================================
// standard initialization 
// ============================================================================
StatusCode Hlt::GEC::initialize() 
{
  StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ){ return sc ; }
  //
  m_rawBankDecoder = tool<IOTRawBankDecoder>("OTRawBankDecoder");
  //
  return sc;
}
// ============================================================================
// standard finalization 
// ============================================================================
StatusCode Hlt::GEC::finalize () 
{
  m_rawBankDecoder = 0 ;
  return GaudiTool::finalize ();
}
// ============================================================================
// accept 
// ============================================================================
bool Hlt::GEC::accept () const 
{ return m_rawBankDecoder->totalNumberOfHits() < m_maxOTHits ; }
// ============================================================================
// check
// ============================================================================
StatusCode Hlt::GEC::check  ()  
{ 
  return 
    accept() ? 
    StatusCode ( StatusCode::SUCCESS     , true ) : 
    StatusCode ( StatusCode::RECOVERABLE , true ) ;
}
// ============================================================================
// The Facttory
// ============================================================================
DECLARE_NAMESPACE_TOOL_FACTORY( Hlt, GEC )
// ============================================================================
// The END 
// ============================================================================


