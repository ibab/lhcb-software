// $Id: DeMuonChamber.h,v 1.4 2002-09-27 13:59:36 dhcroft Exp $
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

#include "DetDesc/DetectorElement.h"
#include "MuonDet/CLID_DeMuonChamber.h"

/// Gaudi interfaces
#include "GaudiKernel/IService.h"
#include "GaudiKernel/SmartDataPtr.h"

/** @class DeMuonChamber DeMuonChamber.h MuonDet/DeMuonChamber.h
 *  
 *  Detector element class for a single chamber in the muon system
 *
 *  @author David Hutchcroft
 *  @date   21/01/2002
 */

class DeMuonChamber: public DetectorElement {

public:

  /// Constructor (empty)
  DeMuonChamber();

  /// Constructor used by XmlMuonRegionCnv to create chambers
  /// pad sizes in mm 
  DeMuonChamber( int nStation, int nRegion, int nChamber, int nGasGaps);

  /// Destructor
  ~DeMuonChamber();

  inline virtual const CLID& clID() const {
    return classID();
  }

  /// Retrieve reference to class identifier
  inline static const CLID& classID(){
    return CLID_DEMuonChamber;
  }

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

  /// Stores the number of active gas gaps in the detector
  void setNumberGasGaps( int nGasGaps ){
    m_NumberGasGaps=nGasGaps;
  }

  /// Returns the number of active gas gaps in the detector
  inline int numberGasGaps() const {
    return m_NumberGasGaps;
  }
  
private:

  /// Station number
  int m_StationNumber;

  /// Region number in station
  int m_RegionNumber;

  /// Chamber number in region
  int m_ChamberNumber;

  /// Number of gas gaps in this chamber
  int m_NumberGasGaps;

};

#endif    // MUONDET_DEMUONCHAMBER_H
