
//-----------------------------------------------------------------------------
/** @file RichRecProcCode.h
 *
 * Header file for enumeration Rich::Rec::ProcStat
 *
 * CVS Log :-
 * $Id: RichRecProcCode.h,v 1.4 2004-11-20 12:30:24 jonrob Exp $
 * $Log: not supported by cvs2svn $
 * Revision 1.3  2004/07/26 18:00:58  jonrob
 * Various improvements to the doxygen comments
 *
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date   2003-05-10
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECBASE_RICHRECPROCCODE_H
#define RICHRECBASE_RICHRECPROCCODE_H 1

/** @namespace Rich
 *
 *  General namespace for RICH specific definitions
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
namespace Rich {

  /** @namespace Rich::Rec
   *
   *  General namespace for RICH reconstruction specific definitions
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   08/07/2004
   */
  namespace Rec {

    /** @enum Rich::Rec::ProcStat
     *
     *  Enumeration for RICH reconstruction processing status codes
     */
    enum ProcStat
      {
        ProcStatAbort         = -1, ///< Processing was aborted prior to the RICH
        NoRichPixels          = -2, ///< No RICH pixels were found
        ReachedPixelLimit     = -3, ///< The maximum number of pixels was exceeded
        NoRichTracks          = -4, ///< No tracks were found with RICH information
        ReachedTrTrackLimit   = -5, ///< The maximum number of raw tracks was exceeded
        ReachedRichTrackLimit = -6  ///< The maximum number of tracks with RICH information was exceeded
      };

  }

}

#endif // RICHRECBASE_RICHRECPROCCODE_H
