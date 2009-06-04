// $Id: TrackEffChecker.cpp,v 1.14 2009-06-04 13:26:53 smenzeme Exp $
// Include files 
#include "TrackEffChecker.h"

//event
#include "Event/Track.h"
#include "Event/State.h"
#include "Event/GhostTrackInfo.h"

// Gaudi
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/ToStream.h"
#include "GaudiKernel/AlgFactory.h"

#include "Map.h"

#include "AIDA/IHistogram1D.h"
DECLARE_ALGORITHM_FACTORY( TrackEffChecker );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackEffChecker::TrackEffChecker(const std::string& name,
                           ISvcLocator* pSvcLocator ) :
TrackCheckerBase( name , pSvcLocator ){ 

  declareProperty( "OnlyBTracksInDenominator",
                   m_fromB = false  );
  declareProperty( "RequireLongTrack",
                   m_requireLong = false );

}

//=============================================================================
// Destructor
//=============================================================================
TrackEffChecker::~TrackEffChecker() {}; 

StatusCode TrackEffChecker::initialize()
{
  // Mandatory initialization of GaudiAlgorithm
  StatusCode sc = TrackCheckerBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Execute
//=============================================================================
StatusCode TrackEffChecker::execute()
{

  if (initializeEvent().isFailure()){
    return Error("Failed to initialize event", StatusCode::FAILURE);
  }

  // get the input data
  LHCb::Tracks* tracks = get<LHCb::Tracks>(inputContainer());
 
  

  LHCb::Tracks::const_iterator iterT = tracks->begin();
  for (; iterT != tracks->end(); ++iterT){  
    if ((*iterT)->type() != LHCb::Track::Long && m_requireLong )
      continue;
    counter("nTracksInThisEvent") += 1;
  }
  
  counter("nTrack") += counter("nTracksInThisEvent").flag();

  // we want to count ghosts etc
  ghostInfo(tracks);

  // then we want to look at efficiencies
  effInfo();


  counter("nTracksInThisEvent") = 0;

  return StatusCode::SUCCESS;
};

void TrackEffChecker::ghostInfo(const LHCb::Tracks* tracks) {

  unsigned int nGhost = 0;
  std::string type = "";
  LHCb::Tracks::const_iterator iterT = tracks->begin();
  for (; iterT != tracks->end(); ++iterT){
    
    if ((*iterT)->type() != LHCb::Track::Long && m_requireLong )
      continue;

    const LHCb::MCParticle* particle = mcTruth(*iterT);

    splitByAlgorithm() == true ? 
        type = Gaudi::Utils::toString((*iterT)->history()) : type= all(); 

    if (particle == 0){
      ++nGhost;
      if (fullDetail() == true) {
        plots(type+"/ghost",(*iterT));
        LHCb::GhostTrackInfo gInfo;
        ghostClassification()->info(**iterT,gInfo);
        const LHCb::GhostTrackInfo::Classification& gtype = gInfo.classification();
        ++counter(Gaudi::Utils::toString(gtype));
      }
    }
    else {
      if (fullDetail() == true) plots(type+"/real",(*iterT));
    }
  } // iterT

  // counter for ghost rate  
  counter("nGhost") += nGhost;
 
  // plot the event ghost rate
  if (counter("nTracksInThisEvent").flag() != 0)
    plot(nGhost/double(counter("nTracksInThisEvent").flag()),"ghost rate", -0.01, 1.01, 51);
  if (fullDetail() == true) {
    // ghost rate versus # interactions
    long nVert = visPrimVertTool()->countVertices();
    if (counter("nTracksInThisEvent").flag() != 0)
      plot2D(nVert,100*nGhost/double(counter("nTracksInThisEvent").flag()), "ghost rate vs visible",
	     -0.5,20.5, -0.5, 100.5,21, 101);
  }
}


void TrackEffChecker::effInfo(){

  unsigned int nToFind = 0u;
  unsigned int nFound = 0u;
  unsigned int nToFindG5 = 0u;
  unsigned int nFoundG5 = 0u;

  const LHCb::MCParticles* partCont = get<LHCb::MCParticles>(LHCb::MCParticleLocation::Default); 
  LHCb::MCParticles::const_iterator iterP = partCont->begin();
  for (; iterP != partCont->end(); ++iterP){

    bool reconstructible = false;
    
    if (m_tracksRefContainer != "") {
      TrackCheckerBase::LinkInfo info = reconstructedInRefContainer(*iterP);
      reconstructible = (info.track!=0 && info.track->type() == LHCb::Track::Long);
    } else
      reconstructible = selected(*iterP);
    
    if (m_fromB)
      reconstructible = reconstructible && bAncestor(*iterP);


    if (reconstructible == true){ 

      TrackCheckerBase::LinkInfo info = reconstructed(*iterP);

      


      plots("mcSelected", *iterP);
      ++nToFind;
      if ((*iterP)->p() > 5*Gaudi::Units::GeV)
	++nToFindG5;
      
      if (info.track != 0 && (info.track->type() == LHCb::Track::Long || !m_requireLong)) {
	++nFound;
	if ((*iterP)->p() > 5*Gaudi::Units::GeV)
	  ++nFoundG5;
	
	plots("selected",info.track);
	plots("mcSelectedAndRec", *iterP);
	
	plot(info.purity, "hitpurity",-0.01, 1.01, 51);
	if ((*iterP)->p() > 5*Gaudi::Units::GeV)
	  plot(info.purity, "hitpurityG5",-0.01, 1.01, 51);

	counter("nClone") += info.clone;
	if ((*iterP)->p() > 5*Gaudi::Units::GeV)
	  counter("nCloneG5") += info.clone;
      }
    }
    
  } // loop particles 

  // update counters 
  counter("nToFind") += nToFind;
  counter("nFound") += nFound;

  counter("nToFindG5") += nToFindG5;
  counter("nFoundG5") += nFoundG5;

  // event efficiency 
  if (nToFind !=0u)
    plot(nFound/double(nToFind),"eff", -0.01, 1.01, 51);
  if (nToFindG5 != 0u)
    plot(nFoundG5/double(nToFindG5),"effG5", -0.01, 1.01, 51);

  if (fullDetail() == true) {
    // ghost rate versus # interactions
    long nVert = visPrimVertTool()->countVertices();
    if (nToFind !=0u)
      plot2D(nVert,100*nFound/double(nToFind), "eff vs visible",
	     -0.5,20.5, -0.5, 100.5,21, 101);
  }
}


void TrackEffChecker::plots(const std::string& type, 
                            const LHCb::MCParticle* part) const{

  plot(part->p()/Gaudi::Units::GeV,type+"/p","p", 0., 50., 25);
  plot(part->pt()/Gaudi::Units::GeV,type+"/pt","pt", 0., 10., 20);
  plot(part->pseudoRapidity(),type+"/eta","eta",1., 6., 25);
}


void TrackEffChecker::plots(const std::string& type, 
                            const LHCb::Track* track ) const{

  //  const double nMeas = weightedMeasurementSum(track);
  
  if (!track->history() == LHCb::Track::PatVeloR && 
      !track->history() == LHCb::Track::PatVeloGeneric &&
      !track->history() == LHCb::Track::PatVeloGeneral &&
      !track->history() == LHCb::Track::PatVeloOpen){
    plot(track->pt()/Gaudi::Units::GeV,type+"/pt","pt", 0., 10., 100);
    plot(track->p()/Gaudi::Units::GeV,type+"/p","p", 0., 50., 25);
  }
  plot(track->chi2PerDoF(),type+"/chi2","chi2", 0., 500., 100);
  plot(track->probChi2(),type+"/probchi2","probChi2", 0., 1., 100);
  plot(track->pseudoRapidity(),type+"/eta" ,"eta" ,1., 6., 25);

  // information from the extra info list
  const LHCb::Track::ExtraInfo& info = track->extraInfo();
  LHCb::Track::ExtraInfo::const_iterator iterInfo = info.begin();
  for (;iterInfo != info.end(); ++iterInfo ){
    LHCb::Track::AdditionalInfo infoName = 
    LHCb::Track::AdditionalInfo(iterInfo->first);
    std::string title = Gaudi::Utils::toString(infoName);
    const TrackMaps::InfoHistMap& histMap = TrackMaps::infoHistDescription();
    TrackMaps::InfoHistMap::const_iterator iterM = histMap.find(infoName);
    if (iterM != histMap.end()) {
      const TrackMaps::HistoRange range = histMap.find(infoName)->second;
      plot(iterInfo->second,type+"/info/"+range.fid, title,range.fxMin ,range.fxMax , 100);
    }
  } // iterInfo

}


double TrackEffChecker::weightedMeasurementSum(const LHCb::Track* aTrack) const{ 

  double wSum = 0;
  const std::vector<LHCb::LHCbID>& ids = aTrack->lhcbIDs();
  for (std::vector<LHCb::LHCbID>::const_iterator iter = ids.begin(); iter != ids.end(); ++iter){
    if (iter->isVelo() == true){
      wSum += 1;
    }
    else if (iter->isST() == true) {
      wSum += 1;
    }
    else if (iter->isOT() == true) {
      wSum += 0.5;
    }
    else {
     // nothing
    }
  }
  return wSum;
}

StatusCode TrackEffChecker::finalize(){

  //ghost rate
  std::string histName = "ghost rate";
  AIDA::IHistogram1D* hist = histo1D(histName);
  double eGhost = 0;
  if (hist != 0) eGhost = hist->mean();
  const double tGhost = counter("nTrack").flag() == 0 ? 0.0 :
                        double(counter("nGhost").flag())
                        /double(counter("nTrack").flag());
  
  histName = "hitpurity";
  hist = histo1D(histName);
  double pur = 0;
  if (hist != 0) pur = hist->mean();

  histName = "hitpurityG5";
  hist = histo1D(histName);
  double purG5 = 0;
  if (hist != 0) purG5 = hist->mean();

  const double tEff = counter("nToFind").flag() == 0 ? 0.0 :
                      double(counter("nFound").flag())
                      /double(counter("nToFind").flag());

  const double tEffG5 = counter("nToFindG5").flag() == 0 ? 0.0 :
                        double(counter("nFoundG5").flag())
                      /double(counter("nToFindG5").flag());

  
  const double tCloneG5 = counter("nToFindG5").flag() == 0 ? 0.0 :
                          double(counter("nCloneG5").flag())
                      /double(counter("nToFindG5").flag());  

  const double tClone  = counter("nToFind").flag() == 0 ? 0.0 :
                         double(counter("nClone").flag())
                        /double(counter("nToFind").flag());
  
  info() << "             **************         " 
         << format( "       %8.0f tracks including %8.0f ghosts [%5.1f %%] Event average %5.1f  ****",
                    double(counter("nTrack").flag()), double(counter("nGhost").flag()), tGhost*100, eGhost*100) << endreq;

   info() << "     all  long     "
	  << format( " :     %8.0f from %8.0f [%5.1f %%]; %5.1f %% clones; %5.1f %% hit purity",
             double(counter("nFound").flag()), double(counter("nToFind").flag()), tEff*100, tClone*100, pur*100) << endreq;
  
  info() << "   long, p > 5 GeV "
         << format( " :     %8.0f from %8.0f [%5.1f %%]; %5.1f %% clones; %5.1f %% hit purity",
                      double(counter("nFoundG5").flag()), double(counter("nToFindG5").flag()), tEffG5*100, tCloneG5*100, purG5*100) << endreq;

  return TrackCheckerBase::finalize();
}
