// $Id: $
#ifndef PVTRACKREMOVER_H 
#define PVTRACKREMOVER_H 1

// Include files
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IPVReFitter.h"

class IPVOfflineTool;

/** @class PVTrackRemover PVTrackRemover.h
 *  
 *  Remove weighted contribution of tracks from an LHCb::Particle 
 *  to a given LHCb::RecVertex.
 *  This class is <b>temporary</b> and for testing purposes. The reFit
 *  method makes no sense and is not implemented.
 * 
 *  @author Juan Palacios
 *  @date   2010-12-07
 */
class PVTrackRemover : public GaudiTool, virtual public IPVReFitter {

public: 
  /// Standard constructor
  PVTrackRemover( const std::string& type,
                  const std::string& name,
                  const IInterface* parent ); 

  virtual StatusCode initialize();

  virtual StatusCode reFit(LHCb::VertexBase* PV) const;
  
  virtual StatusCode remove(const LHCb::Particle* particle,
                            LHCb::VertexBase* referencePV) const;

  virtual ~PVTrackRemover( ); ///< Destructor

private:

  std::string m_pvToolType;
  IPVOfflineTool* m_pvTool;

};
#endif // PVTRACKREMOVER_H
