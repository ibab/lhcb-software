// $Id: LogicalPads2DMaps.cpp,v 1.2 2008-05-06 12:19:27 jucogan Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
#include "Kernel/MuonTileID.h"

// local
#include "L0MuonMonUtils.h"
#include "LogicalPads2DMaps.h"

//-----------------------------------------------------------------------------
// Implementation file for class : LogicalPads2DMaps
//
// 2008-04-02 : 
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( LogicalPads2DMaps );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
LogicalPads2DMaps::LogicalPads2DMaps( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : GaudiHistoTool ( type, name , parent )
{
  declareInterface<LogicalPads2DMaps>(this);
  
  m_stationLayouts[M1]=MuonLayout(24,8);
  m_stationLayouts[M2]=MuonLayout(48,8);
  m_stationLayouts[M3]=MuonLayout(48,8);
  m_stationLayouts[M4]=MuonLayout(12,8);
  m_stationLayouts[M5]=MuonLayout(12,8);
  
  for (int sta=0; sta<L0MuonMonUtils::NStations; ++sta) {
    m_hmap[sta]=NULL;  
    m_hmultiBx[sta]=NULL;
    for (int reg=0; reg<L0MuonMonUtils::NRegions; ++reg) {
      m_hmultiBx_region[sta][reg]=NULL;
    }
  }
  
}

//=============================================================================
// Destructor
//=============================================================================
LogicalPads2DMaps::~LogicalPads2DMaps() {} 

//=============================================================================

void LogicalPads2DMaps::bookHistos() {
  for (int i=0; i<L0MuonMonUtils::NStations; ++i) bookHistos(i);
}
 
void LogicalPads2DMaps::bookHistos(int sta) {
  std::string hname;

  int xgrid=m_stationLayouts[sta].xGrid();
  int ygrid=m_stationLayouts[sta].yGrid();
  int nx=xgrid*16;
  int ny=ygrid*16;

  hname = name()+"_"+L0MuonMonUtils::stationName(sta);
  AIDA::IHistogram2D * h2D=book2D(hname,hname,-nx,nx,2*nx,-ny,ny,2*ny);
  m_hmap[sta]= h2D;

  hname = name()+"_multiplicity_per_time_slot_"+L0MuonMonUtils::stationName(sta);
  m_hmultiBx[sta]= book2D(hname,hname,-7.5,7.5,15,0.5,10.5,10);;
  for (int reg=0; reg<L0MuonMonUtils::NRegions; ++reg) {
    hname = name()+"_multiplicity_per_time_slot_"+L0MuonMonUtils::stationName(sta)+"_"+L0MuonMonUtils::regionName(reg);
    m_hmultiBx_region[sta][reg]= book2D(hname,hname,-7.5,7.5,15,0.5,10.5,10);;
  }
} 

void LogicalPads2DMaps::fillHistos(const std::vector<LHCb::MuonTileID> &pads, int ts) 
{ 
  int multi[L0MuonMonUtils::NStations][L0MuonMonUtils::NRegions];
  for (int sta=0; sta<L0MuonMonUtils::NStations; ++sta) {
    for (int reg=0; reg<L0MuonMonUtils::NRegions; ++reg) {
      multi[sta][reg]=0;
    }
  }
  
  std::vector<LHCb::MuonTileID>::const_iterator itpads;
  for (itpads=pads.begin();itpads<pads.end();++itpads){
    int sta = itpads->station();
    if (m_hmap[sta]==NULL) continue;
    int qua = itpads->quarter();
    int X  =itpads->nX();
    int Y  =itpads->nY();
    int reg=itpads->region();
    int f=1<<reg;
    for (int ix=X*f; ix<X*f+f; ++ix){
      for (int iy=Y*f; iy<Y*f+f; ++iy){
        int x= ix;
        int y= iy;
        L0MuonMonUtils::flipCoord(x,y,qua);
        fill(m_hmap[sta],x,y,1);
      }
    }
    ++multi[sta][reg];
  }

  for (int sta=0; sta<L0MuonMonUtils::NStations; ++sta) {
    int n=0;
    for (int reg=0; reg<L0MuonMonUtils::NRegions; ++reg) {
      if (m_hmultiBx_region[sta][reg]==NULL) continue;
      n+=multi[sta][reg];
      fill(m_hmultiBx_region[sta][reg],ts,multi[sta][reg],1);
    }
    if (m_hmultiBx[sta]==NULL) continue;
    fill(m_hmultiBx[sta],ts,n,1);
  }
  
}
