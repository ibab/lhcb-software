// STL
#include <vector>
#include <map>

// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/PhysicalConstants.h"

// LHCb
// Kernel/LHCbKernel
#include "Kernel/VPDataFunctor.h"
#include "Kernel/VPChannelID.h"
#include "LHCbMath/LHCbMath.h"
// Event/MCEvent
#include "Event/MCHit.h"
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
    m_det(NULL), m_radDamageTool(NULL) {

  declareProperty("HitLocation",   m_hitLocation = LHCb::MCHitLocation::VP);
  declareProperty("DigitLocation", m_digitLocation = LHCb::MCVPDigitLocation::Default);

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
  m_digits = new LHCb::MCVPDigits();
  // Loop over the hits and calculate the deposited charge on each pixel.
  LHCb::MCHits::const_iterator ith;
  for (ith = hits->begin(); ith != hits->end(); ++ith) createDeposits(*ith);
  // Sort the MC digits by channel ID.
  std::stable_sort(m_digits->begin(), m_digits->end(),
                   VPDataFunctor::Less_by_Channel<const LHCb::MCVPDigit*>());
  // Store the MC digits on the transient store.
  put(m_digits, m_digitLocation);
  return StatusCode::SUCCESS;

}


//============================================================================
/// Create deposits
//============================================================================
void VPDepositCreator::createDeposits(LHCb::MCHit* hit) {

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
    Gaudi::XYZPoint localPoint = sensor->globalToLocal(point);
    charges[i] *= adjust;
#ifdef DEBUG_HISTO
    plot(charges[i], "ChargePerPoint",
         "Number of electrons per point", 0., 2000., 100);
#endif
    double dz = 0.;
    // Pixel side is always at z = -thickness/2 in local coordinates.
    dz = fabs(localPoint.z() + 0.5 * thickness);
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
      Gaudi::XYZPoint endpoint = localPoint + Gaudi::XYZVector(dx, dy, 0.);
      LHCb::VPChannelID channel;
      StatusCode valid = sensor->pointToChannel(endpoint, true, channel);
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
/// Sample charge from 1 / n^2 distribution.
//=============================================================================
double VPDepositCreator::randomTail(const double qmin, const double qmax) {

  const double offset = 1. / qmax;
  const double range = (1. / qmin) - offset;
  const double u = offset + m_uniform() * range;
  return 1. / u;

}

