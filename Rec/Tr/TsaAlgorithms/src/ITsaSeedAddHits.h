// $Id: ITsaSeedAddHits.h,v 1.2 2007-03-21 14:35:01 cattanem Exp $
#ifndef _ITsaSeedAddHits_H
#define _ITsaSeedAddHits_H

/** @class ITsaSeedAddHits ITsaSeedAddHits.h
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
