// $Id: DeMuonGasGap.h,v 1.2 2002-01-31 10:00:09 dhcroft Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
//
// ============================================================================
#ifndef MUONDET_DEMUONGASGAP_H
#define MUONDET_DEMUONGASGAP_H 1


// Include files
#include <vector>
#include <string>

#include "DetDesc/DetectorElement.h"
#include "MuonDet/CLID_DeMuonGasGap.h"

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
class DeMuonGasGap: public DetectorElement {

public:

  /// Constructor
  DeMuonGasGap();

  /// Destructor
  ~DeMuonGasGap();

  inline virtual const CLID& clID() const {
    return classID();
  }

  /// Retrieve reference to class identifier
  inline static const CLID& classID(){
    return CLID_DEMuonGasGap;
  }

private:
};

#endif    // MUONDET_DEMUONGASGAP_H
