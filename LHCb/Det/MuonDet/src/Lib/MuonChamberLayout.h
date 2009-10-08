// $Id: MuonChamberLayout.h,v 1.5 2009-10-08 15:20:47 asatta Exp $
#ifndef PUBLIC_MUONCHAMBERLAYOUT_H 
#define PUBLIC_MUONCHAMBERLAYOUT_H 1

#include "GaudiKernel/MsgStream.h"

//From Muon
#include "MuonDet/DeMuonChamber.h"
#include "MuonKernel/MuonSystemLayout.h"

//Gaudi
#include "GaudiKernel/IDataProviderSvc.h"
class IMessageSvc;

/** @class MuonChamberLayout MuonChamberLayout.h MuonDet/MuonChamberLayout.h
 *  
 *
 *  @author Alessio Sarti
 *  @date   2005-10-06
 */
class MuonChamberLayout : public DetectorElement {
public: 

  /// Standard constructor
  MuonChamberLayout(MuonLayout R1,
                    MuonLayout R2,
                    MuonLayout R3,
                    MuonLayout R4,
                    IDataProviderSvc* detSvc,
                    IMessageSvc * msgSvc); 
  
  // Void constructor: calls the previous one
  // with dummy DataProvider and default grid Layout
  MuonChamberLayout(); 

  virtual ~MuonChamberLayout( ); /// Destructor
    
  //Fill the vector with all the chambers
  virtual StatusCode initialize();

  //Copy function
  void Copy(MuonChamberLayout &lay);

  //Fill the vector with all the chambers
  std::vector<DeMuonChamber*> fillChambersVector(IDataProviderSvc* detSvc); 

  //Find the most likely chamber for a given x,y,station set
  void chamberMostLikely(float x,float y, int station, int& chmb, int& reg) const;
  
  //Return the tiles of the neighbor Chambers
  std::vector<int> neighborChambers(float myX, float myY, int stat, int x_direction, int y_direction) const;
  
  //Return the tiles of the neighbor Chambers
  void returnChambers(int sta, float st_x, float st_y, int x_dir, int y_dir, std::vector<int>& regs, std::vector<int>& chs) const;
  
  //Convert tiles in chambers
  std::vector<DeMuonChamber*> createChambersFromTile(std::vector<LHCb::MuonTileID> mytiles);

  //Returns the chamber number for a shift in X and Y direction
  void chamberXY(int sx, int sy, int shx, int shy, int reg, std::vector<int> &chamberNumber) const;

  //Returns the Tile for a given chamber
  LHCb::MuonTileID tileChamber(DeMuonChamber* chmb)const;

  //Returns the Tile for a given chamber number
  LHCb::MuonTileID tileChamberNumber(int sta, int reg, int chmbNum) const ;


  StatusCode Tile2XYZpos(const LHCb::MuonTileID& tile, 
			 double& x, double& deltax,
			 double& y, double& deltay,
			 double& z, double& deltaz);

  //Fill the system grids for a chamber in a given region
  StatusCode fillSystemGrids(DeMuonChamber *deChmb, 
			     int vIdx, int reg);

  //Returns the region for a given chamber with numbering scheme 
  //defined in the MuonGeometry.h file
  int findRegion(int chamber) const ;

  //Returns the grid indexes for a given X,Y couple
  void gridPosition(float x, float y, int iS, int &idx, int &idy, int &reg) const ;

  //Sets the xS, yS reference values (needed for grid computation)
  void setGridStep();

  //Checks if a region lowering is needed
  bool shouldLowReg(int idX, int idY, int reg) const;

  //Returns the layout
  MuonLayout layout(int region);

  //Returns the Data Provider
  IDataProviderSvc* DataProvider();

  void setXGrid(std::vector<unsigned int> xg);

  void setYGrid(std::vector<unsigned int> yg);

  //Sets the layout
  void setLayout(int region, MuonLayout lay);

  //Assigna a given encode to the chamber
  void setChamberInGrid(int enc, int num);

  //Reconstruct the encode of a given chamber
  int getEncode(int idx, int idy, int reg);

  //Set the data Provider
  void setDataProvider(IDataProviderSvc* dataPr);

  //Converts the chamber tile into a chamber number
  int getChamberNumber(const LHCb::MuonTileID& tile);

  //Function for chamber x,y,z retrieval from tile info
  StatusCode getXYZChamberTile(const LHCb::MuonTileID& tile, 
			       double& x, double& deltax,
			       double& y, double& deltay,
			       double& z, double& deltaz,
			       bool toGlob);

  /// get position of a "named" chamber 
  /// NOTE: station and region are indexed from 0 (C style)
  /// chamberNum is the real chamber number (from 0)
  StatusCode getXYZChamber(const int& station,
                           const int& region,
                           const int& chamberNum,
                           double& x, double& deltax,
                           double& y, double& deltay,
                           double& z, double& deltaz,
			   bool toGlob);

  /// get position of a "named" gasgap
  /// NOTE: station, region and gapNum are indexed from 0 (C style)
  /// chamberNum is the real chamber number (from 0)
  StatusCode getXYZGasGap(const int& station,
                          const int& region,
                          const int& chamberNum,
                          const int& gapNum,
                          double& x, double& deltax,
                          double& y, double& deltay,
                          double& z, double& deltaz);
  
  /// get position of chamber or gas gap with caching of results and pointers
  /// NOTE: station, region and gapNum are indexed from 0 (C style)
  /// chamberNum is the real chamber number (from 0)
  StatusCode getXYZ(const int& station,
		    const int& region,
		    const int& chamberNum,
		    const int& gapNum,
		    double& x, double& deltax,
		    double& y, double& deltay,
		    double& z, double& deltaz,
		    bool toGlob);

  /// get xyz of specific pad
  StatusCode getXYZPad(const LHCb::MuonTileID& tile, 
		       double& x, double& deltax,
		       double& y, double& deltay,
		       double& z, double& deltaz);
  
  /// get postion of logical channel (may be multiple chambers)
  StatusCode getXYZLogical(const LHCb::MuonTileID& tile, 
                           double& x, double& deltax,
                           double& y, double& deltay,
                           double& z, double& deltaz);

  /// get xyz of twelfth (useful for defining regions)
  StatusCode getXYZTwelfth(const LHCb::MuonTileID& tile, 
                           double& x, double& deltax,
                           double& y, double& deltay,
                           double& z, double& deltaz);

  /// returns the chamber number (same for each station) on the corner of
  /// the region
  int getTwelfthCorner(const int& region, 
                       const int& twelfth,
                       const int& chamberNum);

  /// get the xIndex and yIndex of the corner chamber in the twelfth
  void getTwelfthCornerIndex(const int& region, 
                             const int& twelfth,
                             const int& chamberNum,
                             int &xPos, int &yPos);

  
  void localToglobal(IGeometryInfo* gInfo,
		     Gaudi::XYZPoint cent, Gaudi::XYZPoint corn,
		     double &dx, double &dy, double &dz);

  ///get the chamber number (vector) from the MuonTile
  std::vector<unsigned int> Tile2ChamberNum(const LHCb::MuonTileID& tile);
  
  ///get the chamber number (vector) from the logical channel tile
  std::vector<unsigned int> Logical2ChamberNum(const LHCb::MuonTileID& tile);
  
  ///get the chamber number (vector) from the twelfth-chamber tile
  std::vector<unsigned int> Twelfth2ChamberNum(const LHCb::MuonTileID& tile);
  
private:

  /// Access to Msgstream object
  inline MsgStream & msgStream() const
  {
    if ( !m_msgStream ) m_msgStream = new MsgStream(m_msgSvc,name());
    return *m_msgStream;
  }

private:
  mutable MsgStream * m_msgStream;
  IMessageSvc* m_msgSvc ;
  //Chambers
  std::vector<int> m_offSet;
  std::vector<unsigned int> m_cgX;
  std::vector<unsigned int> m_cgY;
  std::vector<DeMuonChamber*> m_ChVec;
  std::vector<unsigned int> m_chaVect;

  //Smallest chmb dimensions
  std::vector<float> m_xS;
  std::vector<float> m_yS;

  MuonLayout m_layout[4];

  //ChamberVector as a function of the ixds in the region grid
  std::vector<int> m_chamberGrid;

  //My data provider
  IDataProviderSvc* m_detSvc;

  unsigned int offset[4];

  // size of logical channels
  std::vector<unsigned int> m_logVertGridX;
  std::vector<unsigned int> m_logVertGridY;
  std::vector<unsigned int> m_logHorizGridX;
  std::vector<unsigned int> m_logHorizGridY;
  // size of pads
  std::vector<unsigned int> m_padGridX;
  std::vector<unsigned int> m_padGridY;

};

// -----------------------------------------------------------------------------
//   end of class
// -----------------------------------------------------------------------------

//Returns the Layout
inline MuonLayout MuonChamberLayout::layout(int region){
  //Region index starts from 0 (R1 -> idx = 0)
  return m_layout[region];
}

//Returns the DataProvider
inline IDataProviderSvc* MuonChamberLayout::DataProvider(){
  //Data service
  return m_detSvc;
}

//Sets the xGrid
inline void MuonChamberLayout::setXGrid(std::vector<unsigned int> xg){
  m_cgX = xg;
  return;
}

//Set chamber in grid
inline void MuonChamberLayout::setChamberInGrid(int enc, int num){
  m_chamberGrid.at(enc) = num;
  return;
}

//Sets the Layout
inline void MuonChamberLayout::setYGrid(std::vector<unsigned int> yg){
  m_cgY = yg;
  return;
}

//Sets the Layout
inline void MuonChamberLayout::setLayout(int region, MuonLayout lay){
  //Region index starts from 0 (R1 -> idx = 0)
  m_layout[region] = lay;
  return;
}

//Gets the encoding
inline int MuonChamberLayout::getEncode(int idx, int idy, int reg){
  //Returns  the encoding of a given chamber
  int enc = idx+4*m_cgX.at(reg)*idy+m_offSet.at(reg);
  return enc;
}

inline void MuonChamberLayout::setDataProvider(IDataProviderSvc* dataPr)
{
  //Set the data Provder
  m_detSvc = dataPr;
  return;
}

#endif // PUBLIC_MUONCHAMBERLAYOUT_H
