// $Id: TrackEffChecker.cpp,v 1.22 2010-03-25 05:18:42 rlambert Exp $
// Include files
#include "TrackEffChecker.h"

//event
#include "Event/Track.h"
#include "Event/State.h"
#include "Event/GhostTrackInfo.h"

// Linker
#include "Linker/AllLinks.h"

// Gaudi
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/ToStream.h"

#include "Map.h"

#include "AIDA/IHistogram1D.h"
DECLARE_ALGORITHM_FACTORY( TrackEffChecker )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackEffChecker::TrackEffChecker(const std::string& name,
                                 ISvcLocator* pSvcLocator ) :
  TrackCheckerBase( name , pSvcLocator ){

  declareProperty( "RequireLongTrack",
                   m_requireLong = false );

}

//=============================================================================
// Destructor
//=============================================================================
TrackEffChecker::~TrackEffChecker() {}

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

  AllLinks<LHCb::MCParticle,ContainedObject> allIds( evtSvc(), msgSvc(), "Pat/LHCbID" );
  m_linkedIds.clear();
  LHCb::MCParticle* part = allIds.first();
  while ( NULL != part ) {
    unsigned int minSize = part->key();
    while ( m_linkedIds.size() <= minSize ) {
      std::vector<int> dum;
      m_linkedIds.push_back( dum );
    }
    m_linkedIds[part->key()].push_back( allIds.key() );
    part = allIds.next();
  }



  if (initializeEvent().isFailure()){
    return Error("Failed to initialize event", StatusCode::FAILURE);
  }

  // get the input data
  LHCb::Track::Range tracks = getIfExists<LHCb::Track::Range> ( inputContainer() );


  LHCb::Track::Range::const_iterator iterT = tracks.begin();
  for (; iterT != tracks.end(); ++iterT){
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
}

void TrackEffChecker::ghostInfo(const LHCb::Track::Range tracks) {

  unsigned int nGhost = 0;
  std::string type = "";
  LHCb::Track::Range::const_iterator iterT = tracks.begin();
  for (; iterT != tracks.end(); ++iterT){

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
        StatusCode sc = ghostClassification()->info(**iterT,gInfo);
        if (sc.isSuccess()) {
          const LHCb::GhostTrackInfo::Classification& gtype = gInfo.classification();
          plot(gtype,"ghost classification",-0.5, 30.5, 31);
          ++counter(Gaudi::Utils::toString(gtype));
        }
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

  double efficiency       = 0;
  double efficiencyG5     = 0;
  double efficiencyB      = 0;
  double efficiencyG5B    = 0;
  double efficiencyBAll   = 0;
  double efficiencyG5BAll = 0;
  double efficiencyKsL   = 0;
  double efficiencyG5KsL  = 0;
  double efficiencyIP   = 0;
  double efficiencyG5IP  = 0;

  unsigned int nToFind = 0u;
  unsigned int nFound = 0u;
  unsigned int nToFindG5 = 0u;
  unsigned int nFoundG5 = 0u;

  unsigned int nToFindB = 0u;
  unsigned int nFoundB = 0u;
  unsigned int nToFindG5B = 0u;
  unsigned int nFoundG5B = 0u;

  unsigned int nToFindBAll = 0u;
  unsigned int nFoundBAll = 0u;
  unsigned int nToFindG5BAll = 0u;
  unsigned int nFoundG5BAll = 0u;

  unsigned int nToFindKsL = 0u;
  unsigned int nFoundKsL = 0u;
  unsigned int nToFindG5KsL = 0u;
  unsigned int nFoundG5KsL = 0u;

  unsigned int nToFindIP = 0u;
  unsigned int nFoundIP  = 0u;
  unsigned int nToFindG5IP = 0u;
  unsigned int nFoundG5IP  = 0u;


  const LHCb::MCParticles* partCont = get<LHCb::MCParticles>(LHCb::MCParticleLocation::Default);
  LHCb::MCParticles::const_iterator iterP = partCont->begin();
  for (; iterP != partCont->end(); ++iterP){

    bool reconstructible = false;
    bool reconstructibleB = false;
    bool reconstructibleBAll = false;
    bool reconstructibleKsL = false;
    bool largeIP = false;

    if (m_tracksRefContainer != "") {
      TrackCheckerBase::LinkInfo info = reconstructedInRefContainer(*iterP);
      reconstructible = (info.track!=0 && (info.track->type() == LHCb::Track::Long || !m_requireLong));
    } else
      reconstructible = selected(*iterP);

    reconstructibleBAll = reconstructible && bAncestorWithReconstructibleDaughters(*iterP);

    reconstructibleB = reconstructible && bAncestor(*iterP);

    reconstructibleKsL = reconstructible && ksLambdaAncestor(*iterP);

    if (reconstructible == true){

      double x  = (*iterP)->originVertex()->position4vector().x();
      double y  = (*iterP)->originVertex()->position4vector().y();
      double tx = (*iterP)->momentum().x() / (*iterP)->momentum().z();
      double ty = (*iterP)->momentum().y() / (*iterP)->momentum().z();

      double IP2 = (x*x + y*y) - (tx*x + ty*y)*(tx*x + ty*y) / (tx*tx + ty*ty );
      largeIP = reconstructibleB && IP2 > 1. && IP2 < 4.;  // 1 to 2 mm

      double nTrue = 0;

      TrackCheckerBase::LinkInfo info = reconstructed(*iterP);

      if (info.track !=0) {

        std::vector<LHCb::LHCbID> ids;
        if ( m_linkedIds.size() > (unsigned int) (*iterP)->key() ) {
          for ( std::vector<int>::const_iterator itIm = m_linkedIds[(*iterP)->key()].begin();
                m_linkedIds[(*iterP)->key()].end() != itIm; ++itIm ) {
            LHCb::LHCbID temp;
            temp.setDetectorType( (*itIm) >> 28 );  // create LHCbId from int. Clumsy !
            temp.setID( (*itIm) );
            ids.push_back( temp );
          }
        }

        for ( std::vector<LHCb::LHCbID>::const_iterator itId = ids.begin();
              ids.end() != itId; ++itId ) {

          if ( (*itId).isVelo() ) {
            if ( info.track->type() == LHCb::Track::Ttrack || info.track->type() == LHCb::Track::Downstream ) continue;
            bool isPhi = (*itId).veloID().isPhiType();
            if ( isPhi && info.track->type() == LHCb::Track::VeloR ) continue;
            nTrue += 1.;
          } else if ( (*itId).isVP() ) {
            if ( info.track->type() == LHCb::Track::Ttrack || info.track->type() == LHCb::Track::Downstream ) continue;
            nTrue += 1.;
          } else if ( (*itId).isOT() ) {
            if ( info.track->type() != LHCb::Track::Upstream &&
                 info.track->type() != LHCb::Track::VeloR &&
                 info.track->type() != LHCb::Track::Velo ) nTrue += 1.;
          } else if ( (*itId).isST() ) {
            if ( (*itId).stID().isTT() ) {
              if ( info.track->type() != LHCb::Track::Ttrack &&
                   info.track->type() != LHCb::Track::VeloR &&
                   info.track->type() != LHCb::Track::Velo ) nTrue += 1.;
            } else {
              if ( info.track->type() != LHCb::Track::Upstream &&
                   info.track->type() != LHCb::Track::VeloR &&
                   info.track->type() != LHCb::Track::Velo ) nTrue += 1.;
            }
          }
        }
      }


      plots("mcSelected", *iterP);
      ++nToFind;
      if ((*iterP)->p() > 5*Gaudi::Units::GeV)
        ++nToFindG5;

      if (reconstructibleB){
        ++nToFindB;
        if ((*iterP)->p() > 5*Gaudi::Units::GeV)
          ++nToFindG5B;
      }

      if (reconstructibleBAll){
        ++nToFindBAll;
        if ((*iterP)->p() > 5*Gaudi::Units::GeV)
          ++nToFindG5BAll;
      }

      if (reconstructibleKsL) {
        ++nToFindKsL;
        if ((*iterP)->p() > 5*Gaudi::Units::GeV)
          ++nToFindG5KsL;
      }

      if (largeIP) {
        ++nToFindIP;
        if ((*iterP)->p() > 5*Gaudi::Units::GeV)
          ++nToFindG5IP;
      }

      if (info.track != 0 && (info.track->type() == LHCb::Track::Long || !m_requireLong)) {
        ++nFound;
        efficiency += info.purity * info.track->lhcbIDs().size()/nTrue;

        if ((*iterP)->p() > 5*Gaudi::Units::GeV){
          ++nFoundG5;
          efficiencyG5 += info.purity * info.track->lhcbIDs().size()/nTrue;;
        }


        plots("selected",info.track);
        plots("mcSelectedAndRec", *iterP);

        plot(info.purity, "hitpurity",-0.01, 1.01, 51);
        if ((*iterP)->p() > 5*Gaudi::Units::GeV)
          plot(info.purity, "hitpurityG5",-0.01, 1.01, 51);

        counter("nClone") += info.clone;
        if ((*iterP)->p() > 5*Gaudi::Units::GeV)
          counter("nCloneG5") += info.clone;

        if (reconstructibleB){

          if (info.track != 0 && (info.track->type() == LHCb::Track::Long || !m_requireLong)) {
            ++nFoundB;
            efficiencyB += info.purity * info.track->lhcbIDs().size()/nTrue;
            if ((*iterP)->p() > 5*Gaudi::Units::GeV){
              ++nFoundG5B;
              efficiencyG5B += info.purity * info.track->lhcbIDs().size()/nTrue;
            }

            plot(info.purity, "hitpurityB",-0.01, 1.01, 51);
            if ((*iterP)->p() > 5*Gaudi::Units::GeV)
              plot(info.purity, "hitpurityG5B",-0.01, 1.01, 51);

            counter("nCloneB") += info.clone;
            if ((*iterP)->p() > 5*Gaudi::Units::GeV)
              counter("nCloneG5B") += info.clone;

          }
        }

        if (reconstructibleBAll){

          if (info.track != 0 && (info.track->type() == LHCb::Track::Long || !m_requireLong)) {
            ++nFoundBAll;
            efficiencyBAll += info.purity * info.track->lhcbIDs().size()/nTrue;
            if ((*iterP)->p() > 5*Gaudi::Units::GeV){
              ++nFoundG5BAll;
              efficiencyG5BAll += info.purity * info.track->lhcbIDs().size()/nTrue;
            }

            plot(info.purity, "hitpurityBAll",-0.01, 1.01, 51);
            if ((*iterP)->p() > 5*Gaudi::Units::GeV)
              plot(info.purity, "hitpurityG5BAll",-0.01, 1.01, 51);

            counter("nCloneBAll") += info.clone;
            if ((*iterP)->p() > 5*Gaudi::Units::GeV)
              counter("nCloneG5BAll") += info.clone * info.track->lhcbIDs().size()/nTrue;
          }
        }

        if (reconstructibleKsL){

          if (info.track != 0 && (info.track->type() == LHCb::Track::Long || !m_requireLong)) {
            ++nFoundKsL;
            efficiencyKsL += info.purity * info.track->lhcbIDs().size()/nTrue;

            if ((*iterP)->p() > 5*Gaudi::Units::GeV){
              ++nFoundG5KsL;
              efficiencyG5KsL += info.purity * info.track->lhcbIDs().size()/nTrue;
            }

            plot(info.purity, "hitpurityKsL",-0.01, 1.01, 51);
            if ((*iterP)->p() > 5*Gaudi::Units::GeV)
              plot(info.purity, "hitpurityG5KsL",-0.01, 1.01, 51);

            counter("nCloneKsL") += info.clone;
            if ((*iterP)->p() > 5*Gaudi::Units::GeV)
              counter("nCloneG5KsL") += info.clone;

          }
        }

        if (largeIP) {

          if (info.track != 0 && (info.track->type() == LHCb::Track::Long || !m_requireLong)) {
            ++nFoundIP;
            efficiencyIP += info.purity * info.track->lhcbIDs().size()/nTrue;

            if ((*iterP)->p() > 5*Gaudi::Units::GeV){
              ++nFoundG5IP;
              efficiencyG5IP += info.purity * info.track->lhcbIDs().size()/nTrue;
            }

            plot(info.purity, "hitpurityIP",-0.01, 1.01, 51);
            if ((*iterP)->p() > 5*Gaudi::Units::GeV)
              plot(info.purity, "hitpurityG5IP",-0.01, 1.01, 51);

            counter("nCloneIP") += info.clone;
            if ((*iterP)->p() > 5*Gaudi::Units::GeV)
              counter("nCloneG5IP") += info.clone;

          }
        }
      }
    }
  } // loop particles

    // update counters
  counter("nToFind") += nToFind;
  counter("nFound") += nFound;
  counter("nToFindB") += nToFindB;
  counter("nFoundB") += nFoundB;
  counter("nToFindBAll") += nToFindBAll;
  counter("nFoundBAll") += nFoundBAll;
  counter("nToFindKsL") += nToFindKsL;
  counter("nFoundKsL") += nFoundKsL;
  counter("nToFindIP") += nToFindIP;
  counter("nFoundIP")  += nFoundIP;

  counter("nToFindG5") += nToFindG5;
  counter("nFoundG5") += nFoundG5;
  counter("nToFindG5B") += nToFindG5B;
  counter("nFoundG5B") += nFoundG5B;
  counter("nToFindG5BAll") += nToFindG5BAll;
  counter("nFoundG5BAll") += nFoundG5BAll;
  counter("nToFindG5KsL") += nToFindG5KsL;
  counter("nFoundG5KsL") += nFoundG5KsL;
  counter("nToFindG5IP") += nToFindG5IP;
  counter("nFoundG5IP")  += nFoundG5IP;

  counter("efficiency") += efficiency;
  counter("efficiencyG5") += efficiencyG5;
  counter("efficiencyB") += efficiencyB;
  counter("efficiencyG5B") += efficiencyG5B;
  counter("efficiencyBAll") += efficiencyBAll;
  counter("efficiencyG5BAll") += efficiencyG5BAll;
  counter("efficiencyKsL") += efficiencyKsL;
  counter("efficiencyG5KsL") += efficiencyG5KsL;
  counter("efficiencyIP") += efficiencyIP;
  counter("efficiencyG5IP") += efficiencyG5IP;


  // event efficiency
  if (nToFind !=0u)
    plot(nFound/double(nToFind),"eff", -0.01, 1.01, 51);
  if (nToFindG5 != 0u)
    plot(nFoundG5/double(nToFindG5),"effG5", -0.01, 1.01, 51);
  if (nToFindB !=0u)
    plot(nFoundB/double(nToFindB),"effB", -0.01, 1.01, 51);
  if (nToFindG5B != 0u)
    plot(nFoundG5B/double(nToFindG5B),"effG5B", -0.01, 1.01, 51);
  if (nToFindBAll !=0u)
    plot(nFoundBAll/double(nToFindBAll),"effBAll", -0.01, 1.01, 51);
  if (nToFindG5BAll != 0u)
    plot(nFoundG5BAll/double(nToFindG5BAll),"effG5BAll", -0.01, 1.01, 51);
  if (nToFindKsL !=0u)
    plot(nFoundKsL/double(nToFindKsL),"effKsL", -0.01, 1.01, 51);
  if (nToFindG5KsL != 0u)
    plot(nFoundG5KsL/double(nToFindG5KsL),"effG5KsL", -0.01, 1.01, 51);

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

  if (track->history() != LHCb::Track::PatVeloR &&
      track->history() != LHCb::Track::PatVeloGeneric &&
      track->history() != LHCb::Track::PatVeloGeneral &&
      track->history() != LHCb::Track::PatVPCheated &&
      track->type() != LHCb::Track::Velo &&
      track->type() != LHCb::Track::VeloR &&
      track->history() != LHCb::Track::PatVeloOpen){
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
    if (iter->isVP() == true){
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

  histName = "hitpurityB";
  hist = histo1D(histName);
  double purB = 0;
  if (hist != 0) purB = hist->mean();

  histName = "hitpurityG5B";
  hist = histo1D(histName);
  double purG5B = 0;
  if (hist != 0) purG5B = hist->mean();

  histName = "hitpurityBAll";
  hist = histo1D(histName);
  double purBAll = 0;
  if (hist != 0) purBAll = hist->mean();

  histName = "hitpurityG5BAll";
  hist = histo1D(histName);
  double purG5BAll = 0;
  if (hist != 0) purG5BAll = hist->mean();

  histName = "hitpurityKsL";
  hist = histo1D(histName);
  double purKsL = 0;
  if (hist != 0) purKsL = hist->mean();

  histName = "hitpurityG5KsL";
  hist = histo1D(histName);
  double purG5KsL = 0;
  if (hist != 0) purG5KsL = hist->mean();

  histName = "hitpurityIP";
  hist = histo1D(histName);
  double purIP = 0;
  if (hist != 0) purIP = hist->mean();

  histName = "hitpurityG5IP";
  hist = histo1D(histName);
  double purG5IP = 0;
  if (hist != 0) purG5IP = hist->mean();

  const double tEff = counter("nToFind").flag() == 0 ? 0.0 :
    double(counter("nFound").flag())
    /double(counter("nToFind").flag());

  const double tEffG5 = counter("nToFindG5").flag() == 0 ? 0.0 :
    double(counter("nFoundG5").flag())
    /double(counter("nToFindG5").flag());

  const double hEff = counter("nFound").flag() == 0 ? 0.0 :
    double(counter("efficiency").flag())
    /double(counter("nFound").flag());

  const double hEffG5 = counter("nFoundG5").flag() == 0 ? 0.0 :
    double(counter("efficiencyG5").flag())
    /double(counter("nFoundG5").flag());

  const double tCloneG5 = counter("nToFindG5").flag() == 0 ? 0.0 :
    double(counter("nCloneG5").flag())
    /double(counter("nToFindG5").flag());

  const double tClone  = counter("nToFind").flag() == 0 ? 0.0 :
    double(counter("nClone").flag())
    /double(counter("nToFind").flag());

  const double tEffB = counter("nToFindB").flag() == 0 ? 0.0 :
    double(counter("nFoundB").flag())
    /double(counter("nToFindB").flag());

  const double tEffG5B = counter("nToFindG5B").flag() == 0 ? 0.0 :
    double(counter("nFoundG5B").flag())
    /double(counter("nToFindG5B").flag());

  const double hEffB = counter("nFoundB").flag() == 0 ? 0.0 :
    double(counter("efficiencyB").flag())
    /double(counter("nFoundB").flag());

  const double hEffG5B = counter("nFoundG5B").flag() == 0 ? 0.0 :
    double(counter("efficiencyG5B").flag())
    /double(counter("nFoundG5B").flag());

  const double tCloneG5B = counter("nToFindG5B").flag() == 0 ? 0.0 :
    double(counter("nCloneG5B").flag())
    /double(counter("nToFindG5B").flag());

  const double tCloneB  = counter("nToFindB").flag() == 0 ? 0.0 :
    double(counter("nCloneB").flag())
    /double(counter("nToFindB").flag());

  const double tEffBAll = counter("nToFindBAll").flag() == 0 ? 0.0 :
    double(counter("nFoundBAll").flag())
    /double(counter("nToFindBAll").flag());

  const double tEffG5BAll = counter("nToFindG5BAll").flag() == 0 ? 0.0 :
    double(counter("nFoundG5BAll").flag())
    /double(counter("nToFindG5BAll").flag());

  const double hEffBAll =  counter("nFoundBAll").flag() == 0 ? 0.0 :
    double(counter("efficiencyBAll").flag())
    /double(counter("nFoundBAll").flag());

  const double hEffG5BAll = counter("nFoundG5BAll").flag() == 0 ? 0.0 :
    double(counter("efficiencyG5BAll").flag())
    /double(counter("nFoundG5BAll").flag());

  const double tCloneG5BAll = counter("nToFindG5BAll").flag() == 0 ? 0.0 :
    double(counter("nCloneG5BAll").flag())
    /double(counter("nToFindG5BAll").flag());

  const double tCloneBAll = counter("nToFindBAll").flag() == 0 ? 0.0 :
    double(counter("nCloneBAll").flag())
    /double(counter("nToFindBAll").flag());

  const double tEffKsL = counter("nToFindKsL").flag() == 0 ? 0.0 :
    double(counter("nFoundKsL").flag())
    /double(counter("nToFindKsL").flag());

  const double tEffG5KsL = counter("nToFindG5KsL").flag() == 0 ? 0.0 :
    double(counter("nFoundG5KsL").flag())
    /double(counter("nToFindG5KsL").flag());

  const double hEffKsL = counter("nFoundKsL").flag() == 0 ? 0.0 :
    double(counter("efficiencyKsL").flag())
    /double(counter("nFoundKsL").flag());

  const double hEffG5KsL = counter("nFoundG5KsL").flag() == 0 ? 0.0 :
    double(counter("efficiencyG5KsL").flag())
    /double(counter("nFoundG5KsL").flag());

  const double tCloneG5KsL = counter("nToFindG5KsL").flag() == 0 ? 0.0 :
    double(counter("nCloneG5KsL").flag())
    /double(counter("nToFindG5KsL").flag());

  const double tCloneKsL  = counter("nToFindKsL").flag() == 0 ? 0.0 :
    double(counter("nCloneKsL").flag())
    /double(counter("nToFindKsL").flag());


  const double tEffIP = counter("nToFindIP").flag() == 0 ? 0.0 :
    double(counter("nFoundIP").flag())
    /double(counter("nToFindIP").flag());

  const double tEffG5IP = counter("nToFindG5IP").flag() == 0 ? 0.0 :
    double(counter("nFoundG5IP").flag())
    /double(counter("nToFindG5IP").flag());

  const double hEffIP = counter("nFoundIP").flag() == 0 ? 0.0 :
    double(counter("efficiencyIP").flag())
    /double(counter("nFoundIP").flag());

  const double hEffG5IP = counter("nFoundG5IP").flag() == 0 ? 0.0 :
    double(counter("efficiencyG5IP").flag())
    /double(counter("nFoundG5IP").flag());

  const double tCloneG5IP = counter("nToFindG5IP").flag() == 0 ? 0.0 :
    double(counter("nCloneG5IP").flag())
    /double(counter("nToFindG5IP").flag());

  const double tCloneIP  = counter("nToFindIP").flag() == 0 ? 0.0 :
    double(counter("nCloneIP").flag())
    /double(counter("nToFindIP").flag());






  info() << "             **************            "
         << format( "       %8.0f tracks including %8.0f ghosts [%5.1f %%] Event average %5.1f  ****",
                    double(counter("nTrack").flag()), double(counter("nGhost").flag()), tGhost*100, eGhost*100) << endmsg;

  info() << "     all  long                         "
         << format( " :     %8.0f from %8.0f [%5.1f %%]; %5.1f %% clones; %5.1f %% hit purity; %5.1f %% hit efficiency",
                    double(counter("nFound").flag()), double(counter("nToFind").flag()), 
                    tEff*100, tClone*100, pur*100, hEff*100 ) << endmsg;

  info() << "     long, p > 5 GeV                   "
         << format( " :     %8.0f from %8.0f [%5.1f %%]; %5.1f %% clones; %5.1f %% hit purity; %5.1f %% hit efficiecy",
                    double(counter("nFoundG5").flag()), double(counter("nToFindG5").flag()), 
                    tEffG5*100, tCloneG5*100, purG5*100, hEffG5*100) << endmsg;

  if (counter("nToFindB").flag() > 0){
    info() << "     all  long B daughers              "
           << format( " :     %8.0f from %8.0f [%5.1f %%]; %5.1f %% clones; %5.1f %% hit purity; %5.1f %% hit efficiency",
                      double(counter("nFoundB").flag()), double(counter("nToFindB").flag()), 
                      tEffB*100, tCloneB*100, purB*100, hEffB*100) << endmsg;

    info() << "   long B daughters, p > 5 GeV         "
           << format( " :     %8.0f from %8.0f [%5.1f %%]; %5.1f %% clones; %5.1f %% hit purity; %5.1f %% hit efficiency",
                      double(counter("nFoundG5B").flag()), double(counter("nToFindG5B").flag()), 
                      tEffG5B*100, tCloneG5B*100, purG5B*100, hEffG5B*100) << endmsg;
  }

  if (counter("nToFindBAll").flag() > 0){
    info() << "     all  long good B daughers         "
           << format( " :     %8.0f from %8.0f [%5.1f %%]; %5.1f %% clones; %5.1f %% hit purity; %5.1f %% hit efficiency",
                      double(counter("nFoundBAll").flag()), double(counter("nToFindBAll").flag()), 
                      tEffBAll*100, tCloneBAll*100, purBAll*100, hEffBAll*100) << endmsg;

    info() << "   long good B daughters , p > 5 GeV   "
           << format( " :     %8.0f from %8.0f [%5.1f %%]; %5.1f %% clones; %5.1f %% hit purity; %5.1f %% hit efficiency",
                      double(counter("nFoundG5BAll").flag()), double(counter("nToFindG5BAll").flag()), 
                      tEffG5BAll*100, tCloneG5BAll*100, purG5BAll*100, hEffG5BAll*100) << endmsg;
  }

  if (counter ("nToFindKsL").flag() > 0) {
    info() << "     all  long  Ks/Lambda              "
           << format( " :     %8.0f from %8.0f [%5.1f %%]; %5.1f %% clones; %5.1f %% hit purity; %5.1f %% hit efficiency",
                      double(counter("nFoundKsL").flag()), double(counter("nToFindKsL").flag()), 
                      tEffKsL*100, tCloneKsL*100, purKsL*100, hEffKsL*100) << endmsg;

    info() << "   long Ks/Lambda, p > 5 GeV           "
           << format( " :     %8.0f from %8.0f [%5.1f %%]; %5.1f %% clones; %5.1f %% hit purity; %5.1f %% hit efficiency",
                      double(counter("nFoundG5KsL").flag()), double(counter("nToFindG5KsL").flag()),
                      tEffG5KsL*100, tCloneG5KsL*100, purG5KsL*100, hEffG5KsL*100) << endmsg;
  }

  if (counter ("nToFindIP").flag() > 0) {
    info() << "     all  long large IP                "
           << format( " :     %8.0f from %8.0f [%5.1f %%]; %5.1f %% clones; %5.1f %% hit purity; %5.1f %% hit efficiency",
                      double(counter("nFoundIP").flag()), double(counter("nToFindIP").flag()), 
                      tEffIP*100, tCloneIP*100, purIP*100, hEffIP*100) << endmsg;

    info() << "   long largeIP, p > 5 GeV             "
           << format( " :     %8.0f from %8.0f [%5.1f %%]; %5.1f %% clones; %5.1f %% hit purity; %5.1f %% hit efficiency",
                      double(counter("nFoundG5IP").flag()), double(counter("nToFindG5IP").flag()), 
                      tEffG5IP*100, tCloneG5IP*100, purG5IP*100, hEffG5IP*100) << endmsg;
  }


  return TrackCheckerBase::finalize();
}
