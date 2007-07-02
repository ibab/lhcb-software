// $Id: OTTimeChecker.cpp,v 1.15 2007-07-02 16:08:40 janos Exp $
// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

// Linker
#include "Linker/LinkedTo.h"

// AIDA
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

// OT geometry
#include "OTDet/DeOTDetector.h"
#include "OTDet/DeOTModule.h"

// MCEvent
#include "Event/MCHit.h"

// OTEvent
#include "Event/OTTime.h"

// local
#include "OTTimeChecker.h"

/** @file OTTimeChecker.cpp 
 *
 *  Implementation of OTTimeChecker algorithm.
 *  
 *  @author J. van Tilburg and Jacopo Nardulli
 *  @date   22-06-2004
 */

using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( OTTimeChecker );

OTTimeChecker::OTTimeChecker(const std::string& name, 
                              ISvcLocator* pSvcLocator) :
  GaudiHistoAlg(name, pSvcLocator) {
  // constructor
  declareProperty("doMomentumCut", m_doMomentumCut = false);
  declareProperty("momentumCut", m_momentumCut = 1.500*Gaudi::Units::GeV);
}

OTTimeChecker::~OTTimeChecker() {
  // destructor
}

StatusCode OTTimeChecker::initialize() {

  StatusCode sc = GaudiHistoAlg::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);
  
  // set path
  if("" == histoTopDir()) setHistoTopDir("OT/");

  // Get OT Geometry from XML
  m_tracker = getDet<DeOTDetector>(DeOTDetectorLocation::Default );
  
  // intialize histos
  this->initHistograms();
  
  return StatusCode::SUCCESS;
}

StatusCode OTTimeChecker::execute() {
  
  // retrieve times
  OTTimes* timeCont = get<OTTimes>( OTTimeLocation::Default );

  // retrieve hits
  MCHits* mcHitCont = get<MCHits>( MCHitLocation::OT );

  // Get linker table
  LinkedTo<LHCb::MCHit> myLink(evtSvc(), msgSvc(), LHCb::OTTimeLocation::Default+"2MCHits");
  if (myLink.notFound()) Error( "Failed to find OT MCHits linker table", StatusCode::FAILURE );

  // Calculate efficiencies from MCHits to OTTimes
  // loop over OTTimes, find the MCHit and store multiplicity
  HitMultVec hitMultCont;
  PartMultVec partMultCont;
  int nGhosts = 0;  
  OTTimes::const_iterator iterTime = timeCont->begin();
  for ( ; iterTime != timeCont->end(); ++iterTime) {
    unsigned int key = unsigned((*iterTime)->channel()); 
    std::vector<LHCb::MCHit*> range = myLink.range(key);
    // get MC truth for this time
    //Range range = aTable->relations(*iterTime);
    if (range.empty()) {
      ++nGhosts;
      continue;
    }

    // fill the non-ghosts in a temporary vector of MCHits
    std::vector<const MCHit*> mcHitVec;
    std::vector<LHCb::MCHit*>::const_iterator iH = range.begin();
    for ( ; iH != range.end(); ++iH) {
      if (mcHitCont != (*iH)->parent()) continue;
      mcHitVec.push_back( (*iH) );
    }
    if (mcHitVec.empty()) {
      ++nGhosts;
      continue;
    }

    // loop over all MCHits in the temporary vector
    std::vector<const MCHit*>::iterator iMCHit = mcHitVec.begin();
    for ( ; iMCHit != mcHitVec.end(); ++iMCHit) {
      const MCHit* aHit = (*iMCHit);

      // Check if it is associated to an MCParticle
      const MCParticle* aPart = aHit->mcParticle();
      if (aPart != 0) {
        // get momentum
        double mom = aPart->p();        

        // do not fill histograms if momentum is too low
        if (m_doMomentumCut && mom < m_momentumCut) continue;
        
        // Fill momentum distribution histogram
        if ( fullDetail() ) {
          m_momentumHisto->fill(mom);      
         
          m_pdgCodeHisto->fill(double(myParticleCode(aPart)), 1.0 );
        
          // fill particle multiplicities container
          bool found = false ;
          PartMultVec::iterator iPart = partMultCont.begin();
          for ( ; iPart != partMultCont.end() && !found; ++iPart) {
            if ( iPart->mcParticle == aPart ) {
              found = true;
              ++(iPart->mult);
            }
          }
          if (!found) {
            PartMultiplicity partMult;
            partMult.mult = 1;
            partMult.mcParticle = aPart;
            partMultCont.push_back(partMult);
          }
        }  //if Full Detail
      } // if aPart

      // fill resolution histo's
      fillResolutionHistos(*iterTime, aHit);

      // fill hit multiplicities container
      bool found = false ;
      HitMultVec::iterator iHit = hitMultCont.begin();
      for ( ; iHit != hitMultCont.end() && !found; ++iHit) {
        if ( iHit->mcHit == aHit ) {
          found = true;
          ++(iHit->mult);
        }
      }
      if (!found) {
        HitMultiplicity hitMult;
        hitMult.mult = 1;
        hitMult.mcHit = aHit;
        hitMultCont.push_back(hitMult);
      }
    } // loop iHit
    mcHitVec.clear();
  } // loop iterTime

  // loop over hit multiplicity container to count efficiency and fill histo's
  // fill efficiency histogram
  double singleHitEff = 0.0;
  double doubleHitEff = 0.0;
  double tripleHitEff = 0.0;
  double totalHitEff = 0.0;
  HitMultVec::const_iterator iHit = hitMultCont.begin();
  
  for ( ; iHit != hitMultCont.end(); ++iHit) {
    if ( iHit->mult >= 1) totalHitEff += 1.0;
    if ( fullDetail() ) {
      if ( iHit->mult == 1) singleHitEff += 1.0;
      if ( iHit->mult == 2) doubleHitEff += 1.0;
      if ( iHit->mult > 2) tripleHitEff += 1.0;
      m_hitMultHisto->fill(iHit->mult);
    } // Full Detail
  }

  int numHits = mcHitCont->size();
  // count the number of hits with momentum greater than cut
  if (m_doMomentumCut) {
    numHits = 0;
    MCHits::const_iterator iterHit = mcHitCont->begin();
    for ( ; iterHit != mcHitCont->end(); ++iterHit) {
      const MCParticle* mcPart = (*iterHit)->mcParticle();
      if (mcPart == 0) continue;
      double mom = mcPart->p();        
      if (mom > m_momentumCut) ++numHits;
    }
  }
  
  // fill the inefficient MCHits in the multiplicity container
  if ( fullDetail() ) {
    int numHitMult = hitMultCont.size();
    m_hitMultHisto->fill(double(numHits - numHitMult));
  }  
  hitMultCont.clear();

  if ( 0 != numHits ) {
    totalHitEff /= double(numHits);
    m_effHisto->fill(totalHitEff);
    if ( fullDetail() ) { 
      singleHitEff /= double(numHits);
      doubleHitEff /= double(numHits);
      tripleHitEff /= double(numHits);
      m_eff1Histo->fill(singleHitEff);
      m_eff2Histo->fill(doubleHitEff);
      m_eff3Histo->fill(tripleHitEff);
    }
  }
    
  int nTimes = timeCont->size();
  if ( 0 != nTimes ) {    
    double ghostRate = double(nGhosts)/double(nTimes);
    m_ghostRateHisto->fill(ghostRate);
  }
  
  // loop over particle multiplicity container to fill histo
  if ( fullDetail() ) {
    PartMultVec::const_iterator iPart = partMultCont.begin();
    for ( ; iPart != partMultCont.end() ; ++iPart) {
      m_partMultHisto->fill( double(iPart->mult) );
    }
    m_nParticlesHisto->fill( double(partMultCont.size()) );
    partMultCont.clear();
  }

  return StatusCode::SUCCESS;
}

void OTTimeChecker::initHistograms() {
  
  // Intialize histograms
  m_effHisto = book(100, "Efficiency for 1 hits or more", -0.005, 1.005, 101);
  m_ghostRateHisto = book(110, "Ghost rate", -0.005, 1.005, 101);
  m_resHisto = book(120, "Drift distance resolution (mm)", -4., 4., 100);
  
  if ( fullDetail() ) {
    m_eff1Histo = book(101, "Efficiency for a single hit", -0.005, 1.005, 101);
    m_eff2Histo = book(102, "Efficiency for a double hit", -0.005, 1.005, 101);
    m_eff3Histo = book(103, "Efficiency for 3 hits or more", -0.005, 1.005, 101);
    m_hitMultHisto = book(111, "Number of times per mchit", -0.5, 10.5, 11);
    m_pdgCodeHisto = book(201, "Particle type distribution", 0.5, 6.5, 6);
    m_momentumHisto = book(202, "Momentum distribution (GeV)", 0.0*Gaudi::Units::GeV, 
                           40.0*Gaudi::Units::GeV, 100);
    m_nParticlesHisto = book(203, "Number of MCParticles contributing to OT times",
			     -5.0, 405., 41);
    m_partMultHisto = book(204, "Number of times per MCParticle", -0.5, 100.5, 101);
  }
}

void OTTimeChecker::fillResolutionHistos(OTTime* time, const MCHit* aHit) {
  // get the distance to the wire from MCHit
  const OTChannelID channel = time->channel();
  DeOTModule* module = m_tracker->findModule( channel );
  const Gaudi::XYZPoint entryP = aHit->entry();
  const Gaudi::XYZPoint exitP = aHit->exit();
  const Gaudi::XYZPoint middleP = aHit->midPoint();  

  plot(time->calibratedTime(),"12","time spectrum",0., 100., 100);
 
  double deltaZ = (exitP - entryP).z();
  // 2.45 == cell radius
  if (!(deltaZ < 2.45)) {
    const double tx = (exitP - entryP).x() / deltaZ;
    const double ty = (exitP - entryP).y() / deltaZ;
    double mcDist = module->distanceToWire(channel.straw(), entryP, tx, ty);
    // get the distance to the wire from the measurement
    double driftTime = time->calibratedTime() - 
      m_tracker->propagationTime(channel, middleP.x(), middleP.y() );
    double measDist = m_tracker->driftDistance( driftTime );  
  
    // Get the cheated ambiguity
    int ambiguity = ((mcDist < 0.0) ? -1 : 1);

    m_resHisto->fill( ambiguity*measDist - mcDist );
  }
}
