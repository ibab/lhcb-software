// $Id: DeMuonRegion.h,v 1.3 2002-03-20 16:43:28 dhcroft Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2002/01/31 10:00:09  dhcroft
// Moved CLIDs to seperate files for Visual C linker
//
//
// ============================================================================
#ifndef MUONDET_DEMUONREGION_H
#define MUONDET_DEMUONREGION_H 1


// Include files
#include "DetDesc/DetectorElement.h"
#include "MuonDet/CLID_DeMuonRegion.h"
#include "MuonDet/MuonParameters.h"

/** @class DeMuonGasGap DeMuonGasGap.h MuonDet/DeMuonGasGap.h
 *  
 *  Detector element class for a single gas gap in the muon system
 *
 *  The Xml converted for this object also makes all of the chamber objects
 *  at the same time. 
 *
 *  @author David Hutchcroft
 *  @date   21/01/2002
 */

class DeMuonRegion: public DetectorElement {

public:
  /// Constructor, empty
  DeMuonRegion();

  /// Destructor
  ~DeMuonRegion();

  /// Class ID of DeMuonRegion
  inline virtual const CLID& clID() const {
    return classID();
  }

  /// Retrieve reference to class identifier
  inline static const CLID& classID(){
    return CLID_DEMuonRegion;
  }

  /// Set the number of chambers in the region
  void setchamberNum( int numberChambers ){
    m_chamberNum=numberChambers;
  }

  /// Return the number of chambers in the region
  inline int chamberNum(){
    return m_chamberNum;
  }

  /// Set the number of FE Anode readout channels cover a chamber in x
  void setFEAnodeX( int nFEx ){
    m_FEAnodeX = nFEx;
  }

  /// Return the number of FE Anode readout channels cover a chamber in x
  inline int FEAnodeX(){
    return m_FEAnodeX;
  }
    
  /// Set the number of FE Anode readout channels cover a chamber in y
  void setFEAnodeY( int nFEy ){
    m_FEAnodeY = nFEy;
  }

  /// Return the number of FE Anode readout channels cover a chamber
  inline int FEAnodeY(){
    return m_FEAnodeY;
  }

  /// Set the number of FE Cathode readout channels cover a chamber in x
  void setFECathodeX( int nFEx ){
    m_FECathodeX = nFEx;
  }

  /// Return the number of FE Cathode readout channels cover a chamber in x
  inline int FECathodeX(){
    return m_FECathodeX;
  }
    
  /// Set the number of FE Cathode readout channels cover a chamber in y
  void setFECathodeY( int nFEy ){
    m_FECathodeY = nFEy;
  }

  /// Return the number of FE Cathode readout channels cover a chamber
  inline int FECathodeY(){
    return m_FECathodeY;
  }

  /// Add a logical mapping between FE readout channels and 
  /// logical channels (or pads if not crossed)
  void addLogicalMap( const MuonParameters::ReadoutType &type, 
                      const int &xMap, const int &yMap);

  /// Nulls logical to FE mapping 
  void clearLogicalMap(){
    m_logMap.clear();
  }

  /// Get the number of logical maps stored
  inline unsigned int numberLogicalMap(){
    return m_logMap.size();
  }

  /// Get the logical mappings beween FE readout channels and 
  /// logical channels (or pads if not crossed)
  StatusCode logicalMap(const unsigned int& mapNum,
                        MuonParameters::ReadoutType &rType,
                        int &xMap, int &yMap);

private:

  /// The number of chambers in this region
  int m_chamberNum;

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

  class logMap_{
  public:
    logMap_(MuonParameters::ReadoutType type,int x,int y) : 
      m_type(type),m_xMap(x),m_yMap(y)
    {}      
  public:
    MuonParameters::ReadoutType m_type;
    int m_xMap;
    int m_yMap;
  };

  /// The logical channel mappings for X amd Y for up to two sets of strips
  std::vector<logMap_> m_logMap;

};

#endif    // MUONDET_DEMUONREGION_H
