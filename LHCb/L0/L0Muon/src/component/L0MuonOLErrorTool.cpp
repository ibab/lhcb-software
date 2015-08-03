// Include files 

// from Gaudi
#include "Kernel/MuonLayout.h"
#include "MuonKernel/MuonStationLayout.h"
#include "Event/L0MuonError.h"

// local
#include "L0MuonOLErrorTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0MuonOLErrorTool
//
// 2010-02-25 : Julien Cogan
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( L0MuonOLErrorTool )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0MuonOLErrorTool::L0MuonOLErrorTool( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IL0MuonOLErrorTool>(this);
  declareProperty( "L0Context" , m_l0Context = ""  );

  // Set the optical link layout
  m_opt_link_layout = MuonSystemLayout(MuonStationLayout(MuonLayout(2,4)),
                                       MuonStationLayout(MuonLayout(4,1),
                                                         MuonLayout(4,2),
                                                         MuonLayout(2,2),
                                                         MuonLayout(2,2)),
                                       MuonStationLayout(MuonLayout(4,1),
                                                         MuonLayout(4,2),
                                                         MuonLayout(2,2),
                                                         MuonLayout(2,2)),
                                       MuonStationLayout(MuonLayout(2,2)),
                                       MuonStationLayout(MuonLayout(2,2)));
}
//=============================================================================
// Destructor
//=============================================================================
L0MuonOLErrorTool::~L0MuonOLErrorTool() {} 

//=============================================================================

StatusCode L0MuonOLErrorTool::getTiles(std::vector<LHCb::MuonTileID> & ols, std::string rootInTes)
{
  StatusCode sc=StatusCode::SUCCESS;
  
  ols.clear();

  sc = setProperty( "RootInTES", rootInTes );
  if( sc.isFailure() ) return Error( "Unable to set RootInTES property",StatusCode::FAILURE,50);

  // L0Muon error container
  std::string location=LHCb::L0MuonErrorLocation::ProcPU + m_l0Context;
  LHCb::L0MuonErrors *perrors = getIfExists<LHCb::L0MuonErrors> (location);
  if ( NULL == perrors ) {
    return Warning("L0MuonErrors not found at "+rootInTes+"/.../"+location,StatusCode::FAILURE,50);
  }

  // Loop over L0MuonErrors
  for (LHCb::L0MuonErrors::const_iterator iterr=perrors->begin(); iterr!=perrors->end(); ++iterr)
  {

    const LHCb::L0MuonError *err = (*iterr);
    LHCb::MuonTileID pu=err->key();
    
    // optical link error word
    unsigned int optlinkError = ( (err->hardware()>>16) &0xFF);
    if (optlinkError==0) continue;
    // optlinkError :
    //   0 -> M1a
    //   1 -> M1a
    //   2 -> M2a
    //   3 -> M2b
    //   4 -> M3a
    //   5 -> M3b
    //   6 -> M4
    //   7 -> M5
    
    int qua = pu.quarter();
    int reg = pu.region();
    for (int ib =0; ib<8; ++ib) { // Loop over opt link error bits
      if ( (optlinkError>>ib)&0x1 ) { // If error bit is ON
        LHCb::MuonTileID olid;
        int sta=ib/2;
        if (ib==7) sta=4;
        MuonLayout layOL = m_opt_link_layout.stationLayout(sta).regionLayout(reg);
        pu.setStation(sta);
        std::vector<LHCb::MuonTileID> ols_in_pu=layOL.tiles(pu);
        if (ols_in_pu.size()==1) 
        { 
          olid=ols_in_pu[0];
        } 
        else if (ols_in_pu.size()==2)
        {
          if (sta==0) { // M1 : 2 optical links per PU
            olid=ols_in_pu[ib%2];
          } else if (sta==1 || sta==2) {
            if (reg==0) { // M2-M3 R1 : the layout gives 2 OLs per PU only one was received by this PU                
              if ( (pu.nY()%2)==1 ) {
                olid=ols_in_pu[0];
              } else {
                olid=ols_in_pu[1];
              }
            } else if (reg==1) { // M2-M3 R2 : 2 optical links per PU
              olid=ols_in_pu[ib%2];
            } else {
              return Error("Wrong number of optical links M2 M3",StatusCode::FAILURE,50);
            }
          } else {
            return Error("Wrong number of optical links M4 M5",StatusCode::FAILURE,50);
          }
        }
        else
        {
          return Error("Wrong number of optical links >2",StatusCode::FAILURE,50);
        }
        olid.setQuarter(qua);
        olid.setStation(sta);
        ols.push_back(olid);
        if( msgLevel(MSG::VERBOSE) ) {
          int quarter,iboard,ipu;
          err->index(quarter,iboard,ipu);
          verbose()<<"Error on "<<pu.toString()
                   <<" Q"<<(quarter+1)<<" R"<<(reg+1)<<" M"<<(sta+1)<<" PB"<<iboard<<" PU"<<ipu
                   <<" OL "<<olid.toString()
                   <<endmsg;
        }
      } // End if error bit is ON
    } // End of loop over opt link error bits
  } // End of loop over L0MuonError in container
  
  sc = setProperty( "RootInTES", "" );
  if( sc.isFailure() ) return Error( "Unable to reset RootInTES property",StatusCode::FAILURE,50);

  return StatusCode::SUCCESS;

}


