// $Id: $
// Include files
// from Gaudi
#include "GaudiKernel/IDataStoreLeaves.h"
#include "GaudiKernel/ObjectContainerBase.h"
// local
#include "TESFingerPrint.h"
#include <fstream>

//-----------------------------------------------------------------------------
// Implementation file for class : TESFingerPrint
//
// 2011-05-11 : Illya Shapoval
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TESFingerPrint )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TESFingerPrint::TESFingerPrint( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ), m_leavesTool(0)
{
  declareProperty("HeuristicsLevel", m_heur_level = "Low",
                  "The level of TES heuristic analysis to be performed to obtain"
                  " its finger print.");
  declareProperty("OutputFileName", m_output_file_name = "tes_finger_print.dat",
                  "The name of the output file to store the TES finger print.");
}
//=============================================================================
// Destructor
//=============================================================================
TESFingerPrint::~TESFingerPrint() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode TESFingerPrint::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  sc = toolSvc()->retrieveTool("DataSvcFileEntriesTool", m_leavesTool);
  if (sc.isFailure()) return sc;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode TESFingerPrint::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // Get the objects in the same file as the root node
  try {
    const IDataStoreLeaves::LeavesList & leaves = m_leavesTool->leaves();
    m_objects.assign(leaves.begin(), leaves.end());
  } catch (GaudiException &e) {
    error() << e.message() << endmsg;
    return StatusCode::FAILURE;
  }
  // Collect TES statistics
  std::string entry;
  int cont_size;
  for (std::vector<DataObject*>::iterator o = m_objects.begin(); o != m_objects.end(); ++o) {
    entry = (*o)->registry()->identifier();

    if ( m_stat_map[entry] ) m_stat_map[entry]++;
    else m_stat_map[entry] = 1;

    if (m_heur_level == "Medium") {
      ObjectContainerBase* pCont = dynamic_cast<ObjectContainerBase*>(*o);
      if (pCont) {
        cont_size = pCont->numberOfObjects();
        if (m_cont_stat_map[entry][cont_size]) m_cont_stat_map[entry][cont_size]++;
        else m_cont_stat_map[entry][cont_size] = 1;
      }
    }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode TESFingerPrint::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  toolSvc()->releaseTool(m_leavesTool).ignore();
  m_leavesTool = 0;

  std::ofstream tes_finger_print;
  tes_finger_print.open(m_output_file_name.c_str());

  using namespace GaudiUtils;
  // Write low level TES statistics
  tes_finger_print << m_stat_map << "\n";
  if ( msgLevel(MSG::VERBOSE) ) {
    verbose() << "==> TES DataObject statistics: " << m_stat_map << endmsg;
  }
  
  // Write medium level TES statistics
  if (m_heur_level == "Medium") {
    tes_finger_print << m_cont_stat_map << "\n";
    if ( msgLevel(MSG::VERBOSE) ) {
      verbose() << "==> TES Container sizes: " << m_cont_stat_map << endmsg;
    }
  }
  
  tes_finger_print.close();

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
