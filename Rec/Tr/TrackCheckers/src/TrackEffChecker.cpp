// $Id: TrackEffChecker.cpp,v 1.1 2007-06-27 15:05:06 mneedham Exp $
// Include files 
#include "TrackEffChecker.h"

//event
#include "Event/Track.h"
#include "Event/State.h"
#include "Event/GhostTrackInfo.h"

#include "GaudiKernel/PhysicalConstants.h"

// gsl
#include "gsl/gsl_math.h"

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
  counter("nTrack") += tracks->size();

  // we want to count ghosts etc
  ghostInfo(tracks);

  // then we want to look at efficiencies
  effInfo();

  return StatusCode::SUCCESS;
};

void TrackEffChecker::ghostInfo(const LHCb::Tracks* tracks) {

  unsigned int nGhost = 0;
  std::string type = "";
  LHCb::Tracks::const_iterator iterT = tracks->begin();
  for (; iterT != tracks->end(); ++iterT){
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
  plot(nGhost/double(tracks->size()),"ghost rate", -0.01, 1.01, 51);
  if (fullDetail() == true) {
    // ghost rate versus # interactions
    long nVert = visPrimVertTool()->countVertices();
    plot2D(nVert,100*nGhost/double(tracks->size()), "ghost rate vs visible",
           -0.5,20.5, -0.5, 100.5,21, 101);
  }
}


void TrackEffChecker::effInfo(){

  unsigned int nToFind = 0u;
  unsigned int nFound = 0u;
  const LHCb::MCParticles* partCont = get<LHCb::MCParticles>(LHCb::MCParticleLocation::Default); 
  LHCb::MCParticles::const_iterator iterP = partCont->begin();
  for (; iterP != partCont->end(); ++iterP){
     if (selected(*iterP) == true){
       ++nToFind;
       TrackCheckerBase::LinkInfo info = reconstructed(*iterP);
       if (info.track != 0) {
         ++nFound;
         counter("clone") += info.clone;
         plots("selected",info.track);
       }
    }
  } // loop particles 

  // update counters 
  counter("nToFind") += nToFind;
  counter("nFound") += nFound;

  // event efficiency 
  plot(nFound/double(nToFind),"eff", -0.01, 1.01, 51);
  if (fullDetail() == true) {
    // ghost rate versus # interactions
    long nVert = visPrimVertTool()->countVertices();
    plot2D(nVert,100*nFound/double(nToFind), "eff vs visible",
           -0.5,20.5, -0.5, 100.5,21, 101);
  }
}

void TrackEffChecker::plots(const std::string& type, 
                            const LHCb::Track* track ) const{

  const double nMeas = weightedMeasurementSum(track);
  plot(nMeas, type+"/1","meas", 0., 100., 100);
  plot(track->pt()/Gaudi::Units::GeV,type+"/2","pt", 0., 10., 100);
  plot(track->p()/Gaudi::Units::GeV,type+"/3","p", 0., 100., 100);
  plot(track->chi2PerDoF(),type+"/4","chi2", 0., 500., 100);
  plot(track->probChi2(),type+"/5","probChi2", 0., 1., 100);
  plot(track->pseudoRapidity(),type+"/6" ,"eta" ,1., 6., 100);

  // information from the extra info list
  const LHCb::Track::ExtraInfo& info = track->extraInfo();
  LHCb::Track::ExtraInfo::const_iterator iterInfo = info.begin();
  for (;iterInfo != info.end(); ++iterInfo ){
    LHCb::Track::AdditionalInfo infoName = 
    LHCb::Track::AdditionalInfo(iterInfo->first);
    std::string title = Gaudi::Utils::toString(infoName);
    const TrackMaps::InfoHistMap& histMap = TrackMaps::infoHistDescription();
    const TrackMaps::HistoRange range = histMap.find(infoName)->second;
    plot(iterInfo->second,type+"/info/"+range.fid, title,range.fxMin ,range.fxMax , 100);
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
  const double tGhost = double(counter("nGhost").flag())
                      /double(counter("nTrack").flag());
  
  // efficiency
  histName = "eff";
  hist = histo1D(histName);
  double eEff = 0;
  if (hist != 0) eEff = hist->mean();
  const double tEff = double(counter("nFound").flag())
                      /double(counter("nToFind").flag());

  info() << "***Ghost Rate:" << tGhost <<"(Track weighted) " << 
         eGhost << "(Event weighted)" << endmsg;

  info() << "***Efficiency:" << tEff <<"(Track weighted) " << 
            eEff << "(Event weighted)" << endmsg;

  return TrackCheckerBase::finalize();
}
