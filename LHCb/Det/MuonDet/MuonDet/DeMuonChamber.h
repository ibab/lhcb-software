// $Id: DeMuonChamber.h,v 1.10 2009-10-02 13:24:19 asatta Exp $
// ============================================================================
// CVS tag $Name:
// ============================================================================
//
// ============================================================================
#ifndef MUONDET_DEMUONCHAMBER_H
#define MUONDET_DEMUONCHAMBER_H 1


// Include files
#include <vector>
#include <string>
#include <sstream>

#include "GaudiKernel/MsgStream.h"

#include "DetDesc/DetectorElement.h"
#include "MuonDet/MuonNamespace.h"
#include "DetDesc/PVolume.h"

/** @class DeMuonChamber DeMuonChamber.h MuonDet/DeMuonChamber.h
 *  
 *  Detector element class for a single chamber in the muon system
 *
 *  @author David Hutchcroft
 *  @date   21/01/2002
 */

/// Class ID of chambers 
static const CLID CLID_DEMuonChamber = 11006;  

class DeMuonChamber: public DetectorElement {

public:

  /// Constructor (empty)
  DeMuonChamber();

  /// Constructor used by XmlMuonRegionCnv to create chambers
  /// pad sizes in mm 
  DeMuonChamber( int nStation, int nRegion, int nChamber);

  /// Destructor
  ~DeMuonChamber();

  inline static const CLID& classID(){
    return CLID_DEMuonChamber;
  }

  inline virtual const CLID& clID() const {
    return classID();
  }

  //Initialize
  StatusCode initialize();

  /// get Station Number
  //  inline int stationNumber() const {
  inline std::string stationNumber() const {
    return m_StationName.str();
  }

  /// set Station Number (from 0 -> nStations)
  void setStationNumber(int nStation){
    m_StationNumber = nStation;
    m_StationName <<"M"<<nStation+1;
  }

  /// get Region Number
  inline int regionNumber() const {
    return m_RegionNumber;
  }

  /// set Region Number
  void setRegionNumber(int nRegion){
    m_RegionNumber = nRegion;
  }

  /// get Chamber Number
  inline int chamberNumber() const {
    return m_ChamberNumber;
  }

  /// set Chamber Number
  void setChamberNumber(int nChamber){
    m_ChamberNumber = nChamber;
  }

  /// get chamber Grid 
  inline std::string getGridName() const {
    return m_chmbGrid;
  }

  /// set chamber Grid
  void setGridName(std::string grid){
    m_chmbGrid = grid;
  }

  IPVolume* getFirstGasGap();
  IPVolume* getGasGap(int number);
  int getGasGapNumber();
  StatusCode  isPointInGasGap(Gaudi::XYZPoint 
  pointInChamber,Gaudi::XYZPoint&
  pointInGap,IPVolume* & gasVolume);
  StatusCode  isPointInGasGap(Gaudi::XYZPoint 
  pointInChamber,Gaudi::XYZPoint&
  pointInGap,int& number,IPVolume*  & gasVolume);
  IPVolume* getGasGapLayer(int number);

private:

  /// Access to Msgstream object
  inline MsgStream & msgStream() const
  {
    if ( !m_msgStream ) m_msgStream = new MsgStream(msgSvc(),name());
    return *m_msgStream;
  }

private:

  mutable MsgStream * m_msgStream;

  /// Chamber Grid
  std::string m_chmbGrid;

  /// Station number
  int m_StationNumber;

  /// Region number in station
  int m_RegionNumber;

  /// Chamber number in region
  int m_ChamberNumber;

  /// Station name
  std::stringstream m_StationName;
};

#endif    // MUONDET_DEMUONCHAMBER_H
