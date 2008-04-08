// $Id: LogicalPads2DMaps.cpp,v 1.1 2008-04-08 11:31:03 jucogan Exp $
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
  
  for (int i=0;i<L0MuonMonUtils::NStations;++i) m_hmap[i]=NULL;
    
}

//=============================================================================
// Destructor
//=============================================================================
LogicalPads2DMaps::~LogicalPads2DMaps() {} 

//=============================================================================

void LogicalPads2DMaps::bookHistos() {
  for (int i=0; i<5; ++i) bookHistos(i);
}
 
void LogicalPads2DMaps::bookHistos(int sta) {
  int xgrid=m_stationLayouts[sta].xGrid();
  int ygrid=m_stationLayouts[sta].yGrid();
  int nx=xgrid*16;
  int ny=ygrid*16;
  std::string hname = name()+"_"+L0MuonMonUtils::stationName(sta);
  info()<<"tool name "<<name()<<endmsg;
  
  m_hmap[sta]=book2D(hname,hname,-nx,nx,2*nx,-ny,ny,2*ny);
} 

void LogicalPads2DMaps::fillHistos(const std::vector<LHCb::MuonTileID> &pads) 
{ 
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
    }
}
