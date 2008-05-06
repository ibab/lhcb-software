// $Id: PhysicalChannelsHist.cpp,v 1.3 2008-05-06 12:19:27 jucogan Exp $
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

  declareProperty("H2D", m_2D=false);
  declareProperty("HBX", m_BX=false);
  
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
      AIDA::IHistogram1D * h = book1D(hname,hname,-0.5,nbins-0.5,nbins);
      m_hist[quarter][type][station][region]= h;
      if (m_2D) {
        int xbins=2*lay.xGrid();
        int ybins=2*lay.yGrid();
        AIDA::IHistogram2D * h2D = book2D(hname+"2D",hname+"2D",-0.5,xbins-0.5,xbins,-0.5,ybins-0.5,ybins);
        m_hist2D[quarter][type][station][region]= h2D;
      }
      if (m_BX){
        AIDA::IHistogram2D * hBX = book2D(hname+"-VS-BX",hname+"-VS-BX",-0.5,nbins-0.5,nbins,-7.5,7.5,15);
        m_histBX[quarter][type][station][region]= hBX;
      }
    }
  }
}

void PhysicalChannelsHist::fillHistos(const std::vector<LHCb::MuonTileID> &tiles, int ts)
{
    std::vector<LHCb::MuonTileID>::const_iterator it_tiles;
    for (it_tiles=tiles.begin();it_tiles<tiles.end();++it_tiles){
      int sta = it_tiles->station();
      int reg = it_tiles->region();
      MuonLayout lay = it_tiles->layout();
      for (L0MuonMonUtils::Channel_type type =L0MuonMonUtils::Pad; type<L0MuonMonUtils::nb_channel_types; type++){
        if (lay==m_channel_layout[type].stationLayout(sta).regionLayout(reg)) {
          int qua = it_tiles->quarter();
          if (m_hist[qua][type][sta][reg]==NULL) continue;
          MuonLayout layOL = m_opt_link_layout.stationLayout(sta).regionLayout(reg);
          LHCb::MuonTileID ol=it_tiles->containerID(layOL);

          int xgrid=lay.xGrid();
          int ygrid=lay.yGrid();
          int xOLgrid=layOL.xGrid();
          int yOLgrid=layOL.yGrid();

          int ipb=ol.nX()/xOLgrid+2*(ol.nY()/yOLgrid)-1;
          int ind_ol=ipb*xOLgrid*yOLgrid+ol.localX(layOL)+ol.localY(layOL)*xOLgrid;
          int ind_local=it_tiles->localX(lay)%(xgrid/xOLgrid)+it_tiles->localY(lay)%(ygrid/yOLgrid)*xgrid/xOLgrid;

          int ind = ind_local+ind_ol*xgrid*ygrid/(xOLgrid*yOLgrid);

          fill(m_hist[qua][type][sta][reg],ind,1);
          if (m_2D) fill(m_hist2D[qua][type][sta][reg],it_tiles->nX(),it_tiles->nY(),1);
          if (m_BX) fill(m_histBX[qua][type][sta][reg],ind,ts,1);
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
  
  
  m_channel_layout[L0MuonMonUtils::Pad]   =pad_layout;
  m_channel_layout[L0MuonMonUtils::StripV]=stripV_layout;
  m_channel_layout[L0MuonMonUtils::StripH]=stripH_layout;


}
