
//-----------------------------------------------------------------------------
/** @file RichRecPhotonKey.h
 *
 * Header file for utility class : RichRecPhotonKey
 *
 * $Id: RichRecPhotonKey.h,v 1.8 2007-02-01 17:26:22 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date   2003-06-20
 */
//-----------------------------------------------------------------------------

#ifndef RichRecEvent_RichRecPhotonKey_H
#define RichRecEvent_RichRecPhotonKey_H 1

// Gaudi
#include "GaudiKernel/StreamBuffer.h"

//-----------------------------------------------------------------------------
/** @namespace Rich
 *
 *  General namespace for RICH software
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//-----------------------------------------------------------------------------
namespace Rich
{


  /** @namespace Rich::Rec
   *
   *  General namespace for RICH reconstruction software
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   08/07/2004
   */
  namespace Rec
  {

    /** @class PhotonKey RichRecPhotonKey.h RichRecBase/RichRecPhotonKey.h
     *
     *  Custom Key for RichRecPhotons. Encodes the parent RichRecSegment and
     *  RichRecPixel key values into a unique key for each RichRecPhoton.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date 15/02/2002
     */

    class PhotonKey
    {

    public:

      /** Constructor from long int
       *  @param key The raw data key to use as the bit-packed data
       */
      PhotonKey( const long key = 0 ) : m_key ( key ) { }

      /** Constructor from segment and pixel numbers
       *
       *  @param pixelKey    The key for the associated RichRecPixel
       *  @param segmentKey  The key for the associated RichRecSegment
       */
      PhotonKey ( const long pixelKey,
                  const long segmentKey )
        : m_key ( pixelKey%65536 | segmentKey<<16 ) { }

      /// Destructor
      ~PhotonKey() {}

      /// Retrieve 32 bit integer key
      inline long key() const
      {
        return m_key;
      }

      /// long operator
      inline operator long() const
      {
        return key();
      }

      /// Update 32 bit integer key
      inline void setKey(const long key)
      {
        m_key = key;
      }

      /// Retrieve associated RichRecSegment key
      inline int segmentNumber() const
      {
        return key()/65536;
      }

      /// Retrieve associated RichRecPixel key
      inline int pixelNumber() const
      {
        return key()%65536;
      }

    public:

      /// Fill the ASCII output stream
      inline std::ostream& fillStream(std::ostream& s) const
      {
        return s << "{ " << " key:\t" << key() << " } ";
      }

      /// Implement textual ostream << method for Rich::Rec::TrackID
      friend inline std::ostream& operator << ( std::ostream& s,
                                                const PhotonKey& key )
      {
        return key.fillStream(s);
      }

    private:

      /// 32 bit integer key.
      /// First 16 bits are segment number, last 16 pixel number.
      unsigned long m_key;

    };

  }
} // RICH

/** Backwards compatibility typedef
 * @todo Remove eventually
 */
typedef Rich::Rec::PhotonKey RichRecPhotonKey;

#endif   // RichRecEvent_RichRecPhotonKey_H
