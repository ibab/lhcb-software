// STL
#include <vector>
#include <map>

// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Point3DTypes.h"

// LHCb
// Kernel/LHCbKernel
#include "Kernel/VPDataFunctor.h"
#include "Kernel/VPChannelID.h"
#include "LHCbMath/LHCbMath.h"
// Event/MCEvent
#include "Event/MCHit.h"
#include "Event/MCVPDeposit.h"
#include "Event/MCVPDigit.h"
// Det/VPDet
#include "VPDet/DeVP.h"

// Local
#include "VPDepositCreator.h"

//------------------------------------------------------------
// Implementation file for class : VPDepositCreator
//
// 20/09/2009 : Marcin Kucharczyk
//------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY(VPDepositCreator)

//=============================================================================
/// Constructor
//=============================================================================
VPDepositCreator::VPDepositCreator(const std::string& name, 
                                   ISvcLocator* pSvcLocator) : 
#ifdef DEBUG_HISTO
    GaudiTupleAlg(name, pSvcLocator),
#else
    GaudiAlgorithm(name, pSvcLocator),
#endif
    m_oldXml(true), m_det(NULL), m_radDamageTool(NULL) {

  declareProperty("HitLocation",     m_hitLocation = LHCb::MCHitLocation::VP);
  declareProperty("DepositLocation", m_depositLocation = LHCb::MCVPDepositLocation::MCVPDepositLocation);
  declareProperty("DigitLocation",   m_digitLocation = LHCb::MCVPDigitLocation::MCVPDigitLocation);

  declareProperty("StepSize",    m_stepSize = 5 * Gaudi::Units::micrometer);
  declareProperty("MaxNumSteps", m_nMaxSteps = 150);

  declareProperty("eVPerElectron", m_eVPerElectron = 3.6);
  declareProperty("ChargeUniform", m_chargeUniform = 70.0);
  declareProperty("MinChargeTail", m_minChargeTail = 10.0);

  declareProperty("Temperature", m_temperature = 253.15 * Gaudi::Units::kelvin);
  declareProperty("BiasVoltage", m_biasVoltage = 105.0);

  declareProperty("Irradiated", m_irradiated = false);
  declareProperty("DataTaken", m_dataTaken = 0.);

}

//=============================================================================
/// Destructor
//=============================================================================
VPDepositCreator::~VPDepositCreator() {}

//=============================================================================
/// Initialisation
//=============================================================================
StatusCode VPDepositCreator::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;
  m_debug = msgLevel(MSG::DEBUG);
  // Get detector element.
  m_det = getDet<DeVP>(DeVPLocation::Default);
  // Get radiation damage tool.
  if (m_irradiated) {
    m_radDamageTool = tool<VPRadiationDamageTool>("VPRadiationDamageTool");
  }
  // Initialize random number generators.
  sc = m_gauss.initialize(randSvc(), Rndm::Gauss(0.0, 1.0));
  if (!sc) {
    error() << "Cannot initialize Gaussian random number generator." << endmsg;
    return sc;
  }
  sc = m_uniform.initialize(randSvc(), Rndm::Flat(0.0, 1.0));
  if (!sc) {
    error() << "Cannot initialize uniform random number generator." << endmsg;
    return sc;
  }
  // Temporary check if we are running with old or new geometry.
  if (m_det->numberSensors() > 100) m_oldXml = false;
  // Calculate diffusion coefficient.
  const double kt = m_temperature * Gaudi::Units::k_Boltzmann / Gaudi::Units::eV;
  const double thickness = m_det->sensor(0)->siliconThickness();
  if (m_irradiated) m_diffusionCoefficient = sqrt(2. * kt * thickness / 100.);
  else m_diffusionCoefficient = sqrt(2. * kt * thickness / m_biasVoltage);
#ifdef DEBUG_HISTO
  setHistoTopDir("VP/");
#endif
  return StatusCode::SUCCESS;

}

//=========================================================================
///  Main execution
//=========================================================================
StatusCode VPDepositCreator::execute() {

  // Pick up the MC hits.
  const LHCb::MCHits* hits = getIfExists<LHCb::MCHits>(m_hitLocation);
  if (!hits) {
    error() << "No hits in " << m_hitLocation << endmsg;
    return StatusCode::FAILURE;
  }
  // Loop over the hits and calculate the deposited charge on each pixel.
  LHCb::MCVPDeposits* deposits = new LHCb::MCVPDeposits();
  deposits->reserve(5 * hits->size());
  LHCb::MCHits::const_iterator ith;
  for (ith = hits->begin(); ith != hits->end(); ++ith) {
    LHCb::MCHit* hit = *ith;
    createDeposits(hit, deposits);
  }
  // Sort the deposits by channel ID.
  std::stable_sort(deposits->begin(), deposits->end(),
                   VPDataFunctor::Less_by_Channel<const LHCb::MCVPDeposit*>());
  // Store the deposits on the transient store.
  put(deposits, m_depositLocation);
  // Sum up the deposits on each pixel and create MC digits.
  LHCb::MCVPDigits* digits = new LHCb::MCVPDigits();
  LHCb::VPChannelID lastChannel;
  LHCb::MCVPDigit* digit = NULL;
  LHCb::MCVPDeposits::const_iterator itd;
  for (itd = deposits->begin(); itd != deposits->end(); ++itd) {
    if (!digit) {
      // Create the first digit.
      digit = new LHCb::MCVPDigit();
      lastChannel = (*itd)->channelID();
    } else {
      // Check if the deposits belongs to a new channel.
      if ((*itd)->channelID() != lastChannel) {
        digits->insert(digit, lastChannel);
        digit = new LHCb::MCVPDigit();
        lastChannel = (*itd)->channelID();
      }
    } 
    digit->addToMcDeposit(*itd);
  }
  // Add the last digit to the list.
  if (digit) digits->insert(digit, lastChannel);
  // Store the MC digits on the transient store.
  put(digits, m_digitLocation);
  return StatusCode::SUCCESS;

}


//============================================================================
/// Create MCVPDeposits
//============================================================================
void VPDepositCreator::createDeposits(LHCb::MCHit* hit, 
                                      LHCb::MCVPDeposits*& deposits) {

  // Calculate total number of electrons based on energy deposit from G4.
  const double charge = (hit->energy() / Gaudi::Units::eV) / m_eVPerElectron;
  // Calculate the number of points to distribute the deposited charge on.
  const double path = hit->pathLength();
  unsigned int nPoints = int(ceil(path / m_stepSize));
  if (nPoints > m_nMaxSteps) nPoints = m_nMaxSteps;
  // Calculate charge on each point.
  std::vector<double> charges(nPoints);
  double sum = 0.;
  const double mpv = m_chargeUniform * (path / Gaudi::Units::micrometer) / nPoints;
  const double sigma = sqrt(fabs(mpv));
  for (unsigned int i = 0; i < nPoints; ++i) {
    const double q = mpv + m_gauss() * sigma; 
    if (q > 0.) {
      charges[i] = q;
      sum += q;
    }
  }
  // Skip very small deposits.
  if (sum < 1.) return;
  while (charge > sum + m_minChargeTail) {
    // Add additional charge sampled from 1 / n^2 distribution.
    const double q = randomTail(m_minChargeTail, charge - sum);
    const unsigned int i = int(LHCb::Math::round(m_uniform() * (nPoints - 1)));
    charges[i] += q;
    sum += q;
  }

  // Get the sensor that was hit and its thickness.
  const DeVPSensor* sensor = m_det->sensor(hit->sensDetID());
  const double thickness = sensor->siliconThickness();
  double activeDepth = thickness;
  if (m_irradiated) {
    const double f = m_radDamageTool->fluence(hit->midPoint(), m_dataTaken);
    activeDepth *= m_radDamageTool->chargeCollectionEfficiency(f, m_biasVoltage);
  }
  // Calculate the distance between two points on the trajectory.
  Gaudi::XYZVector step = hit->displacement() / static_cast<double>(nPoints);
  // Coordinates of the first point on the trajectory.
  Gaudi::XYZPoint point = hit->entry() + 0.5 * step;
  // Calculate scaling factor to match total deposited charge.
  const double adjust = charge / sum;
  // Accumulate deposits on pixels.
  std::map<LHCb::VPChannelID, double> pixels;
  for (unsigned int i = 0; i < nPoints; ++i) {
    charges[i] *= adjust;
#ifdef DEBUG_HISTO
    plot(charges[i], "ChargePerPoint",
         "Number of electrons per point", 0., 2000., 100);
#endif
    double dz = 0.;
    if (m_oldXml) {
      dz = fabs(point.z() - hit->entry().z()); 
    } else {
      // Pixel side is always at z = -thickness/2 in local coordinates.
      Gaudi::XYZPoint localPoint = sensor->globalToLocal(point);
      dz = fabs(localPoint.z() + 0.5 * thickness);
    }
    if (m_irradiated) {
      if (dz > activeDepth){
        point += step;
        continue;
      }
    }
    const double sigmaD = m_diffusionCoefficient * sqrt(dz);
    const unsigned int nSplit = 5;
    const double q = charges[i] / nSplit; 
    for (unsigned int j = 0; j < nSplit; ++j) {
      const double dx = sigmaD * m_gauss();
      const double dy = sigmaD * m_gauss();
      Gaudi::XYZPoint endpoint = sensor->globalToLocal(point) + Gaudi::XYZVector(dx, dy, 0.);
      LHCb::VPChannelID channel;
      StatusCode valid = sensor->pointToChannel(sensor->localToGlobal(endpoint), channel);
      if (valid) {
        if (pixels.find(channel) == pixels.end()) {
          pixels[channel] = q;
        } else {
          pixels[channel] += q;
        }
      }
#ifdef DEBUG_HISTO
      if (valid) {
        plot2D(endpoint.x(), endpoint.y(), "ActiveSensorArea", 
               "Active sensor area [mm]", -40.0, 40.0, -40.0, 40.0, 160, 160);
      } else {
        plot2D(endpoint.x(), endpoint.y(), "DeadSensorArea", 
               "Dead sensor area [mm]", -40.0, 40.0, -40.0, 40.0, 160, 160);
      }
#endif
    }
    point += step;
  }
  std::map<LHCb::VPChannelID, double>::iterator itm;
  for (itm = pixels.begin(); itm != pixels.end(); ++itm) {
    LHCb::MCVPDeposit* deposit = new LHCb::MCVPDeposit((*itm).second, (*itm).first, hit);
    deposits->insert(deposit);
  }

}

//=============================================================================
/// Sample charge from 1 / n^2 distribution.
//=============================================================================
double VPDepositCreator::randomTail(const double qmin, const double qmax) {

  const double offset = 1. / qmax;
  const double range = (1. / qmin) - offset;
  const double u = offset + m_uniform() * range;
  return 1. / u;

}

