// $Id: RichSellmeirFuncSICB.h,v 1.2 2003-10-13 16:32:34 jonrob Exp $
#ifndef RICHRECTOOLS_RICHSELLMEIRFUNCSICB_H
#define RICHRECTOOLS_RICHSELLMEIRFUNCSICB_H 1

// base class
#include "RichRecBase/RichRecToolBase.h"

// RichKernel
#include "RichKernel/RichRadiatorType.h"
#include "RichKernel/RichParticleIDType.h"
#include "RichKernel/BoostArray.h"

// Event model
#include "Event/RichRecSegment.h"

// interfaces
#include "RichRecBase/IRichSellmeirFunc.h"
#include "RichRecBase/IRichParticleProperties.h"

/** @class RichSellmeirFuncSICB RichSellmeirFuncSICB.h
 *
 *  Tool to calculate the Sellmeir parameters
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class RichSellmeirFuncSICB : public RichRecToolBase,
                             virtual public IRichSellmeirFunc {

public:

  /// Standard constructor
  RichSellmeirFuncSICB( const std::string& type,
                        const std::string& name,
                        const IInterface* parent );

  /// Destructor
  virtual ~RichSellmeirFuncSICB() {};

  /// Initialize method
  StatusCode initialize();

  /// Finalize method
  StatusCode finalize();

  double paraWt ( Rich::RadiatorType rad, double topEn );

  double paraWb ( Rich::RadiatorType rad, double botEn );

  /// Computes the number of photons emitted in the given energy range for a
  /// given RichRecSegment under a certain mass hypothesis
  double photonsInEnergyRange( RichRecSegment * segment,
                               const Rich::ParticleIDType id,
                               double botEn,
                               double topEn );

private:  // Private data

  // Sellmeir parameters
  double m_selF1[Rich::NRadiatorTypes];
  double m_selF2[Rich::NRadiatorTypes];
  double m_selE1[Rich::NRadiatorTypes];
  double m_selE2[Rich::NRadiatorTypes];
  double m_molW[Rich::NRadiatorTypes];
  double m_rho[Rich::NRadiatorTypes];

  // Aerogel specific parameters
  double m_waveIndepTrans;

  /// particle hypothesis masses squared
  boost::array<double,Rich::NParticleTypes> m_particleMassSq;

};

#endif // RICHRECTOOLS_RICHSELLMEIRFUNCSICB_H
