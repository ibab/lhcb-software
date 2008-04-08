// $Id: PhysicalChannelsHist.cpp,v 1.1 2008-04-08 11:31:03 jucogan Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "L0MuonMonUtils.h"
#include "PhysicalChannelsHist.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PhysicalChannelsHist
//
// 2008-04-07 : 
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( PhysicalChannelsHist );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PhysicalChannelsHist::PhysicalChannelsHist( const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent )
  : GaudiHistoTool ( type, name , parent )
{
  declareInterface<PhysicalChannelsHist>(this);

  setLayouts();
  
  for (int qua=0;qua<4;++qua){
    for (L0MuonMonUtils::Channel_type type =L0MuonMonUtils::Pad; type<L0MuonMonUtils::nb_channel_types; type++){
      for (int sta=0;sta<5;++sta){
        for (int reg=0; reg<4; ++reg){  
          m_hist[qua][type][sta][reg]=NULL;
        }
      }
    } 
  }
  
}
//=============================================================================
// Destructor
//=============================================================================
PhysicalChannelsHist::~PhysicalChannelsHist() {} 

//=============================================================================

void PhysicalChannelsHist::bookHistos()
{
  for (int qua=0;qua<4;++qua){
    for (int reg=0; reg<4; ++reg){  
      for (int sta=0;sta<5;++sta){
        bookHistos(qua, reg, sta);
      }
    }
  } 
}

void PhysicalChannelsHist::bookHistos(int quarter, int region, int station)
{
  // Physical channels
  for (L0MuonMonUtils::Channel_type type=L0MuonMonUtils::Pad; type<L0MuonMonUtils::nb_channel_types; type++){
    std::string hname = name()+"_Hits_"+L0MuonMonUtils::quarterName(quarter)+"_"+L0MuonMonUtils::stationName(station)
      +"_"+L0MuonMonUtils::regionName(region)+"_"+L0MuonMonUtils::channelTypeName(type);
    MuonLayout lay = m_channel_layout[type].stationLayout(station).regionLayout(region);
    if (lay.isDefined()){  
      int nbins=lay.xGrid()*lay.yGrid()*3;
      m_hist[quarter][type][station][region]=book1D(hname,hname,-0.5,nbins-0.5,nbins);
    }
  }
}

void PhysicalChannelsHist::fillHistos(const std::vector<LHCb::MuonTileID> &tiles)
{
    std::vector<LHCb::MuonTileID>::const_iterator it_tiles;
    for (it_tiles=tiles.begin();it_tiles<tiles.end();++it_tiles){
      int qua = it_tiles->quarter();
      int sta = it_tiles->station();
      int reg = it_tiles->region();
      MuonLayout lay = it_tiles->layout();
      for (L0MuonMonUtils::Channel_type type =L0MuonMonUtils::Pad; type<L0MuonMonUtils::nb_channel_types; type++){
        if (lay==m_channel_layout[type].stationLayout(sta).regionLayout(reg)) {
          if (m_hist[qua][type][sta][reg]==NULL) continue;
          int xgrid=lay.xGrid();
          int ygrid=lay.yGrid();
          int x = it_tiles->nX();
          int y = it_tiles->nY();
          int qua = it_tiles->quarter();
          int offset = (int)(x/xgrid) +  ( ( ((int)(y/ygrid)) <<1)&2 ) -1;
          int ind = (x%xgrid) + (y%ygrid)*xgrid + offset*xgrid*ygrid;
          fill(m_hist[qua][type][sta][reg],ind,1);
        }
      }
    }

}

void PhysicalChannelsHist::setLayouts()
{


  MuonSystemLayout pad_layout =MuonSystemLayout(MuonStationLayout(MuonLayout(24,8)),
                                                MuonStationLayout(MuonLayout(0,0)),
                                                MuonStationLayout(MuonLayout(0,0)),
                                                MuonStationLayout(MuonLayout(12,8),
                                                                  MuonLayout(0,0),
                                                                  MuonLayout(0,0),
                                                                  MuonLayout(0,0)),
                                                MuonStationLayout(MuonLayout(12,8),
                                                                  MuonLayout(0,0),
                                                                  MuonLayout(0,0),
                                                                  MuonLayout(0,0)));
  
  MuonSystemLayout stripH_layout =MuonSystemLayout(MuonStationLayout(MuonLayout(0,0)),
                                                   MuonStationLayout(MuonLayout(8,8),
                                                                     MuonLayout(4,8),
                                                                     MuonLayout(2,8),
                                                                     MuonLayout(2,8)),
                                                   MuonStationLayout(MuonLayout(8,8),
                                                                     MuonLayout(4,8),
                                                                     MuonLayout(2,8),
                                                                     MuonLayout(2,8)),
                                                   MuonStationLayout(MuonLayout(0,0),
                                                                     MuonLayout(4,8),
                                                                     MuonLayout(2,8),
                                                                     MuonLayout(2,8)),
                                                   MuonStationLayout(MuonLayout(0,0),
                                                                     MuonLayout(4,8),
                                                                     MuonLayout(2,8),
                                                                     MuonLayout(2,8)));
  
  MuonSystemLayout stripV_layout =MuonSystemLayout(MuonStationLayout(MuonLayout(0,0)),
                                                   MuonStationLayout(MuonLayout(48,1),
                                                                     MuonLayout(48,2),
                                                                     MuonLayout(48,2),
                                                                     MuonLayout(48,2)),
                                                   MuonStationLayout(MuonLayout(48,1),
                                                                     MuonLayout(48,2),
                                                                     MuonLayout(48,2),
                                                                     MuonLayout(48,2)),
                                                   MuonStationLayout(MuonLayout(0,0),
                                                                     MuonLayout(12,2),
                                                                     MuonLayout(12,2),
                                                                     MuonLayout(12,2)),
                                                   MuonStationLayout(MuonLayout(0,0),
                                                                     MuonLayout(12,2),
                                                                     MuonLayout(12,2),
                                                                     MuonLayout(12,2)));

  m_channel_layout[L0MuonMonUtils::Pad]   =pad_layout;
  m_channel_layout[L0MuonMonUtils::StripV]=stripV_layout;
  m_channel_layout[L0MuonMonUtils::StripH]=stripH_layout;


}
