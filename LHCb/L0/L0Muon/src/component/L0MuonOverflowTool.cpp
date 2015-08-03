// Include files 

// from Gaudi
#include "Event/L0MuonError.h"

// local
#include "L0MuonOverflowTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0MuonOverflowTool
//
// 2010-03-09 : Julien Cogan
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( L0MuonOverflowTool )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0MuonOverflowTool::L0MuonOverflowTool( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IL0MuonOverflowTool>(this);
  declareProperty( "L0Context" , m_l0Context = ""  );

}
//=============================================================================
// Destructor
//=============================================================================
L0MuonOverflowTool::~L0MuonOverflowTool() {} 

//=============================================================================
// Get the list of quarters that have seen an overflow
//=============================================================================
StatusCode L0MuonOverflowTool::getQuarters(std::vector<int> & quarters, std::string rootInTes)
{

  quarters.clear();
  
  StatusCode sc = setProperty( "RootInTES", rootInTes );
  if( sc.isFailure() ) return Error( "Unable to set RootInTES property",StatusCode::FAILURE,50);

  std::string location=LHCb::L0MuonErrorLocation::CtrlCUSU + m_l0Context;
  if (!exist<LHCb::L0MuonErrors> (location)) {
    return Warning("L0MuonErrors not found at "+rootInTES()+"/.../"+location,StatusCode::FAILURE,50);
  }
  
  std::vector<LHCb::MuonTileID> qs;
  getOverflows(qs,location);
  for (std::vector<LHCb::MuonTileID>::iterator iq=qs.begin(); iq<qs.end(); ++iq){
    quarters.push_back(iq->quarter());
  }
  
  return StatusCode::SUCCESS;

}

//=============================================================================
// Get the list of BCSUs that have seen an overflow
//=============================================================================
StatusCode L0MuonOverflowTool::getBCSUs(std::vector<LHCb::MuonTileID> & bcsus, std::string rootInTes)
{

  bcsus.clear();
  
  StatusCode sc = setProperty( "RootInTES", rootInTes );
  if( sc.isFailure() ) return Error( "Unable to set RootInTES property",StatusCode::FAILURE,50);

  std::string location=LHCb::L0MuonErrorLocation::ProcBCSU + m_l0Context;
  if (!exist<LHCb::L0MuonErrors> (location)) {
    return Warning("L0MuonErrors not found at "+rootInTES()+"/.../"+location,StatusCode::FAILURE,50);
  }
  
  getOverflows(bcsus,location);
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Get the list of PUs that have seen an overflow
//=============================================================================
StatusCode L0MuonOverflowTool::getPUs(std::vector<LHCb::MuonTileID> & pus, std::string rootInTes)
{

  pus.clear();
  
  StatusCode sc = setProperty( "RootInTES", rootInTes );
  if( sc.isFailure() ) return Error( "Unable to set RootInTES property",StatusCode::FAILURE,50);

  std::string location=LHCb::L0MuonErrorLocation::ProcPU + m_l0Context;
  if (!exist<LHCb::L0MuonErrors> (location)) {
    return Warning("L0MuonErrors not found at "+rootInTES()+"/.../"+location,StatusCode::FAILURE,50);
  }
  
  getOverflows(pus,location);
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Get the list of overflow reported at the given location 
//=============================================================================
void L0MuonOverflowTool::getOverflows(std::vector<LHCb::MuonTileID> & overflows, std::string location)
{

  LHCb::L0MuonErrors *perrors = get<LHCb::L0MuonErrors> (location);

  // Loop over L0MuonErrors
  for (LHCb::L0MuonErrors::const_iterator iterr=perrors->begin(); iterr!=perrors->end(); ++iterr)
  {
    const LHCb::L0MuonError *err = (*iterr);
    int status = err->status();
    if(  (status&0x3) == 3) overflows.push_back( err->key() );
  }
  
}

