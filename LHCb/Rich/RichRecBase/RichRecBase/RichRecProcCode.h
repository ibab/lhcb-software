// $Id: RichRecProcCode.h,v 1.1 2003-06-30 15:11:57 jonrob Exp $
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
      ProcStatAbort     = -1,
      ReachedPixelLimit = -2,
      NoRichTracks      = -3,
      ReachedTrackLimit = -4
    };

  }
}

#endif // RICHRECBASE_RICHRECPROCCODE_H
