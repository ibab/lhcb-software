#ifndef MUONALGS_MUONDIGITIZATIONPARAMETERS_H
#define MUONALGS_MUONDIGITIZATIONPARAMETERS_H 1
#include <GaudiKernel/IDataProviderSvc.h>
#include <GaudiKernel/IToolSvc.h> 
#include <GaudiKernel/IMessageSvc.h> 
#include "MuonTools/IMuonTileXYZTool.h"
#include "GaudiKernel/SmartDataPtr.h"       
#include "GaudiKernel/MsgStream.h"  
#include <GaudiKernel/ObjectVector.h>  
#include "DetDesc/ILVolume.h"
#include "DetDesc/IPVolume.h"
#include "DetDesc/Material.h"
#include "DetDesc/IDetectorElement.h"
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/DetectorElementException.h"
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/ISolid.h"
#include "DetDesc/SolidBox.h"  
#include "MuonKernel/MuonTile.h"
#include "MuonDet/DeMuonRegion.h"
#include "MuonDet/DeMuonChamber.h"
#include "MuonDet/MuonParameters.h"
#include "MuonDet/DeMuonGasGap.h"
#include "MuonDet/MuonBasicGeometry.h"


namespace MuonDigitizationParameters {
  static const int partition=20;
  static const int maxReadoutType=2;
  static const int numberChamber=1380;
  static const int numberGap=4;
	static const int nFE=2;
	static const int nLogMap=2;	





class Parameters{
  public:
  Parameters(){};
  Parameters(IToolSvc* toolSvc, IDataProviderSvc* detSvc, IMessageSvc * msgSvc);
  void setReadoutNumber(int partitionNumber, int value);
  void setChamberPerRegion(int partitionNumber, unsigned int value);
  void setGapPerRegion(int partitionNumber, unsigned int value);
  void setLogMapPerRegion(int partitionNumber, unsigned int value);
	void setStartGapZ(int numberOfGap,int partitionNumber, float value);
	void setStopGapZ(int numberOfGap,int partitionNumber,float value);
  
  void setReadoutType( int readout, int partitionNumber, unsigned int value);
  void setLogMapRType(int readout, int partitionNumber,unsigned int value);
  void setLogMapMergex(int readout, int partitionNumber,unsigned int value);
  void setLogMapMergey(int readout,int partitionNumber,unsigned int value);
	void setPhChannelNX( int readout, int partitionNumber, unsigned int value);
	void setPhChannelNY( int readout, int partitionNumber, unsigned int value);
	void setPhChannelSizeX( int readout, int partitionNumber, float value);
	void setPhChannelSizeY( int readout, int partitionNumber, float value);
	void setStartChamberPositionX(int chamber, float xPosition);
	void setStartChamberPositionY(int chamber, float yPosition);
	void setStartPositionFirstGapZ(int chamber, float zPosition);
  //	void setMiddleChamberPositionZ(int readout, int chamber, float zPosition);
	void setGridX(int chamber, unsigned int value);
	void setGridY(int chamber, unsigned int value);
	void setQuadrantChamber(int chamber, unsigned int value);
	void setLayoutX(int map, int partitionNumber,int value);
	void setLayoutY( int map, int partitionNumber, int value);
	
  void setGapPerFE(int partitionNumber, int value); 	
  
  int getReadoutNumber(int partitionNumber);
  unsigned int getChamberPerRegion(int partitionNumber);
  unsigned int getGapPerRegion(int partitionNumber);
  unsigned int getLogMapPerRegion(int partitionNumber);
  float getStartGapZ(int numberOfGap,int partitionNumber);
  float getStopGapZ(int numberOfGap,int partitionNumber);
  unsigned int getReadoutType( int readout, int partitionNumber);
  unsigned int getLogMapRType(int readout,int partitionNumber);
  unsigned int getLogMapMergex(int readout,int partitionNumber);
  unsigned int getLogMapMergey(int readout,int partitionNumber);
	unsigned int getPhChannelNX( int readout, int partitionNumber);
	unsigned int getPhChannelNY( int readout, int partitionNumber);
	float getPhChannelSizeX( int readout, int partitionNumber);
	float getPhChannelSizeY( int readout, int partitionNumber);
	float getStartChamberPositionX(int chamber);
	float getStartChamberPositionY(int chamber);
  float getStartPositionFirstGapZ(int chamber);
  //	float getMiddleChamberPositionZ(int readout,int chamber);
	unsigned int getGridX(int chamber);
	unsigned int getGridY(int chamber);
	unsigned int getQuadrantChamber(int chamber);
	int getGapPerFE(int partitionNumber);
	int getLayoutX( int map, int partitionNumber);
	int getLayoutY( int map, int partitionNumber);
private:
/*  static const int partition=20;
    static const int maxReadoutType=2;
    static const int numberChamber=1380;
    static const int numberGap=4;
    static const int nFE=2;*/
  static int readoutNumber[partition];
  static unsigned int chamberPerRegion[partition];
  static unsigned int gapPerRegion[partition];
  static unsigned int LogMapPerRegion[partition];
	static float startGapZ[numberGap][partition];
	static float stopGapZ[numberGap][partition];
	
  static unsigned int readoutType[maxReadoutType][partition];
  static unsigned int LogMapRType[maxReadoutType][partition];
  static unsigned int LogMapMergex[maxReadoutType][partition];
  static unsigned int LogMapMergey[maxReadoutType][partition];
  static unsigned int phChannelNX[maxReadoutType][partition];
  static unsigned int phChannelNY[maxReadoutType][partition];
  static float phChannelSizeX[maxReadoutType][partition];
  static float phChannelSizeY[maxReadoutType][partition];
  static float startPositionChamberX[numberChamber];
  static float startPositionChamberY[numberChamber];
  static float startPositionFirstGapZ[numberChamber];
	
  //  static float middlePositionChamberZ[nFE][numberChamber];
	static unsigned int gridXChamber[numberChamber];
	static unsigned int gridYChamber[numberChamber];
	static unsigned int quarterChamber[numberChamber];
	static int gapPerFE[partition];
	static int layoutX[nLogMap][partition];
	static int layoutY[nLogMap][partition];
  int m_stationNumber;
  int m_regionNumber;
  int m_partition;  
};
  
  inline  void Parameters::setReadoutNumber(int partitionNumber, int value)
  {readoutNumber[partitionNumber]=value ;}
  inline  void Parameters::setChamberPerRegion(int partitionNumber, 
                                               unsigned int value)
  {chamberPerRegion[partitionNumber]=value ;}
  inline  void Parameters::setGapPerRegion(int partitionNumber, 
                                           unsigned int value)
  {gapPerRegion[partitionNumber]=value ;}
  inline  void Parameters::setLogMapPerRegion(int partitionNumber, 
                                              unsigned int value)
  {LogMapPerRegion[partitionNumber]=value ;}
  inline  	void Parameters::setStartGapZ(int numberOfGap,
                                          int partitionNumber, 
                                          float value)
  {startGapZ[numberOfGap][partitionNumber]=value;}
  inline  	void Parameters::setStopGapZ(int numberOfGap,int partitionNumber,
                                         float value)
  {stopGapZ[numberOfGap][partitionNumber]=value;}
  
  inline  void Parameters::setReadoutType( int readout, int partitionNumber, 
                                           unsigned int value)
  {readoutType[readout][partitionNumber]=value ;}
  inline void Parameters::setLogMapRType (int readout,int partitionNumber, 
                                          unsigned int value)
  {LogMapRType[readout][partitionNumber]=value;}
  inline void Parameters::setLogMapMergex(int readout,int partitionNumber, 
                                          unsigned int value)
  {LogMapMergex[readout][partitionNumber]=value;}
  inline void Parameters::setLogMapMergey(int readout,int partitionNumber, 
                                          unsigned int value)
  {LogMapMergey[readout][partitionNumber]=value;}
  inline void Parameters::setPhChannelNX( int readout, int partitionNumber, 
                                          unsigned int value)
 {phChannelNX[readout][partitionNumber]=value ;}
  inline void Parameters::setPhChannelNY( int readout, int partitionNumber, 
                                          unsigned int value)
  {phChannelNY[readout][partitionNumber]=value ;}
  inline void Parameters::setPhChannelSizeX( int readout, 
                                             int partitionNumber, 
                                             float value)
  {phChannelSizeX[readout][partitionNumber]=value ;}
  inline void Parameters::setPhChannelSizeY( int readout, 
                                             int partitionNumber, 
                                             float value)
  {phChannelSizeY[readout][partitionNumber]=value ;}
  inline void Parameters::setStartChamberPositionX(int chamber, 
                                                   float value)
  {startPositionChamberX[chamber]=value;}
  inline void Parameters::setStartChamberPositionY(int chamber, 
                                                   float value)
  {startPositionChamberY[chamber]=value;}
 inline void Parameters::setStartPositionFirstGapZ(int chamber, 
                                                   float value)
 {startPositionFirstGapZ[chamber]=value;}
  // inline void Parameters::setMiddleChamberPositionZ(int readout, 
  // int chamber, float zPosition){
  // middlePositionChamberZ[readout][chamber]=zPosition;}
  
  
  inline void Parameters::setGridX(int chamber, unsigned int value)
  { gridXChamber[chamber]=value;}
  inline void Parameters::setGridY(int chamber, unsigned int value)
  { gridYChamber[chamber]=value;}
 inline void Parameters::setQuadrantChamber(int chamber, unsigned int value)
 { quarterChamber[chamber]=value ;}
  inline void Parameters::setGapPerFE(int partitionNumber, int
                                      value){gapPerFE[partitionNumber]=value;}
  inline void Parameters::setLayoutX( int map , int partitionNumber,int
                                      value)
  { layoutX[map][partitionNumber]=value ;}
  inline	void Parameters::setLayoutY(int map, int partitionNumber, int
                                      value)
  { layoutY[map][partitionNumber]=value ;}
  

  
  inline  int Parameters::getReadoutNumber(int partitionNumber)
  {return readoutNumber[partitionNumber];}
  inline  unsigned int Parameters::getChamberPerRegion(int partitionNumber)
  {return chamberPerRegion[partitionNumber];}
  inline  unsigned int Parameters::getGapPerRegion(int partitionNumber)
  {return gapPerRegion[partitionNumber];}
  inline  unsigned int Parameters::getLogMapPerRegion(int partitionNumber)
  {return LogMapPerRegion[partitionNumber];}
  inline  float Parameters::getStartGapZ(int numberOfGap,int partitionNumber)
  {return startGapZ[numberOfGap][partitionNumber];}
  inline  float Parameters::getStopGapZ(int numberOfGap,int partitionNumber)
  {return stopGapZ[numberOfGap][partitionNumber];}
  
  inline  unsigned int Parameters::getReadoutType( int readout, 
                                                   int partitionNumber)
  {return readoutType[readout][partitionNumber];}
  inline  unsigned int Parameters::getLogMapRType (int readout, 
                                                   int partitionNumber)
  {return LogMapRType[readout][partitionNumber];}
  inline  unsigned int Parameters::getLogMapMergex(int readout,
                                                   int partitionNumber)
  {return LogMapMergex[readout][partitionNumber];}
  inline  unsigned int Parameters::getLogMapMergey(int readout,
                                                   int partitionNumber)
  {return LogMapMergey[readout][partitionNumber];}
  
  inline  unsigned int Parameters::getPhChannelNX( int readout, 
                                                   int partitionNumber)
  {return phChannelNX[readout][partitionNumber];}
	inline  unsigned int Parameters::getPhChannelNY( int readout, 
                                                   int partitionNumber)
  {return phChannelNY[readout][partitionNumber];}
	inline  float Parameters::getPhChannelSizeX( int readout, int 
                                               partitionNumber)
  {return phChannelSizeX[readout][partitionNumber];}
	inline  float Parameters::getPhChannelSizeY( int readout, 
                                               int partitionNumber)
  {return phChannelSizeY[readout][partitionNumber];}
	inline  float Parameters::getStartChamberPositionX(int chamber )
  {return startPositionChamberX[chamber];}
	inline  float Parameters::getStartChamberPositionY(int chamber )
  {return startPositionChamberY[chamber];}
	inline  float Parameters::getStartPositionFirstGapZ(int chamber )
  {return startPositionFirstGapZ[chamber];}
	inline  unsigned int Parameters::getGridX(int chamber)
  { return gridXChamber[chamber];}
	inline  unsigned int Parameters::getGridY(int chamber)
  { return  gridYChamber[chamber];}
	inline  unsigned int Parameters::getQuadrantChamber(int chamber)
  { return quarterChamber[chamber];}
  inline  int Parameters::getGapPerFE(int partitionNumber)
  {return  gapPerFE[partitionNumber]; } 
	inline int Parameters::getLayoutX(int map, int partitionNumber)
  { return layoutX[map][partitionNumber];}
	inline int Parameters::getLayoutY( int map,  int partitionNumber)
  {return layoutY[map][partitionNumber];;}
  
};
#endif

