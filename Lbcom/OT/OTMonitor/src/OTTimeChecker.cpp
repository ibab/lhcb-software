// $Id: OTTimeChecker.cpp,v 1.9 2006-03-30 21:51:27 janos Exp $
// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IHistogramSvc.h"

// Linker
#include "Linker/LinkerTool.h"

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

// MathCore
#include "Kernel/SystemOfUnits.h"

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
  OTMonitorAlgorithm(name, pSvcLocator)
{
  // constructor
  this->declareProperty("doMomentumCut", m_doMomentumCut = false);
  this->declareProperty("momentumCut", m_momentumCut = 1500.*MeV);
}

OTTimeChecker::~OTTimeChecker()
{
  // destructor
}

StatusCode OTTimeChecker::initialize()
{
  // intialize histos
  this->initHistograms();

  // Get OT Geometry from XML
  m_tracker = getDet<DeOTDetector>(DeOTDetectorLocation::Default );
  
  return StatusCode::SUCCESS;
}


StatusCode OTTimeChecker::execute()
{
  
  // retrieve times
  OTTimes* timeCont = get<OTTimes>( OTTimeLocation::Default );

  // retrieve hits
  MCHits* mcHitCont = get<MCHits>( MCHitLocation::OT );

  // Get linker table
  typedef LinkerTool<LHCb::OTTime, LHCb::MCHit> OTTime2MCHitAsct;
  typedef OTTime2MCHitAsct::DirectType Table;
  typedef Table::Range Range;
  typedef Table::iterator iterator;

  OTTime2MCHitAsct associator( evtSvc(), OTTimeLocation::Default+"2MCHits" );
  const Table* aTable = associator.direct();
  if (!aTable) return Error( "Failed to find table", StatusCode::FAILURE );
  
  // Calculate efficiencies from MCHits to OTTimes
  // loop over OTTimes, find the MCHit and store multiplicity
  HitMultVec hitMultCont;
  PartMultVec partMultCont;
  int nGhosts = 0;  
  OTTimes::const_iterator iterTime;
  for (iterTime = timeCont->begin(); iterTime != timeCont->end(); ++iterTime) {
    // get MC truth for this time
    Range range = aTable->relations(*iterTime);
    if (range.empty()) {
      nGhosts++;
      continue;
    }

    // fill the non-ghosts in a temporary vector of MCHits
    std::vector<const MCHit*> hitVec;
    iterator iterHit;    
    for (iterHit = range.begin(); iterHit != range.end(); ++iterHit) {
      const MCHit* aHit = iterHit->to();
      if ( 0 == aHit ) continue;
      // Check if the hit is in the right spill
      if (mcHitCont != aHit->parent()) continue;
      hitVec.push_back( aHit );
    }
    if (0 == hitVec.size()) {
      nGhosts++;
      continue;
    }

    // loop over all MCHits in the temporary vector
    std::vector<const MCHit*>::iterator iHit;
    for (iHit = hitVec.begin(); iHit != hitVec.end(); ++iHit) {
      const MCHit* aHit = (*iHit);

      // Check if it is associated to an MCParticle
      const MCParticle* aPart = aHit->mcParticle();
      if (aPart != 0) {

        // calculate momentum
        Gaudi::XYZVector mom3 = (aPart->momentum()).Vect();
        double momentum = mom3.R();        

        // do not fill histograms if momentum is too low
        if (m_doMomentumCut && momentum < m_momentumCut) continue;
        
        // Fill momentum distribution histogram
        if ( fullDetail() ) {
          m_momentumHisto->fill( momentum / GeV, 1.0 );      
          
          // Determine PID code and fill PID histogram
          int myCode;
          switch ( (aPart->particleID()).abspid() ) {
          case 11: // electron
            myCode = 1;
            break; 
          case 13: // muon
            myCode = 2;
            break;
          case 211: // pion
            myCode = 3;
            break; 
          case 321: // kaon
            myCode = 4;
            break; 
          case 2212: // proton
            myCode = 5;
            break; 
          default:  // other
            myCode = 6;
            break;
          }
          m_pdgCodeHisto->fill( (float)myCode, 1.0 );
        
          // fill particle multiplicities container
          bool found = false ;
          PartMultVec::iterator iPart;
          for ( iPart = partMultCont.begin() ; 
                iPart != partMultCont.end() && !found; ++iPart) {
            if ( iPart->mcParticle == aPart ) {
              found = true;
              (iPart->mult)++;
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
      HitMultVec::iterator iHit;
      for ( iHit = hitMultCont.begin() ; 
            iHit != hitMultCont.end() && !found; ++iHit) {
        if ( iHit->mcHit == aHit ) {
          found = true;
          (iHit->mult)++;
        }
      }
      if (!found) {
        HitMultiplicity hitMult;
        hitMult.mult = 1;
        hitMult.mcHit = aHit;
        hitMultCont.push_back(hitMult);
      }
    } // loop iHit
    hitVec.clear();
  } // loop iterTime

  // loop over hit multiplicity container to count efficiency and fill histo's
  // fill efficiency histogram
  double singleHitEff = 0.0;
  double doubleHitEff = 0.0;
  double tripleHitEff = 0.0;
  double totalHitEff = 0.0;
  HitMultVec::const_iterator iHit;
  
  for (iHit = hitMultCont.begin(); iHit != hitMultCont.end(); ++iHit) {
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
    MCHits::const_iterator iterHit;
    for (iterHit = mcHitCont->begin(); iterHit != mcHitCont->end(); ++iterHit) {
      const MCParticle* part = (*iterHit)->mcParticle();
      if (part == 0) continue;
      Gaudi::XYZVector mom3 = ( part->momentum()).Vect();
      double mom = mom3.R();        
      if (mom >= m_momentumCut) numHits++;
    }
  }
  
  // fill the inefficient MCHits in the multiplicity container
  if ( fullDetail() ) {
    int numHitMult = hitMultCont.size();
    m_hitMultHisto->fill(0.0,(double) numHits - numHitMult);
  }  
  hitMultCont.erase(hitMultCont.begin(), hitMultCont.end());

  if ( 0 != numHits ) {
    totalHitEff /= (double) numHits;
    m_effHisto->fill(totalHitEff);
    if ( fullDetail() ) { 
      singleHitEff /= (double) numHits;
      doubleHitEff /= (double) numHits;
      tripleHitEff /= (double) numHits;
      m_eff1Histo->fill(singleHitEff);
      m_eff2Histo->fill(doubleHitEff);
      m_eff3Histo->fill(tripleHitEff);
    }
  }
    
  int nTimes = timeCont->size();
  if ( 0 != nTimes ) {    
    double ghostRate = (double) nGhosts/nTimes;
    m_ghostRateHisto->fill(ghostRate);
  }
  

  // loop over particle multiplicity container to fill histo
  if ( fullDetail() ) {
    PartMultVec::const_iterator iPart;
    for ( iPart = partMultCont.begin(); iPart != partMultCont.end() ; ++iPart) {
      m_partMultHisto->fill( (float) iPart->mult );
    }
    m_nParticlesHisto->fill( (float) partMultCont.size() );
    partMultCont.erase(partMultCont.begin(), partMultCont.end());
  }

  return StatusCode::SUCCESS;
}

StatusCode OTTimeChecker::initHistograms()
{
  // Intialize histograms
  std::string tDirPath = this->histoDirPath();

  m_effHisto = histoSvc()->book( tDirPath + "100",
                                 "efficiency for 1 hits or more",
                                 101, -0.005, 1.005);
  if ( fullDetail() ) {
    m_eff1Histo = histoSvc()->book( tDirPath + "101",
                                    "efficiency for a single hit",
                                    101, -0.005, 1.005);
    m_eff2Histo = histoSvc()->book( tDirPath + "102",
                                    "efficiency for a double hit",
                                    101, -0.005, 1.005);
    m_eff3Histo = histoSvc()->book( tDirPath + "103",
                                    "efficiency for 3 hits or more",
                                    101, -0.005, 1.005);
  }
  m_ghostRateHisto = histoSvc()->book( tDirPath + "110",
                                       "ghost rate",
                                       101, -0.005, 1.005);
  if ( fullDetail() ) {
    m_hitMultHisto = histoSvc()->book( tDirPath + "111",
                                       "number of times per mchit",
                                       11, -0.5, 10.5);
  }
  m_resHisto = histoSvc()->book( tDirPath + "120",
                                 "distance resolution (mm)",
                                 100, -4., 4.);
  if ( fullDetail() ) {
    m_pdgCodeHisto = histoSvc()->book( tDirPath + "201",
                                       "Particle type distribution",
                                       6, 0.5, 6.5);
    m_momentumHisto = histoSvc()->book( tDirPath + "202",
                                        "Momentum distribution (GeV)",
                                        100, 0.0, 40.);
    m_nParticlesHisto = histoSvc()->book( tDirPath + "203",
                                          "Number of MCParticles contributing " 
                                          "to OT times",
                                          41, -5.0, 405.);
    m_partMultHisto = histoSvc()->book( tDirPath + "204",
                                        "Number of times per MCParticle",
                                        101, -0.5, 100.5);
  }
 
 return StatusCode::SUCCESS;

}

StatusCode OTTimeChecker::fillResolutionHistos(OTTime* time, const MCHit* aHit) 
{
  // get the distance to the wire from MCHit
  const OTChannelID channel = time->channel();
  DeOTModule* module = m_tracker->findModule( channel );
  const Gaudi::XYZPoint entryP = aHit->entry();
  const Gaudi::XYZPoint exitP = aHit->exit();
  const Gaudi::XYZPoint middleP = entryP + (exitP - entryP)/2.;  
  double deltaZ = (exitP - entryP).z();
  if (0.0 == deltaZ) return StatusCode::SUCCESS; // curling track inside layer
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

  return StatusCode::SUCCESS;
}
