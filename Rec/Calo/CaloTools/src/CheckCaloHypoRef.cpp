// Include files 

// from  LHCb
#include "CaloUtils/CaloAlgUtils.h"
#include "Event/CaloHypo.h"
// local
#include "CheckCaloHypoRef.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CheckCaloHypoRef
//
// 2012-05-14 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CheckCaloHypoRef )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CheckCaloHypoRef::CheckCaloHypoRef( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ){



  declareProperty( "CaloHypos", m_inputs);
  //
 using namespace LHCb::CaloAlgUtils;

 m_inputs.push_back(CaloHypoLocation("Photons"      , context()));
 m_inputs.push_back(CaloHypoLocation("Electrons"    , context()));
 m_inputs.push_back(CaloHypoLocation("MergedPi0s"   , context()));
}
//=============================================================================
// Destructor
//=============================================================================
CheckCaloHypoRef::~CheckCaloHypoRef() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CheckCaloHypoRef::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CheckCaloHypoRef::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;



 for (std::vector<std::string>::iterator iloc = m_inputs.begin(); m_inputs.end() != iloc; ++iloc) {
    std::string loc = *iloc;
    const LHCb::CaloHypos* hypos = getIfExists<LHCb::CaloHypos> (loc);
    if ( NULL != hypos ) {
      counter("#Hypos in " + loc) += hypos->size();
      int bLink=0;
      for (LHCb::CaloHypos::const_iterator h = hypos->begin(); hypos->end()!=h;++h){
        const SmartRefVector<LHCb::CaloCluster>& clusters=(*h)->clusters();
        for( SmartRefVector<LHCb::CaloCluster>::const_iterator c=clusters.begin();clusters.end()!=c;++c){
          const LHCb::CaloCluster* cluster = *c;
          if( cluster == NULL )bLink++;
          else counter("Cluster energy " +loc)+=cluster->e();
        }
      }
      counter("Broken SmarRef " +loc) += bLink;
      if(bLink != 0)Warning("CaloHypo -> CaloCluster* SmartReference is broken for "+loc,StatusCode::SUCCESS).ignore();
    }
 }
  return StatusCode::SUCCESS;
} 

//=============================================================================
