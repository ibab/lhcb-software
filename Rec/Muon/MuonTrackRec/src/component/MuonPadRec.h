// $Id: MuonPadRec.h,v 1.3 2009-10-01 11:47:32 ggiacomo Exp $
#ifndef MUONPADREC_H 
#define MUONPADREC_H 1
#include <map>
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IIncidentListener.h"
#include "MuonTrackRec/IMuonPadRec.h"            // Interface

class DeMuonDetector;

/** @class MuonPadRec MuonPadRec.h
 *  special reconstruction tool for Monitoring
 *  code based on MuonRec by A.Satta et al.
 *
 *  @author G.Graziani
 *  @date   2008-01-25
 */


class MuonPadRec : public GaudiTool, virtual public IMuonPadRec , virtual public IIncidentListener 
{
public: 
  /// Standard constructor
  MuonPadRec( const std::string& type, 
              const std::string& name,
              const IInterface* parent);

  virtual ~MuonPadRec( ); ///< Destructor

  virtual StatusCode buildLogicalPads(const std::vector<MuonLogHit*>* myhits );
  virtual const std::vector<MuonLogPad*>* pads();


  // from GaudiTool
  virtual StatusCode 	initialize ();
  virtual StatusCode 	finalize ();

  // from IIncidentListener
  virtual void handle ( const Incident& incident );    
private:
  std::vector<long int>  m_TileVeto;  
  std::vector<MuonLogPad*> m_pads;
  bool m_padsReconstructed;
  DeMuonDetector* m_muonDetector;
  std::map<long int, bool> m_TileIsVetoed;
  void clearPads();

  StatusCode addCoordsNoMap(std::vector<MuonLogHit*> &hits);
  StatusCode addCoordsCrossingMap(std::vector<MuonLogHit*> &hits);
  StatusCode removeDoubleHits(std::vector<MuonLogHit*> &hits);
};
#endif // MUONPADREC_H
