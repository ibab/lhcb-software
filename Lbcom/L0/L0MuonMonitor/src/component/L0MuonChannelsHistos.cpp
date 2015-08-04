// Include files 

// local
#include "L0MuonChannelsHistos.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0MuonChannelsHistos
//
// 2008-04-07 : 
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( L0MuonChannelsHistos )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0MuonChannelsHistos::L0MuonChannelsHistos( const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent )
  : GaudiHistoTool ( type, name , parent )
{
  declareInterface<L0MuonChannelsHistos>(this);

  declareProperty("H2D", m_2D=false);
  declareProperty("HBX", m_BX=false);
  declareProperty("HDT", m_DT=false);
  
  setLayouts();
  
  for (int qua=0;qua<4;++qua){
    for (L0Muon::MonUtilities::Channel_type type =L0Muon::MonUtilities::Pad; 
         type<L0Muon::MonUtilities::nb_channel_types; 
         type++){
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
L0MuonChannelsHistos::~L0MuonChannelsHistos() {} 

//=============================================================================

void L0MuonChannelsHistos::bookHistos(bool shortname)
{
  for (int qua=0;qua<4;++qua){
    for (int reg=0; reg<4; ++reg){  
      for (int sta=0;sta<5;++sta){
        bookHistos(qua, reg, sta,shortname);
      }
    }
  } 
}

void L0MuonChannelsHistos::bookHistos(int quarter, int region, int station, bool shortname)
{
  std::string toolname="";
  if (!shortname) toolname=name();

  // Physical channels
  for (L0Muon::MonUtilities::Channel_type type=L0Muon::MonUtilities::Pad;
       type<L0Muon::MonUtilities::nb_channel_types;
       type++){
    std::string hname = L0Muon::MonUtilities::hname_channels_hist(quarter,type,station,region,toolname);
    MuonLayout lay = m_channel_layout[type].stationLayout(station).regionLayout(region);
    if (lay.isDefined()){  
      int nbins=lay.xGrid()*lay.yGrid()*3;
      AIDA::IHistogram1D * h = book1D(hname,hname,-0.5,nbins-0.5,nbins);
      m_hist[quarter][type][station][region]= h;
      if (m_2D) {
        int xbins=2*lay.xGrid();
        int ybins=2*lay.yGrid();
        hname=L0Muon::MonUtilities::hname_channels_hist2D(quarter,type,station,region,toolname);
        AIDA::IHistogram2D * h2D = book2D(hname,hname,-0.5,xbins-0.5,xbins,-0.5,ybins-0.5,ybins);
        m_hist2D[quarter][type][station][region]= h2D;
      }
      if (m_BX){
        hname=L0Muon::MonUtilities::hname_channels_histBX(quarter,type,station,region,toolname);
        AIDA::IHistogram2D * hBX = book2D(hname,hname,-0.5,nbins-0.5,nbins,-7.5,7.5,15);
        m_histBX[quarter][type][station][region]= hBX;
      }
      if (m_DT){
        hname=L0Muon::MonUtilities::hname_channels_histDT(quarter,type,station,region,toolname);
        AIDA::IHistogram2D * hDT = book2D(hname,hname,-0.5,nbins-0.5,nbins,-15.5,15.5,31);
        m_histDT[quarter][type][station][region]= hDT;
      }
    }
  }
}

void L0MuonChannelsHistos::fillHistos(const std::vector<std::pair<LHCb::MuonTileID,int > > &tiles)
{
  std::map<int, std::vector<LHCb::MuonTileID> > vtiles;
  for (std::vector<std::pair<LHCb::MuonTileID,int > >::const_iterator it=tiles.begin(); it!=tiles.end();++it){
    vtiles[(*it).second].push_back((*it).first);
  }
  std::map<int, std::vector<LHCb::MuonTileID> >::iterator it_vtiles;
  for (it_vtiles=vtiles.begin(); it_vtiles!=vtiles.end(); ++it_vtiles) {
    fillHistos(it_vtiles->second,it_vtiles->first);
  }
  
}

void L0MuonChannelsHistos::fillHistos(const std::vector<std::pair<LHCb::MuonTileID, std::pair<int,int> > > &tiles)
{
  std::map<int, std::vector<LHCb::MuonTileID> > vtiles;
  for (std::vector<std::pair<LHCb::MuonTileID,std::pair<int,int> > >::const_iterator it=tiles.begin(); it!=tiles.end();++it){
    vtiles[((*it).second).first].push_back((*it).first);
  }
  std::map<int, std::vector<LHCb::MuonTileID> >::iterator it_vtiles;
  for (it_vtiles=vtiles.begin(); it_vtiles!=vtiles.end(); ++it_vtiles) {
    fillHistos(it_vtiles->second,it_vtiles->first);
  }
  
}


void L0MuonChannelsHistos::fillHistos(const std::vector<LHCb::MuonTileID> &tiles, int ts)
{
    std::vector<LHCb::MuonTileID>::const_iterator it_tiles;
    for (it_tiles=tiles.begin();it_tiles<tiles.end();++it_tiles){
      int sta = it_tiles->station();
      int reg = it_tiles->region();
      MuonLayout lay = it_tiles->layout();
      for (L0Muon::MonUtilities::Channel_type type =L0Muon::MonUtilities::Pad;
           type<L0Muon::MonUtilities::nb_channel_types;
           type++){
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

void L0MuonChannelsHistos::fillHistosDT(const std::vector<std::pair<LHCb::MuonTileID, int > > &tiles)
{
  
  if (!m_DT) return;
  
  std::vector<std::pair<LHCb::MuonTileID, int > >::const_iterator it_tiles;
  for (it_tiles=tiles.begin();it_tiles<tiles.end();++it_tiles){
    LHCb::MuonTileID tile=it_tiles->first;
    int sta = tile.station();
    int reg = tile.region();
    MuonLayout lay = tile.layout();
    for (L0Muon::MonUtilities::Channel_type type =L0Muon::MonUtilities::Pad;
         type<L0Muon::MonUtilities::nb_channel_types;
         type++){
      if (lay==m_channel_layout[type].stationLayout(sta).regionLayout(reg)) {
        int qua = tile.quarter();
        if (m_hist[qua][type][sta][reg]==NULL) continue;
        MuonLayout layOL = m_opt_link_layout.stationLayout(sta).regionLayout(reg);
        LHCb::MuonTileID ol=tile.containerID(layOL);
        
        int xgrid=lay.xGrid();
        int ygrid=lay.yGrid();
        int xOLgrid=layOL.xGrid();
        int yOLgrid=layOL.yGrid();
        
        int ipb=ol.nX()/xOLgrid+2*(ol.nY()/yOLgrid)-1;
        int ind_ol=ipb*xOLgrid*yOLgrid+ol.localX(layOL)+ol.localY(layOL)*xOLgrid;
        int ind_local=tile.localX(lay)%(xgrid/xOLgrid)+tile.localY(lay)%(ygrid/yOLgrid)*xgrid/xOLgrid;
        
        int ind = ind_local+ind_ol*xgrid*ygrid/(xOLgrid*yOLgrid);
          
        //if ( (it_tiles->second>14.) || (it_tiles->second<-14.) ) {
        //  info()<< "deltaT= "<<it_tiles->second<<" Q"<<(qua+1)<<" type "<<type<<" M"<<(sta+1)<<" R"<<reg+1<<endmsg;
        //}
        
        fill(m_histDT[qua][type][sta][reg],ind,it_tiles->second,1);
      }
    }
  }
}


void L0MuonChannelsHistos::setLayouts()
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
  
  
  m_channel_layout[L0Muon::MonUtilities::Pad]   =pad_layout;
  m_channel_layout[L0Muon::MonUtilities::StripV]=stripV_layout;
  m_channel_layout[L0Muon::MonUtilities::StripH]=stripH_layout;


}
