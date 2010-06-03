// $Id: MuonClusterRec.h,v 1.3 2010-02-10 19:20:17 ggiacomo Exp $
#ifndef COMPONENT_MUONCLUSTERREC_H 
#define COMPONENT_MUONCLUSTERREC_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "MuonInterfaces/IMuonClusterRec.h"            // Interface
#include "GaudiKernel/IIncidentListener.h"
#include "Kernel/MuonTileID.h"

/** @class MuonClusterRec MuonClusterRec.h component/MuonClusterRec.h
 *  
 *  clustering tool for the muon detector
 *  @author Giacomo GRAZIANI
 *  @date   2009-10-15
 */
class MuonClusterRec : public GaudiTool, virtual public IMuonClusterRec, 
                      virtual public IIncidentListener {
public: 
  /// Standard constructor
  MuonClusterRec( const std::string& type, 
                      const std::string& name,
                      const IInterface* parent);

  virtual ~MuonClusterRec( ); ///< Destructor
  virtual StatusCode initialize();    
  virtual StatusCode finalize  ();  
  virtual void handle ( const Incident& incident );   
  virtual const std::vector<MuonHit*>* clusters(const std::vector<MuonLogPad*>* pads,
                                                bool force=false);


private:
  unsigned int m_maxPadsPerStation;

  inline int regX(const LHCb::MuonTileID* tile) {
    return ( (tile->quarter() > 1 ? -1 : 1) * tile->nX() );
  }
  inline int regY(const LHCb::MuonTileID* tile) {
    return ( ((tile->quarter() > 0 && tile->quarter()< 3) ? -1 : 1) * tile->nY() );
  }

  void clear();
  
};
#endif // COMPONENT_MUONCLUSTERREC_H
