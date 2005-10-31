// $Id: DeMuonGasGap.h,v 1.5 2005-10-31 15:27:28 asarti Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2005/10/25 06:55:46  asarti
// New .h classes
//
// Revision 1.3  2002/02/21 16:38:30  dhcroft
// Added methods to retrieve the number of the station, region, chamber and gas gap to DeMuonChamber and
// DeMuonGasGap objects. Modified XmlMuonRegionCnv to fill these parameters when making the objects.
//
// Revision 1.2  2002/01/31 10:00:09  dhcroft
// Moved CLIDs to seperate files for Visual C linker
//
//
// ============================================================================
#ifndef MUONDET_DEMUONGASGAP_H
#define MUONDET_DEMUONGASGAP_H 1


// Include files
#include <vector>
#include <string>

#include "DetDesc/DetectorElement.h"

/// Gaudi interfaces
#include "GaudiKernel/IService.h"
#include "GaudiKernel/SmartDataPtr.h"

/** @class DeMuonGasGap DeMuonGasGap.h MuonDet/DeMuonGasGap.h
 *  
 *  Detector element class for a single gas gap in the muon system
 *
 *  Does not currently do anything beyond a base detetcor element
 *
 *  @author David Hutchcroft
 *  @date   21/01/2002
 */


/// CLID of a Muon system gas gap
static const CLID& CLID_DEMuonGasGap = 11007;  

class DeMuonGasGap: public DetectorElement {

public:

  /// Constructor
  DeMuonGasGap();

  /// Constructor with chamber number and gas gap number known
  DeMuonGasGap(int nStation, int nRegion, int nChamber, int nGasGap);

  /// Destructor
  ~DeMuonGasGap();

  inline static const CLID& classID(){
    return CLID_DEMuonGasGap;
  }

  inline virtual const CLID& clID() const {
    return classID();
  }

  //Initialize of Detector Element
  virtual StatusCode initialize();

  /// get Station Number
  inline int stationNumber() const {
    return m_StationNumber;
  }

  /// set Station Number
  void setStationNumber(int nStation){
    m_StationNumber = nStation;
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

  /// get Gas Gap number
   inline int gasGapNumber() const {
    return m_GasGapNumber;
  }

  /// set Gas Gap Number
  void setGasGapNumber(int nGasGap){
    m_GasGapNumber = nGasGap;
  }

private:

  /// Station number
  int m_StationNumber;

  /// Region number in station
  int m_RegionNumber;

  /// Number of the chamber within the region
  int m_ChamberNumber;

  /// Number of the gas gap within the chamber
  int m_GasGapNumber;

};

#endif    // MUONDET_DEMUONGASGAP_H
