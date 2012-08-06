#ifndef PRVLDEBUGTOOL_H 
#define PRVLDEBUGTOOL_H 1

// Gaudi
#include "GaudiAlg/GaudiTool.h"
// Interface
#include "PrKernel/IPrDebugTool.h"

/** @class PrVLDebugTool PrVLDebugTool.h
 *  Debug VL processing using MC truth
 *
 */
class PrVLDebugTool : public GaudiTool, virtual public IPrDebugTool {
public: 
  /// Standard constructor
  PrVLDebugTool(const std::string& type, 
                const std::string& name,
                const IInterface* parent);
  /// Destructor
  virtual ~PrVLDebugTool() {}

  virtual bool matchKey(LHCb::LHCbID id, int key);
  virtual void printKey(MsgStream& msg, LHCb::LHCbID id);

  virtual double xTrue(int key, double z);
  virtual double yTrue(int key, double z);

};
#endif 
