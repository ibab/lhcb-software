// $Id: RichPhotonPredictor.h,v 1.4 2004-02-02 14:27:01 jonesc Exp $
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

/** @class RichPhotonPredictor RichPhotonPredictor.h
 *
 *  Tool which performs the association between RichRecTracks and
 *  RichRecPixels to form RichRecPhotons
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class RichPhotonPredictor : public RichRecToolBase,
                            virtual public IRichPhotonPredictor {

public:

  /// Standard constructor
  RichPhotonPredictor( const std::string& type,
                       const std::string& name,
                       const IInterface* parent );

  /// Destructor
  virtual ~RichPhotonPredictor(){}

  /// Initialize method
  StatusCode initialize();

  /// Finalize method
  StatusCode finalize();

  /// Returns square of distance seperating pixel and track hits on PDs
  double trackPixelHitSep2( const RichRecSegment * segment,
                            const RichRecPixel * pixel ) const;

  /// Is it possible to make a photon candidate using this segment and pixel.
  bool photonPossible( RichRecSegment * segment,
                       RichRecPixel * pixel ) const;

private: // private data

  std::vector<double> m_minROI;  ///< Min hit radius of interest around track centres
  std::vector<double> m_maxROI;  ///< Max hit radius of interest around track centres
  std::vector<double> m_maxROI2; ///< Square of m_maxROI
  std::vector<double> m_minROI2; ///< Square of m_minROI

};

#endif // RICHRECTOOLS_RICHPHOTONPREDICTOR_H
