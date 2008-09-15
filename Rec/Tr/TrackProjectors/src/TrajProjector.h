// $Id: TrajProjector.h,v 1.6 2008-09-15 13:19:27 wouter Exp $
#ifndef TRAJPROJECTOR_H 
#define TRAJPROJECTOR_H 1

// Include files

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "TrackProjector.h"


/** @class TrajProjector TrajProjector.h TrajProjector.h
 *  
 *  Projects into the measurement space
 *
 *  @author Jeroen van Tilburg
 *  @date   2006-02-21
 */

template <typename T>
class TrajProjector : public TrackProjector {

public: 
  /// FIXME/TODO: project given a traj instead of a state

  /// Standard constructor
  TrajProjector( const std::string& type, 
                 const std::string& name,
                 const IInterface* parent );
  /// Destructor
  virtual ~TrajProjector();

};

#endif // TRAJPROJECTOR_H
