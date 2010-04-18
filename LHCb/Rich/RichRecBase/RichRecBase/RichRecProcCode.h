
//-----------------------------------------------------------------------------
/** @file RichRecProcCode.h
 *
 * Header file for enumeration Rich::Rec::ProcStat
 *
 * $Id: RichRecProcCode.h,v 1.7 2007-03-09 18:04:34 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date   2003-05-10
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECBASE_RICHRECPROCCODE_H
#define RICHRECBASE_RICHRECPROCCODE_H 1

namespace Rich 
{
  namespace Rec 
  {

    /** @enum Rich::Rec::ProcStat
     *
     *  Enumeration for RICH reconstruction processing status codes
     */
    enum ProcStat
      {
        ProcStatAbort          = -1, ///< Processing was aborted prior to the RICH
        NoRichPixels           = -2, ///< No RICH pixels were found
        ReachedPixelLimit      = -3, ///< The maximum number of pixels was exceeded
        NoRichTracks           = -4, ///< No tracks were found with RICH information
        ReachedTrTrackLimit    = -5, ///< The maximum number of raw tracks was exceeded
        ReachedRichTrackLimit  = -6, ///< The maximum number of tracks with RICH information was exceeded
        ReachedRichPhotonLimit = -7  ///< The maximum number of RICH photon candidates was exceeded
      };

  }

}

#endif // RICHRECBASE_RICHRECPROCCODE_H
