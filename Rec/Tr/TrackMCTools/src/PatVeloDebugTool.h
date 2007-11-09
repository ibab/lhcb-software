// $Id: PatVeloDebugTool.h,v 1.1 2007-11-09 07:48:06 ocallot Exp $
#ifndef PATVELODEBUGTOOL_H 
#define PATVELODEBUGTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "TrackInterfaces/IPatVeloDebugTool.h"            // Interface


/** @class PatVeloDebugTool PatVeloDebugTool.h
 *  Debug Velo processing using MC truth
 *
 *  @author Olivier Callot
 *  @date   2007-10-25
 */
class PatVeloDebugTool : public GaudiTool, virtual public IPatVeloDebugTool {
public: 
  /// Standard constructor
  PatVeloDebugTool( const std::string& type, 
                    const std::string& name,
                    const IInterface* parent);

  virtual ~PatVeloDebugTool( ); ///< Destructor

  virtual bool matchKey( LHCb::LHCbID& id, int key );

  virtual void printKey( MsgStream& msg, LHCb::LHCbID& id );

protected:

private:

};
#endif // PATVELODEBUGTOOL_H
