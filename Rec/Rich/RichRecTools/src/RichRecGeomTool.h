// $Id: RichRecGeomTool.h,v 1.1 2004-06-10 14:39:24 jonesc Exp $
#ifndef RICHRECTOOLS_RICHRECGEOMTOOL_H
#define RICHRECTOOLS_RICHRECGEOMTOOL_H 1

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// Event
#include "Event/RichRecPixel.h"
#include "Event/RichRecSegment.h"

// Interfaces
#include "RichRecBase/IRichRecGeomTool.h"

/** @class RichRecGeomTool RichRecGeomTool.h
 *
 *  Tool to answer simple geometrical questions
 *  using the reconstruction event model
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class RichRecGeomTool : public RichRecToolBase,
                        virtual public IRichRecGeomTool {

public:

  /// Standard constructor
  RichRecGeomTool( const std::string& type,
                   const std::string& name,
                   const IInterface* parent );

  /// Destructor
  virtual ~RichRecGeomTool(){}

  /// Initialize method
  StatusCode initialize();

  /// Finalize method
  StatusCode finalize();

  /// Returns square of distance seperating the pixel hit and hit position extrapolated
  /// using the RichRecSegment direction in global corrdinates
  double trackPixelHitSep2Global( const RichRecSegment * segment,
                                  const RichRecPixel * pixel ) const;

  /// Returns square of distance seperating the pixel hit and hit position extrapolated
  /// using the RichRecSegment direction in local corrdinates
  double trackPixelHitSep2Local( const RichRecSegment * segment,
                                 const RichRecPixel * pixel ) const;

private: // private data

};

#endif // RICHRECTOOLS_RICHRECGEOMTOOL_H
