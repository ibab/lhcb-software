// $Id: RichGeomEffDetailed.h,v 1.2 2003-08-06 11:08:12 jonrob Exp $
#ifndef RICHRECTOOLS_RICHGEOMEFFDETAILED_H
#define RICHRECTOOLS_RICHGEOMEFFDETAILED_H 1

// from Gaudi
#include "GaudiKernel/RndmGenerators.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// Event model
#include "Event/RichRecSegment.h"

// interfaces
#include "RichRecBase/IRichCherenkovAngle.h"
#include "RichRecBase/IRichGeomEff.h"
#include "RichDetTools/IRichDetInterface.h"

/** @class RichGeomEffDetailed RichGeomEffDetailed.h
 *
 *  Tool to perform a full detailed calculation of the geometrical
 *  efficiency for a given RichRecSegment and mass hypothesis.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class RichGeomEffDetailed : public RichRecToolBase,
                            virtual public IRichGeomEff {

public:

  /// Standard constructor
  RichGeomEffDetailed( const std::string& type,
                       const std::string& name,
                       const IInterface* parent );

  /// Destructor
  virtual ~RichGeomEffDetailed() {};

  /// Initialize method
  StatusCode initialize();

  /// Finalize method
  StatusCode finalize();

  /// Obtain geometrical efficiency for this track and hypothesis
  double geomEfficiency ( RichRecSegment * segment,
                          const Rich::ParticleIDType id );

  /// Obtain scattered geometrical efficiency for this track and hypothesis
  double geomEfficiencyScat ( RichRecSegment * segment,
                              const Rich::ParticleIDType id );

private: // Private data

  // Pointers to tool instances
  IRichDetInterface * m_richDetInt;
  IRichCherenkovAngle * m_ckAngle;
 
  /// Number of photons to use in geometrical efficiency calculation
  int m_nGeomEff;

  /// Number of photons to quit after in geometrical efficiency calculation
  /// if all so far have failed
  int m_nGeomEffBailout;

  /// Increment parameter for HPD efficiencies
  double m_hpdInc;

  /// Cherenkov phi incrementation
  double m_incPhi;

  /// Cache Sin(angle) for geometrical efficiency calculation
  std::vector<double> m_sinCkPhi;
  /// Cache Cos(angle) for geometrical efficiency calculation
  std::vector<double> m_cosCkPhi;

  /// Flat Randomn distribution between 0 and 1
  Rndm::Numbers m_uniDist;

};

#endif // RICHRECTOOLS_RICHGEOMEFFDETAILED_H
