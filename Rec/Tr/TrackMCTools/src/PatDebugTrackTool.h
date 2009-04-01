// $Id: PatDebugTrackTool.h,v 1.1 2009-04-01 08:50:27 ocallot Exp $
#ifndef TRACKMCTOOLS_PATDEBUGTRACKTOOL_H 
#define TRACKMCTOOLS_PATDEBUGTRACKTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "PatKernel/IPatDebugTrackTool.h"            // Interface


/** @class PatDebugTrackTool PatDebugTrackTool.h TrackMCTools/PatDebugTrackTool.h
 *  
 *
 *  @author Olivier Callot
 *  @date   2009-03-31
 */
class PatDebugTrackTool : public GaudiTool, virtual public IPatDebugTrackTool {
public: 
  /// Standard constructor
  PatDebugTrackTool( const std::string& type, 
                     const std::string& name,
                     const IInterface* parent);

  virtual ~PatDebugTrackTool( ); ///< Destructor

  virtual std::vector<int> keys( const LHCb::Track* track );

protected:

private:

};
#endif // TRACKMCTOOLS_PATDEBUGTRACKTOOL_H
