// $Id: RichRecProcCode.h,v 1.2 2003-08-06 09:55:31 jonrob Exp $
#ifndef RICHRECBASE_RICHRECPROCCODE_H
#define RICHRECBASE_RICHRECPROCCODE_H 1

// Include files

/** @class RichRecProcCode RichRecProcCode.h RichRecBase/RichRecProcCode.h
 *
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-05-10
 */


namespace Rich {
  namespace Rec {

    enum ProcStat {
      ProcStatAbort         = -1,
      ReachedPixelLimit     = -2,
      NoRichTracks          = -3,
      ReachedTrTrackLimit   = -4,
      ReachedRichTrackLimit = -5
    };

  }
}

#endif // RICHRECBASE_RICHRECPROCCODE_H
