#ifndef MUONFASTHWTOOL_H 
#define MUONFASTHWTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/Point3DTypes.h"

// local
#include "MuonDet/DeMuonDetector.h"
#include "MuonDet/MuonBasicGeometry.h"
#include "MuonInterfaces/IMuonFastHWTool.h"
#include "Kernel/MuonLayout.h"
#include "Kernel/MuonTileID.h"

/** @class MuonFastHWTool MuonFastHWTool.h
 *  
 *  Convert an MuonTileID into an xyz position in the detector (with size)
 *  No abstract interface as I do not want to make more than one of these...
 *
 *  @author David Hutchcroft
 *  @date   07/03/2002
 */
class MuonFastHWTool : public GaudiTool, 
                    virtual public IMuonFastHWTool {
public:
  /// Standard constructor
  MuonFastHWTool( const std::string& type, 
                 const std::string& name,
                 const IInterface* parent);
  
  virtual ~MuonFastHWTool( ){} ; ///< Destructor

  /** Calculate the x,y,z and dx,dy,dz of a MuonTileID in mm
   * this ignores gaps: these can never be read out independently
   */
   virtual StatusCode GetHWName(LHCb::MuonTileID tile,int& ODENum,int &SyncOne,int &SynchTwo);
  virtual StatusCode GetI2C(LHCb::MuonTileID tile,int& I2C);  
  virtual StatusCode GetFEBInCh(LHCb::MuonTileID tile,int& FEB);
  virtual StatusCode GetFEBInI2C(LHCb::MuonTileID tile,int& FEB);
 
  StatusCode initialize();
  void initFEB();
 LHCb::MuonTileID transformTile(int quarter,int chX,int chY,LHCb::MuonTileID tile);
 
int getRealLayoutNumber(int station,int region){
return m_realLayout[station*4+region];
} 
int getI2CNumber(int station,int region,int layout){
return m_daisy[station*4+region][layout];
}
int getFEBInI2CNumber(int station,int region,int layout,int daisy){
return m_cardiac[station*4+region][layout][daisy];
}
int getChamberGridX(int region){
return m_chamberX[region];
}
int getChamberGridY(int region){
return m_chamberY[region];
}
MuonLayout getLayoutInChamber(int station, int region,int lay){
return m_padInChLayout[2*(station*4+region)+lay];
}
  void fillTileParameter(LHCb::MuonTileID tile,int lay,int i2c,int feb);
  
std::vector<LHCb::MuonTileID>& getListOfTile(int station,int region,int layout,int I2C,int feb){
return m_padInCh[station*4+region][layout][I2C][feb];
}
  
private:
  int getPadChIndex(LHCb::MuonTileID tile);
  StatusCode getGlobalIndex(LHCb::MuonTileID tile,int &result,int& index,int& lay);


  void setI2CFeb(int xmin, int xmax, int ymin, int ymax,int offsetX,int index,int lay,int d, int c,LHCb::MuonTileID  tilePart);

  
  // Number of stations
  int m_stationNumber;
  // Number of regions
  int m_regionNumber;
  int m_layoutX[2][20];
  int m_layoutY[2][20];
  int layout[20];
  int m_realLayout[20];
  int m_chamberX[4];
  int m_chamberY[4];
  //  MuonLayout m_padInChLayout[40];
  std::vector<MuonLayout> m_padInChLayout;//[40];
  
  int m_daisy[20][2];
  int m_cardiac[20][2][4];
  std::vector<LHCb::MuonTileID> m_padInCh[20][2][4][8];
  std::vector<int> m_I2C[20][2];
  std::vector<int> m_FEB[20][2];
  
  std::vector<int>   m_ODENum[80][2];
  std::vector<int>            m_synchNum[80][2];
  DeMuonDetector* m_muonDetector;  
};
#endif // MUONFASTHWTOOL_H
