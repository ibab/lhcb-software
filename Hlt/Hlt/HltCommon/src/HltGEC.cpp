// $Id: HltGEC.cpp,v 1.4 2010-08-25 07:41:02 graven Exp $
// ============================================================================
// Include files 
// ============================================================================
// local
// ============================================================================
#include "HltGEC.h"
#include "Event/STLiteCluster.h"
#include "Event/VeloLiteCluster.h"
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
  , m_maxITHits ( 3000 )
  , m_maxVeloHits ( 3000 )
  , m_minOTHits ( 50 )
  , m_minITHits ( 50 )
  , m_minVeloHits ( 50 )
  , m_isActivity ( false) 
{
  declareProperty ( "MaxOTHits" , m_maxOTHits , "Maximum number of OT-hits" ) ;
  declareProperty ( "MaxITHits" , m_maxITHits , "Maximum number of IT-hits" ) ;
  declareProperty ( "MaxVeloHits" , m_maxVeloHits , "Maximum number of Velo-hits" ) ;
  declareProperty ( "MinOTHits" , m_minOTHits , "Minimum number of OT-hits" ) ;
  declareProperty ( "MinITHits" , m_minITHits , "Minimum number of IT-hits" ) ;
  declareProperty ( "MinVeloHits" , m_minVeloHits , "Minimum number of Velo-hits" ) ;
  declareProperty ( "IsActivity" , m_isActivity , "is activity trigger or upper GEC " );
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
{ 
  if ( m_isActivity ){
    //always()<<"run activity trigger"<<endmsg;
    return (m_minOTHits<0   || (int)m_rawBankDecoder->totalNumberOfHits() > m_minOTHits)
      || (m_minITHits<0   || (int)get<LHCb::STLiteCluster::STLiteClusters>(LHCb::STLiteClusterLocation::ITClusters)->size() > m_minITHits)
      || (m_minVeloHits<0 || (int)get<LHCb::VeloLiteCluster::VeloLiteClusters>(LHCb::VeloLiteClusterLocation::Default)->size() > m_minVeloHits) 
      ;
    
  }
  else {
    return (m_maxOTHits<0   || (int)m_rawBankDecoder->totalNumberOfHits() < m_maxOTHits)
      && (m_maxITHits<0   || (int)get<LHCb::STLiteCluster::STLiteClusters>(LHCb::STLiteClusterLocation::ITClusters)->size() < m_maxITHits)
      && (m_maxVeloHits<0 || (int)get<LHCb::VeloLiteCluster::VeloLiteClusters>(LHCb::VeloLiteClusterLocation::Default)->size() < m_maxVeloHits) 
      ;
  }
  
}
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
