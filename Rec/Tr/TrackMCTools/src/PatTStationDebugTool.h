// $Id: $
#ifndef PATTSTATIONDEBUGTOOL_H 
#define PATTSTATIONDEBUGTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "PatKernel/IPatDebugTool.h"            // Interface


/** @class PatTStationDebugTool PatTStationDebugTool.h
 *  
 *
 *  @author Olivier Callot
 *  @date   2010-10-20
 */
class PatTStationDebugTool : public GaudiTool, virtual public IPatDebugTool {
public: 
  /// Standard constructor
  PatTStationDebugTool( const std::string& type, 
                        const std::string& name,
                        const IInterface* parent);

  virtual ~PatTStationDebugTool( ); ///< Destructor

  virtual bool matchKey( LHCb::LHCbID& id, int key );

  virtual void printKey( MsgStream& msg, LHCb::LHCbID& id );

  virtual double xTrue( int key, double z );

  virtual double yTrue( int key, double z );

protected:

private:

};
#endif // PATTSTATIONDEBUGTOOL_H
