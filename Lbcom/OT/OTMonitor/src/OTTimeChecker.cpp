// $Id: OTTimeChecker.cpp,v 1.16 2010-01-12 14:13:11 akozlins Exp $

// local
#include "OTTimeChecker.h"

// Gaudi
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
#include "Event/OTLiteTime.h"

using namespace LHCb;

#include "boost/foreach.hpp"
using namespace boost;

/** @file OTTimeChecker.cpp 
 *
 *  Implementation of OTTimeChecker algorithm.
 *  
 *  @author J. van Tilburg and Jacopo Nardulli
 *  @date   22-06-2004
 */

DECLARE_ALGORITHM_FACTORY( OTTimeChecker )

OTTimeChecker::OTTimeChecker(const std::string& name, ISvcLocator* pSvcLocator)
  : GaudiHistoAlg(name, pSvcLocator)
  , m_decoder("OTRawBankDecoder")
  , m_tracker(0)
  , m_effHisto(0)
  , m_eff1Histo(0)
  , m_eff2Histo(0)
  , m_eff3Histo(0)
  , m_ghostRateHisto(0)
  , m_hitMultHisto(0)
  , m_resHisto(0)
  , m_pdgCodeHisto(0)
  , m_momentumHisto(0)
  , m_nParticlesHisto(0)
  , m_partMultHisto(0)
{
  // constructor
  declareProperty("doMomentumCut", m_doMomentumCut = false);
  declareProperty("momentumCut", m_momentumCut = 1.500 * Gaudi::Units::GeV);
}

OTTimeChecker::~OTTimeChecker()
{
  // destructor
}

StatusCode OTTimeChecker::initialize()
{
  StatusCode sc = GaudiHistoAlg::initialize();
  if(sc.isFailure()) return Error("Failed to initialize", sc);

  m_decoder.retrieve().ignore();

  // set path
  if("" == histoTopDir()) setHistoTopDir("OT/");

  // Get OT Geometry from XML
  m_tracker = getDet<DeOTDetector>(DeOTDetectorLocation::Default );
  
  // intialize histos
  initHistograms();
  
  return StatusCode::SUCCESS;
}

StatusCode OTTimeChecker::execute()
{
  // retrieve hits
  MCHits* mcHitCont = get<MCHits>( MCHitLocation::OT );

  // Get linker table
  LinkedTo<MCHit> myLink(evtSvc(), msgSvc(), OTTimeLocation::Default + "2MCHits");
  if (myLink.notFound()) Error( "Failed to find OT MCHits linker table", StatusCode::FAILURE );

  HitMultVec hitMultCont;
  PartMultVec partMultCont;

  int nGhosts = 0;
  int nTimes = 0;

  BOOST_FOREACH(const DeOTModule* module, m_tracker->modules())
  {
    OTChannelID moduleId = module->elementID();
    OTLiteTimeRange liteTimes = m_decoder->decodeModule(moduleId);
    nTimes += liteTimes.size();

    BOOST_FOREACH(const OTLiteTime& liteTime, liteTimes)
    {
      unsigned int key = (unsigned int)liteTime.channel();
      LinkedTo<MCHit>::LRange range = myLink.range(key);

      if(range.empty()) { nGhosts++; continue; }

      std::vector<const MCHit*> mcHitVec;
      BOOST_FOREACH(MCHit* hit, range)
      {
        if(mcHitCont != hit->parent()) continue;
        mcHitVec.push_back(hit);
      }

      if(mcHitVec.empty()) { nGhosts++; continue; }

      BOOST_FOREACH(const MCHit* hit, mcHitVec)
      {
        // Check if it is associated to an MCParticle
        const MCParticle* particle = hit->mcParticle();
        if(particle != 0)
        {
          // get momentum
          double mom = particle->p();

          // do not fill histograms if momentum is too low
          if (m_doMomentumCut && mom < m_momentumCut) continue;

          // Fill momentum distribution histogram
          if ( fullDetail() )
          {
            m_momentumHisto->fill(mom);

            m_pdgCodeHisto->fill(double(myParticleCode(particle)), 1.0 );

            // fill particle multiplicities container
            bool found = false;
            BOOST_FOREACH(PartMultiplicity& partMult, partMultCont)
            {
              if(partMult.mcParticle == particle) { found = true; partMult.mult++; break; }
            }

            if (!found)
            {
              PartMultiplicity partMult;
              partMult.mult = 1;
              partMult.mcParticle = particle;
              partMultCont.push_back(partMult);
            }
          } //if Full Detail
        } // if aPart

        // fill resolution histo's
        fillResolutionHistos(&liteTime, hit);

        // fill hit multiplicities container
        bool found = false;
        BOOST_FOREACH(HitMultiplicity& hitMult, hitMultCont)
        {
          if(hitMult.mcHit == hit) { found = true; hitMult.mult++; break; }
        }

        if(!found)
        {
          HitMultiplicity hitMult;
          hitMult.mult = 1;
          hitMult.mcHit = hit;
          hitMultCont.push_back(hitMult);
        }
      }
    } // foreach(liteTime)
  } // foreach(module)

  // loop over hit multiplicity container to count efficiency and fill histo's
  // fill efficiency histogram
  double singleHitEff = 0.0;
  double doubleHitEff = 0.0;
  double tripleHitEff = 0.0;
  double totalHitEff = 0.0;

  BOOST_FOREACH(HitMultiplicity& hitMult, hitMultCont)
  {
    if(hitMult.mult >= 1) totalHitEff += 1.0;
    if(fullDetail())
    {
      if (hitMult.mult == 1) singleHitEff += 1.0;
      if (hitMult.mult == 2) doubleHitEff += 1.0;
      if (hitMult.mult > 2) tripleHitEff += 1.0;
      m_hitMultHisto->fill(hitMult.mult);
    }
  }

  int numHits = mcHitCont->size();
  // count the number of hits with momentum greater than cut
  if (m_doMomentumCut)
  {
    numHits = 0;
    BOOST_FOREACH(MCHit* hit, *mcHitCont)
    {
      const MCParticle* mcPart = hit->mcParticle();
      if (mcPart == 0) continue;
      double mom = mcPart->p();
      if (mom > m_momentumCut) ++numHits;
    }
  }

  // fill the inefficient MCHits in the multiplicity container
  if ( fullDetail() )
  {
    int numHitMult = hitMultCont.size();
    m_hitMultHisto->fill(double(numHits - numHitMult));
  }

  if ( 0 != numHits )
  {
    totalHitEff /= double(numHits);
    m_effHisto->fill(totalHitEff);
    if ( fullDetail() )
    {
      singleHitEff /= double(numHits);
      doubleHitEff /= double(numHits);
      tripleHitEff /= double(numHits);
      m_eff1Histo->fill(singleHitEff);
      m_eff2Histo->fill(doubleHitEff);
      m_eff3Histo->fill(tripleHitEff);
    }
  }

  if (0 != nTimes)
  {
    double ghostRate = double(nGhosts)/double(nTimes);
    m_ghostRateHisto->fill(ghostRate);
  }

  // loop over particle multiplicity container to fill histo
  if ( fullDetail() )
  {
    BOOST_FOREACH(PartMultiplicity& partMult, partMultCont)
    {
      m_partMultHisto->fill( double(partMult.mult) );
    }
    m_nParticlesHisto->fill( double(partMultCont.size()) );
  }

  return StatusCode::SUCCESS;
}

void OTTimeChecker::initHistograms()
{
  // Intialize histograms
  m_effHisto = book(100, "Efficiency for 1 hits or more", -0.005, 1.005, 101);
  m_ghostRateHisto = book(110, "Ghost rate", -0.005, 1.005, 101);
  m_resHisto = book(120, "Drift distance resolution (mm)", -4., 4., 100);

  if ( fullDetail() )
  {
    m_eff1Histo = book(101, "Efficiency for a single hit", -0.005, 1.005, 101);
    m_eff2Histo = book(102, "Efficiency for a double hit", -0.005, 1.005, 101);
    m_eff3Histo = book(103, "Efficiency for 3 hits or more", -0.005, 1.005, 101);
    m_hitMultHisto = book(111, "Number of times per mchit", -0.5, 10.5, 11);
    m_pdgCodeHisto = book(201, "Particle type distribution", 0.5, 6.5, 6);
    m_momentumHisto = book(202, "Momentum distribution (GeV)", 0.0 * Gaudi::Units::GeV, 40.0 * Gaudi::Units::GeV, 100);
    m_nParticlesHisto = book(203, "Number of MCParticles contributing to OT times", -5.0, 405.0, 41);
    m_partMultHisto = book(204, "Number of times per MCParticle", -0.5, 100.5, 101);
  }
}

void OTTimeChecker::fillResolutionHistos(const OTLiteTime* time, const MCHit* aHit)
{
  // get the distance to the wire from MCHit
  const OTChannelID channel = time->channel();
  DeOTModule* module = m_tracker->findModule( channel );
  const Gaudi::XYZPoint entryP = aHit->entry();
  const Gaudi::XYZPoint exitP = aHit->exit();
  const Gaudi::XYZPoint middleP = aHit->midPoint();

  plot(time->calibratedTime(),"12","time spectrum",0., 100., 100);
 
  double deltaZ = (exitP - entryP).z();
  // 2.45 == cell radius
  if (!(deltaZ < 2.45))
  {
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
