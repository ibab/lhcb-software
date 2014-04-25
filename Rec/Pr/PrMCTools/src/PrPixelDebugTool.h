#ifndef PRPIXELDEBUGTOOL_H
#define PRPIXELDEBUGTOOL_H 1

// Gaudi
#include "GaudiAlg/GaudiTool.h"
// Interface
#include "PrKernel/IPrDebugTool.h"

/** @class PrPixelDebugTool PrPixelDebugTool.h
 *  Debug Pixel processing using MC truth
 *
 *  @author Olivier Callot
 *  @date   2007-10-25
 */
class PrPixelDebugTool : public GaudiTool, virtual public IPrDebugTool {
 public:
  /// Standard constructor
  PrPixelDebugTool(const std::string& type, const std::string& name,
                   const IInterface* parent);
  /// Destructor
  virtual ~PrPixelDebugTool();

  virtual bool matchKey(LHCb::LHCbID id, int key);
  virtual void printKey(MsgStream& msg, LHCb::LHCbID id);

  virtual double xTrue(int key, double z);
  virtual double yTrue(int key, double z);

};
#endif
