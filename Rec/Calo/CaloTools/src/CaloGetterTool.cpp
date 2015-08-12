// ============================================================================
// Include files 
// ============================================================================
// from Gaudi
// ============================================================================
#include "GaudiKernel/IIncidentSvc.h" 
#include "GaudiKernel/Incident.h" 
#include "CaloUtils/CaloAlgUtils.h"
// ============================================================================
// local
// ============================================================================
#include "CaloGetterTool.h"
// ============================================================================
/** @file 
 *  Implementation file for class : CaloGetterTool
 *
 *  @date 2009-04-17 
 *  @author Olivier Deschamps
 */
// ============================================================================
// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( CaloGetterTool )
// ============================================================================
// Standard constructor, initializes variables
// ============================================================================
CaloGetterTool::CaloGetterTool 
( const std::string& type,
  const std::string& name,
  const IInterface* parent ): GaudiTool ( type, name , parent )
  ,m_detMask(0xF){
  declareInterface<ICaloGetterTool>(this);
  declareProperty ( "GetDigits"        , m_digiUpd = true  ) ;
  declareProperty ( "GetClusters"      , m_clusUpd = false ) ;
  declareProperty ( "GetHypos"         , m_hypoUpd = false ) ;
  //declareProperty ( "GetProviders"     , m_provUpd = false ) ;
  declareProperty ( "DigitLocations"   , m_digiLoc ) ;
  declareProperty ( "ClusterLocations" , m_clusLoc ) ;
  declareProperty ( "HypoLocations"    , m_hypoLoc ) ;
  declareProperty ( "DetectorMask"     , m_detMask=0xF);

  
  std::string flag = context();
  if( std::string::npos != name.find ("HLT") || 
      std::string::npos != name.find ("Hlt") )flag="Hlt";

  // digits
  if((m_detMask&8)!=0)m_digiLoc.push_back( LHCb::CaloAlgUtils::CaloDigitLocation( "Hcal" , flag ) ) ;
  if((m_detMask&4)!=0)m_digiLoc.push_back( LHCb::CaloAlgUtils::CaloDigitLocation( "Ecal" , flag ) ) ;
  if((m_detMask&2)!=0)m_digiLoc.push_back( LHCb::CaloAlgUtils::CaloDigitLocation( "Prs"  , flag ) ) ;
  if((m_detMask&1)!=0)m_digiLoc.push_back( LHCb::CaloAlgUtils::CaloDigitLocation( "Spd"  , flag ) ) ;
  
  if((m_detMask&8)!=0)m_clusLoc.push_back( LHCb::CaloAlgUtils::CaloClusterLocation( "Hcal" , flag ) ) ;
  if((m_detMask&4)!=0)m_clusLoc.push_back( LHCb::CaloAlgUtils::CaloClusterLocation( "Ecal" , flag ) ) ;
  m_clusLoc.push_back( LHCb::CaloAlgUtils::CaloSplitClusterLocation( flag ) ) ;
  
  m_hypoLoc.push_back( LHCb::CaloAlgUtils::CaloHypoLocation("Photons"      , flag ) );
  m_hypoLoc.push_back( LHCb::CaloAlgUtils::CaloHypoLocation("Electrons"    , flag ) );
  m_hypoLoc.push_back( LHCb::CaloAlgUtils::CaloHypoLocation("SplitPhotons" , flag ) );
  m_hypoLoc.push_back( LHCb::CaloAlgUtils::CaloHypoLocation("MergedPi0s"   , flag ) );

  // ==========================================================================
}
// ============================================================================
// Destructor
// ============================================================================
CaloGetterTool::~CaloGetterTool() {}
// ============================================================================
StatusCode CaloGetterTool::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << "Initialize Calo2Calo tool " << endmsg;


  if( m_detMask != 0xF) info() << "Incomplete calorimeter detector is requested - mask = " << m_detMask << endmsg;
  //
  if((m_detMask&8)!=0)m_provider["Hcal"] = tool<ICaloDataProvider>( "CaloDataProvider" , "HcalDataProvider" );
  if((m_detMask&4)!=0)m_provider["Ecal"] = tool<ICaloDataProvider>( "CaloDataProvider" , "EcalDataProvider" );
  if((m_detMask&2)!=0)m_provider["Prs"]  = tool<ICaloDataProvider>( "CaloDataProvider" , "PrsDataProvider"  );
  if((m_detMask&1)!=0)m_provider["Spd"]  = tool<ICaloDataProvider>( "CaloDataProvider" , "SpdDataProvider"  );
  //
  // subscribe to the incidents
  IIncidentSvc* inc = incSvc() ;
  if ( 0 != inc ) 
  {
    inc -> addListener  ( this , IncidentType::BeginEvent ) ;
    inc -> addListener  ( this , IncidentType::EndEvent   ) ;    
  }
  // prepare the known locations:
  //
  // digits
  if ( m_digiUpd ){
    m_digits.clear() ;
    for ( std::vector<std::string>::iterator iloc = m_digiLoc.begin() ;m_digiLoc.end() != iloc; ++iloc ){
      m_digits[ *iloc ] = 0 ; 
    }
  }
  // clusters
  if ( m_clusUpd ){
    m_clusters.clear() ;
    for ( std::vector<std::string>::iterator iloc = m_clusLoc.begin() ;m_clusLoc.end() != iloc; ++iloc){
      m_clusters[ *iloc ] = 0 ; 
    }
  }
  // hypos
  if ( m_hypoUpd ){
    m_hypos.clear() ;
    for ( std::vector<std::string>::iterator iloc = m_hypoLoc.begin() ;m_hypoLoc.end() != iloc; ++iloc){ 
      m_hypos[ *iloc ] = 0 ; 
    }
  }  
  //
  return StatusCode::SUCCESS;
}
// ============================================================================
StatusCode CaloGetterTool::finalize() 
{
  // un-subscribe to the incidents
  IIncidentSvc* inc = incSvc() ;
  if ( 0 != inc ) { inc -> removeListener  ( this ) ; }
  // clear data 
  nullify () ;
  m_provider . clear () ;
  // finalize the base 
  return GaudiTool::finalize () ;
}
// ============================================================================
namespace 
{
  // =========================================================================
  template <class TYPE>
  void __nullify ( std::map<std::string,TYPE*>& _map ) 
  {
    for ( typename std::map<std::string,TYPE*>::iterator ientry = _map.begin() ; 
          _map.end() != ientry ; ++ientry ) { ientry->second = 0 ; }
  }
  // ==========================================================================
}
// ============================================================================
void CaloGetterTool::nullify() 
{
  __nullify ( m_digits   ) ;
  __nullify ( m_clusters ) ;
  __nullify ( m_hypos    ) ;
}
// ============================================================================
// getters
// ============================================================================
LHCb::CaloDigits* CaloGetterTool::digits ( const std::string& loc )
{  
  std::map<std::string,LHCb::CaloDigits*>::iterator it = m_digits.find( loc );
  //
  if ( m_digits.end () == it ){
    Error ( "Illegal Attempt to retrive digits   from '" + loc + "'" ).ignore() ;
    return 0 ;
  }
  //
  if ( 0 != it->second ) { return it -> second ; } 
  //
  it -> second = getIfExists<LHCb::CaloDigits>( loc );  
  if ( NULL != it->second ){
    counter ("#Digits   @ " + loc ) += it->second->size() ;
    return it->second ;
  }
  //
  Error ("No Digits   at " + loc ).ignore() ;
  return 0 ;
}
// ============================================================================
LHCb::CaloClusters* CaloGetterTool::clusters ( const std::string& loc )
{
  std::map<std::string,LHCb::CaloClusters*>::iterator it = m_clusters.find( loc );
  //
  if ( m_clusters.end () == it ) 
  {
    Error ( "Illegal attempt to retrive clusters from '" + loc + "'" ).ignore() ;
    return 0 ;
  }
  //
  if ( 0 != it->second ) { return it->second ; } 
  //
  it -> second = getIfExists<LHCb::CaloClusters>( loc );  
  if ( NULL != it->second )
  {
    counter ("#Clusters @ " + loc  ) += it->second->size() ;
    return it->second ;
  }
  //
  Error ("No Clusters at " + loc ).ignore() ;
  return 0 ;
}
// ============================================================================
LHCb::CaloHypos*    CaloGetterTool::hypos    ( const std::string& loc )
{
  std::map<std::string,LHCb::CaloHypos*>::iterator it = m_hypos.find( loc );
  //
  if ( m_hypos.end () == it ) 
  {
    Error ( "Illegal attempt to retrive hypos    from '" + loc + "'" ).ignore() ;
    return 0 ;
  }
  //
  if ( 0 != it->second ) { return it->second ; } 
  //
  it -> second = getIfExists<LHCb::CaloHypos>( loc );  
  if ( NULL != it->second )
  {
    counter ("#Hypos    @ " + loc  ) += it->second->size() ;
    return it->second ;
  }
  //
  Error ("No Hypos    at " + loc ).ignore() ;
  return 0 ;
}
// ============================================================================
void CaloGetterTool::update()
{
  // digits
  if ( m_digiUpd )
  {
    for(std::vector<std::string>::iterator iloc = m_digiLoc.begin();m_digiLoc.end() != iloc; ++iloc){
      if ( exist<LHCb::CaloDigits>(*iloc) && 0 == m_digits[*iloc] ){ 
        LHCb::CaloDigits* digits = get<LHCb::CaloDigits>( *iloc );
        m_digits[ *iloc ] =  digits ;
        counter ("#Digits   @ " + (*iloc) ) += digits->size() ;
      }
    }
  }
  // clusters
  if( m_clusUpd )
  {
    for(std::vector<std::string>::iterator iloc = m_clusLoc.begin();m_clusLoc.end() != iloc; ++iloc)
    {
      LHCb::CaloClusters* clusters = getIfExists<LHCb::CaloClusters>( *iloc );  
      if ( NULL != clusters ) 
      {
        m_clusters[ *iloc ] = clusters ;
        counter ("#Clusters @ " + (*iloc) ) += clusters->size() ;        
      }
    }
  }
  // hypos
  if( m_hypoUpd )
  {
    for(std::vector<std::string>::iterator iloc = m_hypoLoc.begin();m_hypoLoc.end() != iloc; ++iloc)
    {
      LHCb::CaloHypos* hypos = getIfExists<LHCb::CaloHypos>( *iloc );  
      if ( NULL != hypos ) 
      {
        m_hypos[ *iloc ] = hypos ;
        counter ("#Hypos    @ " + (*iloc) ) += hypos->size() ;        
      }      
    }
  }  
  // provider
  //  if( m_provUpd)
  // {
  //  for(std::map<std::string,ICaloDataProvider*>::iterator ip = m_provider.begin();m_provider.end()!=ip;++ip)
  //  {
  //    const std::string& det = ip->first;
  //    ICaloDataProvider* provider = ip->second;
  //    m_prov[det] = provider->getBanks();
  //  }    
  //  }

}


 
  
