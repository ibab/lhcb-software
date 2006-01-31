// $Id: MuonRawBuffer.h,v 1.3 2006-01-31 15:22:33 asatta Exp $
#ifndef MUONRAWBUFFER_H 
#define MUONRAWBUFFER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "MuonDAQ/IMuonRawBuffer.h"            // Interface
#include "Event/RawEvent.h"

#include <string>

/** @class MuonRawBuffer MuonRawBuffer.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2005-10-18
 */
class MuonRawBuffer : public GaudiTool, virtual public IMuonRawBuffer {
public: 
  /// Standard constructor
  MuonRawBuffer( const std::string& type, 
                 const std::string& name,
                 const IInterface* parent);

  virtual ~MuonRawBuffer( ); ///< Destructor
  StatusCode initialize();
  StatusCode finalize();
  
  std::vector<LHCb::MuonTileID> getTile();
  std::vector<std::pair<LHCb::MuonTileID,unsigned int> > getTileAndTDC();


protected:

private:

  unsigned int m_TotTell1;
unsigned int m_ODENumberInTell1[12];
  std::vector<unsigned int> m_ODEInTell1[12];  
  std::vector<LHCb::MuonTileID> m_mapTileInODE[180];
  std::string getBasePath(int station);
  std::string basePath[5] ;




};
#endif // MUONRAWBUFFER_H
