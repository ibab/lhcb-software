// Include files 

// from Gaudi
#include "Kernel/MuonTileID.h"
#include "Event/ODIN.h"
#include "Event/L0MuonCandidate.h"

// local
#include "L0MuonMonitorCandidates.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0MuonMonitorCandidates
//
// 2010-02-25 : Julien Cogan
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( L0MuonMonitorCandidates )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0MuonMonitorCandidates::L0MuonMonitorCandidates( const std::string& name,
                                                  ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator )
  , m_ptGev(0)
  , m_ptBin(0)
  , m_charge(0)
  , m_multi(0)
  , m_seedX(0)
  , m_seedY(0)
  , m_ovf(0)
  , m_ovfTool(0)
{
  declareProperty( "EnableTAE" , m_enableTAE = true  );
  declareProperty( "L0Context" , m_l0Context = ""  );
  declareProperty( "AllCandidates" , m_allCandidates = false  );
}
//=============================================================================
// Destructor
//=============================================================================
L0MuonMonitorCandidates::~L0MuonMonitorCandidates() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode L0MuonMonitorCandidates::initialize() {
  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiHistoAlg

  debug() << "==> Initialize" << endmsg;

  m_location = LHCb::L0MuonCandidateLocation::Default;
  if (m_allCandidates) m_location = LHCb::L0MuonCandidateLocation::PU;
  m_location += m_l0Context;

  // TAE slots names
  if (m_enableTAE){
    m_tae_items[-7] = "Prev7/";
    m_tae_items[-6] = "Prev6/";
    m_tae_items[-5] = "Prev5/";
    m_tae_items[-4] = "Prev4/";
    m_tae_items[-3] = "Prev3/";
    m_tae_items[-2] = "Prev2/";
    m_tae_items[-1] = "Prev1/";
    m_tae_items[ 0] = "";
    m_tae_items[ 1] = "Next1/";
    m_tae_items[ 2] = "Next2/";
    m_tae_items[ 3] = "Next3/";
    m_tae_items[ 4] = "Next4/";
    m_tae_items[ 5] = "Next5/";
    m_tae_items[ 6] = "Next6/";
    m_tae_items[ 7] = "Next7/";
  }

  // Tools
  m_ovfTool = tool<IL0MuonOverflowTool> ("L0MuonOverflowTool","CandMonOverflowTool",this);
  
  // Book histos
  std::string title_prefix = "Final_Candidates_";
  if (m_allCandidates) title_prefix = "All_Candidates_";

  m_ptGev = book1D(title_prefix+"PT_GeV",0.,15.,300);
  m_ptBin = book1D(title_prefix+"PT_bin",0.5,255.5,255);
  m_charge = book1D(title_prefix+"charge",-2.,2.,2);
  if (!m_allCandidates)
    m_multi = book1D(title_prefix+"Multiplicity",-0.5,8.5,9);
  else
    m_multi = book1D(title_prefix+"Multiplicity",-0.5,32.5,33);

  int xgrid = 48;
  int ygrid =  8;
  m_seedX = book1D(title_prefix+"seed_density_X",-16.*xgrid,16.*xgrid,32*xgrid);
  m_seedY = book1D(title_prefix+"seed_density_Y",-16.*ygrid,16.*ygrid,32*ygrid);
  
  if (!m_allCandidates)
    m_ovf = book1D(title_prefix+"Number of overflows",+0.5,4.5,4);
  else
    m_ovf = book1D(title_prefix+"Number of overflows",+0.5,192.5,192);

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0MuonMonitorCandidates::execute() {

  StatusCode sc = StatusCode::SUCCESS;

  int tae_size = 0;
  if (m_enableTAE) {
    if (exist<LHCb::ODIN>(LHCb::ODINLocation::Default,false)) {
      // TAE size from odin
      LHCb::ODIN* odin = get<LHCb::ODIN>(LHCb::ODINLocation::Default,false);
      tae_size = int(odin->timeAlignmentEventWindow());
    } else {
      Warning("ODIN not found at "+LHCb::ODINLocation::Default+", TAE mode requested but not used"
              ,StatusCode::FAILURE,50).ignore();
    }
  }

  int ncands = 0;
  int novfs = 0;

  int ntae = 0;
  for (int itae = -1*tae_size; itae<=tae_size; ++itae){
    std::string rootInTes = m_tae_items[itae];

    if (!m_allCandidates){
      std::vector<int> quarters;
      sc = m_ovfTool -> getQuarters(quarters,rootInTes);
      novfs += quarters.size();
    } else {
      std::vector<LHCb::MuonTileID> pus;
      sc = m_ovfTool -> getPUs(pus,rootInTes);
      novfs += pus.size();
    }
  
    sc=setProperty("RootInTES",rootInTes);
    if (sc.isFailure()) return Error("Can not set RootInTES ... abort",StatusCode::SUCCESS,50);

    if (!exist<LHCb::L0MuonCandidates>( m_location ) ) {
      Warning("L0MuonCandidates not found at "+rootInTes+"/..."+m_location,StatusCode::FAILURE,20).ignore();
      continue;
    }
    LHCb::L0MuonCandidates* candidates = get<LHCb::L0MuonCandidates>( m_location );

    ncands += candidates->size();

    for (LHCb::L0MuonCandidates::const_iterator itcand= candidates->begin(); itcand<candidates->end();++itcand) {
      int encodedPt = (*itcand)->encodedPt();
      int charge = ( ((encodedPt>>7)&0x1)*2 ) -1;
      double pt = charge*((*itcand)->pt()/1000.);
      fill(m_ptGev,pt,1.);
      fill(m_ptBin,encodedPt,1.);
      fill(m_charge,charge,1.);
      std::vector<LHCb::MuonTileID> seeds = (*itcand)->muonTileIDs(2);
      if (seeds.size()!=1) return Error("Candidate has a wrong number of seed",StatusCode::SUCCESS, 50);
      LHCb::MuonTileID seed = seeds[0];
      int qua = seed.quarter();
      int X   = seed.nX();
      int Y   = seed.nY();
      int reg = seed.region();
      int f = 1<<reg;
      double weight = 1./f;
      for (int ix=X*f; ix<X*f+f; ++ix){
        double x = ix+0.5;
        if ( (qua==2) || (qua==3) ) x = -x;
        fill(m_seedX,x,weight);
      }
      for (int iy=Y*f; iy<Y*f+f; ++iy){
        double y = iy+0.5;
        if ( (qua==1) || (qua==2) ) y = -y;
        fill(m_seedY,y,weight);
      }
    }

    ++ntae;
  }
  
  if (ntae==0) return Error( "No valid time slice found",StatusCode::SUCCESS,20);

  fill(m_multi,ncands,1.);
  fill(m_ovf,novfs,1.);

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0MuonMonitorCandidates::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiHistoAlg::finalize();  // must be called after all other actions
}

//=============================================================================
