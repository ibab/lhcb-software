// $Id: DeMuonRegion.h,v 1.10 2007-11-28 08:00:21 cattanem Exp $
// ============================================================================
#ifndef MUONDET_DEMUONREGION_H
#define MUONDET_DEMUONREGION_H 1


// Include files
#include "DetDesc/DetectorElement.h"

/** @class DeMuonRegion DeMuonRegion.h MuonDet/DeMuonRegion.h
 *  
 *  Detector element class for a region in the muon system
 *
 *  The Xml converted for this object also makes all of the chamber objects
 *  at the same time. 
 *
 *  @author David Hutchcroft
 *  @date   21/01/2002
 */

/// Class ID of DeMuonRegion
static const CLID CLID_DEMuonRegion = 11005;  

class DeMuonRegion: public DetectorElement {

public:
  /// Constructor, empty
  DeMuonRegion();

  /// Destructor
  ~DeMuonRegion();


  inline virtual const CLID& clID() const {
    return classID();
  }

  static const CLID& classID(){  return CLID_DEMuonRegion;  }

  StatusCode initialize();


private:

};

#endif    // MUONDET_DEMUONREGION_H
