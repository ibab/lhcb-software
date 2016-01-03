// $Id: DeMuonRegion.h,v 1.11 2009-09-14 08:58:35 jonrob Exp $
// ============================================================================
#ifndef MUONDET_DEMUONREGION_H
#define MUONDET_DEMUONREGION_H 1

#include "GaudiKernel/MsgStream.h"

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

  /// Access to Msgstream object
  inline MsgStream & msgStream() const
  {
    if ( !m_msgStream ) m_msgStream = new MsgStream(msgSvc(),name());
    return *m_msgStream;
  }

private:

  mutable MsgStream * m_msgStream;

};

#endif    // MUONDET_DEMUONREGION_H
