#ifndef MUONALGS_MUONDIGITIZATIONPARAMETERS_H
#define MUONALGS_MUONDIGITIZATIONPARAMETERS_H 1

namespace MuonDigitizationParameters {
class Parameters{
   public:
  void setReadoutNumber(int partitionNumber, int value);
  void setChamberPerRegion(int partitionNumber, unsigned int value);
  void setReadoutType( int readout, int partitionNumber, unsigned int value);
	void setPhChannelNX( int readout, int partitionNumber, unsigned int value);
	void setPhChannelNY( int readout, int partitionNumber, unsigned int value);
	void setPhChannelSizeX( int readout, int partitionNumber, float value);
	void setPhChannelSizeY( int readout, int partitionNumber, float value);
	void setStartChamberPositionX(int chamber, float xPosition);
	void setStartChamberPositionY(int chamber, float yPosition);
	
	
	
  int getReadoutNumber(int partitionNumber);
  unsigned int getChamberPerRegion(int partitionNumber);
  unsigned int getReadoutType( int readout, int partitionNumber);
	unsigned int getPhChannelNX( int readout, int partitionNumber);
	unsigned int getPhChannelNY( int readout, int partitionNumber);
	float getPhChannelSizeX( int readout, int partitionNumber);
	float getPhChannelSizeY( int readout, int partitionNumber);
	float getStartChamberPositionX(int chamber );
	float getStartChamberPositionY(int chamber );
	
  private:
  static const int partition=20;
  static const int maxReadoutType=2;
  static const int numberChamber=1380;
  static const int numberGap=4;
  static int readoutNumber[partition];
  static unsigned int chamberPerRegion[partition];
  static unsigned int readoutType[maxReadoutType][partition];
  static unsigned int phChannelNX[maxReadoutType][partition];
  static unsigned int phChannelNY[maxReadoutType][partition];
  static float phChannelSizeX[maxReadoutType][partition];
  static float phChannelSizeY[maxReadoutType][partition];
  static float startPositionChamberX[numberChamber];
  static float startPositionChamberY[numberChamber];
};



 inline  void setReadoutNumber(int partitionNumber, int value){readoutNumber[partitionNumber]=value ;}
 inline  void setChamberPerRegion(int partitionNumber, unsigned int value){chamberPerRegion[partitionNumber]=value ;}
 inline  void setReadoutType( int readout, int partitionNumber, unsigned int value){readoutType[readout][partitionNumber]=value ;}
 inline void setPhChannelNX( int readout, int partitionNumber, unsigned int value){phChannelNX[readout][partitionNumber]=value ;}
 inline void setPhChannelNY( int readout, int partitionNumber, unsigned int value){phChannelNY[readout][partitionNumber]=value ;}
 inline void setPhChannelSizeX( int readout, int partitionNumber, float value){phChannelSizeX[readout][partitionNumber]=value ;}
 inline void setPhChannelSizeY( int readout, int partitionNumber, float value){phChannelSizeY[readout][partitionNumber]=value ;}
 inline void setStartChamberPositionX(int chamber float xPosition){startPositionChamberX[chamber]=value;}
 inline void setStartChamberPosition(Yint chamber float yPosition){startPositionChamberY[chamber]=value;}

  inline  int getReadoutNumber(int partitionNumber){return readoutNumber[partitionNumber];}
  inline  unsigned int getChamberPerRegion(int partitionNumber){return chamberPerRegion[partitionNumber];}
  inline  unsigned int getReadoutType( int readout, int partitionNumber){return readoutType[readout][partitionNumber];}
	inline  unsigned int getPhChannelNX( int readout, int partitionNumber){return phChannelNX[readout][partitionNumber];}
	inline  unsigned int getPhChannelNY( int readout, int partitionNumber){return phChannelNY[readout][partitionNumber];}
	inline  float getPhChannelSizeX( int readout, int partitionNumber){return phChannelSizeX[readout][partitionNumber];}
	inline  float getPhChannelSizeY( int readout, int partitionNumber){return phChannelSizeY[readout][partitionNumber];}
	inline  float getStartChamberPositionX(int chamber ){return startPositionChamberX[chamber];}
	inline  float getStartChamberPositionY(int chamber ){return startPositionChamberY[chamber];}


};
#endif
