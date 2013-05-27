// $Id: IMuonFastHWTool.h,v 1.1.1.1 2010-02-09 12:58:35 asatta Exp $
#ifndef MUONDET_IMUONFASTHWTOOL_H 
#define MUONDET_IMUONFASTHWTOOL_H 1

// Include files
#include "Kernel/MuonTileID.h"

// GaudiKernel
#include "GaudiKernel/IAlgTool.h"

/** @class IMuonHWTool IMuonHWTool.h MuonTools/IMuonHWTool.h
 *  
 *  Interface for the tools to convert MuonTileID to coordinates 
 *
 *  @author David Hutchcroft
 *  @date   11/03/2002
 */
static const InterfaceID IID_IMuonFastHWTool( "IMuonFastHWTool" , 5 , 0 
);

class IMuonFastHWTool : public virtual IAlgTool {
public:

  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() { 
    return  IID_IMuonFastHWTool; 
  }
  
  /** Calculate the x,y,z and dx,dy,dz of a MuonTileID in mm
   *  this ignores gaps: these can never be read out independently
   */
  virtual StatusCode GetHWName(LHCb::MuonTileID tile,int& ODENum,int &SyncOne,int 
&SynchTwo)=0;
  virtual StatusCode GetI2C(LHCb::MuonTileID tile,int& I2C)=0;
  virtual StatusCode GetFEBInCh(LHCb::MuonTileID tile,int& FEB)=0;
  virtual StatusCode GetFEBInI2C(LHCb::MuonTileID tile,int& FEB)=0;
  virtual int getRealLayoutNumber(int station,int region)=0;
  

  virtual int getI2CNumber(int station,int region,int layout)=0;
  

  virtual int getFEBInI2CNumber(int station,int region,int layout,int daisy)=0;
  

  virtual int getChamberGridX(int region)=0;
  

  virtual int getChamberGridY(int region)=0;
  

  virtual MuonLayout getLayoutInChamber(int station, int region,int lay)=0;
  virtual std::vector<LHCb::MuonTileID>& getListOfTile(int station,int region,int layout,int I2C,int feb)=0;
  
  virtual LHCb::MuonTileID transformTile(int quarter,int chX,int chY,
                                        LHCb::MuonTileID tile)=0;
  

  

protected:

private:

};
#endif // MUONDET_IMUONFASTHWTOOL_H
