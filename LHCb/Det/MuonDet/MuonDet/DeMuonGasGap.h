// $Id: DeMuonGasGap.h,v 1.1.1.1 2002-01-21 17:13:46 dhcroft Exp $
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

/// Gaudi interfaces
#include "GaudiKernel/IService.h"
#include "GaudiKernel/SmartDataPtr.h"

// External declarations
extern const CLID& CLID_DEMuonGasGap;


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

  const CLID& clID() const {
    return classID();
  }

  /// Retrieve reference to class identifier
  static const CLID& classID();

private:
};

#endif    // MUONDET_DEMUONGASGAP_H
