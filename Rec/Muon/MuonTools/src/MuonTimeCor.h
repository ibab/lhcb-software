// $Id: MuonTimeCor.h,v 1.1 2010-02-09 16:02:24 asatta Exp $
#ifndef MUONTIMECOR_H 
#define MUONTIMECOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "MuonDet/DeMuonDetector.h"
#include "MuonDAQ/IMuonRawBuffer.h"
#include "MuonInterfaces/IMuonFastHWTool.h"
#include "MuonInterfaces/IMuonTimeCor.h"            // Interface


/** @class MuonTimeCor MuonTimeCor.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2009-12-22
 */
class MuonTimeCor : public GaudiTool, virtual public IMuonTimeCor {
public: 
  /// Standard constructor
  MuonTimeCor( const std::string& type, 
               const std::string& name,
               const IInterface* parent);

  virtual ~MuonTimeCor( ); ///< Destructor
  StatusCode initialize();
  virtual StatusCode getCorrection(LHCb::MuonTileID tile,int & cor);
  virtual StatusCode getOutCorrection(LHCb::MuonTileID tile,int & cor);
  virtual StatusCode setOutCorrection(LHCb::MuonTileID tile,int  cor);
  virtual StatusCode writeOutCorrection();
  virtual StatusCode writeCorrection();

protected:

private:
  int getIndex(LHCb::MuonTileID tile);
  int getLayer(LHCb::MuonTileID tile);
  virtual StatusCode setCorrection(LHCb::MuonTileID tile,int  cor);

  
  std::vector<std::string> m_correctionFiles;
  std::string m_correctionFileOut;
  std::vector<int> m_correctionSign;
  std::vector<int> m_correction[20][2];
  std::vector<int> m_Outcorrection[20][2];
  DeMuonDetector* m_muonDetector;
  IMuonRawBuffer* m_muonBuffer;
  IMuonFastHWTool* m_muonHWTool;

  
};
#endif // MUONTIMECOR_H
