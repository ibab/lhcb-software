
/** @file RichFunctionalRayleighScatter.h
 *
 *  Header file for tool : RichFunctionalRayleighScatter
 *
 *  CVS Log :-
 *  $Id: RichFunctionalRayleighScatter.h,v 1.5 2004-07-27 20:15:30 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

#ifndef RICHRECTOOLS_RICHFUNCTIONALRAYLEIGHSCATTER_H
#define RICHRECTOOLS_RICHFUNCTIONALRAYLEIGHSCATTER_H 1

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"

// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// RichDet
#include "RichDet/DeRich1.h"

// Event model
#include "Event/RichRecSegment.h"

// interfaces
#include "RichRecBase/IRichRayleighScatter.h"

/** @class RichFunctionalRayleighScatter RichFunctionalRayleighScatter.h
 *
 *  Tool to calculate quantities related to Rayleigh scattering using an
 *  analytic functional form.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class RichFunctionalRayleighScatter : public RichRecToolBase,
                                      virtual public IRichRayleighScatter {

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichFunctionalRayleighScatter( const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent );

  /// Destructor
  virtual ~RichFunctionalRayleighScatter() {};

  // Initialize method
  StatusCode initialize();

  // Finalize method
  StatusCode finalize();

public: // methods (and doxygen comments) inherited from public interface

  // Rayleigh scattering probability for given pathlength (segment) and photyon energy
  double photonScatteredProb( const RichRecSegment * segment,
                              const double energy ) const;

private:  // Private data

  // Rayleigh sattering parameters
  double m_eVToMicron;  ///< To converter between energy in eV to wavelenght in micrometers
  double m_AeroClarity; ///< The areogel clarity

};

#endif // RICHRECTOOLS_RICHFUNCTIONALRAYLEIGHSCATTER_H
