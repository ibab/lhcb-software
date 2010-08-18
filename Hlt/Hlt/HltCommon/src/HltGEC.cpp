// $Id: HltGEC.cpp,v 1.2 2010-08-18 22:06:31 gligorov Exp $
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
{
  declareProperty
    ( "MaxOTHits" , 
      m_maxOTHits , 
      "Maximalnumber of OT-hits" ) ;
  declareProperty
    ( "MaxITHits" , 
      m_maxITHits , 
      "Maximalnumber of IT-hits" ) ;
  declareProperty
    ( "MaxVeloHits" , 
      m_maxVeloHits , 
      "Maximalnumber of Velo-hits" ) ;
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
  const LHCb::STLiteCluster::STLiteClusters* clusterContIT 
    = get<LHCb::STLiteCluster::STLiteClusters>(LHCb::STLiteClusterLocation::ITClusters);
  const LHCb::VeloLiteCluster::VeloLiteClusters* clusterContVelo
    = get<LHCb::VeloLiteCluster::VeloLiteClusters>(LHCb::VeloLiteClusterLocation::Default);
  return ((m_rawBankDecoder->totalNumberOfHits() < m_maxOTHits) && 
          (clusterContIT->size() < m_maxITHits) &&
          (clusterContVelo->size() < m_maxVeloHits)
         ); 
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


