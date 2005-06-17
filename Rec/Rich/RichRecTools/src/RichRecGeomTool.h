
//-----------------------------------------------------------------------------
/** @file RichRecGeomTool.h
 *
 *  Header file for tool : RichRecGeomTool
 *
 *  CVS Log :-
 *  $Id: RichRecGeomTool.h,v 1.5 2005-06-17 15:08:36 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RICHRECGEOMTOOL_H
#define RICHRECTOOLS_RICHRECGEOMTOOL_H 1

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// Rich Kernel
#include "RichKernel/BoostArray.h"

// Event
#include "Event/RichRecPixel.h"
#include "Event/RichRecSegment.h"

// interfaces
#include "RichKernel/IRichDetParameters.h"
#include "RichRecBase/IRichRecGeomTool.h"
#include "RichRecBase/IRichCherenkovAngle.h"

//-----------------------------------------------------------------------------
/** @class RichRecGeomTool RichRecGeomTool.h
 *
 *  Tool to answer simple geometrical questions
 *  using the reconstruction event model
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

class RichRecGeomTool : public RichRecToolBase,
                        virtual public IRichRecGeomTool 
{

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichRecGeomTool( const std::string& type,
                   const std::string& name,
                   const IInterface* parent );

  /// Destructor
  virtual ~RichRecGeomTool(){}

  // Initialize method
  StatusCode initialize();

  // Finalize method
  StatusCode finalize();

public: // methods (and doxygen comments) inherited from public interface

  // Returns square of distance seperating the pixel hit and hit position extrapolated
  // using the RichRecSegment direction in local corrdinates
  double trackPixelHitSep2( const RichRecSegment * segment,
                            const RichRecPixel * pixel ) const;

  // Computes the fraction of the Cherenkov cone for a given segment that
  //  is within the average HPD panel acceptance
  double hpdPanelAcceptance( RichRecSegment * segment,
                             const Rich::ParticleIDType id ) const; 

  // Correct the given position (in local HPD coordinates) for the average
  // optical distortion for given radiator
  HepPoint3D correctAvRadiatorDistortion( const HepPoint3D & point,
                                          const Rich::RadiatorType rad ) const;

private: // private data

  // Pointers to tool instances
  IRichDetParameters  * m_detParams; ///< Detector parameters tool
  IRichCherenkovAngle * m_ckAngle;   ///< Pointer to the Cherenkov angle tool

  /// Radiator correction scale parameter
  std::vector<double> m_radScale;

  /// The radiator outer limits in local coordinates
  boost::array< IRichDetParameters::RadLimits, Rich::NRadiatorTypes > m_radOutLimLoc;

};

#endif // RICHRECTOOLS_RICHRECGEOMTOOL_H
