// $Id: MuonRec.h,v 1.2 2008-04-02 11:52:05 asatta Exp $
#ifndef MUONREC_MUONREC_H 
#define MUONREC_MUONREC_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
//#include "GaudiAlg/GaudiAlgorithm.h"

#include "MuonDet/MuonBasicGeometry.h"
#include "MuonDet/DeMuonDetector.h"
#include "MuonDAQ/IMuonRawBuffer.h"


/** @class MuonRec MuonRec.h
 *  This is the muon reconstruction algorithm
 *  This just crosses the logical strips back into pads
 *
 *  @author David Hutchcroft
 *  @date   22/03/2002
 */
class MuonRec : public GaudiAlgorithm {
public:
  /// Standard constructor
  MuonRec( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MuonRec( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  /// Copy MuonTileID from digits to coords with no logical map (1:1 copy)
  StatusCode addCoordsNoMap(LHCb::MuonCoords *coords, 
                            std::vector<std::pair<LHCb::MuonTileID ,
                            unsigned int> > & digit,
                            const int & station,
                            const int & region);

  /// Copy MuonTileID from digits to coord by crossing the digits
  StatusCode addCoordsCrossingMap(LHCb::MuonCoords *coords, 
                                  std::vector<std::pair<LHCb::MuonTileID, 
                                  unsigned int> > &digit,
                                  const int & station,
                                  const int & region);

  /// fills in the two readout layouts by querying the DeMuonRegion
  StatusCode makeStripLayouts(int station , int region, 
                              MuonLayout &layout1,
                              MuonLayout &layout2);

  // Number of stations
  int m_NStation;
  // Number of regions
  int m_NRegion;
  DeMuonDetector* m_muonDetector;
  IMuonRawBuffer* m_muonBuffer;  
  bool m_forceResetDAQ;
  int  m_Exccounter;  
  std::string m_rawEventLoc; // Specific raw event location set by options
};
#endif // MUONREC_MUONREC_H
