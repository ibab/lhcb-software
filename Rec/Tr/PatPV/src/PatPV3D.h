#ifndef PATPV_PATPV3D_H 
#define PATPV_PATPV3D_H 1

// Include files
// -------------
// From Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
// Interfaces
#include "TrackInterfaces/IPVOfflineTool.h"

/** @class PatPV3D PatPV3D.h
 *  Algorithm to find the primary vertices at the HLT.
 *
 *  @author Eduardo Rodrigues
 *  @date   2004-02-17
 */

//-----------------------------------------------------------------------------

class PatPV3D : public GaudiAlgorithm {
public:
  /// Standard constructor
  PatPV3D( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PatPV3D( );    ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

protected:
 
private: 
  
  std::string m_recVertexLocation;
  std::string m_primaryVertexLocation;

  IPVOfflineTool* m_pvsfit;    // PV fitting tool

};
#endif // PATPV_PATPV3D_H
