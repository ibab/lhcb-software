// $Id: RichSellmeirFunc.h,v 1.1 2003-08-06 11:08:14 jonrob Exp $
#ifndef RICHRECTOOLS_RICHSELLMEIRFUNC_H
#define RICHRECTOOLS_RICHSELLMEIRFUNC_H 1

// base class
#include "RichRecBase/RichRecToolBase.h"

// RichKernel
#include "RichKernel/RichRadiatorType.h"
#include "RichKernel/RichParticleIDType.h"

// Event model
#include "Event/RichRecSegment.h"

// interfaces
#include "RichRecBase/IRichSellmeirFunc.h"

/** @class RichSellmeirFunc RichSellmeirFunc.h
 *
 *  Tool to calculate the Sellmeir parameters
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class RichSellmeirFunc : public RichRecToolBase,
                         virtual public IRichSellmeirFunc {

public:

  /// Standard constructor
  RichSellmeirFunc( const std::string& type,
                    const std::string& name,
                    const IInterface* parent );

  /// Destructor
  virtual ~RichSellmeirFunc() {};

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

  /// Array containing particle masses
  std::vector<double> m_particleMass;

  /// Array containing particle masses
  std::vector<double> m_particleMassSq;

};

#endif // RICHRECTOOLS_RICHSELLMEIRFUNC_H
