// $Id: DeMuonRegion.h,v 1.2 2005-12-08 10:02:43 cattanem Exp $
#ifndef MUONDET_DEMUONREGION_H
#define MUONDET_DEMUONREGION_H 1

// Include files
#include "DetDesc/DetectorElement.h"

/** @class DeMuonGasGap DeMuonGasGap.h MuonDet/DeMuonGasGap.h
 *  
 *  Detector element class for a single gas gap in the muon system
 *
 *  @author David Hutchcroft
 *  @author Sebastien Ponce
 */

class DeMuonRegion: public DetectorElement {

public:
  /**
   * Constructor for this class
   */
  DeMuonRegion();

  /**
   * Default destructor
   */
  ~DeMuonRegion() {};

  /**
   * Retrieves reference to class identifier
   * @return the class identifier for this class
   */
  inline virtual const CLID& clID() const {
    return classID();
  }

  /**
   * Retrieves reference to class identifier
   * @return the class identifier for this class
   */
  static const CLID& classID();

  /// Set the number of chambers in the region
  void setchamberNum (int numberChambers){
    m_chamberNum=numberChambers;
  }

  /// Return the number of chambers in the region
  inline int chamberNum() const {
    return m_chamberNum;
  }

  /// Set the number of gaps that are connect to one FE channel
  void setGapsPerFE (int nGaps){
    m_GapsPerFE = nGaps;
  }
  
  /// Return the number of gaps that are connected to one FE Channel
  inline int gapsPerFE() const {
    return m_GapsPerFE;
  }

  /// Set the number of FE Anode readout channels cover a chamber in x
  void setFEAnodeX( int nFEx ){
    m_FEAnodeX = nFEx;
  }

  /// Return the number of FE Anode readout channels cover a chamber in x
  inline int FEAnodeX() const {
    return m_FEAnodeX;
  }
    
  /// Set the number of FE Anode readout channels cover a chamber in y
  void setFEAnodeY( int nFEy ){
    m_FEAnodeY = nFEy;
  }

  /// Return the number of FE Anode readout channels cover a chamber
  inline int FEAnodeY() const {
    return m_FEAnodeY;
  }

  /// Set the number of FE Cathode readout channels cover a chamber in x
  void setFECathodeX( int nFEx ){
    m_FECathodeX = nFEx;
  }

  /// Return the number of FE Cathode readout channels cover a chamber in x
  inline int FECathodeX() const {
    return m_FECathodeX;
  }
    
  /// Set the number of FE Cathode readout channels cover a chamber in y
  void setFECathodeY( int nFEy ){
    m_FECathodeY = nFEy;
  }

  /// Return the number of FE Cathode readout channels cover a chamber
  inline int FECathodeY() const {
    return m_FECathodeY;
  }

private:
  /// The number of chambers in this region
  int m_chamberNum;

  /// Number of gaps in a hardwired OR to each FE channel
  int m_GapsPerFE;

  /// The number of FE channels in the X direction for Anode readout 
  /// (0 means no anode readout)
  int m_FEAnodeX;

  /// The number of FE channels in the Y direction for Anode readout 
  /// (0 means no anode readout)
  int m_FEAnodeY;

  /// The number of FE channels in the X direction for Cathode readout 
  /// (0 means no Cathode readout)
  int m_FECathodeX;

  /// The number of FE channels in the Y direction for Cathode readout 
  /// (0 means no Cathode readout)
  int m_FECathodeY;

};

#endif    // MUONDET_DEMUONREGION_H
