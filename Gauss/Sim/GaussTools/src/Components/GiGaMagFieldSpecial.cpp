// Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IMagneticFieldSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/SystemOfUnits.h"

// Local
#include "GiGaMagFieldSpecial.h"

/** @file
 *
 *  implementation of class GiGaMagFieldSpecial
 *
 */

DECLARE_TOOL_FACTORY(GiGaMagFieldSpecial)

// ============================================================================
/** standard constructor
 *  @see GiGaMagFieldBase
 *  @see GiGaBase
 *  @see AlgTool
 *  @param type type of the object (?)
 *  @param name name of the object
 *  @param parent  pointer to parent object
 */
// ============================================================================
GiGaMagFieldSpecial::GiGaMagFieldSpecial(const std::string& type,
                                         const std::string& name,
                                         const IInterface* parent)
    : GiGaMagFieldBase(type, name, parent),
      m_ByUniform(0.0),
      m_QuadGrad(0.0),
      m_LHCbFieldMax(0.0),
      m_Type(0) {
  declareProperty("Type", m_Type = 0);
  declareProperty("ByUniform", m_ByUniform);
  declareProperty("QuadGrad", m_QuadGrad);
  declareProperty("LHCbFieldMax",
                  m_LHCbFieldMax = 1.06476 * Gaudi::Units::tesla);
};

// ============================================================================
// destructor
// ============================================================================
GiGaMagFieldSpecial::~GiGaMagFieldSpecial() {};

// ============================================================================
/** get the field value
 *  @see G4MagneticField
 *  @param point point
 *  @param B value of magnetic field in point
 */
// ============================================================================
void GiGaMagFieldSpecial::GetFieldValue(const double partvec[4],
                                        double* B) const {
  B[0] = 0.0;
  B[1] = 0.0;
  B[2] = 0.0;

  const double& z = partvec[2];

  // Fields units Tesla=1 (sorry for caps, no reason)
  const double bmax = m_LHCbFieldMax;
  const double zmean = 525.0 * Gaudi::Units::cm;
  const double zsig = 158.07 * Gaudi::Units::cm;
  const double sign = z / abs(z);
  double mu = 0.;
  switch (m_Type) {
    // Uniform field, but invert Z for
    case UniformInvertZ:
      B[1] = sign * m_ByUniform;
      break;

    // Quadrupole fields
    case QuadGrad:
      B[0] = partvec[1] * m_QuadGrad;
      B[1] = partvec[0] * m_QuadGrad;
      break;
    case QuadGradInvertZ:
      B[0] = sign * partvec[1] * m_QuadGrad;
      B[1] = sign * partvec[0] * m_QuadGrad;
      break;

    // LHCb approximate field
    case LHCbUp:
    case LHCbDown:
      mu = (z - zmean);
      if (fabs(mu) < zsig * 4.0) {
        const double fieldLHCb = bmax * exp(-mu * mu / (2.0 * zsig * zsig));
        double fac = 1.0;
        if (m_Type == LHCbDown) fac = -1.0;
        B[1] = fac * fieldLHCb;
      }
      break;

    default:
      error() << "GiGaMagFieldSpecial: REQUESTED FIELD TYPE DOES NOT EXIST"
              << endmsg;
      break;
  }
  if (outputLevel() == MSG::DEBUG) {
    debug() << "Magnetic field at (" << partvec[0] << "," << partvec[1] << ","
            << partvec[2] << "), returning (" << B[0] << "," << B[1] << ","
            << B[2] << ")" << endmsg;
  }
};
