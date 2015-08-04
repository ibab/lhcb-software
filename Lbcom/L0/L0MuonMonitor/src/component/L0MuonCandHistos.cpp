// Include files 

#include "L0MuonKernel/MonUtilities.h"

// local
#include "L0MuonCandHistos.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0MuonCandHistos
//
// 2008-07-23 : Julien Cogan
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( L0MuonCandHistos )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0MuonCandHistos::L0MuonCandHistos( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : GaudiHistoTool ( type, name , parent )
{
  declareInterface<L0MuonCandHistos>(this);

  declareProperty( "unweighted"  , m_unweighted  = false ) ;

  m_stationLayouts[L0Muon::MonUtilities::M1]=MuonLayout(24,8);
  m_stationLayouts[L0Muon::MonUtilities::M2]=MuonLayout(48,8);
  m_stationLayouts[L0Muon::MonUtilities::M3]=MuonLayout(48,8);

  m_hpt=NULL;
  m_hencodedpt=NULL;
  for (int sta=0; sta<3;++sta) m_hpos[sta]=NULL;
  m_hnumber=NULL;

}
//=============================================================================
// Destructor
//=============================================================================
L0MuonCandHistos::~L0MuonCandHistos() {} 

//=============================================================================

void L0MuonCandHistos::bookHistos(int nmax, bool shortname)
{
  std::string toolname="";
  if (!shortname) toolname=name();

  std::string hname;
  
  hname = L0Muon::MonUtilities::hname_cand_pt(toolname);
  m_hpt=book1D(hname,hname,0.,20.,200);

  hname = L0Muon::MonUtilities::hname_cand_encodedpt(toolname);
  m_hencodedpt=book1D(hname,hname,-127.5,127.5,255);

  for (int sta=0; sta<3;++sta) {
    int xgrid=m_stationLayouts[sta].xGrid();
    int ygrid=m_stationLayouts[sta].yGrid();
    int nx=xgrid*16;
    int ny=ygrid*16;
    
    hname = L0Muon::MonUtilities::hname_cand_pos(sta,toolname);
    m_hpos[sta]=book2D(hname,hname,-nx,nx,2*nx,-ny,ny,2*ny);
  }

  hname = L0Muon::MonUtilities::hname_cand_number(toolname);
  m_hnumber=book2D(hname,hname,-7.5,7.5,15,-0.5,0.5+nmax,nmax+1);

  hname = L0Muon::MonUtilities::hname_cand_bid(toolname);
  m_hbid=book1D(hname,hname,-0.5,-0.5+3564,3564);

  hname = L0Muon::MonUtilities::hname_cand_evtbid(toolname);
  m_hevtbid=book1D(hname,hname,-0.5,-0.5+3564,3564);

  hname = L0Muon::MonUtilities::hname_cand_sum(toolname);
  m_hsum=book1D(hname,hname,-0.5,0.5+nmax,nmax+1);

}

void L0MuonCandHistos::fillHistos(LHCb::L0MuonCandidates* cands, int ts, int bid)
{
  LHCb::L0MuonCandidates::const_iterator itcand;

  for ( itcand= cands->begin(); itcand<cands->end();++itcand) {

    int encodedPt = (*itcand)->encodedPt();
    int value=encodedPt&0x7F;
    int charge=(encodedPt>>7)&0x1;
    int sign=(charge*2) -1;
    encodedPt= value*sign;
    double abs_pt_gev=sign*(*itcand)->pt()/1000.;
    fill(m_hpt,abs_pt_gev,1);
    fill(m_hencodedpt,encodedPt,1);
    
    for (int sta=0; sta<3;++sta) {
      std::vector<LHCb::MuonTileID> pads = (*itcand)->muonTileIDs(sta);
      std::vector<LHCb::MuonTileID>::iterator itpad;
      for (itpad=pads.begin(); itpad<pads.end(); ++itpad) {
        int qua = itpad->quarter();
        int X  =itpad->nX();
        int Y  =itpad->nY();
        int reg=itpad->region();
        int f=1<<reg;
        //         debug()<<"Q"<<(qua+1)
        //                <<" M"<<(sta+1)
        //                <<" ("<<itpad->layout().xGrid()<<","<<itpad->layout().yGrid()<<")"
        //                <<" X: "<<X
        //                <<" Y: "<<Y
        //                <<" reg: "<<reg
        //                <<" f: "<<f<<endmsg;
        
        for (int ix=X*f; ix<X*f+f; ++ix){
          for (int iy=Y*f; iy<Y*f+f; ++iy){
            int iix= ix;
            int iiy= iy;
            L0Muon::MonUtilities::flipCoord(iix,iiy,qua);
            double x=iix;
            double y=iiy;
            L0Muon::MonUtilities::offsetCoord(x,y,qua);
            double weight=1./(1.*f*f);
            if (m_unweighted) weight=1.;
            fill(m_hpos[sta],x,y,weight);
          }
        }
      }
    }
  }

  fill(m_hnumber,ts,cands->size(),1);
  if ( (cands->size()>0) & (bid>-1) ) fill(m_hbid,bid,1);
}

void L0MuonCandHistos::fillHistos(int sum, int bid)
{
  fill(m_hsum,sum,1);
  if ( (sum>0) & (bid>-1) ) fill(m_hevtbid,bid,1);
  
}
