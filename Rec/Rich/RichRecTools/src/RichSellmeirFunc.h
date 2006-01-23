
//-----------------------------------------------------------------------------
/** @file RichSellmeirFunc.h
 *
 *  Header file for tool : RichSellmeirFunc
 *
 *  CVS Log :-
 *  $Id: RichSellmeirFunc.h,v 1.11 2006-01-23 14:20:44 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RICHSELLMEIRFUNC_H
#define RICHRECTOOLS_RICHSELLMEIRFUNC_H 1

// base class
#include "RichRecBase/RichRecToolBase.h"

// Kernel
#include "Kernel/RichRadiatorType.h"
#include "Kernel/RichParticleIDType.h"
#include "RichKernel/BoostArray.h"

// Event model
#include "Event/RichRecSegment.h"

// Detector Description
#include "RichDet/DeRich1.h"

// interfaces
#include "RichRecBase/IRichSellmeirFunc.h"
#include "RichKernel/IRichParticleProperties.h"

//-----------------------------------------------------------------------------
/** @class RichSellmeirFunc RichSellmeirFunc.h
 *
 *  Tool to calculate quantities using the Sellmeir function and related
 *  parameters.
 *
 *  Uses formulae 37-39 in CERN-EP/89-150  (Ypsilantis)
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

class RichSellmeirFunc : public RichRecToolBase,
                         virtual public IRichSellmeirFunc
{

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichSellmeirFunc( const std::string& type,
                    const std::string& name,
                    const IInterface* parent );

  /// Destructor
  virtual ~RichSellmeirFunc() {};

  // Initialize method
  StatusCode initialize();

  // Finalize method
  StatusCode finalize();

public: // methods (and doxygen comments) inherited from public interface

  // Computes the number of photons emitted in the given energy range for a
  // given RichRecSegment under a certain mass hypothesis
  double photonsInEnergyRange( LHCb::RichRecSegment * segment,
                               const Rich::ParticleIDType id,
                               const double botEn,
                               const double topEn ) const;

private: // methods

  /// internal calculation for photon yield
  double paraW( const Rich::RadiatorType rad, const double energy ) const;

private: // data

  // Sellmeir parameters
  double m_selF1[Rich::NRadiatorTypes];
  double m_selF2[Rich::NRadiatorTypes];
  double m_selE1[Rich::NRadiatorTypes];
  double m_selE2[Rich::NRadiatorTypes];
  double m_molW[Rich::NRadiatorTypes];
  double m_rho[Rich::NRadiatorTypes];

  // Aerogel specific parameters
  double m_waveIndepTrans;

  // cached parameters for speed
  double m_RXSPscale[Rich::NRadiatorTypes];
  double m_RXSMscale[Rich::NRadiatorTypes];
  double m_REP[Rich::NRadiatorTypes];
  double m_REM[Rich::NRadiatorTypes];
  double m_X[Rich::NRadiatorTypes];

  // particle hypothesis masses squared
  boost::array<double,Rich::NParticleTypes> m_particleMassSq;

};

#endif // RICHRECTOOLS_RICHSELLMEIRFUNC_H
