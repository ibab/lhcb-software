// $Id: DeMuonRegion.h,v 1.1.1.1 2002-01-21 17:13:46 dhcroft Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
//
// ============================================================================
#ifndef MUONDET_DEMUONREGION_H
#define MUONDET_DEMUONREGION_H 1


// Include files
#include "DetDesc/DetectorElement.h"


// External declarations
extern const CLID& CLID_DEMuonRegion;

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

  /// Constructor, initialise chamber 
  DeMuonRegion( int numberChambers );

  /// Destructor
  ~DeMuonRegion();

  /// Class ID of DeMuonRegion
  const CLID& clID() const {
    return classID();
  }

  /// Retrieve reference to class identifier
  static const CLID& classID();

  /// Set the number of chambers in the region
  void setchamberNum( int numberChambers ){
    m_chamberNum=numberChambers;
  }

  /// Return the number of chambers in the region
  int chamberNum(){
    return m_chamberNum;
  }

private:

  /// The number of chambers in this region
  int m_chamberNum;

};

#endif    // MUONDET_DEMUONREGION_H
