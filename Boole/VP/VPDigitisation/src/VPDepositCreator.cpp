#include <vector>
#include <map>

// Gaudi
#include "GaudiKernel/PhysicalConstants.h"

// LHCb
// Kernel/LHCbKernel
#include "Kernel/VPDataFunctor.h"
#include "Kernel/VPChannelID.h"
// Kernel/LHCbMath
#include "LHCbMath/LHCbMath.h"
// Event/MCEvent
#include "Event/MCHit.h"
// Det/VPDet
#include "VPDet/DeVP.h"

// Local
#include "VPDepositCreator.h"

DECLARE_ALGORITHM_FACTORY(VPDepositCreator)

//=============================================================================
// Constructor
//=============================================================================
VPDepositCreator::VPDepositCreator(const std::string& name, 
                                   ISvcLocator* pSvcLocator) : 
    GaudiTupleAlg(name, pSvcLocator),
    m_det(NULL), m_radDamageTool(NULL) {

  declareProperty("HitLocation", m_hitLocation = LHCb::MCHitLocation::VP);
  declareProperty("DigitLocation", m_digitLocation = LHCb::MCVPDigitLocation::Default);

  declareProperty("StepSize", m_stepSize = 5 * Gaudi::Units::micrometer);
  declareProperty("MaxNumSteps", m_nMaxSteps = 150);

  declareProperty("eVPerElectron", m_eVPerElectron = 3.6);
  declareProperty("ChargeUniform", m_chargeUniform = 70.0);
  declareProperty("MinChargeTail", m_minChargeTail = 10.0);

  declareProperty("Temperature", m_temperature = 253.15 * Gaudi::Units::kelvin);
  declareProperty("BiasVoltage", m_biasVoltage = 105.0);

  declareProperty("Irradiated", m_irradiated = false);
  declareProperty("DataTaken", m_dataTaken = 0.);

  declareProperty("Monitoring", m_monitoring = false);

}

//=============================================================================
// Destructor
//=============================================================================
VPDepositCreator::~VPDepositCreator() {}

//=============================================================================
// Initialisation
//=============================================================================
StatusCode VPDepositCreator::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;

  // Get the detector element.
  m_det = getDet<DeVP>(DeVPLocation::Default);

  // Get the radiation damage tool.
  if (m_irradiated) {
    m_radDamageTool = tool<VPRadiationDamageTool>("VPRadiationDamageTool");
  }

  // Initialize the random number generators.
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

  // Calculate the diffusion coefficient.
  const double kt = m_temperature * Gaudi::Units::k_Boltzmann / Gaudi::Units::eV;
  const double thickness = m_det->sensor(0)->siliconThickness();
  if (m_irradiated) {
    m_diffusionCoefficient = sqrt(2. * kt * thickness / 100.);
  } else {
    m_diffusionCoefficient = sqrt(2. * kt * thickness / m_biasVoltage);
  }
  setHistoTopDir("VP/");
  return StatusCode::SUCCESS;

}

//=========================================================================
//  Main execution
//=========================================================================
StatusCode VPDepositCreator::execute() {

  // Pick up the MC hits.
  const LHCb::MCHits* hits = getIfExists<LHCb::MCHits>(m_hitLocation);
  if (!hits) {
    error() << "No hits in " << m_hitLocation << endmsg;
    return StatusCode::FAILURE;
  }

  // Create a container for the MC digits and transfer ownership to the TES. 
  m_digits = new LHCb::MCVPDigits();
  put(m_digits, m_digitLocation);

  // Loop over the hits and calculate the deposited charge on each pixel.
  LHCb::MCHits::const_iterator ith;
  for (ith = hits->begin(); ith != hits->end(); ++ith) createDeposits(*ith);

  // Sort the MC digits by channel ID.
  std::stable_sort(m_digits->begin(), m_digits->end(),
                   VPDataFunctor::Less_by_Channel<const LHCb::MCVPDigit*>());
  return StatusCode::SUCCESS;

}


//============================================================================
// Create deposits
//============================================================================
void VPDepositCreator::createDeposits(LHCb::MCHit* hit) {

  // Calculate the total number of electrons based on the G4 energy deposit.
  const double charge = (hit->energy() / Gaudi::Units::eV) / m_eVPerElectron;
  // Skip very small deposits.
  if (charge < 100.) return;
  const double path = hit->pathLength();
  // Skip very small path lengths.
  if (path < 1.e-6) {
    warning() << "Path length in active silicon: " << path << endmsg;
    return;
  }
  // Calculate the number of points to distribute the deposited charge on.
  unsigned int nPoints = int(ceil(path / m_stepSize));
  if (nPoints > m_nMaxSteps) nPoints = m_nMaxSteps;
  // Calculate the charge on each point.
  std::vector<double> charges(nPoints, 0.);
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
  while (charge > sum + m_minChargeTail) {
    // Add additional charge sampled from an 1 / n^2 distribution.
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
  Gaudi::XYZPoint entry = sensor->globalToLocal(hit->entry());
  Gaudi::XYZPoint exit = sensor->globalToLocal(hit->exit());
  Gaudi::XYZVector step = (exit - entry) / static_cast<double>(nPoints);
  // Coordinates of the first point on the trajectory.
  Gaudi::XYZPoint point = entry + 0.5 * step;
  // Calculate scaling factor to match total deposited charge.
  const double adjust = charge / sum;
  // Accumulate deposits on pixels.
  std::map<LHCb::VPChannelID, double> pixels;
  for (unsigned int i = 0; i < nPoints; ++i) {
    charges[i] *= adjust;
    if (m_monitoring) {
      plot(charges[i], "ChargePerPoint",
           "Number of electrons per point", 0., 2000., 100);
    }
    // Calculate the distance to the pixel side of the sensor.
    // In local coordinates, the pixel side is at z = -thickness/2.
    const double dz = fabs(point.z() + 0.5 * thickness);
    if (m_irradiated) {
      // Skip points outside the active depth.
      if (dz > activeDepth) {
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
      Gaudi::XYZPoint endpoint = point + Gaudi::XYZVector(dx, dy, 0.);
      LHCb::VPChannelID channel;
      const bool valid = sensor->pointToChannel(endpoint, true, channel);
      if (valid) {
        if (pixels.find(channel) == pixels.end()) {
          pixels[channel] = q;
        } else {
          pixels[channel] += q;
        }
      }
      if (m_monitoring) {
        if (valid) {
          plot2D(endpoint.x(), endpoint.y(), "ActiveSensorArea", 
                 "Active sensor area [mm]", -40.0, 40.0, -40.0, 40.0, 160, 160);
        } else {
          plot2D(endpoint.x(), endpoint.y(), "DeadSensorArea", 
                 "Dead sensor area [mm]", -40.0, 40.0, -40.0, 40.0, 160, 160);
        }
      }
    }
    point += step;
  }
  for (auto itm = pixels.cbegin(), end = pixels.cend(); itm != end; ++itm) {
    LHCb::VPChannelID id = (*itm).first;
    const double charge = (*itm).second; 
    LHCb::MCVPDigit* digit = m_digits->object(id);
    if (digit) {
      // MC digit already exists.
      digit->addToMcHits(hit, charge);
    } else {
      digit = new LHCb::MCVPDigit();
      digit->addToMcHits(hit, charge);
      m_digits->insert(digit, id);
    } 
  }

}

//=============================================================================
// Sample charge from 1 / n^2 distribution.
//=============================================================================
double VPDepositCreator::randomTail(const double qmin, const double qmax) {

  const double offset = 1. / qmax;
  const double range = (1. / qmin) - offset;
  const double u = offset + m_uniform() * range;
  return 1. / u;

}

