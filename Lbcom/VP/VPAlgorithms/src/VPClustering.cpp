// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

// Kernel/LHCbKernel
#include "Kernel/VPChannelID.h"

// Local
#include "VPClustering.h"

using namespace LHCb;

//------------------------------------------------------------
// Implementation file for class : VPClustering
//
//------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY(VPClustering);

//=============================================================================
/// Constructor
//=============================================================================
VPClustering::VPClustering(const std::string& name, 
                               ISvcLocator* pSvcLocator) : 
    GaudiAlgorithm(name, pSvcLocator),
    m_det(NULL) {

}

//=============================================================================
/// Destructor
//=============================================================================
VPClustering::~VPClustering() {}

//=============================================================================
/// Initialisation
//=============================================================================
StatusCode VPClustering::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;
  m_det = getDet<DeVP>(DeVPLocation::Default);
  return StatusCode::SUCCESS;

}

//=============================================================================
///  Execution
//=============================================================================
StatusCode VPClustering::execute() {

  return StatusCode::SUCCESS;

}

