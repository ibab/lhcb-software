
//-----------------------------------------------------------------------------
/** @file RichSimplePhotonPredictor.h
 *
 *  Header file for tool : RichSimplePhotonPredictor
 *
 *  CVS Log :-
 *  $Id: RichSimplePhotonPredictor.h,v 1.1 2005-06-17 15:08:37 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RICHPHOTONPREDICTOR_H
#define RICHRECTOOLS_RICHPHOTONPREDICTOR_H 1

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// Event
#include "Event/RichRecPixel.h"
#include "Event/RichRecSegment.h"

// Interfaces
#include "RichRecBase/IRichPhotonPredictor.h"
#include "RichRecBase/IRichRecGeomTool.h"

// RichKernel
#include "RichKernel/RichPoissonEffFunctor.h"

//-----------------------------------------------------------------------------
/** @class RichSimplePhotonPredictor RichSimplePhotonPredictor.h
 *
 *  Tool which performs the association between RichRecTracks and
 *  RichRecPixels to form RichRecPhotons. 
 * 
 *  This particular implementation uses a simple fixed cut range per radiator, 
 *  on the seperation between the pixel and ray-traced track impact point.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

class RichSimplePhotonPredictor : public RichRecToolBase,
                                  virtual public IRichPhotonPredictor {

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichSimplePhotonPredictor( const std::string& type,
                             const std::string& name,
                             const IInterface* parent );

  /// Destructor
  virtual ~RichSimplePhotonPredictor(){}

  // Initialize method
  StatusCode initialize();

  // Finalize method
  StatusCode finalize();

public: // methods (and doxygen comments) inherited from public interface

  // Is it possible to make a photon candidate using this segment and pixel.
  bool photonPossible( RichRecSegment * segment,
                       RichRecPixel * pixel ) const;

private: // private data

  /// Geometry tool
  IRichRecGeomTool * m_geomTool;

  std::vector<double> m_minROI;  ///< Min hit radius of interest around track centres
  std::vector<double> m_maxROI;  ///< Max hit radius of interest around track centres
  std::vector<double> m_maxROI2; ///< Square of m_maxROI
  std::vector<double> m_minROI2; ///< Square of m_minROI

  mutable std::vector<unsigned int> m_Nselected; ///< Number of selected combinations for each radiator
  mutable std::vector<unsigned int> m_Nreject;   ///< Number of rejected combinations for each radiator

};

#endif // RICHRECTOOLS_RICHPHOTONPREDICTOR_H
