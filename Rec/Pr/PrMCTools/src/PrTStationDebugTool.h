// $Id: $
#ifndef PRTSTATIONDEBUGTOOL_H 
#define PRTSTATIONDEBUGTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "PrKernel/IPrDebugTool.h"            // Interface


/** @class PrTStationDebugTool PrTStationDebugTool.h
 *  
 *
 *  @author Olivier Callot
 *  @date   2012-03-22
 */
class PrTStationDebugTool : public GaudiTool, virtual public IPrDebugTool {
public: 
  /// Standard constructor
  PrTStationDebugTool( const std::string& type, 
                  const std::string& name,
                  const IInterface* parent);

  virtual ~PrTStationDebugTool( ); ///< Destructor

  virtual bool matchKey( LHCb::LHCbID id, int key );

  virtual void printKey( MsgStream& msg, LHCb::LHCbID id );

protected:

private:

};
#endif // PRTSTATIONDEBUGTOOL_H
