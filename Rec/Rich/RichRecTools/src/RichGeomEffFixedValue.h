
/** @file RichGeomEffFixedValue.h
 *
 *  Header file for tool : RichGeomEffFixedValue
 *
 *  CVS Log :-
 *  $Id: RichGeomEffFixedValue.h,v 1.4 2004-07-27 20:15:30 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

#ifndef RICHRECTOOLS_RICHGEOMEFFFIXEDVALUE_H
#define RICHRECTOOLS_RICHGEOMEFFFIXEDVALUE_H 1

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

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichGeomEffFixedValue( const std::string& type,
                         const std::string& name,
                         const IInterface* parent );

  /// Destructor
  virtual ~RichGeomEffFixedValue() {};

  // Initialize method
  StatusCode initialize();

  // Finalize method
  StatusCode finalize();

public: // methods (and doxygen comments) inherited from public interface

  // Obtain geometrical efficiency for this track and hypothesis
  double geomEfficiency ( RichRecSegment * segment,
                          const Rich::ParticleIDType id ) const;

  // Obtain scattered geometrical efficiency for this track and hypothesis
  double geomEfficiencyScat ( RichRecSegment * segment,
                              const Rich::ParticleIDType id ) const;

private: // Private data

  // Pointers to tool instances
  IRichCherenkovAngle * m_ckAngle; ///< Cherenkov angle tool

  std::vector<double> m_fixedValue; ///< vector of fixed geometrical efficiency values
  double m_fixedScatValue; ///< Scatter efficiency

};

#endif // RICHRECTOOLS_RICHGEOMEFFFIXEDVALUE_H
