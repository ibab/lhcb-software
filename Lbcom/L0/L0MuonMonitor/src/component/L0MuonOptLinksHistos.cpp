// Include files 

#include "Event/RecHeader.h"
#include "Event/ODIN.h"

// local
#include "L0MuonOptLinksHistos.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0MuonOptLinksHistos
//
// 2008-04-07 : 
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( L0MuonOptLinksHistos )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0MuonOptLinksHistos::L0MuonOptLinksHistos( const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent )
  : GaudiHistoTool ( type, name , parent )
{
  declareInterface<L0MuonOptLinksHistos>(this);

  setLayouts();
  
  m_hevt=NULL;
  m_hmultitot=NULL;
  for (int qua=0;qua<4;++qua){
    m_hmulti[qua]=NULL;
    for (int sta=0;sta<5;++sta){
      for (int reg=0; reg<4; ++reg){  
        m_hist[qua][sta][reg]=NULL;
        m_hist2D[qua][sta][reg]=NULL;
      }
    } 
  }
  
}
//=============================================================================
// Destructor
//=============================================================================
L0MuonOptLinksHistos::~L0MuonOptLinksHistos() {} 

//=============================================================================

void L0MuonOptLinksHistos::bookHistos(bool shortname)
{
  for (int qua=0;qua<4;++qua){
    for (int reg=0; reg<4; ++reg){  
      for (int sta=0;sta<5;++sta){
        bookHistos(qua, reg, sta,shortname);
      }
    }
  } 
}
void L0MuonOptLinksHistos::bookMultiHistos(bool shortname)
{
  std::string toolname="";
  if (!shortname) toolname=name();
  std::string hname;

  hname= L0Muon::MonUtilities::hname_optlinks_evt(toolname);
  m_hevt=book1D(hname,hname,-0.5,65535.5,1024);
  hname= L0Muon::MonUtilities::hname_optlinks_multitot(toolname);
  m_hmultitot=book1D(hname,hname,-0.5,100.5,101);
  for (int qua=0;qua<4;++qua){
    hname= L0Muon::MonUtilities::hname_optlinks_multi(qua,toolname);
    m_hmulti[qua]=book1D(hname,hname,-0.5,25.5,26);
  } 
}

void L0MuonOptLinksHistos::bookHistos(int quarter, int region, int station, bool shortname)
{
  std::string hname;
  
  std::string toolname="";
  if (!shortname) toolname=name();

  MuonLayout lay = m_optlink_layout.stationLayout(station).regionLayout(region);

  int nbins=lay.xGrid()*lay.yGrid()*3;
  hname = L0Muon::MonUtilities::hname_optlinks_hist(quarter,station,region,toolname);
  AIDA::IHistogram1D * h = book1D(hname,hname,-0.5,nbins-0.5,nbins);
  m_hist[quarter][station][region]= h;
  
  int xbins=2*lay.xGrid();
  int ybins=2*lay.yGrid();
  hname=L0Muon::MonUtilities::hname_optlinks_hist2D(quarter,station,region,toolname);
  AIDA::IHistogram2D * h2D = book2D(hname,hname,-0.5,xbins-0.5,xbins,-0.5,ybins-0.5,ybins);
  m_hist2D[quarter][station][region]= h2D;

}

void L0MuonOptLinksHistos::fillHistos(const std::map<std::pair<LHCb::MuonTileID,int>, bool> &tiles)
{
  std::vector<LHCb::MuonTileID> vtiles;
  for (std::map<std::pair<LHCb::MuonTileID,int>, bool>::const_iterator it=tiles.begin(); it!=tiles.end();++it){
    vtiles.push_back((it->first).first);
  }
  fillHistos(vtiles);
}

void L0MuonOptLinksHistos::fillHistos(const std::vector<LHCb::MuonTileID> &tiles)
{

  std::vector<LHCb::MuonTileID>::const_iterator it_tiles;

  std::map<LHCb::MuonTileID,std::vector<LHCb::MuonTileID> > ols[4];
  std::map<LHCb::MuonTileID,std::vector<LHCb::MuonTileID> >::iterator it_ols;

  for (it_tiles=tiles.begin(); it_tiles!=tiles.end(); ++it_tiles) {
    int qua =(*it_tiles).quarter();
    int sta =(*it_tiles).station();
    LHCb::MuonTileID ol=m_optlink_layout.contains(*it_tiles);
    ol.setQuarter(qua);
    ol.setStation(sta);
    it_ols=ols[qua].find(ol);
    if (it_ols==ols[qua].end()) {
      std::vector<LHCb::MuonTileID> vide;
      ols[qua][ol]=vide;
    }
    ols[qua][ol].push_back(*it_tiles);
  }

  int ntot=0;
  
  for (int qua=0;qua<4;++qua){
    int n=ols[qua].size();
    ntot+=n;
    fill(m_hmulti[qua],n,1);
    for (it_ols=ols[qua].begin();it_ols!=ols[qua].end();++it_ols){
    
      LHCb::MuonTileID tile=it_ols->first;
      int sta = tile.station();
      int reg = tile.region();
      MuonLayout lay = m_optlink_layout.stationLayout(tile.station()).regionLayout(tile.region());
      LHCb::MuonTileID ol=tile.containerID(lay);
      if (m_hist[qua][sta][reg]==NULL) continue;
      
      int ind = tile.nX()+tile.nY()*lay.xGrid()-lay.xGrid()*lay.yGrid();
      
      fill(m_hist[qua][sta][reg],ind,1);
      fill(m_hist2D[qua][sta][reg],tile.nX(),tile.nY(),1);
    }
  }
  fill(m_hmultitot,ntot,1);

  static ulonglong ievt=0;
  if (exist<LHCb::ODIN> (LHCb::ODINLocation::Default)) {
    ievt = get<LHCb::ODIN>(LHCb::ODINLocation::Default)->eventNumber();
  } else if (exist<LHCb::RecHeader> (LHCb::RecHeaderLocation::Default)) {
    LHCb::RecHeader* evt = get<LHCb::RecHeader> (LHCb::RecHeaderLocation::Default);
    ievt = evt->evtNumber();
  } else {
    ++ievt;
  }
  fill(m_hevt,(double)(ievt&0xFFFF),ntot*1024./65536.);
  
}


void L0MuonOptLinksHistos::setLayouts()
{

  m_optlink_layout = MuonSystemLayout(MuonStationLayout(MuonLayout(2,4)),
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
