
//-----------------------------------------------------------------------------
/** @file RichTabulatedRayleighScatter.h
 *
 *  Header file for tool : RichTabulatedRayleighScatter
 *
 *  CVS Log :-
 *  $Id: RichTabulatedRayleighScatter.h,v 1.8 2006-01-23 14:20:44 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RICHTABULATEDRAYLEIGHSCATTER_H
#define RICHRECTOOLS_RICHTABULATEDRAYLEIGHSCATTER_H 1

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"

// constants
#include "Kernel/PhysicalConstants.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// Detector Description
#include "DetDesc/DetectorElement.h"

// RichKernel
#include "RichKernel/Rich1DTabProperty.h"

// Event model
#include "Event/RichRecSegment.h"

// RichDet
#include "RichDet/DeRichRadiator.h"

// interfaces
#include "RichRecBase/IRichRayleighScatter.h"

//-----------------------------------------------------------------------------
/** @class RichTabulatedRayleighScatter RichTabulatedRayleighScatter.h
 *
 *  Tool to calculate quantities related to Rayleigh scattering using
 *  tabulated quantities in the XML database.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

class RichTabulatedRayleighScatter : public RichRecToolBase,
                                     virtual public IRichRayleighScatter
{

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichTabulatedRayleighScatter( const std::string& type,
                                const std::string& name,
                                const IInterface* parent );

  /// Destructor
  virtual ~RichTabulatedRayleighScatter() {};

  // Initialize method
  StatusCode initialize();

  // Finalize method
  StatusCode finalize();

public: // methods (and doxygen comments) inherited from public interface

  // Rayleigh scattering probability for given pathlength (segment) and photyon energy
  double photonScatteredProb( const LHCb::RichRecSegment * segment,
                              const double energy ) const;

private:  // Private data

  /// Tabulated Rayleigh scattering length
  Rich1DTabProperty * m_rayScatL;

};

#endif // RICHRECTOOLS_RICHTABULATEDRAYLEIGHSCATTER_H
