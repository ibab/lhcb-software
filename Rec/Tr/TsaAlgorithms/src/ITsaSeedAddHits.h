// $Id: ITsaSeedAddHits.h,v 1.1 2006-12-06 14:35:00 mneedham Exp $
#ifndef _ITsaSeedAddHits_H
#define _ITsaSeedAddHits_H

/** @class ITsaStep TsaKernel/ITsaStep
 *
 *  Interface to data svc
 *
 *  @author M.Needham
 *  @date   12/11/2006
 */

#include "GaudiKernel/IAlgTool.h"
#include "TsaKernel/SeedTrack.h"
#include "TsaKernel/SeedHit.h"

static const InterfaceID IID_ITsaSeedAddHits( "ITsaSeedAddHits", 0, 0 );

#include <vector>



class ITsaSeedAddHits: virtual public IAlgTool {
public:

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_ITsaSeedAddHits ; }

  virtual StatusCode execute(SeedTracks* seeds, SeedHits* hits) = 0;

};

#endif
