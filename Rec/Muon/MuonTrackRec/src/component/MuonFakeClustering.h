// $Id: MuonFakeClustering.h,v 1.1 2009-10-16 17:05:59 ggiacomo Exp $
#ifndef COMPONENT_MUONFAKECLUSTERING_H 
#define COMPONENT_MUONFAKECLUSTERING_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "MuonTrackRec/IMuonClusterRec.h"            // Interface
#include "GaudiKernel/IIncidentListener.h"

/** @class MuonFakeClustering MuonFakeClustering.h component/MuonFakeClustering.h
 *  
 *produce fake clusters by just copying the list of MuonLogPads to MuonHits
 *  @author Giacomo GRAZIANI
 *  @date   2009-10-15
 */
class MuonFakeClustering : public GaudiTool, virtual public IMuonClusterRec, 
                      virtual public IIncidentListener {
public: 
  /// Standard constructor
  MuonFakeClustering( const std::string& type, 
                      const std::string& name,
                      const IInterface* parent);

  virtual ~MuonFakeClustering( ); ///< Destructor
  virtual StatusCode initialize();    
  virtual StatusCode finalize  ();  
  virtual void handle ( const Incident& incident );   
  virtual const std::vector<MuonHit*>* clusters(const std::vector<MuonLogPad*>* pads,
                                                bool force=false);


private:
  void clear();
};
#endif // COMPONENT_MUONFAKECLUSTERING_H
