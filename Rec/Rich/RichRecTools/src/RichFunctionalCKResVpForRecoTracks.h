
//----------------------------------------------------------------------------------------
/** @file RichFunctionalCKResVpForRecoTracks.h
 *
 *  Header file for tool : RichFunctionalCKResVpForRecoTracks
 *
 *  CVS Log :-
 *  $Id: RichFunctionalCKResVpForRecoTracks.h,v 1.3 2006-01-23 14:20:44 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   17/10/2004
 */
//----------------------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RICHFUNCTIONALCKRESVPFORRECOTRACKS_H
#define RICHRECTOOLS_RICHFUNCTIONALCKRESVPFORRECOTRACKS_H 1

// base class
#include "RichRecBase/RichRecHistoToolBase.h"

// interfaces
#include "RichRecBase/IRichCherenkovResolution.h"
#include "RichRecBase/IRichCherenkovAngle.h"
#include "RichKernel/IRichRefractiveIndex.h"
#include "RichKernel/IRichParticleProperties.h"

// GSL
#include "gsl/gsl_math.h"

// temporary histogramming numbers
#include "RichRecBase/RichDetParams.h"

//----------------------------------------------------------------------------------------
/** @class RichFunctionalCKResVpForRecoTracks RichFunctionalCKResVpForRecoTracks.h
 *
 *  Provides an analytical calculation of the expected Cherenkoc angle resolution.
 *
 *  Based on results discussed in LHCb-2004-057
 *  see http://cdsweb.cern.ch/search.py?recid=815494&ln=en
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2004-04-07
 */
//----------------------------------------------------------------------------------------

class RichFunctionalCKResVpForRecoTracks : public RichRecHistoToolBase,
                                           virtual public IRichCherenkovResolution
{

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichFunctionalCKResVpForRecoTracks( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent);

  /// Destructor
  virtual ~RichFunctionalCKResVpForRecoTracks() {};

  // Initialize method
  StatusCode initialize();

  // Finalize method
  StatusCode finalize();

public: // methods (and doxygen comments) inherited from public interface

  // Photon resolution
  double ckThetaResolution( LHCb::RichRecSegment * segment,
                            const Rich::ParticleIDType id = Rich::Pion ) const;

private:

  /// Pointer to RichCherenkovAngle interface
  const IRichCherenkovAngle * m_ckAngle;

  /// Pointer to refractive index tool
  const IRichRefractiveIndex * m_refIndex;

  /// Pointer to RichParticleProperties interface
  const IRichParticleProperties * m_richPartProp;

  std::vector<double> m_chromFact; ///< Chromatic error factor for each radiator

  std::vector<double> m_matThickness; ///< Material thickness infront of each radiator

  double m_scatt; ///< Scattering coefficent

  std::vector<double> m_curvX; ///< x curvature error parameter
  std::vector<double> m_curvY; ///< y curvature error parameter

  /// Asymtopic Errors
  std::vector<double> m_asmpt[Rich::NRadiatorTypes];

};

#endif // RICHRECTOOLS_RICHFUNCTIONALCKRESVPFORRECOTRACKS_H
