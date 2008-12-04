// $Id: PatVeloDebugTool.h,v 1.3 2008-12-04 09:05:07 cattanem Exp $
#ifndef PATVELODEBUGTOOL_H 
#define PATVELODEBUGTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "PatKernel/IPatDebugTool.h"            // Interface


/** @class PatVeloDebugTool PatVeloDebugTool.h
 *  Debug Velo processing using MC truth
 *
 *  @author Olivier Callot
 *  @date   2007-10-25
 */
class PatVeloDebugTool : public GaudiTool, virtual public IPatDebugTool {
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
