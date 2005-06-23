
//-----------------------------------------------------------------------------------------------
/** @file RichPhotonCreatorWithGaussianCKSmear.h
 *
 *  Header file for RICH reconstruction tool : RichPhotonCreatorWithGaussianCKSmear
 *
 *  CVS Log :-
 *  $Id: RichPhotonCreatorWithGaussianCKSmear.h,v 1.4 2005-06-23 15:14:12 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   20/05/2005
 */
//-----------------------------------------------------------------------------------------------

#ifndef RICHRECMCTOOLS_RICHPHOTONCREATORWITHGAUSSIANCKSMEAR_H
#define RICHRECMCTOOLS_RICHPHOTONCREATORWITHGAUSSIANCKSMEAR_H 1

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/RndmGenerators.h"

// base class
#include "RichRecBase/RichPhotonCreatorBase.h"

// Interfaces
#include "RichRecBase/IRichPhotonCreator.h"
#include "RichRecBase/IRichRecMCTruthTool.h"

// RichKernel
#include "RichKernel/BoostArray.h"

// Event model
#include "Event/MCRichOpticalPhoton.h"

//-----------------------------------------------------------------------------------------------
/** @class RichPhotonCreatorWithGaussianCKSmear RichPhotonCreatorWithGaussianCKSmear.h
 *
 *  Tool which first delegates the photon creator to another tool, but then applies
 *  a Gaussian smear to the true Cherenkov photons for each Radiator. 
 *  The about of smearing is seperately configurable for each radiator.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   20/05/2005
 */
//-----------------------------------------------------------------------------------------------

class RichPhotonCreatorWithGaussianCKSmear : public RichPhotonCreatorBase
{

public: // methods for Gaudi framework

  /// Standard constructor
  RichPhotonCreatorWithGaussianCKSmear( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent );

  /// Destructor
  virtual ~RichPhotonCreatorWithGaussianCKSmear(){}

  // Initialize method
  StatusCode initialize();

  // Finalize method
  StatusCode finalize();

private: // private methods

  /// Form a Photon candidate from a Segment and a pixel.
  virtual RichRecPhoton * buildPhoton( RichRecSegment * segment,
                                       RichRecPixel * pixel,
                                       const RichRecPhotonKey key ) const;

  /// Access RICH MC reconstruction tool on demand
  /// Means if not needed, this tool runs MC free and can be used on real data (if wanted)
  const IRichRecMCTruthTool * richMCRecTool() const;

private: // private data

  // Pointers to tool instances
  mutable const IRichRecMCTruthTool * m_mcRecTool;  ///< Rich Reconstruction MC Truth tool
  const IRichPhotonCreator * m_delPhotCr;   ///< Delegated photon creator

  /// Apply smearing to all photons, or only to true photons
  bool m_applySmearingToAll;

  /// Which radiators to apply the smearing to
  std::vector<bool> m_smearRad;

  /// Smearing value to apply to each radiators photons
  std::vector<double> m_smearWid;

  /// Gaussian random distributions
  mutable boost::array< Rndm::Numbers, Rich::NRadiatorTypes > m_rand;

};

inline const IRichRecMCTruthTool * 
RichPhotonCreatorWithGaussianCKSmear::richMCRecTool() const
{
  if ( !m_mcRecTool ) acquireTool( "RichRecMCTruthTool", m_mcRecTool );
  return m_mcRecTool;
}

#endif // RICHRECMCTOOLS_RICHPHOTONCREATORWITHGAUSSIANCKSMEAR_H
