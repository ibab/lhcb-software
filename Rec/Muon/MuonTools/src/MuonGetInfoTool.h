// $Id: MuonGetInfoTool.h,v 1.1 2003-06-18 15:38:30 asatta Exp $
#ifndef MUONGETINFOTOOL_H 
#define MUONGETINFOTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/AlgTool.h"
#include "MuonTools/IMuonGetInfoTool.h"
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


/** @class MuonGetInfoTool MuonGetInfoTool.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2003-04-16
 */
class MuonGetInfoTool : public AlgTool,
                        public virtual IMuonGetInfoTool{
public:
  /// Standard constructor
  MuonGetInfoTool( const std::string& type, 
                   const std::string& name,
                   const IInterface* parent);

  virtual ~MuonGetInfoTool( ); ///< Destructor
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
  int getGlobalChamberNumber(unsigned int chamberIndex,
                                      int partition);
  

protected:

private:
  static const int partition=20;
  static const int maxReadoutType=2;
  static const int numberChamber=1380;
  static const int numberGap=4;
  static	const int nFE=2;
static	const int nLogMap=2;	
  int readoutNumber[partition];
  unsigned int chamberPerRegion[partition];
  unsigned int gapPerRegion[partition];
  unsigned int LogMapPerRegion[partition];
	float startGapZ[numberGap][partition];
	float stopGapZ[numberGap][partition];
	
  unsigned int readoutType[maxReadoutType][partition];
  unsigned int LogMapRType[maxReadoutType][partition];
  unsigned int LogMapMergex[maxReadoutType][partition];
  unsigned int LogMapMergey[maxReadoutType][partition];
  unsigned int phChannelNX[maxReadoutType][partition];
  unsigned int phChannelNY[maxReadoutType][partition];
  float phChannelSizeX[maxReadoutType][partition];
  float phChannelSizeY[maxReadoutType][partition];
  float startPositionChamberX[numberChamber];
  float startPositionChamberY[numberChamber];
  float startPositionFirstGapZ[numberChamber];
	
  //  static float middlePositionChamberZ[nFE][numberChamber];
  unsigned int gridXChamber[numberChamber];
	unsigned int gridYChamber[numberChamber];
	unsigned int quarterChamber[numberChamber];
	int gapPerFE[partition];
	int layoutX[nLogMap][partition];
	int layoutY[nLogMap][partition];
  int m_stationNumber;
  int m_regionNumber;
  int m_partition;
  IMuonTileXYZTool* m_tileTool ;
  IDataProviderSvc* m_DDS;
  



};


inline  void MuonGetInfoTool::setReadoutNumber(int partitionNumber, int value)
{readoutNumber[partitionNumber]=value ;}
inline  void MuonGetInfoTool::setChamberPerRegion(int partitionNumber, 
                                                  unsigned int value)
{chamberPerRegion[partitionNumber]=value ;}
inline  void MuonGetInfoTool::setGapPerRegion(int partitionNumber, 
                                              unsigned int value)
{gapPerRegion[partitionNumber]=value ;}
inline  void MuonGetInfoTool::setLogMapPerRegion(int partitionNumber, 
                                                 unsigned int value)
{LogMapPerRegion[partitionNumber]=value ;}
inline  	void MuonGetInfoTool::setStartGapZ(int numberOfGap,
                                             int partitionNumber, 
                                          float value)
{startGapZ[numberOfGap][partitionNumber]=value;}
inline void MuonGetInfoTool::setStopGapZ(int numberOfGap,int partitionNumber,
                                         float value)
{stopGapZ[numberOfGap][partitionNumber]=value;}

inline  void MuonGetInfoTool::setReadoutType( int readout, 
                                              int partitionNumber, 
                                              unsigned int value)
{readoutType[readout][partitionNumber]=value ;}
inline void MuonGetInfoTool::setLogMapRType (int readout,int partitionNumber, 
                                             unsigned int value)
{LogMapRType[readout][partitionNumber]=value;}
  inline void MuonGetInfoTool::setLogMapMergex(int readout,
                                               int partitionNumber, 
                                               unsigned int value)
{LogMapMergex[readout][partitionNumber]=value;}
inline void MuonGetInfoTool::setLogMapMergey(int readout,int partitionNumber, 
                                             unsigned int value)
{LogMapMergey[readout][partitionNumber]=value;}
inline void MuonGetInfoTool::setPhChannelNX( int readout, int partitionNumber, 
                                             unsigned int value)
 {phChannelNX[readout][partitionNumber]=value ;}
inline void MuonGetInfoTool::setPhChannelNY( int readout, int partitionNumber, 
                                             unsigned int value)
{phChannelNY[readout][partitionNumber]=value ;}
inline void MuonGetInfoTool::setPhChannelSizeX( int readout, 
                                                int partitionNumber, 
                                             float value)
{phChannelSizeX[readout][partitionNumber]=value ;}
inline void MuonGetInfoTool::setPhChannelSizeY( int readout, 
                                                int partitionNumber, 
                                                float value)
{phChannelSizeY[readout][partitionNumber]=value ;}
  inline void MuonGetInfoTool::setStartChamberPositionX(int chamber, 
                                                        float value)
{startPositionChamberX[chamber]=value;}
inline void MuonGetInfoTool::setStartChamberPositionY(int chamber, 
                                                      float value)
  {startPositionChamberY[chamber]=value;}
inline void MuonGetInfoTool::setStartPositionFirstGapZ(int chamber, 
                                                       float value)
{startPositionFirstGapZ[chamber]=value;}
// inline void MuonGetInfoTool::setMiddleChamberPositionZ(int readout, 
// int chamber, float zPosition){
  // middlePositionChamberZ[readout][chamber]=zPosition;}


inline void MuonGetInfoTool::setGridX(int chamber, unsigned int value)
{ gridXChamber[chamber]=value;}
inline void MuonGetInfoTool::setGridY(int chamber, unsigned int value)
  { gridYChamber[chamber]=value;}
inline void MuonGetInfoTool::setQuadrantChamber(int chamber, 
                                                unsigned int value)
{ quarterChamber[chamber]=value ;}
inline void MuonGetInfoTool::setGapPerFE(int partitionNumber, int
                                         value){gapPerFE[partitionNumber]=
                                                  value;}
inline void MuonGetInfoTool::setLayoutX( int map , int partitionNumber,int
                                         value)
{ layoutX[map][partitionNumber]=value ;}
inline	void MuonGetInfoTool::setLayoutY(int map, int partitionNumber, int
                                         value)
{ layoutY[map][partitionNumber]=value ;}



inline  int MuonGetInfoTool::getReadoutNumber(int partitionNumber)
{return readoutNumber[partitionNumber];}
inline  unsigned int MuonGetInfoTool::getChamberPerRegion(int partitionNumber)
{return chamberPerRegion[partitionNumber];}
inline  unsigned int MuonGetInfoTool::getGapPerRegion(int partitionNumber)
{return gapPerRegion[partitionNumber];}
inline  unsigned int MuonGetInfoTool::getLogMapPerRegion(int partitionNumber)
{return LogMapPerRegion[partitionNumber];}
inline  float MuonGetInfoTool::getStartGapZ(int numberOfGap,
                                            int partitionNumber)
{return startGapZ[numberOfGap][partitionNumber];}
inline  float MuonGetInfoTool::getStopGapZ(int numberOfGap,int partitionNumber)
{return stopGapZ[numberOfGap][partitionNumber];}

inline  unsigned int MuonGetInfoTool::getReadoutType( int readout, 
                                                      int partitionNumber)
{return readoutType[readout][partitionNumber];}
inline  unsigned int MuonGetInfoTool::getLogMapRType (int readout, 
                                                      int partitionNumber)
{return LogMapRType[readout][partitionNumber];}
inline  unsigned int MuonGetInfoTool::getLogMapMergex(int readout,
                                                      int partitionNumber)
{return LogMapMergex[readout][partitionNumber];}
inline  unsigned int MuonGetInfoTool::getLogMapMergey(int readout,
                                                      int partitionNumber)
{return LogMapMergey[readout][partitionNumber];}

inline  unsigned int MuonGetInfoTool::getPhChannelNX( int readout, 
                                                      int partitionNumber)
{return phChannelNX[readout][partitionNumber];}
inline  unsigned int MuonGetInfoTool::getPhChannelNY( int readout, 
                                                      int partitionNumber)
{return phChannelNY[readout][partitionNumber];}
inline  float MuonGetInfoTool::getPhChannelSizeX( int readout, int 
                                               partitionNumber)
{return phChannelSizeX[readout][partitionNumber];}
inline  float MuonGetInfoTool::getPhChannelSizeY( int readout, 
                                                  int partitionNumber)
{return phChannelSizeY[readout][partitionNumber];}
inline  float MuonGetInfoTool::getStartChamberPositionX(int chamber )
{return startPositionChamberX[chamber];}
inline  float MuonGetInfoTool::getStartChamberPositionY(int chamber )
{return startPositionChamberY[chamber];}
inline  float MuonGetInfoTool::getStartPositionFirstGapZ(int chamber )
{return startPositionFirstGapZ[chamber];}
inline  unsigned int MuonGetInfoTool::getGridX(int chamber)
{ return gridXChamber[chamber];}
inline  unsigned int MuonGetInfoTool::getGridY(int chamber)
{ return  gridYChamber[chamber];}
inline  unsigned int MuonGetInfoTool::getQuadrantChamber(int chamber)
{ return quarterChamber[chamber];}
inline  int MuonGetInfoTool::getGapPerFE(int partitionNumber)
{return  gapPerFE[partitionNumber]; } 
inline int MuonGetInfoTool::getLayoutX(int map, int partitionNumber)
{ return layoutX[map][partitionNumber];}
inline int MuonGetInfoTool::getLayoutY( int map,  int partitionNumber)
{return layoutY[map][partitionNumber];}


#endif // MUONGETINFOTOOL_H
