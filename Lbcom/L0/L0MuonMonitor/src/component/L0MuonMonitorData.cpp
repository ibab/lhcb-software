// Include files 
#include <sstream>

// from Gaudi
#include "Kernel/MuonTileID.h"
#include "Kernel/MuonLayout.h"
#include "Event/ODIN.h"
#include "Event/L0MuonData.h"

// ROOT
#include "GaudiUtils/Aida2ROOT.h"
#include "TH1D.h"
#include "TAxis.h"

// local
#include "L0MuonMonitorData.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0MuonMonitorData
//
// 2010-02-25 : Julien Cogan
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( L0MuonMonitorData )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0MuonMonitorData::L0MuonMonitorData( const std::string& name,
                                                  ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator )
  , m_inputTool(0)
{
  declareProperty( "L0Context" , m_l0Context = ""  );
  declareProperty( "EnableTAE" , m_enableTAE = false  );
}
//=============================================================================
// Destructor
//=============================================================================
L0MuonMonitorData::~L0MuonMonitorData() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode L0MuonMonitorData::initialize() {
  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiHistoAlg

  debug() << "==> Initialize" << endmsg;

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

  MuonLayout station_layouts_pads[5];
  station_layouts_pads[0]=MuonLayout(24,8);
  station_layouts_pads[1]=MuonLayout(48,8);
  station_layouts_pads[2]=MuonLayout(48,8);
  station_layouts_pads[3]=MuonLayout(12,8);
  station_layouts_pads[4]=MuonLayout(12,8);

  for (int sta=0; sta<5; ++sta) {
    int xgrid=station_layouts_pads[sta].xGrid();
    int ygrid=station_layouts_pads[sta].yGrid();

    int ntotpads = 3*xgrid*ygrid*4*4;
    if (sta==0) ntotpads = ntotpads/5;
    else ntotpads = ntotpads/50;

    std::stringstream station("");
    station<<"M"<<(sta+1);
    
    m_multi_pads[sta] = book1D("Logical_pads_multiplicity_"+station.str(),-0.5,ntotpads+0.5,200);
    m_padsX[sta] = book1D("Logical_pads_density_"+station.str()+"_X",-16.*xgrid,16.*xgrid,32*xgrid);
    m_padsY[sta] = book1D("Logical_pads_density_"+station.str()+"_Y",-16.*ygrid,16.*ygrid,32*ygrid);

    if ( fullDetail() ) {
      for (int reg=0; reg<4; ++reg){
        int f=2*(1<<reg);
        std::stringstream region("");
        region<<"R"<<(reg+1);
        m_pads[sta][reg] = book2D("Logical_pads_density_"+station.str()+"_"+region.str()
                                  ,-xgrid*f,xgrid*f,4*xgrid,-ygrid*f,ygrid*f,4*ygrid);
      }
    }
    
  }
      
  m_inputTool   = tool<IL0MuonInputTool>  ("L0MuonInputTool"  ,"DataMonL0MuonTool" ,this);

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0MuonMonitorData::execute() {

  StatusCode sc = StatusCode::SUCCESS;
  
  int tae_size = 0;
  if (m_enableTAE) {
    if (exist<LHCb::ODIN>(LHCb::ODINLocation::Default,false)) {
      // TAE size from odin
      LHCb::ODIN* odin = get<LHCb::ODIN>(LHCb::ODINLocation::Default,false);
      tae_size = int(odin->timeAlignmentEventWindow());
    } else {
      Warning("ODIN not found at "+LHCb::ODINLocation::Default+", TAE mode requested but not used"
              ,StatusCode::SUCCESS,50).ignore();
    }
  }

  int npads[5]={0,0,0,0,0};

  int ntae = 0;
  for (int itae = -1*tae_size; itae<=tae_size; ++itae){
    std::string rootInTes = m_tae_items[itae];

    // Get pads
    std::vector<LHCb::MuonTileID> l0muonpads;
    sc = m_inputTool->getPads(l0muonpads, rootInTes);
    if (sc.isFailure() ) return Error( "Unable to get l0muon pads",StatusCode::SUCCESS,20);
  
    // Fill histos
    for (std::vector<LHCb::MuonTileID>::iterator itpad=l0muonpads.begin(); itpad<l0muonpads.end(); ++itpad){
      int sta = itpad->station();
      ++npads[sta];
      
      int qua = itpad->quarter();
      int X   = itpad->nX();
      int Y   = itpad->nY();
      int reg = itpad->region();
      int f = 1<<reg;
      double weight = 1./f;
      for (int ix=X*f; ix<X*f+f; ++ix){
        double x = ix+0.5;
        if ( (qua==2) || (qua==3) ) x = -x;
        fill(m_padsX[sta],x,weight);
      }
      for (int iy=Y*f; iy<Y*f+f; ++iy){
        double y = iy+0.5;
        if ( (qua==1) || (qua==2) ) y = -y;
        fill(m_padsY[sta],y,weight);
      }
  
      if ( fullDetail() ) {
        double weight2 = 1./(1.*f*f);
        double x = X+0.5;
        if ( (qua==2) || (qua==3) ) x = -x;
        double y = Y+0.5;
        if ( (qua==1) || (qua==2) ) y = -y;
        fill(m_pads[sta][reg],x*f,y*f,weight2);
      }
      
    }

    ++ntae;
  }
  if (ntae==0) return Error( "No valid time slice found",StatusCode::SUCCESS,20);
  
  for (int sta=0; sta<5; ++sta) {
    fill(m_multi_pads[sta],npads[sta],1.);
  }
    
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0MuonMonitorData::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiHistoAlg::finalize();  // must be called after all other actions
}

//=============================================================================
