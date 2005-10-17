// $Id: MuonDSTPrepare.h,v 1.2 2005-10-17 08:16:34 pkoppenb Exp $
#ifndef MUONDSTPREPARE_H 
#define MUONDSTPREPARE_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// From MuonEvent
#include "Event/MuonCoord.h"

/** @class MuonDSTPrepare MuonDSTPrepare.h
 *  This algorihtm filters the MuonCoord list to only include MuonCoords 
 *  references by the MuonID objects
 *
 *  @author David Hutchcroft
 *  @date   16/05/2002
 *          Erica Polycarpo, Miriam Gandelman
 *  @date   28/09/2005 New message service sintaxe> move to GaudiAlgorithm
 */
class MuonDSTPrepare : public GaudiAlgorithm {
public:
  /// Standard constructor
  MuonDSTPrepare( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MuonDSTPrepare( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  /// TES string(s) for the MuonID objects
  std::string m_MuonIDsPath;

  /// removes the MuonTileIDs (keys) in tilesKeep from pCoords
  StatusCode removeUnusedCoords(MuonCoords *pCoords,
                                std::vector<MuonTileID> &tilesKeep);


};
#endif // MUONDSTPREPARE_H
