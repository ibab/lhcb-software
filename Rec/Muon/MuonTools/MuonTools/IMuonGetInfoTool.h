// $Id: IMuonGetInfoTool.h,v 1.1 2003-06-18 15:38:26 asatta Exp $
#ifndef MUONTOOLS_IMUONGETINFOTOOL_H 
#define MUONTOOLS_IMUONGETINFOTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"


/** @class IMuonGetInfoTool IMuonGetInfoTool.h MuonTools/IMuonGetInfoTool.h
 *  
 *
 *  @author Alessia Satta
 *  @date   16/04/2003
 */
static const InterfaceID IID_IMuonGetInfoTool( "IMuonGetInfoTool" , 3 , 0 );

class IMuonGetInfoTool : public virtual IAlgTool {
public:

  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() { 
    return  IID_IMuonGetInfoTool;
  }

  virtual int getReadoutNumber(int partitionNumber) = 0;
  virtual unsigned int getChamberPerRegion(int partitionNumber)= 0;
  virtual unsigned int getGapPerRegion(int partitionNumber)= 0;
  virtual unsigned int getLogMapPerRegion(int partitionNumber)= 0;
  virtual float getStartGapZ(int numberOfGap,int partitionNumber)= 0;
  virtual float getStopGapZ(int numberOfGap,int partitionNumber)= 0;
  virtual unsigned int getReadoutType( int readout, int partitionNumber)= 0;
  virtual unsigned int getLogMapRType(int readout,int partitionNumber)= 0;
  virtual unsigned int getLogMapMergex(int readout,int partitionNumber)= 0;
  virtual unsigned int getLogMapMergey(int readout,int partitionNumber)= 0;
  virtual unsigned int getPhChannelNX( int readout, int partitionNumber)= 0;
  virtual unsigned int getPhChannelNY( int readout, int partitionNumber)= 0;
  virtual float getPhChannelSizeX( int readout, int partitionNumber)= 0;
  virtual float getPhChannelSizeY( int readout, int partitionNumber)= 0;
  virtual float getStartChamberPositionX(int chamber)= 0;
  virtual float getStartChamberPositionY(int chamber)= 0;
  virtual float getStartPositionFirstGapZ(int chamber)= 0;
  //	float getMiddleChamberPositionZ(int readout,int chamber);
  virtual unsigned int getGridX(int chamber)= 0;
  virtual unsigned int getGridY(int chamber)= 0;
  virtual unsigned int getQuadrantChamber(int chamber)= 0;
  virtual int getGapPerFE(int partitionNumber)= 0;
  virtual int getLayoutX( int map, int partitionNumber)= 0;
  virtual int getLayoutY( int map, int partitionNumber)= 0;
  virtual int getGlobalChamberNumber(unsigned int chamberIndex,
                                      int partition)= 0;
  


protected:

private:

};
#endif // MUONTOOLS_IMUONGEOMETRYTOOL_H
