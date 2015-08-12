// Include files 
#include "TrackOccupChecker.h"

//event
#include "Event/Track.h"
#include "Event/State.h"

// Gaudi
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/ToStream.h"

#include "Map.h"



// from Event/DigiEvent
#include "Event/OTTime.h"
#include "Event/STCluster.h"
#include "Event/VeloCluster.h"

// from Tr/TrackFitEvent
#include "Event/STMeasurement.h"
#include "Event/OTMeasurement.h"
#include "Event/VeloRMeasurement.h"
#include "Event/VeloPhiMeasurement.h"


// Detector stuff
#include <OTDet/DeOTDetector.h>
#include <OTDet/DeOTModule.h>
#include <STDet/DeSTDetector.h>
#include <STDet/DeSTLayer.h>
#include <VeloDet/DeVelo.h>

// for histograms
#include "AIDA/IHistogram1D.h"
#include "GaudiAlg/GaudiHistoTool.h"
#include "GaudiAlg/IHistoTool.h"

// from Event/LinkerEvent
#include "Linker/LinkedTo.h"
#include "Linker/LinkedFrom.h"

using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( TrackOccupChecker )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackOccupChecker::TrackOccupChecker(const std::string& name,
				     ISvcLocator* pSvcLocator ) 
  : TrackCheckerBase( name , pSvcLocator )
  , m_tHitManager(0)
  , m_rHitManager(0)
  , m_phiHitManager(0)
  , m_Velo(0)
  , m_IT(0)
  , m_OT(0)
  , m_ITChannels(0)
  , m_OTChannels(0)
{
}

//=============================================================================
// Destructor
//=============================================================================
TrackOccupChecker::~TrackOccupChecker() 
{
}

StatusCode TrackOccupChecker::initialize()
{
  // Mandatory initialization of GaudiAlgorithm
  StatusCode sc = TrackCheckerBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // get necessary tools
  m_tHitManager = tool<Tf::TStationHitManager<PatForwardHit> >("PatTStationHitManager");
  m_rHitManager = tool<Tf::DefaultVeloRHitManager>("Tf::DefaultVeloRHitManager");
  m_phiHitManager = tool<Tf::DefaultVeloPhiHitManager>("Tf::DefaultVeloPhiHitManager");

  // get detector information locations
  m_Velo = getDet<DeVelo>(DeVeloLocation::Default);
  m_IT = getDet<DeSTDetector>(DeSTDetLocation::IT);
  m_OT = getDet<DeOTDetector>(DeOTDetectorLocation::Default);

  // Number of IT/OT channels
  m_ITChannels = m_IT->nStrip();
  m_OTChannels = m_OT->nChannels();
  debug() << "nChan IT: " << m_ITChannels << ", nChan OT: " << m_OTChannels << endmsg;

  return StatusCode::SUCCESS;

}



//=============================================================================
// Execute
//=============================================================================
StatusCode TrackOccupChecker::execute()
{

  if (initializeEvent().isFailure()){
    return Error("Failed to initialize event", StatusCode::FAILURE);
  }

  // access Velo
  long nstrips = 0;
  for (std::vector<DeVeloSensor*>::const_iterator it = m_Velo->sensorsBegin();
       it != m_Velo->sensorsEnd(); ++it){

    debug() << "Station: " << (*it)->station()
            << ", at z: " << (*it)->z()
            << ", R/Phi/PileUp/Dowstream: " 
            << (*it)->isR() << "/" << (*it)->isPhi() <<"/"<< (*it)->isPileUp() 
            <<"/"<< (*it)->isDownstream()
            << ", L/R: " << (*it)->isLeft() <<"/"<< (*it)->isRight()
            <<  ", zones: " <<  (*it)->numberOfZones() 
            <<  ", channels: " <<  (*it)->numberOfStrips() << endmsg;

    nstrips += (*it)->numberOfStrips();
  }
  
  debug() << "number of channels in Velo: " << nstrips << endmsg;


  // calculate the occupancies
  occupInfo();

  return StatusCode::SUCCESS;
}

void TrackOccupChecker::occupInfo() {

  int nVert = visPrimVertTool()->countVertices();

  LinkedTo<LHCb::MCParticle>
    otLink(evtSvc(),msgSvc(),LHCb::OTTimeLocation::Default);
  if( otLink.notFound() ) {
    error() << "Unable to retrieve OTCluster to MCParticle linker table."
            << endmsg;
  }


  // calculate occupancies in OT            - reg 0,1
  //                          IT left+right - reg 2,3
  //                          IT top+bottom - reg 4,5


  int nHitsOT         = 0;
  int nHitsOT0        = 0;
  int nHitsOT1        = 0;
  int nHitsOT2        = 0;

  int nHitsIThor      = 0;
  int nHitsITver      = 0;

  int nHitsIThor0      = 0;
  int nHitsIThor1      = 0;
  int nHitsIThor2      = 0;

  int nHitsITver0      = 0;
  int nHitsITver1      = 0;
  int nHitsITver2      = 0;

  int nHitsSpill      = 0;
  
  std::map<int, int> hitsInMod;
  std::map<int, int> hitsInModSpill;

  for (unsigned sta = 0; sta < m_nSta; ++sta) {
    for (unsigned lay = 0; lay < m_nLay; ++lay) {
      for (unsigned reg = 0; reg < m_nReg; ++reg) {
        
        HitRange range = m_tHitManager->hits(sta, lay, reg);

        if (isRegionOT(reg))    { 
          nHitsOT    += range.size(); 
          if (0==sta) nHitsOT0 += range.size(); 
          if (1==sta) nHitsOT1 += range.size(); 
          if (2==sta) nHitsOT2 += range.size(); 
    
          // for OT:
          // retrieve module (L, S1,2,3)
          
          for (HitRange::const_iterator ihit = range.begin();
               range.end() != ihit; ++ihit) {
            PatForwardHit* PFHit = *ihit;
            const Tf::OTHit* othit = PFHit->hit()->othit();
            if (0 != othit) { 
              unsigned int mod = othit->module().moduleID();
              hitsInMod[int(mod)]++;
	      plot(mod, "OTmoduleFromOTHit", 0, 10, 10); 
	      unsigned int tdct = othit->rawhit().channel().tdcTime();
	      plot (tdct, "tdcTimeFromOTHit", 0., 300., 300);
	      // get spillover hits
	      OTChannelID otID = othit->rawhit().channel();
	      MCParticle* mcp = otLink.first( otID );
	      if (0==mcp) {
		// spillover+crosstalk+noise
		plot (tdct, "spillover", 0., 300., 300);
		hitsInModSpill[int(mod)]++;
		//hitsVert[nVert]++; // global counter! see .h
		nHitsSpill++;
	      } else plot (tdct, "signal", 0., 300., 300);
	    } // 0!=othit
          } // HitRange
        } // OT region
        
        // retrieve number of strips in a IT cluster
        // if (isRegionIThor(reg)) { nHitsIThor += range.size(); }
        if (isRegionIThor(reg)) {
          for (HitRange::const_iterator ihit = range.begin();
               range.end() != ihit; ++ihit) {
            PatForwardHit* PFHit = *ihit;
            const Tf::STHit* sthit = PFHit->hit()->sthit();
            if (0 != sthit) {
              nHitsIThor += sthit->size();
              if (0==sta) nHitsIThor0 += sthit->size();
              if (1==sta) nHitsIThor1 += sthit->size();
              if (2==sta) nHitsIThor2 += sthit->size();
            }
          }
        }
        
        if (isRegionITver(reg)) {
          for (HitRange::const_iterator ihit = range.begin();
               range.end() != ihit; ++ihit) {
            PatForwardHit* PFHit = *ihit;
            const Tf::STHit* sthit = PFHit->hit()->sthit();
            if (0 != sthit) {
              nHitsITver += sthit->size();
              if (0==sta) nHitsITver0 += sthit->size();
              if (1==sta) nHitsITver1 += sthit->size();
              if (2==sta) nHitsITver2 += sthit->size();
            }
          }
        }
        
        // sta/lay/reg
        
      }
    }
  }
    
  debug() << "nHitsOT: " << nHitsOT << endmsg;
  
  plot2D(nVert,nHitsSpill/double(m_OTChannels), "occSpill_vs_visible",
         0., 11., 0., 1., 11, 100);

  for (int imod=1; imod<10; imod++){
    char name[20];
    sprintf(name, "occMod%i", imod);
    if (imod < 9) plot(hitsInMod[imod]/(128.*4*4*3),       std::string(name), 0., 1., 1000);  
    else          plot(hitsInMod[imod]/((128.+64.)*2*4*3), std::string(name), 0., 1., 1000); 
    sprintf(name, "occModSpill%i", imod);
    if (imod < 9) plot(hitsInModSpill[imod]/(128.*4*4*3),       std::string(name), 0., 1., 1000);  
    else          plot(hitsInModSpill[imod]/((128.+64.)*2*4*3), std::string(name), 0., 1., 1000);
  }
  

  
  plot(double(nHitsOT)/double(m_OTChannels),              "occupOT",          0., 1., 1000);
  plot(double(nHitsOT0)/(double(m_OTChannels)/3.),        "occupOT0",         0., 1., 1000);
  plot(double(nHitsOT1)/(double(m_OTChannels)/3.),        "occupOT1",         0., 1., 1000);
  plot(double(nHitsOT2)/(double(m_OTChannels)/3.),        "occupOT2",         0., 1., 1000);
  
  plot(double(nHitsIThor)/(double(m_ITChannels)/2.),       "occupIThor",       0., 1., 1000);
  plot(double(nHitsIThor0)/(double(m_ITChannels)/6.),      "occupIThor0",      0., 1., 1000);
  plot(double(nHitsIThor1)/(double(m_ITChannels)/6.),      "occupIThor1",      0., 1., 1000);
  plot(double(nHitsIThor2)/(double(m_ITChannels)/6.),      "occupIThor2",      0., 1., 1000);
  
  plot(double(nHitsITver)/(double(m_ITChannels)/2.),       "occupITver",       0., 1., 1000);
  plot(double(nHitsITver0)/(double(m_ITChannels)/6.),      "occupITver0",      0., 1., 1000);
  plot(double(nHitsITver1)/(double(m_ITChannels)/6.),      "occupITver1",      0., 1., 1000);
  plot(double(nHitsITver2)/(double(m_ITChannels)/6.),      "occupITver2",      0., 1., 1000);
  

  // VELO occupancies: R sensors
  std::map<int, int> RChannels;
  // access R hits
  for (unsigned int half=0; half < 2; ++half) { // loop over velo halfs
    Tf::DefaultVeloRHitManager::StationIterator si   =
      m_rHitManager->stationsHalfBegin(half);
    Tf::DefaultVeloRHitManager::StationIterator send =
      m_rHitManager->stationsHalfEnd(half);
    // station number
    int stNum = -1;
    for ( ; si != send; ++si) {
      stNum++;
      for (unsigned int zone=0; zone<4; ++zone) { // loop over inner/outer zones
        Tf::VeloRHitRange hits = (*si)->hits(zone);
        debug()<<"Retrieved hits:  "<<hits.size()
               <<" in station " << stNum <<endmsg;
        // loop over hits
        Tf::VeloRHitRange::const_iterator ihit = hits.begin();
        for ( ; ihit!=hits.end(); ++ihit) {
          Tf::VeloRHit *vrh = *ihit;
          plot(vrh->size(),"velo rhit pseudo size", 0, 5, 5);
          RChannels[stNum]+=vrh->size();
        } // ihits
      }
    }
  }
  
  
  std::map<int, int>::const_iterator itR = RChannels.begin();
  for ( ; itR!= RChannels.end(); ++itR){
    debug() << itR->first << " - " << itR->second << endmsg;
    char name[10];
    sprintf(name, "occVeloRSt%i", itR->first);
    plot(double(itR->second)/(2048*2),  std::string(name), 0., 1., 1000);
  }
  
} // occupInfo




StatusCode TrackOccupChecker::finalize(){

  AIDA::IHistogram1D* hist;
  std::string histName = "";
  
  // Occupancies
  // OT
  histName = "occupOT";
  hist = histo1D(histName);
  double occupOT = 0;
  if (hist != 0) occupOT = hist->mean();

  histName = "occupOT0";
  hist = histo1D(histName);
  double occupOT0 = 0;
  if (hist != 0) occupOT0 = hist->mean();
  
  histName = "occupOT1";
  hist = histo1D(histName);
  double occupOT1 = 0;
  if (hist != 0) occupOT1 = hist->mean();
  
  histName = "occupOT2";
  hist = histo1D(histName);
  double occupOT2 = 0;
  if (hist != 0) occupOT2 = hist->mean();
  
  // IT
  // horizontal boxes
  histName = "occupIThor";
  hist = histo1D(histName);
  double occupIThor = 0;
  if (hist != 0) occupIThor = hist->mean();

  // horizontal in stations

  histName = "occupIThor0";
  hist = histo1D(histName);
  double occupIThor0 = 0;
  if (hist != 0) occupIThor0 = hist->mean();

  histName = "occupIThor1";
  hist = histo1D(histName);
  double occupIThor1 = 0;
  if (hist != 0) occupIThor1 = hist->mean();

  histName = "occupIThor2";
  hist = histo1D(histName);
  double occupIThor2 = 0;
  if (hist != 0) occupIThor2 = hist->mean();

  // vertical boxes

  histName = "occupITver";
  hist = histo1D(histName);
  double occupITver = 0;
  if (hist != 0) occupITver = hist->mean();

  // vertical in stations

  histName = "occupITver0";
  hist = histo1D(histName);
  double occupITver0 = 0;
  if (hist != 0) occupITver0 = hist->mean();

  histName = "occupITver1";
  hist = histo1D(histName);
  double occupITver1 = 0;
  if (hist != 0) occupITver1 = hist->mean();

  histName = "occupITver2";
  hist = histo1D(histName);
  double occupITver2 = 0;
  if (hist != 0) occupITver2 = hist->mean();

  info() << " *** Occupancies, %, event-averaged: ";
  info() << format("OT: %.2f (T1: %.2f, T2: %.2f, T3: %.2f), IT (horizontal boxes): %.2f (T1: %.2f, T2: %.2f, T3: %.2f), IT (vertical boxes): %.2f (T1: %.2f, T2: %.2f, T3: %.2f)",
                    occupOT*100, occupOT0*100, occupOT1*100, occupOT2*100,
                    occupIThor*100, occupIThor0*100, occupIThor1*100, occupIThor2*100,
                    occupITver*100, occupITver0*100, occupITver1*100, occupITver2*100     ) << endmsg;

  return TrackCheckerBase::finalize();
}
