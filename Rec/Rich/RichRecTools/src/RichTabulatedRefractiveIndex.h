// $Id: RichTabulatedRefractiveIndex.h,v 1.1 2003-08-26 14:40:21 jonrob Exp $
#ifndef RICHRECTOOLS_RICHTABULATEDREFRACTIVEINDEX_H
#define RICHRECTOOLS_RICHTABULATEDREFRACTIVEINDEX_H 1

// base class
#include "RichRecBase/RichRecToolBase.h"

// RichUtils
#include "RichUtils/Rich1DTabProperty.h"

// Event model
#include "Event/RichRecSegment.h"

// boost
#include "RichKernel/BoostArray.h"

// interfaces
#include "RichRecBase/IRichRefractiveIndex.h"
#include "RichDetTools/IRichDetInterface.h"

/** @class RichTabulatedRefractiveIndex RichTabulatedRefractiveIndex.h
 *
 *  Tool to calculate the effective refractive index for
 *  a given RichRecSegment. An implementation that uses the tabulated
 *  information from the XML
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class RichTabulatedRefractiveIndex : public RichRecToolBase,
                                     virtual public IRichRefractiveIndex {

public:

  /// Standard constructor
  RichTabulatedRefractiveIndex( const std::string& type,
                                const std::string& name,
                                const IInterface* parent );

  /// Destructor
  virtual ~RichTabulatedRefractiveIndex() {};

  /// Initialize method
  StatusCode initialize();

  /// Finalize method
  StatusCode finalize();

  /// Average refractive index for this segments radiator
  double refractiveIndex( RichRecSegment * segment );

  /// Refractive index for a given energy
  double refractiveIndex( const Rich::RadiatorType radiator,
                          double energy );

  /// Average refractive index for given radiator
  double refractiveIndex( const Rich::RadiatorType radiator );

private:  // Private data

  /// Pointers to tool instances
  IRichDetInterface * m_richDetInt;

  /// Quantum Efficiency function.
  Rich1DTabProperty * m_referenceQE;

  /// Pointers to refractive indices for each radiator type
  typedef boost::array<Rich1DTabProperty*,Rich::NRadiatorTypes> RefractiveIndices;
  RefractiveIndices m_refIndex;

  /// vector of locations in XML for refractive indices
  std::vector<std::string> m_refLocations;

};

#endif // RICHRECTOOLS_RICHTABULATEDREFRACTIVEINDEX_H
