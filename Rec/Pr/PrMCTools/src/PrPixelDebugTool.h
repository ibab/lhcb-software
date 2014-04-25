// $Id: PatPixelDebugTool.h,v 1.3 2008-12-04 09:05:07 cattanem Exp $
#ifndef PATPIXELDEBUGTOOL_H 
#define PATPIXELDEBUGTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "PatKernel/IPatDebugTool.h"            // Interface


/** @class PatPixelDebugTool PatPixelDebugTool.h
 *  Debug Pixel processing using MC truth
 *
 *  @author Olivier Callot
 *  @date   2007-10-25
 */
class PatPixelDebugTool : public GaudiTool, virtual public IPatDebugTool {
public: 
  /// Standard constructor
  PatPixelDebugTool( const std::string& type, 
                     const std::string& name,
                     const IInterface* parent);

  virtual ~PatPixelDebugTool( ); ///< Destructor

  virtual bool matchKey( LHCb::LHCbID& id, int key );

  virtual void printKey( MsgStream& msg, LHCb::LHCbID& id );

  virtual double xTrue( int key, double z );

  virtual double yTrue( int key, double z );

protected:

private:

};
#endif // PATPIXELDEBUGTOOL_H
