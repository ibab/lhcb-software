
//-----------------------------------------------------------------------------
/** @file RichTabulatedRayleighScatter.h
 *
 *  Header file for tool : RichTabulatedRayleighScatter
 *
 *  CVS Log :-
 *  $Id: RichTabulatedRayleighScatter.h,v 1.9 2006-03-02 15:29:20 jonrob Exp $
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
 *
 *  @todo Figure out how to properly deal with multiple aerogel volumes
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

  /// Pointer to aerogel detector element
  const DeRichRadiator * m_aero;

};

#endif // RICHRECTOOLS_RICHTABULATEDRAYLEIGHSCATTER_H
