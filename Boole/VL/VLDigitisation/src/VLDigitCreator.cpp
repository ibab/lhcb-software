// GSL
#include "gsl/gsl_sf_erf.h"
// Gaudi
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/AlgFactory.h"
// Kernel/LHCbMath
#include "Kernel/FPEGuard.h"
#include "LHCbMath/LHCbMath.h"
// Kernel/LHCbKernel
#include "Kernel/ISiAmplifierResponse.h"
#include "Kernel/VLDataFunctor.h"
#include "Kernel/VLChannelID.h"
// Event/MCEvent
#include "Event/MCHit.h"
#include "Event/MCParticle.h"
// Det/VLDet
#include "VLDet/DeVL.h"
// Local 
#include "VLThreshold.h"
#include "IStripNoiseTool.h"
#include "VLDigitCreator.h"

using namespace LHCb;

/** @file VLDigitCreator.cpp
 *
 *  Implementation of class : VLDigitCreator
 *
 */

DECLARE_ALGORITHM_FACTORY(VLDigitCreator)

//===========================================================================
/// Constructor
//===========================================================================
VLDigitCreator::VLDigitCreator(const std::string& name,
                               ISvcLocator* pSvcLocator) : 
    GaudiAlgorithm(name, pSvcLocator),
    m_det(0),
    m_threshold(0.),
    m_baseDiffuseSigma(0.) {
  
  declareProperty("HitLocations", m_hitLocations);
  declareProperty("TimeOffsets", m_timeOffsets);
  declareProperty("InputContainerToLink", m_MCHitContainerToLinkName);
  declareProperty("DigitLocation", 
                  m_digitLocation = MCVLDigitLocation::Default); 

  declareProperty("InhomogeneousCharge", m_inhomogeneousCharge = true);
  declareProperty("NoiseSim", m_noise = true);
  declareProperty("CapacitiveCoupling", m_coupling = 0.01);
  declareProperty("StripInefficiency", m_inefficiency = 0.0);

  declareProperty("Threshold", m_threshold = 1600.); 
  declareProperty("kT", m_kT = 0.025);
  declareProperty("BiasVoltage", m_biasVoltage = 150.);
  declareProperty("eVPerElectron", m_eVPerElectron = 3.6);
  declareProperty("PointsPerStrip", m_pointsPerStrip = 3);
  declareProperty("ChargeUniform", m_chargeUniform = 70.);
  declareProperty("DeltaRayMinEnergy", m_deltaRayMinEnergy = 1000.);

  declareProperty("NoiseScale", m_scaleNoise = 1.);
  declareProperty("FluctuationsScale", m_scaleFluctuations = 1.);

  declareProperty("PulseShapePeakTime", m_peakTime = 30.7848);
  declareProperty("OffPeakSamplingTime", m_offPeakSamplingTime = 0.);

  declareProperty("ElectronsPerADC", m_electronsPerAdc = 1600.);
  declareProperty("ADCRange", m_adcRange = 64); 

  Rndm::Numbers m_gauss;
  Rndm::Numbers m_uniform;

}

//===========================================================================
/// Initialisation. Check parameters
//===========================================================================
StatusCode VLDigitCreator::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;
  m_debug   = msgLevel(MSG::DEBUG);
  m_verbose = msgLevel(MSG::VERBOSE);
  if (m_debug) debug() << " ==> initialize()" << endmsg;

  if (m_hitLocations.size() != m_timeOffsets.size()) {
    error() << "Sizes of HitLocations and TimeOffsets do not match" << endmsg;
    return StatusCode::FAILURE;
  }

  // Get the detector element.
  m_det = getDet<DeVL>(DeVLLocation::Default);
  m_baseDiffuseSigma = sqrt(2 * m_kT / m_biasVoltage);

  // Get the front end time tool.
  m_SiTimeTool = tool<ISiAmplifierResponse>("SiAmplifierResponse", 
                                            "SiTimeTool", this);
  // Get the strip noise tool.
  m_StripNoiseTool = tool<IStripNoiseTool>("VLStripNoiseTool",
                                           "StripNoise", this);

  // Initialise the random number generators. 
  sc = m_gauss.initialize(randSvc(), Rndm::Gauss(0., 1.));
  if (!sc) {
    error() << "Cannot initialise Gaussian random number generator" << endmsg;
    return sc;
  }
  sc = m_uniform.initialize(randSvc(), Rndm::Flat(0., 1.));
  if (!sc) {
    error() << "Cannot initialise uniform random number generator" << endmsg;
    return sc;
  }
  // Estimate the number of R strips to add noise to.
  const DeVLRSensor* rSensor = m_det->rSensors().front();
  const unsigned int nStripsR = rSensor->numberOfStrips();
  // Calculate the average strip noise.
  double noiseR = m_StripNoiseTool->averageNoise(rSensor->sensorNumber());
  noiseR *= m_scaleNoise; 
  double noiseHitsR = 2. * erfcSafe(m_threshold / noiseR) * nStripsR;
  sc = m_poissonR.initialize(randSvc(), Rndm::Poisson(noiseHitsR));
  if (!sc) {
    error() << "Cannot initialise Poisson random number generator" << endmsg;
    return sc;
  }
  // Estimate the number of Phi strips to add noise to.
  const DeVLPhiSensor* phiSensor = m_det->phiSensors().front();
  const unsigned int nStripsPhi = phiSensor->numberOfStrips();
  double noisePhi = m_StripNoiseTool->averageNoise(phiSensor->sensorNumber());
  noisePhi *= m_scaleNoise; 
  double noiseHitsPhi = 2. * erfcSafe(m_threshold / noisePhi) * nStripsPhi;
  sc = m_poissonPhi.initialize(randSvc(), Rndm::Poisson(noiseHitsPhi));
  if (!sc) {
    error() << "Cannot initialise Poisson random number generator" << endmsg;
    return sc;
  }
  return StatusCode::SUCCESS;

};

//===========================================================================
/// Main execution
//===========================================================================
StatusCode VLDigitCreator::execute() {

  if (m_debug) debug() << " ==> execute()" << endmsg;
  // Get the container of MCHits to make MC linker table to.
  m_MCHitContainerToLink = get<MCHits>(m_MCHitContainerToLinkName);
  m_digits = new MCVLDigits();
  std::vector<std::string>::const_iterator it;
  for (it = m_hitLocations.begin(); it != m_hitLocations.end(); ++it) {
    if (!exist<MCHits>(*it)) {
      if (m_debug) debug() << "No MCHits in " << *it << endmsg;
      continue;
    }
    // Retrieve the MC hits.
    MCHits* hits = get<MCHits>(*it);
    if (m_debug) debug() << hits->size() << " hits in " << *it << endmsg;
    const unsigned int timeIndex = it - m_hitLocations.begin();
    const double timeOffset = m_timeOffsets[timeIndex];
    // Simulate signals in strips due to MC hits.
    simulateSignal(hits, timeOffset);
    // Simulate charge sharing due to capacitive coupling.
    if (m_coupling > 0.) simulateCoupling();
    // Add noise.
    if (m_noise) simulateNoise();
    // Convert charges to ADC counts.
    digitise();
    // Simulate signal loss due to dead strips.
    if (m_inefficiency > 0.) simulateInefficiency();
    // Remove digits below threshold.
    cleanup();
  }
  // Store the digits.
  if (exist<MCVLDigits>(m_digitLocation)) {
    MCVLDigits* outputContainer = get<MCVLDigits>(m_digitLocation);
    MCVLDigits::const_iterator itd;
    for (itd = m_digits->begin(); itd != m_digits->end(); ++itd) {
      outputContainer->insert(*itd);
    }
  } else {
    put(m_digits, m_digitLocation);
  }
  if (m_debug) {
    debug() << "Created " << m_digits->size() << " digits" << endmsg;
  } 
  return StatusCode::SUCCESS;

}

//=======================================================================
/// Loop through hits and allocate charge to strips
//=======================================================================
void VLDigitCreator::simulateSignal(MCHits* hits, double bunchOffset) {

  if (m_debug) debug() << " ==> simulateSignal()" << endmsg;
  std::vector<double> points;
  // Loop over the hits.
  MCHits::const_iterator it;
  for (it = hits->begin(); it != hits->end(); ++it) {
    MCHit* hit = (*it);
    // Get the effective charge fraction for this hit.
    double tof = hit->time() - fabs(hit->entry().z() / Gaudi::Units::c_light); 
    const double t = m_peakTime + m_offPeakSamplingTime + bunchOffset - tof;
    const double fraction = m_SiTimeTool->response(t);
    // Calculate the deposited charge and distribute it 
    // to points along the hit path.
    points.clear();
    if (!deposit(hit, points, fraction)) continue;
    // Diffuse charges from points to strips.
    drift(hit, points);
  }

}

//=======================================================================
/// Assign the deposited charge to points in the silicon.
//=======================================================================
bool VLDigitCreator::deposit(MCHit* hit, std::vector<double>& points,
                             double fraction) {

  if (m_debug) debug() << " ==> deposit()" << endmsg;
  const DeVLSensor* sens = m_det->sensor(hit->sensDetID());
  if (!sens) {
    error() << "Invalid sensor" << endmsg;
    return false;
  }
  VLChannelID entryChannel, exitChannel;
  double entryFraction = 0., exitFraction = 0.;
  double pitch = 0.;
  StatusCode entryValid = 
    sens->pointToChannel(hit->entry(), entryChannel, entryFraction, pitch);
  StatusCode exitValid = 
    sens->pointToChannel(hit->exit(), exitChannel, exitFraction, pitch);
  
  if (m_verbose) {
    verbose() << "Entry point: sensor " << entryChannel.sensor() 
              << ", strip " << entryChannel.strip() 
              << " + " << entryFraction << endmsg;
    verbose() << "Exit point:  sensor " << exitChannel.sensor() 
              << ", strip " << exitChannel.strip() 
              << " + " << exitFraction << endmsg;
    if (!entryValid) verbose() << "Invalid entry point" << endmsg;
    if (!exitValid)  verbose() << "Invalid exit point"  << endmsg;
  }
  // Skip hits with invalid entry or exit.
  if (!entryValid || !exitValid) return false;
  // Calculate how many full strips entry and exit points are apart.
  int nNeighb;
  StatusCode sc = sens->channelDistance(entryChannel, exitChannel, nNeighb);
  if (!sc) {
    // Entry and exit strips are not in the same sector.
    return false;
  }
  double distance = fabs(nNeighb - (entryFraction - exitFraction));
  unsigned int nPoints = static_cast<int>(ceil(distance) * m_pointsPerStrip);
  points.resize(nPoints);

  // Calculate the total charge in electrons.
  double charge = (hit->energy() / Gaudi::Units::eV) / m_eVPerElectron;
  if (m_verbose) {
    verbose() << "Deposited energy: " 
              << 1.e-3 * hit->energy() / Gaudi::Units::eV << " keV" << endmsg; 
    verbose() << "Deposited charge: " << charge << " electrons" << endmsg;
  }
  // Apply time correction.
  charge *= fraction;
  if (m_verbose) verbose() << "Collected charge: " << charge << endmsg; 
  // Amount of charge to divide equally
  double chargeEqual = charge;
  if (m_inhomogeneousCharge) {
    double thickness = m_det->sensor(hit->sensDetID())->siliconThickness();
    chargeEqual = m_chargeUniform * thickness / Gaudi::Units::micrometer;
    // Apply time correction.
    chargeEqual *= fraction;
    if (chargeEqual > charge) chargeEqual = charge;
  } 
  if (m_verbose) {
    verbose() << "Charge for equal allocation: " << chargeEqual << endmsg;
  }
  // Divide charge equally among the points.
  const double chargeDiv = chargeEqual / points.size();
  const double sqrtChargeDiv = sqrt(fabs(chargeDiv));
  std::vector<double>::iterator itp;
  for (itp = points.begin(); itp != points.end(); ++itp) {
    *itp = chargeDiv;
    if (m_inhomogeneousCharge) {
      // Gaussian fluctuations
      *itp += m_gauss() * sqrtChargeDiv * m_scaleFluctuations;
    }
    if (m_verbose) {
      verbose() << "Charge on point" << itp - points.begin() 
                << ": " << *itp << endmsg;
    }
  }
  // Add inhomogeneous charge distribution from delta rays
  if (m_inhomogeneousCharge) {
    deltaRayCharge(charge, chargeEqual, 0.001 * charge, points);
  }
  double sum = 0.;
  for (itp = points.begin(); itp != points.end(); ++itp) {
    sum += (*itp);
  }
  // Check if distributed charge is equal to charge from hit.
  if (fabs(sum - charge) > 1.e-6) {
    Warning("Normalization problem, see debug for information!");
    if (m_debug) {
      debug() << "Sum of charges distributed on points: " 
              << sum << endmsg;
      debug() << "Deposited charge: " << charge << endmsg;
    }
  }
  return true;

}

//============================================================================
/// Allocate remaining charge from delta ray distribution
//============================================================================
void VLDigitCreator::deltaRayCharge(double total, double equal, double tol, 
                                    std::vector<double>& points) {

  if (m_debug) debug() << " ==> deltaRayCharge() " << endmsg;
  // Upper limit on charge from delta ray
  double tmax = total - equal;
  // Lower limit on charge from delta ray
  double tmin = m_deltaRayMinEnergy / m_eVPerElectron;
  // Amount of charge left to allocate
  if (tol < tmin * 2.) tol = tmin * 2.;
  while (tmax > tol) {
    // Generate delta ray energy
    // dN/DE=k*1/E^2 for relativistic incident particle
    // E(r)=1/r, where r is uniform in range 1 / tmin < r < 1 / tmax
    // but tmax bounded by energy left to allocate, so following is
    // not truly correct
    double charge = ranInvSquare(tmin, tmax);
    // Choose point at random to add delta ray
    int i = static_cast<int>(Math::round(m_uniform() * (points.size() - 1)));
    if (m_verbose) {
      verbose() << "Delta ray charge added to point " << i
                << "/" << points.size() << endmsg;
    }
    points[i] += charge;
    tmax -= charge;
  }

  double sum = 0.;
  std::vector<double>::iterator itp;
  for (itp = points.begin(); itp != points.end(); ++itp) {
    sum += *itp;
  }
  // Scale to ensure that the total charge is allocated
  if (fabs(sum) > 1.e-8) {
    const double adjust = total / sum;
    for (itp = points.begin(); itp != points.end(); ++itp) {
      (*itp) *= adjust;
     }
  }

}

//============================================================================
/// Allocate the charge to the collection strips
//============================================================================
void VLDigitCreator::drift(MCHit* hit, std::vector<double>& points) {

  if (m_debug) debug()<< " ==> drift()" << endmsg;
  // Distance between steps on hit path
  Gaudi::XYZVector step = hit->displacement() / (2. * points.size());
  Gaudi::XYZPoint point = hit->entry() + step;

  const DeVLSensor* sens = m_det->sensor(hit->sensDetID());
  const double thickness = sens->siliconThickness() / Gaudi::Units::micrometer;
  // Assume strips are at opposite side of Si to entry point
  const double dz = thickness / (2. * points.size()); 
  double z = thickness - dz;

  // Loop over points on path
  std::vector<double>::iterator itp;
  for (itp = points.begin(); itp != points.end() ; ++itp) { 
    if (m_verbose) {
      verbose() << "Point " << itp - points.begin() 
                << " at " << point << endmsg;
    }
    // Make sure the point is valid
    double fraction, pitch;
    VLChannelID entryChan;
    StatusCode valid = sens->pointToChannel(point, entryChan, fraction, pitch);
    if (!valid) {
      // A point may be invalid, despite entry and exit points being valid,
      // as it lies in say a bias rail dead area.
      // Charge from this point is considered lost
      if (m_verbose) {
        verbose() << "Point at " << point << " is not in active silicon "  
                  << "(entry " << hit->entry() << ", exit " <<  hit->exit() 
                  << ", detID " << hit->sensDetID() << ")" << endmsg;
      }
      continue;
    }
    // Number of neighbouring strips (+/-) to consider
    const int nNeighbs = 1; 
    double fractions[2 * nNeighbs + 1];
    double sum = 0.;
    const double sigma = m_baseDiffuseSigma * sqrt(thickness * z);
    if (m_verbose) {
      verbose() << "z: " << z << ", sigma: " << sigma 
                << " (base: " << m_baseDiffuseSigma << ")" << endmsg;
    }
    // Loop over the neighbours of this point.
    for (int iNg = -nNeighbs; iNg <= +nNeighbs; ++iNg) {
      double d1 = ((iNg - 0.5) - fraction) * pitch / Gaudi::Units::micrometer;
      double d2 = ((iNg + 0.5) - fraction) * pitch/ Gaudi::Units::micrometer;
      const double p1 = erfcSafe(d1 / sigma);
      const double p2 = erfcSafe(d2 / sigma);
      if (m_verbose) {
        verbose() << "d1 " << d1 << "  " << "d2 " << d2 << "  "
                  << "p1 " << p1 << "  " << "p2 " << p2 << endmsg;
      }
      const int i = (iNg < 0) ? nNeighbs + abs(iNg) : iNg;
      fractions[i] = fabs(p1 - p2);
      sum += fabs(p1 - p2);
      if (m_verbose) {
        verbose() << "Allocate fraction " << fractions[i] 
                  << " to neighbour " << iNg << endmsg;
      }
    }
    // Renormalise allocated fractions and update strip signals
    for (int iNg = -nNeighbs; iNg <= +nNeighbs; ++iNg) {
      if (sum < 1.e-8) continue;
      const int i = (iNg < 0) ? nNeighbs + abs(iNg) : iNg;
      const double charge = (*itp) * fractions[i] / sum;
      if (m_debug) {
        debug() << "Allocate charge " << charge << " to neighbour "
                << iNg << " of point " << itp - points.begin() << endmsg; 
      }
      if (charge > m_threshold * 0.1) {
        // Ignore if below 10% of threshold
        // Calculate index of this strip
        VLChannelID channel;
        valid = sens->neighbour(entryChan, iNg, channel);
        // Update charge and MCHit list
        if (valid) {
          MCVLDigit* digit = getDigit(channel, true);
          if (hit->parent() == m_MCHitContainerToLink) {
            if (m_verbose) verbose() << "MCHit to link" << endreq;
            // Update and add MC link
            addSignal(digit, hit, charge);
          } else {
            if (m_verbose) verbose() << "Non-linked MCHit" << endreq;
            // Update an unlinked digit
            addSignal(digit, 0, charge);
          }
        }
      }
    } 
    // Go to the next point on the path
    point += 2 * step;
    z -= 2 * dz;
  }

}

//============================================================================
/// Update signal and list of MCHits
//============================================================================
void VLDigitCreator::addSignal(MCVLDigit* digit, MCHit* hit, double charge) {

  if (m_debug) debug() << " ==> addSignal()" << endmsg;
  digit->setSignal(digit->signal() + charge);
  if (!hit) return;
  // Add the MC hit and charge deposit.
  const unsigned int nHits = digit->numberOfMCHits();
  bool foundHit = false;
  for (unsigned int i = 0; i < nHits; ++i) {
    MCHit* ref = digit->mcHit(i);
    if (ref == hit) {
      // Hit exists. Update the charge.
      digit->setMCHitCharge(i, digit->mcHitCharge(i) + charge);
      foundHit = true;
      break;
    }
  }
  if (!foundHit) digit->addToMCHits(hit, charge);

}
//============================================================================
/// Add a fraction of signal in strip to the two neighbouring strips.
/// It is assumed that this is a small fraction and hence it doesn't matter
/// in what order this procedure is applied to the strip list.
//============================================================================
void VLDigitCreator::simulateCoupling() {

  if (m_debug) debug() << " ==> simulateCoupling()" << endmsg;
  // Sort digits into order of ascending sensor + strip.
  std::stable_sort(m_digits->begin(), m_digits->end(),
                   VLDataFunctor::LessByKey<const MCVLDigit*>());
  // Make a new container for any added strips.
  m_digitsCoupling = new MCVLDigits();
  MCVLDigits::iterator itd; 
  for (itd = m_digits->begin(); itd != m_digits->end(); ++itd) {
    // Calculate signal to couple to neighbouring strips.
    double coupledSignal = (*itd)->signal() * m_coupling;
    // Subtract the coupled signal from this strip.
    (*itd)->setSignal((*itd)->signal() - 2. * coupledSignal);
    if (m_verbose) {
      verbose() << "Signal after subtraction: " << (*itd)->signal() << endmsg;
    }
    // Set flag whether the coupled signal is worth creating.
    bool create = (coupledSignal > m_threshold * 0.1);
    bool valid;
    // Add to previous strip (if doesn't exist then create).
    MCVLDigit* prev = getDigitPrev(itd, valid, create);
    if (valid) addSignal(prev, 0, coupledSignal);
    // Add to next strip.
    MCVLDigit* next = getDigitNext(itd, valid, create);
    if (valid) addSignal(next, 0, coupledSignal);
  } 
  // Add any newly created digits.
  if (m_debug) {
    debug() << m_digitsCoupling->size() 
            << " digits created by coupling routine" << endmsg;
  }
  MCVLDigits::iterator itc; 
  for (itc = m_digitsCoupling->begin(); itc < m_digitsCoupling->end(); ++itc) {
    MCVLDigit* digit = m_digits->object((*itc)->key());
    if (0 != digit) {
      digit->setSignal(digit->signal() + (*itc)->signal());
      if (m_verbose) {
        verbose() << "Update existing digit (sensor " 
                  << (*itc)->sensor() << ", strip"
                  << (*itc)->strip() << ")" << endmsg;
      }
    } else {
      if (m_verbose) {
        verbose() << "Add coupling digit (sensor " 
                  << (*itc)->sensor() << ", strip " 
                  << (*itc)->strip() << ")" << endmsg;
      }
      m_digits->insert(*itc);
    }
  }
  delete m_digitsCoupling;
  m_digitsCoupling = 0;
  if (m_debug) {
    debug() << "Number of digits after coupling simulation: "
            << m_digits->size() << endmsg;
  }

}

//============================================================================
/// From an originally sorted list, find the strip with the previous key,
/// or create a new one.
//============================================================================
MCVLDigit* VLDigitCreator::getDigitPrev(MCVLDigits::iterator itd, 
                                        bool& valid, bool& create) {
  
  if (m_debug) debug() << " ==> getDigitPrev()" << endmsg;
  // Try the previous entry in the same container.
  MCVLDigit* prev = (*itd);
  if (itd != m_digits->begin()) {
    --itd;
    prev = (*(itd));
    ++itd;
  }
  // Check
  int distance = 0;
  const DeVLSensor* sens = m_det->sensor((*itd)->key().sensor());
  StatusCode sc = sens->channelDistance((*itd)->key(), prev->key(), distance);
  valid = sc.isSuccess();
  if (valid && -1 == distance) return prev;
  // Check if just added this strip in other container
  if (m_digitsCoupling->size() != 0) {
    MCVLDigits::iterator last = m_digitsCoupling->end(); 
    last--;
    prev = (*last);
  }
  // Check
  sc = sens->channelDistance((*itd)->key(), prev->key(), distance);
  valid = sc.isSuccess();
  if (valid && -1 == distance) return prev;

  // Doesn't exist so insert a new strip (if create is true)
  if (create) {
    VLChannelID channel;
    sc = sens->neighbour((*itd)->key(), -1, channel);
    if (sc.isSuccess()) {
      // Protect if key already exists
      prev = m_digitsCoupling->object(channel);
      if (0 != prev) return prev;
      if (m_verbose) {
        verbose() << "Create strip" << channel.strip() 
                  << " (sensor " << channel.sensor() << ")" << endmsg;
      }
      prev = new MCVLDigit(channel);
      m_digitsCoupling->insert(prev);
      valid = true;
    } else {
      valid = false;
      prev = 0;
    }
  } else {
    valid = false;
    prev = 0;
  }
  return prev;

}

//============================================================================
/// From an originally sorted list, find the strip with the next key,
/// or create a new one.
//============================================================================
MCVLDigit* VLDigitCreator::getDigitNext(
    MCVLDigits::iterator itd, bool& valid, bool& create) {
  
  if (m_debug) debug() << " ==> getDigitNext()" << endmsg;
  // Try next entry in container.
  MCVLDigit* nextStrip = *itd;
  MCVLDigits::iterator last = m_digits->end(); 
  last--;
  if (itd != last) {
    ++itd;
    nextStrip = (*(itd));
    --itd;
  }
  // Check
  const DeVLSensor* sens = m_det->sensor((*itd)->key().sensor());
  int checkDistance;
  StatusCode sc = sens->channelDistance((*itd)->key(), nextStrip->key(),
                                        checkDistance);
  valid = sc.isSuccess();
  if (valid && 1 == checkDistance) return nextStrip;

  // Doesn't exist so insert a new strip (if create is true)
  if (create) {
    VLChannelID channel;
    sc = sens->neighbour((*itd)->key(), +1, channel);
    if (sc.isSuccess()) {
     // Protect if key already exists
      nextStrip = m_digitsCoupling->object(channel);
      if (0 != nextStrip) return nextStrip;
      if (m_verbose) {
        verbose() << "Create strip" << channel.strip() 
                  << " (sensor " << channel.sensor() << ")" << endmsg;
      }
      nextStrip = new MCVLDigit(channel);
      m_digitsCoupling->insert(nextStrip);
      valid = true;
    } else {
      valid = false;
      nextStrip = 0;
    }
  } else {
    valid = false;
    nextStrip = 0;
  }
  return nextStrip;

}

//============================================================================
/// Add noise
//============================================================================
void VLDigitCreator::simulateNoise() {

  if (m_debug) debug() << " ==> simulateNoise()" << endmsg;
  // Add noise to channels which already have a hit. 
  MCVLDigits::iterator itd; 
  for (itd = m_digits->begin(); itd != m_digits->end(); ++itd) {
    if ((*itd)->noise() == 0) {
      const unsigned int sensor = (*itd)->sensor();
      const unsigned int strip = (*itd)->strip();
      const double stripNoise = m_StripNoiseTool->noise(sensor, strip);
      if (stripNoise > 0.) {
        (*itd)->setNoise(m_gauss() * stripNoise);
        if (m_verbose) {
          verbose() << "Noise added to strip " << strip 
                    << " (sensor " << sensor << "): " 
                    << (*itd)->noise() << endmsg;
        }
      } 
    }
  }
  // Allocate noise to channels which don't have a hit.
  const std::vector<DeVLSensor*>& sensors = m_det->sensors();
  std::vector<DeVLSensor*>::const_iterator its;
  for (its = sensors.begin(); its != sensors.end(); ++its) {
    const DeVLSensor* sens = *its;
    // Estimate the number of noise hits.
    int nNoiseHits = 0;
    // Turn off FPEs
    FPE::Guard allFPE(FPE::Guard::mask("AllExcept"), true);
    if (sens->isPhi()) {
      nNoiseHits = int(m_poissonPhi());
    } else {
      nNoiseHits = int(m_poissonR());
    }
    const int nStrips = sens->numberOfStrips();
    const unsigned int sensor = sens->sensorNumber();
    if (m_verbose) {
      verbose() << "Add noise to " << nNoiseHits << "/" << nStrips 
                << " strips of sensor " << sensor << endmsg;
    }
    for (int i = 0; i < nNoiseHits; ++i) {
      // Choose a random strip. 
      int strip = int(Math::round(m_uniform() * (nStrips - 1)));
      VLChannelID channel(sensor, strip, VLChannelID::Null);
      if (sens->isPhi()) {
        channel.setType(VLChannelID::PhiType);
      } else {
        channel.setType(VLChannelID::RType);
      }
      MCVLDigit* digit = getDigit(channel, true);
      if (digit->noise() == 0) {
        double stripNoise = m_StripNoiseTool->noise(sensor, strip);
        if (stripNoise > 0.) {
          stripNoise = ranGaussianTail(m_threshold, stripNoise);
          if (m_uniform() > 0.5) stripNoise *= -1.;
          digit->setNoise(stripNoise);
        } 
      }
    }
  }

}

//============================================================================
/// Convert number of electrons to ADC counts 
//============================================================================
void VLDigitCreator::digitise(){

  if (m_debug) debug() << " ==> digitise()" << endmsg;
  MCVLDigits::iterator itd;
  for (itd = m_digits->begin(); itd != m_digits->end(); ++itd) {
    // Assume linear response for now.
    double digi = (*itd)->charge() / m_electronsPerAdc;
    int adc = static_cast<int>(Math::round(digi));
    if (adc >= m_adcRange) {
      adc = m_adcRange - 1;
    } else if (adc < 0) {
      adc = 0;
    }
    (*itd)->setAdc(adc); 
  }

}

//============================================================================
/// Simulate strip inefficiency
//============================================================================
void VLDigitCreator::simulateInefficiency() {
  
  if (m_debug) debug()<< " ==> simulateInefficiency()" << endmsg;
  MCVLDigits::iterator itd;
  for (itd = m_digits->begin(); itd != m_digits->end(); ++itd) {
    if (m_inefficiency > m_uniform()) {
      // Set signal to zero (channel will be removed by cut in final process).
      (*itd)->setSignal(0.);
    }
  }

}

//============================================================================
/// Find a strip in list, or if it does not currently exist create it
//============================================================================
MCVLDigit* VLDigitCreator::getDigit(VLChannelID& channel, const bool create) {

  if (m_debug) debug() << " ==> getDigit()" << endmsg;
  MCVLDigit* digit = m_digits->object(channel);
  if (0 == digit && create) {
    // This strip has not been used before, so create it.
    digit = new MCVLDigit(channel);
    if (m_verbose) {
      verbose() << "Add digit (sensor " << channel.sensor() 
                << ", strip " << channel.strip() << ")" << endmsg;
    } 
    m_digits->insert(digit);
  }
  return digit;

}

//============================================================================
/// Remove any digits with charge below threshold
//============================================================================
void VLDigitCreator::cleanup() {
  
  if (m_debug) debug() << " ==> cleanup()" << endmsg;
  // Sort the digits by charge.
  std::sort(m_digits->begin(), m_digits->end(),
            VLDataFunctor::LessByCharge<const MCVLDigit*>());
  std::reverse(m_digits->begin(),m_digits->end());
  // Remove digits with charge below threshold.
  MCVLDigits::iterator it = std::find_if(m_digits->begin(), m_digits->end(),
                                         VLThreshold(m_threshold));
  m_digits->erase(it, m_digits->end());
  // Sort the digits by sensor and strip number.
  std::sort(m_digits->begin(), m_digits->end(),
            VLDataFunctor::LessByKey<const MCVLDigit*>());

}

//============================================================================
/// Draw random number from delta ray energy distribution 
//============================================================================
double VLDigitCreator::ranInvSquare(const double tmin, const double tmax) {
  
  const double range = (1. / tmin) - (1. / tmax);
  const double offset = 1. / tmax;
  const double uniform = m_uniform() * range + offset;
  return 1. / uniform;

}

//============================================================================
/// Draw random number from upper tail of a Gaussian distribution
//============================================================================
double VLDigitCreator::ranGaussianTail(const double a, const double sigma) {
  
  const double s = a / sigma;
  if (s < 1) {
    // For small s, use a direct rejection method. 
    // The limit s < 1 can be adjusted to optimise the overall efficiency
    double x = m_gauss();
    while (x < s) x = m_gauss();
    return x * sigma;
  } 

  // Use the "supertail" deviates from the last two steps
  // of Marsaglia's rectangle-wedge-tail method, as described
  // in Knuth, v2, 3rd ed, pp 123-128.  (See also exercise 11, p139,
  // and the solution, p586.)
  double u, v, x;
  do {
    u = m_uniform();
    v = m_uniform();
    while (v == 0.) v = m_uniform();
    x = sqrt(s * s - 2 * log(v));
  } while (x * u > s);
  return x * sigma;

}

//============================================================================
/// Complementary error function protected against floating point exceptions
//============================================================================
double VLDigitCreator::erfcSafe(const double arg) {

  if (arg < -10.) return 1.;
  if (arg >  10.) return 0.;
  // Turn off the inexact FPE.
  FPE::Guard reducedFPE(FPE::Guard::mask("Inexact"), true);
  return gsl_sf_erf_Q(arg);

}

//============================================================================
/// Finalization
//============================================================================
StatusCode VLDigitCreator::finalize() {
 
  return GaudiAlgorithm::finalize(); 

}
