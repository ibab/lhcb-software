// STL
#include <vector>
// Gaudi
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/AlgFactory.h"
#include "LHCbMath/LHCbMath.h"
// Kernel/LHCbKernel
#include "Kernel/VeloLiteDataFunctor.h"
#include "Kernel/ISiAmplifierResponse.h"
#include "Kernel/VeloLiteChannelID.h"
// Event/MCEvent
#include "Event/MCHit.h"
#include "Event/MCParticle.h"
// Det/VeloLiteDet
#include "VeloLiteDet/DeVeloLite.h"
// Local 
#include "VeloLiteThreshold.h"
#include "IStripNoiseTool.h"
#include "VeloLiteDigitCreator.h"

/** @file VeloLiteDigitCreator.cpp
 *
 *  Implementation of class : VeloLiteDigitCreator
 *
 */

DECLARE_ALGORITHM_FACTORY(VeloLiteDigitCreator);

//===========================================================================
/// Constructor
//===========================================================================
VeloLiteDigitCreator::VeloLiteDigitCreator(const std::string& name,
                                           ISvcLocator* pSvcLocator) : 
  GaudiAlgorithm(name, pSvcLocator),
  m_det(0),
  m_threshold(0.),
  m_baseDiffuseSigma(0.),
  m_nDigitsTotal(0),
  m_nDigitsKilled(0) {
  
  declareProperty("InputContainers", m_inputContainers);
  declareProperty("InputContainerToLink", m_MCHitContainerToLinkName);
  declareProperty("InputTimeOffsets", m_inputTimeOffsets);
  declareProperty("OutputContainers", m_outputContainers);

  declareProperty("SimNoisePileUp", m_simNoisePileUp = false);
  declareProperty("InhomogeneousCharge", m_inhomogeneousCharge = true);
  declareProperty("Coupling", m_coupling = true);
  declareProperty("NoiseSim", m_noiseSim = true);

  declareProperty("Threshold", m_threshold = 2400.); 
  declareProperty("kT", m_kT = 0.025);
  declareProperty("BiasVoltage", m_biasVoltage = 150.);
  declareProperty("eVPerElectron", m_eVPerElectron = 3.6);
  declareProperty("SimulationPointsPerStrip", m_simulationPointsPerStrip = 3);
  declareProperty("ChargeUniform", m_chargeUniform = 70.);
  declareProperty("DeltaRayMinEnergy", m_deltaRayMinEnergy = 1000.);
  declareProperty("CapacitiveCoupling", m_capacitiveCoupling = 0.01);
  declareProperty("NoiseScale", m_scaleNoise = 1.);
  declareProperty("FluctuationsScale", m_scaleFluctuations = 1.);
  declareProperty("StripInefficiency", m_stripInefficiency = 0.0);

  declareProperty("PulseShapePeakTime", m_pulseShapePeakTime = 30.7848);
  declareProperty("OffPeakSamplingTime", m_offPeakSamplingTime = 0.);

  declareProperty("ElectronsPerADC", m_electronsPerADC = 600.);
  declareProperty("MaxADCOutput", m_maxADCOutput =  127); 
  declareProperty("MinADCOutput", m_minADCOutput = -127);

  Rndm::Numbers m_gaussDist;
  Rndm::Numbers m_uniformDist;

}

//===========================================================================
/// Destructor
//===========================================================================
VeloLiteDigitCreator::~VeloLiteDigitCreator() {

}

//===========================================================================
/// Initialisation. Check parameters
//===========================================================================
StatusCode VeloLiteDigitCreator::initialize() {

  /// Initialise base class.
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;
  m_debug   = msgLevel(MSG::DEBUG);
  m_verbose = msgLevel(MSG::VERBOSE);
  if (m_debug) debug() << " ==> initialize()" << endmsg;

  if (m_inputContainers.size() != m_inputTimeOffsets.size()) {
    error() << "Sizes of InputContainers and InputTimeOffsets do not match" 
            << endmsg;
    return StatusCode::FAILURE;
  }

  m_det = getDet<DeVeloLite>(DeVeloLiteLocation::Default);
  m_baseDiffuseSigma = sqrt(2 * m_kT / m_biasVoltage);

  /// Random number initialisation
  sc = m_gaussDist.initialize(randSvc(), Rndm::Gauss(0., 1.));
  if (!sc) {
    error() << "Failed to initialise Gaussian random number generator" 
            << endmsg;
    return StatusCode::FAILURE;
  }
  sc = m_uniformDist.initialize(randSvc(), Rndm::Flat(0., 1.));
  if (!sc) {
    error() << "Failed to initialise uniform random number generator" 
            << endmsg;
    return StatusCode::FAILURE;
  }

  /// Front end time tool
  m_SiTimeTool = tool<ISiAmplifierResponse>("SiAmplifierResponse", 
                                            "SiTimeTool", this);
  /// Strip noise tool
  m_StripNoiseTool = tool<IStripNoiseTool>("VeloLiteStripNoiseTool",
                                           "StripNoise", this);

  /// Estimate the number of R strips to add noise to.
  std::vector<DeVeloLiteRType*>::const_iterator rsens = m_det->rSensorsBegin();
  const unsigned int nStripsR = (*rsens)->numberOfStrips();
  m_averageStripNoiseR = m_StripNoiseTool->averageNoise((*rsens)->sensorNumber());
  m_averageStripNoiseR *= m_scaleNoise; 
  const double probR = safe_gsl_sf_erf_Q(m_threshold / m_averageStripNoiseR);
  const double averageNoiseHitsR = 2. * probR * nStripsR;
  sc = m_poissonDistR.initialize(randSvc(), Rndm::Poisson(averageNoiseHitsR));
  if (!sc) {
    error() << "Failed to initialise Poisson random number generator" << endmsg;
    return sc;
  }

  /// Estimate the number of Phi strips to add noise to.
  std::vector<DeVeloLitePhiType*>::const_iterator phisens = m_det->phiSensorsBegin();
  const unsigned int nStripsPhi = (*phisens)->numberOfStrips();
  m_averageStripNoisePhi = m_StripNoiseTool->averageNoise((*phisens)->sensorNumber());
  m_averageStripNoisePhi *= m_scaleNoise; 
  const double probPhi = safe_gsl_sf_erf_Q(m_threshold / m_averageStripNoisePhi);
  const double averageNoiseHitsPhi = 2. * probPhi * nStripsPhi;
  sc = m_poissonDistPhi.initialize(randSvc(), Rndm::Poisson(averageNoiseHitsPhi));
  if (!sc) {
    error() << "Failed to initialise Poisson random number generator" << endmsg;
    return sc;
  }
  return StatusCode::SUCCESS;

};

//===========================================================================
/// Main execution
//===========================================================================
StatusCode VeloLiteDigitCreator::execute() {

  if (m_debug) debug() << " ==> execute()" << endmsg;
  /// Get the container of MCHits to make MC linker table to
  m_MCHitContainerToLink = get<LHCb::MCHits>(m_MCHitContainerToLinkName);

  m_digits = new LHCb::MCVeloLiteDigits();
  std::vector<std::string>::const_iterator it;
  for (it = m_inputContainers.begin(); it != m_inputContainers.end(); ++it) {
    if (!exist<LHCb::MCHits>(*it)) {
      if (m_debug) {
        debug() << "Could not find container " << *it << endmsg;
      }
      continue;
    }
    /// Retrieve hits.
    LHCb::MCHits* hits = get<LHCb::MCHits>(*it);
    if (m_debug) debug() << hits->size() << " hits in " << *it << endmsg;
    const unsigned int timeIndex = it - m_inputContainers.begin();
    const double timeOffset = m_inputTimeOffsets[timeIndex];
    /// Simulate signals in strips from MCHits.
    simulateSignal(hits, timeOffset);
    /// Simulate charge sharing due to capacitive coupling.
    // if (m_coupling) simulateCoupling();
    /// Add noise.
    if (m_noiseSim) simulateNoise();
    /// Convert charge to ADC counts.
    digitise();
    // Simulate signal loss due to dead strips.
    if (m_stripInefficiency > 0.) simulateInefficiency();
    /// Remove digits below threshold and sort.
    cleanup();
  }
  StatusCode sc = storeDigits(); 
  return sc;

}

//=======================================================================
/// Loop through hits and allocate charge to strips
//=======================================================================
void VeloLiteDigitCreator::simulateSignal(LHCb::MCHits* hits,
                                          double timeOffset) {

  if (m_debug) debug() << " ==> simulateSignal() " << endmsg;
  // Loop over input hits.
  LHCb::MCHits::const_iterator it; 
  for (it = hits->begin(); it != hits->end(); ++it) {
    LHCb::MCHit* hit = (*it);
    // Calculate number of points to which the charge is assigned.
    const int nPoints = numberOfPoints(hit);
    if (m_verbose) {
      verbose() << "Simulating " << nPoints << " points in Si" << endmsg;
    }
    if (nPoints <= 0) continue; 
    // Calculate charge to assign to each point.
    std::vector<double> points(nPoints);
    chargePerPoint(*it, points, timeOffset);
    // Diffuse charge from points to strips.
    diffusion(*it, points);
  }
  if (m_debug) {
    debug() << "Created " << m_digits->size() << " digits" << endmsg;
  }

}

//=======================================================================
/// Calculate number of points in silicon the simulation will use
//=======================================================================
int VeloLiteDigitCreator::numberOfPoints(LHCb::MCHit* hit) {

  if (m_debug) debug() << " ==> numberOfPoints()" << endmsg;
  const DeVeloLiteSensor* sens = m_det->sensor(hit->sensDetID());
  if (!sens) {
    error() << "Invalid sensor" << endmsg;
    return 0;
  }
  LHCb::VeloLiteChannelID entryChannel, exitChannel;
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
  // Ignore hits with invalid entry or exit.
  if (!entryValid || !exitValid) return 0;
  // Calculate how many full strips entry and exit are apart.
  int nNeighb;
  StatusCode sc = sens->channelDistance(entryChannel, exitChannel, nNeighb);
  if (!sc) {
    if (m_verbose) {
      verbose() << "Entry and exit strips are not in same sector" << endmsg;
    }
    return 0;
  }
  double nPoints = fabs(float(nNeighb) - (entryFraction - exitFraction));
  if (m_verbose) {
    verbose() << "Entry and exit are " << nPoints << " (integer: "
              << nNeighb << ") strips apart" << endmsg;
  }
  return static_cast<int>(ceil(nPoints) * m_simulationPointsPerStrip);

}

//=======================================================================
/// Allocate charge to points
//=======================================================================
void VeloLiteDigitCreator::chargePerPoint(LHCb::MCHit* hit,
                                          std::vector<double>& points, 
                                          double timeOffset) {

  if (m_debug) debug() << " ==> chargePerPoint()" << endmsg;
  // Calculate the total charge in electrons.
  double charge = (hit->energy() / Gaudi::Units::eV) / m_eVPerElectron;
  if (m_verbose) {
    verbose() << "Deposited energy: " 
              << 1.e-3 * hit->energy() / Gaudi::Units::eV << " keV" << endmsg; 
    verbose() << "Deposited charge: " << charge << " electrons" << endmsg;
  }
  // Apply time correction.
  charge *= chargeTimeFactor(hit->time(), timeOffset, hit->entry().z());
  if (m_verbose) {
    verbose() << "Charge after time correction of " 
              << timeOffset << ": " << charge << endmsg;
  }
  // Amount of charge to divide equally
  double chargeEqual = charge;
  if (m_inhomogeneousCharge) {
    double thickness = m_det->sensor(hit->sensDetID())->siliconThickness();
    chargeEqual = m_chargeUniform * thickness / Gaudi::Units::micrometer;
    chargeEqual *= chargeTimeFactor(hit->time(), timeOffset, hit->entry().z());
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
      *itp += m_gaussDist() * sqrtChargeDiv * m_scaleFluctuations;
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

}

//=======================================================================
/// Allocate remaining charge from delta ray distribution
//=======================================================================
void VeloLiteDigitCreator::deltaRayCharge(double total, double equal, 
                                          double tol, 
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
    double charge = ran_inv_E2(tmin, tmax);
    // Choose point at random to add delta ray
    int i = static_cast<int>(LHCb::Math::round(m_uniformDist() * (points.size() - 1)));
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

//=======================================================================
/// Allocate the charge to the collection strips
//=======================================================================
void VeloLiteDigitCreator::diffusion(LHCb::MCHit* hit, std::vector<double>& points) {

  if (m_debug) debug()<< " ==> diffusion()" << endmsg;
  // Distance between steps on hit path
  Gaudi::XYZVector step = hit->displacement() / (2. * points.size());
  Gaudi::XYZPoint point = hit->entry() + step;

  const DeVeloLiteSensor* sens = m_det->sensor(hit->sensDetID());
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
    LHCb::VeloLiteChannelID entryChan;
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
      const double p1 = safe_gsl_sf_erf_Q(d1 / sigma);
      const double p2 = safe_gsl_sf_erf_Q(d2 / sigma);
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
        LHCb::VeloLiteChannelID channel;
        valid = sens->neighbour(entryChan, iNg, channel);
        // Update charge and MCHit list
        if (valid) {
          LHCb::MCVeloLiteDigit* digit = findOrInsert(channel);
          if (hit->parent() == m_MCHitContainerToLink) {
            if (m_verbose) verbose() << "MCHit to link" << endreq;
            // Update and add MC link
            fill(digit, hit, charge);
          } else {
            if (m_verbose) verbose() << "Non-linked MCHit" << endreq;
            // Update an unlinked digit
            fill(digit, charge);
          }
        }
      }
    } 
    // Go to the next point on the path
    point += 2 * step;
    z -= 2 * dz;
  }

}
//=======================================================================
/// Update signal and list of MCHits
//=======================================================================
void VeloLiteDigitCreator::fill(LHCb::MCVeloLiteDigit* digit,
                                LHCb::MCHit* hit, double charge) {

  if (m_debug) debug() << " ==> fill()" << endmsg;
  digit->setSignal(digit->signal() + charge);
  if (m_verbose) {
    verbose() << "Strip signal after update: " << digit->signal() << endmsg;
  }
  if (hit) {
    // Add link to MC hit / update with weight
    if (m_verbose) verbose() << "Key: " << digit->key() << endmsg;
    int size = digit->numberOfMCHits();
    int i = 0;
    LHCb::MCHit* hitChk = NULL;
    while (hit != hitChk && i < size) {
      if (m_verbose) {
        verbose() << "Hit number " << i << "/" << size
                  << " charge " << charge << " hit " << hit->index() << endmsg;
      }
      hitChk = digit->mcHit(i);
      ++i;
    };
    i--;
    if (hit == hitChk) {
      double sig = digit->mcHitCharge(i);
      if (m_verbose) verbose() << "Hit exists, has signal " << sig << endmsg;
      sig += charge;
      digit->setMCHitCharge(i, sig);
      if (m_verbose) {
        verbose() << "New signal value " << sig << " for hit "
                  << hit->index() << " hit check " << hitChk->index() << endmsg;
      }
    } else {
      if (m_verbose) verbose() << "Hit added" << endmsg;
      digit->addToMCHits(hit, charge);
    }
  }

}

//=======================================================================
/// Add a fraction of signal in strip to the two neighbouring strips.
/// It is assumed that this is a small fraction and hence it doesn't matter
/// in what order this procedure is applied to the strip list.
//=======================================================================
void VeloLiteDigitCreator::simulateCoupling() {

  if (m_debug) debug() << " ==> simulateCoupling()" << endmsg;
  // Sort digits into order of ascending sensor + strip.
  std::stable_sort(m_digits->begin(), m_digits->end(),
                   VeloLiteDataFunctor::Less_by_key<const LHCb::MCVeloLiteDigit*>());
  // Make new container for any added strips.
  m_digits_coupling = new LHCb::MCVeloLiteDigits();
  LHCb::MCVeloLiteDigits::iterator itd; 
  for (itd = m_digits->begin(); itd != m_digits->end(); ++itd) {
    // Calculate signal to couple to neighbouring strips.
    double coupledSignal = (*itd)->signal() * m_capacitiveCoupling;
    if (m_verbose) {
      verbose() << "Original signal: " << (*itd)->signal() << endmsg;
      verbose() << "Coupled signal: " << coupledSignal << endmsg;
    }
    // Subtract coupled signal from this strip.
    (*itd)->setSignal((*itd)->signal() - 2. * coupledSignal);
    if (m_verbose) {
      verbose() << "Signal after subtraction: " << (*itd)->signal() << endmsg;
    }
    // Set flag whether the coupled signals is worth creating.
    bool create = (coupledSignal > m_threshold * 0.1);
    bool valid;
    // Add to previous strip (if doesn't exist then create).
    LHCb::MCVeloLiteDigit* prev = findOrInsertPrevStrip(itd, valid, create);
    if (valid) fill(prev, coupledSignal);
    // Add to next strip.
    LHCb::MCVeloLiteDigit* next = findOrInsertNextStrip(itd, valid, create);
    if (valid) fill(next, coupledSignal);
  } 
  // Add any newly created digits.
  if (m_debug) {
    debug() << m_digits_coupling->size() 
            << " digits created by coupling routine" << endmsg;
  }
  LHCb::MCVeloLiteDigits::iterator itc; 
  for (itc = m_digits_coupling->begin(); itc < m_digits_coupling->end(); ++itc) {
    LHCb::MCVeloLiteDigit* digit = m_digits->object((*itc)->key());
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
  delete m_digits_coupling;
  m_digits_coupling = 0;
  if (m_debug) {
    debug() << "Number of digits after coupling simulation: "
            << m_digits->size() << endmsg;
  }

}

//=======================================================================
/// From an originally sorted list, find the strip with the previous key,
/// or create a new one.
//=======================================================================
LHCb::MCVeloLiteDigit* VeloLiteDigitCreator::findOrInsertPrevStrip(
    LHCb::MCVeloLiteDigits::iterator itd, bool& valid, bool& create) {
  
  if (m_debug) debug() << " ==> findOrInsertPrevStrip()" << endmsg;
  // Try previous entry in container.
  LHCb::MCVeloLiteDigit* prevStrip = (*itd);
  if (itd != m_digits->begin()) {
    --itd;
    prevStrip = (*(itd));
    ++itd;
  }
  // Check
  int checkDistance;
  const DeVeloLiteSensor* sens = m_det->sensor((*itd)->key().sensor());
  StatusCode sc = sens->channelDistance((*itd)->key(), prevStrip->key(),
                                        checkDistance);
  valid = sc.isSuccess();
  if (valid && -1 == checkDistance) return prevStrip;
  // Check if just added this strip in other container
  if (m_digits_coupling->size() != 0) {
    LHCb::MCVeloLiteDigits::iterator last = m_digits_coupling->end(); 
    last--;
    prevStrip = (*last);
  }
  // Check
  sc = sens->channelDistance((*itd)->key(), prevStrip->key(),
                             checkDistance);
  valid = sc.isSuccess();
  if (valid && -1 == checkDistance) return prevStrip;

  // Doesn't exist so insert a new strip (if create is true)
  if (create) {
    LHCb::VeloLiteChannelID channel;
    sc = sens->neighbour((*itd)->key(), -1, channel);
    if (sc.isSuccess()) {
      // Protect if key already exists
      prevStrip = m_digits_coupling->object(channel);
      if (0 != prevStrip) return prevStrip;
      if (m_verbose) {
        verbose() << "Create strip" << channel.strip() 
                  << " (sensor " << channel.sensor() << ")" << endmsg;
      }
      prevStrip = new LHCb::MCVeloLiteDigit(channel);
      m_digits_coupling->insert(prevStrip);
      valid = true;
    } else {
      valid = false;
      prevStrip = NULL;
    }
  } else {
    valid = false;
    prevStrip = NULL;
  }
  return prevStrip;

}

//=======================================================================
/// From an originally sorted list, find the strip with the next key,
/// or create a new one.
//=======================================================================
LHCb::MCVeloLiteDigit* VeloLiteDigitCreator::findOrInsertNextStrip(
    LHCb::MCVeloLiteDigits::iterator itd, bool& valid, bool& create) {
  
  if (m_debug) debug() << " ==> findOrInsertNextStrip()" << endmsg;
  // Try next entry in container.
  LHCb::MCVeloLiteDigit* nextStrip = *itd;
  LHCb::MCVeloLiteDigits::iterator last = m_digits->end(); 
  last--;
  if (itd != last) {
    ++itd;
    nextStrip = (*(itd));
    --itd;
  }
  // Check
  const DeVeloLiteSensor* sens = m_det->sensor((*itd)->key().sensor());
  int checkDistance;
  StatusCode sc = sens->channelDistance((*itd)->key(), nextStrip->key(),
                                        checkDistance);
  valid = sc.isSuccess();
  if (valid && 1 == checkDistance) return nextStrip;

  // Doesn't exist so insert a new strip (if create is true)
  if (create) {
    LHCb::VeloLiteChannelID channel;
    sc = sens->neighbour((*itd)->key(), +1, channel);
    if (sc.isSuccess()) {
     // Protect if key already exists
      nextStrip = m_digits_coupling->object(channel);
      if (0 != nextStrip) return nextStrip;
      if (m_verbose) {
        verbose() << "Create strip" << channel.strip() 
                  << " (sensor " << channel.sensor() << ")" << endmsg;
      }
      nextStrip = new LHCb::MCVeloLiteDigit(channel);
      m_digits_coupling->insert(nextStrip);
      valid = true;
    } else {
      valid = false;
      nextStrip = NULL;
    }
  } else {
    valid = false;
    nextStrip = NULL;
  }
  return nextStrip;

}

//=======================================================================
/// Add noise
//=======================================================================
void VeloLiteDigitCreator::simulateNoise() {

  if (m_debug) debug() << " ==> simulateNoise()" << endmsg;
  // Add noise to channels which already have a hit. 
  LHCb::MCVeloLiteDigits::iterator itd; 
  for (itd = m_digits->begin(); itd != m_digits->end(); ++itd) {
    if ((*itd)->noise() == 0) {
      const unsigned int sensor = (*itd)->sensor();
      const unsigned int strip = (*itd)->strip();
      const double stripNoise = m_StripNoiseTool->noise(sensor, strip);
      if (stripNoise > 0.) {
        (*itd)->setNoise(noiseValue(stripNoise)); 
        if (m_verbose) {
          verbose() << "Noise added to strip " << strip 
                    << " (sensor " << sensor << "): " 
                    << (*itd)->noise() << endmsg;
        }
      } 
    }
  }
  // Allocate noise to channels which don't have a signal yet.
  std::vector<DeVeloLiteSensor*>::const_iterator sensBegin;
  std::vector<DeVeloLiteSensor*>::const_iterator sensEnd;
  if (!m_simNoisePileUp) { 
    // Main Velo
    sensBegin = m_det->rPhiSensorsBegin();
    sensEnd   = m_det->rPhiSensorsEnd();
  } else {
    // Pile-up sensors 
    sensBegin = m_det->pileUpSensorsBegin();
    sensEnd   = m_det->pileUpSensorsEnd();
  }
  std::vector<DeVeloLiteSensor*>::const_iterator its; 
  for (its = sensBegin; its != sensEnd; ++its) {
    const DeVeloLiteSensor* sens = *its;
    // Estimate the number of noise hits.
    const int nNoiseHits = sens->isPhi() ? poissonDistPhi() : poissonDistR();
    const int nStrips = sens->numberOfStrips();
    const unsigned int sensor = sens->sensorNumber();
    if (m_verbose) {
      verbose() << "Add noise to " << nNoiseHits << "/" << nStrips 
                << " strips of sensor " << sensor << endmsg;
    }
    for (int i = 0; i < nNoiseHits; ++i) {
      // Choose random strip. 
      int strip = int(LHCb::Math::round(m_uniformDist() * (nStrips - 1)));
      // Find strip in list.
      LHCb::VeloLiteChannelID channel(sensor, strip);
      LHCb::MCVeloLiteDigit* digit = findOrInsert(channel);
      if (digit->noise() == 0) {
        const double stripNoise = m_StripNoiseTool->noise(sensor, strip);
        if (stripNoise > 0.) {
          digit->setNoise(noiseValueTail(stripNoise, m_threshold)); 
        }   
      }
    }
  }

}

//=========================================================================
/// Generate some noise
//=========================================================================
double VeloLiteDigitCreator::noiseValue(double sigma) {
  
  return m_gaussDist() * sigma;

}

//=========================================================================
// Generate some noise from tail of distribution
//=========================================================================
 double VeloLiteDigitCreator::noiseValueTail(double sigma, double threshold) {
  
  double noise = ran_gaussian_tail(threshold, sigma);  
  // Noise negative or positive
  if (m_uniformDist() > 0.5) noise *= -1.; 
  return noise;

}

//=======================================================================
/// Digitisation 
//=======================================================================
void VeloLiteDigitCreator::digitise(){

  if (m_debug) debug() << " ==> digitise()" << endmsg;
  LHCb::MCVeloLiteDigits::iterator itd;
  for (itd = m_digits->begin(); itd != m_digits->end(); ++itd) {
    // Assume linear response for now.
    double digi = (*itd)->charge() / m_electronsPerADC;
    int adc = static_cast<int>(LHCb::Math::round(digi));
    if (adc > m_maxADCOutput) adc = m_maxADCOutput;
    if (adc < m_minADCOutput) adc = m_minADCOutput;
    (*itd)->setAdc(adc); 
  }

}


//=======================================================================
/// Simulate strip inefficiency
//=======================================================================
void VeloLiteDigitCreator::simulateInefficiency() {
  
  if (m_debug) debug()<< " ==> simulateInefficiency()" << endmsg;
  LHCb::MCVeloLiteDigits::iterator itd;
  for (itd = m_digits->begin(); itd != m_digits->end(); ++itd) {
    ++m_nDigitsTotal;
    if (m_stripInefficiency > m_uniformDist()) {
      ++m_nDigitsKilled;
      // Set signal to zero (channel will be removed by cut in final process)
      (*itd)->setSignal(0.);
    }
  }

}

//=======================================================================
/// Find a strip in list, or if it does not currently exist create it
//=======================================================================
LHCb::MCVeloLiteDigit* VeloLiteDigitCreator::findOrInsert(LHCb::VeloLiteChannelID& channel) {

  if (m_debug) debug() << " ==> findOrInsert()" << endmsg;
  LHCb::MCVeloLiteDigit* digit = m_digits->object(channel);
  if (0 == digit) {
    // This strip has not been used before, so create it
    digit = new LHCb::MCVeloLiteDigit(channel);
    if (m_verbose) {
      verbose() << "Add digit (sensor " << channel.sensor() 
                << ", strip " << channel.strip() << ")" << endmsg;
    } 
    m_digits->insert(digit);
  }
  return digit;

}

//=======================================================================
/// Remove any digits with charge below abs(threshold)
//=========================================================================
void VeloLiteDigitCreator::cleanup() {
  
  if (m_debug) debug() << " ==> cleanup()" << endmsg;
  // Sort container by charge.
  std::sort(m_digits->begin(), m_digits->end(),
            VeloLiteDataFunctor::Less_by_charge<const LHCb::MCVeloLiteDigit*>());
  std::reverse(m_digits->begin(),m_digits->end());
  // Remove digits with ADC below threshold
  const double thr = m_threshold / m_electronsPerADC; 
  LHCb::MCVeloLiteDigits::iterator it1 = std::find_if(m_digits->begin(), 
                                                      m_digits->end(),
                                                      VeloLiteThreshold(thr));
  LHCb::MCVeloLiteDigits::iterator it2 = m_digits->end();
  m_digits->erase(it1, it2);
  // Sort container by sensor/strip number.
  std::sort(m_digits->begin(), m_digits->end(),
            VeloLiteDataFunctor::Less_by_key<const LHCb::MCVeloLiteDigit*>());

}

//=======================================================================
/// Store digits
//=======================================================================
StatusCode VeloLiteDigitCreator::storeDigits() {
  
  if (m_debug) debug() << " ==> storeDigits()" << endmsg;
  std::vector<std::string>::const_iterator itc;
  for (itc = m_outputContainers.begin(); itc != m_outputContainers.end(); ++itc) {
    if (exist<LHCb::MCVeloLiteDigits>(*itc)) {
      LHCb::MCVeloLiteDigits* outputCont = get<LHCb::MCVeloLiteDigits>(*itc);
      LHCb::MCVeloLiteDigits::const_iterator itd;
      for (itd = m_digits->begin(); itd != m_digits->end(); ++itd) {
        outputCont->insert(*itd);
      }
    } else {
      // Put local container into TES
      put(m_digits, *itc); 
    }
    if (m_debug) {
      debug() << "Stored " << m_digits->size() 
              << " digits at " << *itc << endmsg;
    }
  }
  return StatusCode::SUCCESS;

}

//=======================================================================
// Delta ray tail random numbers
// dN/DE=k*1/E^2 for relativistic incident particle
// E(r)=1/r, where r is uniform in range 1/Tmin < r < 1/Tmax
// but Tmax bounded by energy left to allocate, so following is
// not truly correct
//=======================================================================
double VeloLiteDigitCreator::ran_inv_E2(double tmin, double tmax) {
  
  const double range = (1. / tmin) - (1. / tmax);
  const double offset = 1. / tmax;
  const double uniform = m_uniformDist() * range + offset;
  return 1. / uniform;

}

//=========================================================================
// Returns a gaussian random variable larger than a
// This implementation does one-sided upper-tailed deviates.
// Markus has promised to add this in the next release of the core Gaudi code
// in autumn 2002, till then need this version here.
// This code is based on that from the gsl library.
//=========================================================================
double VeloLiteDigitCreator::ran_gaussian_tail(const double a, const double sigma) {
  
  const double s = a / sigma;
  if (s < 1) {
    // For small s, use a direct rejection method. 
    // The limit s < 1 can be adjusted to optimise the overall efficiency
    double x;
    do {
      x = m_gaussDist();
    } while (x < s);
    return x * sigma;
  } 

  // Use the "supertail" deviates from the last two steps
  // of Marsaglia's rectangle-wedge-tail method, as described
  // in Knuth, v2, 3rd ed, pp 123-128.  (See also exercise 11, p139,
  // and the solution, p586.)
  double u, v, x;
  do {
    u = m_uniformDist();
    do {
      v = m_uniformDist();
    } while (v == 0.0);
    x = sqrt(s * s - 2 * log(v));
  } while (x * u > s);
  return x * sigma;

}

//============================================================================
//============================================================================
double VeloLiteDigitCreator::chargeTimeFactor(double tof, double bunchOffset, double z) {

  if (m_debug) debug() << " ==> chargeTimeFactor()" << endmsg;
  // Correct for z position of sensor.
  tof -= fabs(z / Gaudi::Units::c_light); 
  const double t = m_pulseShapePeakTime + m_offPeakSamplingTime + bunchOffset - tof;
  const double fraction = m_SiTimeTool->response(t);
  if (m_debug) {
    debug() << "Fraction: " << fraction << " (time " << time 
            << ", bunch offset " << bunchOffset << ")" << endmsg;
  }
  return fraction;

}

//============================================================================
//============================================================================
StatusCode VeloLiteDigitCreator::finalize() {
 
  if (m_nDigitsTotal > 0) {
    const double fKilled = 100. * static_cast<double>(m_nDigitsKilled) / 
                                  static_cast<double>(m_nDigitsTotal);
    info() << format("Removed %5.2f%% of digits", fKilled) << endmsg;    
  }
  return GaudiAlgorithm::finalize(); 

}
