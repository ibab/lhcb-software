
/** @file RichRecGeomTool.h
 *
 *  Header file for tool : RichRecGeomTool
 *
 *  CVS Log :-
 *  $Id: RichRecGeomTool.h,v 1.2 2004-07-27 20:15:32 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

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
  // using the RichRecSegment direction in global corrdinates
  double trackPixelHitSep2Global( const RichRecSegment * segment,
                                  const RichRecPixel * pixel ) const;

  // Returns square of distance seperating the pixel hit and hit position extrapolated
  // using the RichRecSegment direction in local corrdinates
  double trackPixelHitSep2Local( const RichRecSegment * segment,
                                 const RichRecPixel * pixel ) const;

private: // private data

};

#endif // RICHRECTOOLS_RICHRECGEOMTOOL_H
