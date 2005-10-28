// $Id: MuonChamberLayout.h,v 1.2 2005-10-28 09:59:40 asarti Exp $
#ifndef PUBLIC_MUONCHAMBERLAYOUT_H 
#define PUBLIC_MUONCHAMBERLAYOUT_H 1

// Include files

//From Muon
#include "MuonDet/DeMuonChamber.h"
#include "MuonKernel/MuonLayout.h"
#include "MuonKernel/MuonTileID.h"

//Gaudi
#include "GaudiKernel/IDataProviderSvc.h"

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
		    IDataProviderSvc* detSvc); 
  
  // Void constructor: calls the previous one
  // with dummy DataProvider and default grid Layout
  MuonChamberLayout(); 

  virtual ~MuonChamberLayout( ); /// Destructor
    
  //Fill the vector with all the chambers
  virtual StatusCode initialize();

  //Copy function
  void Copy(MuonChamberLayout &lay);

  //Fill the vector with all the chambers
  void fillChambersVector(IDataProviderSvc* detSvc); 

  //Find the most likely chamber for a given x,y,station set
  void chamberMostLikely(float x,float y, int station, int& chmb, int& reg);
  
  //Return the tiles of the neighbor Chambers
  std::vector<DeMuonChamber*> neighborChambers(DeMuonChamber *Chmb, int x_direction, int y_direction);
  
  //Return the tiles of the neighbor Chambers
  std::vector<DeMuonChamber*> neighborChambers(int chmbNum, int sta, int reg, int x_dir, int y_dir);
  
  //Convert tiles in chambers
  std::vector<DeMuonChamber*> createChambers(std::vector<int> mytiles, int station);

  //Returns the chamber number for a shift in X and Y direction
  void chamberXY(int sx, int sy, int shx, int shy, int reg, std::vector<int> &chamberNumber);

  //Returns the Tile for a given chamber
  MuonTileID tileChamber(DeMuonChamber* chmb);

  //Returns the Tile for a given chamber number
  MuonTileID tileChamberNumber(int sta, int reg, int chmbNum);

  //Returns the region for a given chamber with numbering scheme 
  //defined in the MuonGeometry.h file
  int findRegion(int chamber);

  //Returns the grid indexes for a given X,Y couple
  void gridPosition(float x, float y, int iS, int &idx, int &idy, int &reg);

  //Sets the xS, yS reference values (needed for grid computation)
  void setGridStep();

  //Checks if a region lowering is needed
  bool shouldLowReg(int idX, int idY, int reg);

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

protected:

private:

  //Chambers
  std::vector<int> m_offSet;
  std::vector<unsigned int> m_cgX;
  std::vector<unsigned int> m_cgY;

  //Smallest chmb dimensions
  std::vector<float> m_xS;
  std::vector<float> m_yS;

  MuonLayout m_layout[4];

  //ChamberVector as a function of the ixds in the region grid
  std::vector<int> m_chamberGrid;

  //My data provider
  IDataProviderSvc* m_detSvc;

  unsigned int offset[4];

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
