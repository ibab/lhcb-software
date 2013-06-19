#ifndef MUONREAD_H 
#define MUONREAD_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h" //add sim 01.03.07
#include "MuonDet/DeMuonDetector.h"
#include "Event/MCParticle.h"
#include "MuonDAQ/IMuonRawBuffer.h"
#include "Linker/LinkedTo.h"
#include "Linker/LinkedFrom.h"
#include "Event/MuonDigit.h"

/** @class MuonRead MuonRead.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2006-11-13
 */
class MuonRead : public GaudiTupleAlg {
public: 
  /// Standard constructor
  MuonRead( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MuonRead( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  StatusCode searchNature(LHCb::MuonTileID tile,LHCb::MCParticle*& pp);
  
protected:

private:
  DeMuonDetector*  m_muonDet; 
  IMuonRawBuffer* m_muonBuffer;
  std::vector<LHCb::MuonTileID> m_muonDigit;

  std::string m_muonTracksName;
  
};
#endif // MUONREAD_H
