// $Id: RichGeomEffFixedValue.h,v 1.2 2003-11-25 14:06:40 jonrob Exp $
#ifndef RICHRECTOOLS_RICHGEOMEFFFIXEDVALUE_H
#define RICHRECTOOLS_RICHGEOMEFFFIXEDVALUE_H 1

// from Gaudi

// base class
#include "RichRecBase/RichRecToolBase.h"

// Event model
#include "Event/RichRecSegment.h"

// interfaces
#include "RichRecBase/IRichCherenkovAngle.h"
#include "RichRecBase/IRichGeomEff.h"

/** @class RichGeomEffFixedValue RichGeomEffFixedValue.h
 *
 *  Tool to perform a fast determination of the geometrical efficiency for
 *  a given RichRecSegment and mass hypothesis. Uses a fixed value of the signal
 *  and scattered efficiencies
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class RichGeomEffFixedValue : public RichRecToolBase,
                              virtual public IRichGeomEff {

public:

  /// Standard constructor
  RichGeomEffFixedValue( const std::string& type,
                         const std::string& name,
                         const IInterface* parent );

  /// Destructor
  virtual ~RichGeomEffFixedValue() {};

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
  IRichCherenkovAngle * m_ckAngle;

  std::vector<double> m_fixedValue;
  double m_fixedScatValue;

};

#endif // RICHRECTOOLS_RICHGEOMEFFFIXEDVALUE_H
