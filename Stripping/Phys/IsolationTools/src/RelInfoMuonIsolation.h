// $Id: $
#ifndef RELINFOMUONISOLATION_H 
#define RELINFOMUONISOLATION_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IRelatedInfoTool.h"            // Interface
#include "Event/RelatedInfoMap.h"

class IMuonIDTool;

/** @class RelInfoMuonIsolation RelInfoMuonIsolation.h
 *  
 *  \brief Computes isolation variables in the muon stations for muon candidate tracks
 *  
 *
 *  @author Giacomo Graziani
 *  @date   2015-11-20
 */
class RelInfoMuonIsolation : public GaudiTool, virtual public IRelatedInfoTool {
public: 
  /// Standard constructor
  RelInfoMuonIsolation( const std::string& type, 
                        const std::string& name,
                        const IInterface* parent);
  virtual ~RelInfoMuonIsolation( ) {} ///< Destructor
  virtual StatusCode initialize();
  virtual StatusCode calculateRelatedInfo( const LHCb::Particle*,
                                           const LHCb::Particle*);
  
  virtual LHCb::RelatedInfoMap* getInfo(void);
    

private:
  LHCb::RelatedInfoMap m_map;
  IMuonIDTool* m_muIDtool;
  int m_nStations;

};
#endif // RELINFOMUONISOLATION_H
