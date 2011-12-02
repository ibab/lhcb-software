
//-----------------------------------------------------------------------------
/** @file RichRecPhotonKey.h
 *
 * Header file for utility class : RichRecPhotonKey
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date   2003-06-20
 */
//-----------------------------------------------------------------------------

#ifndef RichRecEvent_RichRecPhotonKey_H
#define RichRecEvent_RichRecPhotonKey_H 1

// Gaudi
#include "GaudiKernel/StreamBuffer.h"

// boost
#include "boost/cstdint.hpp"

namespace Rich
{
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

      /// 32 bit Type
      typedef boost::uint32_t Int32_t;
      /// 64 bit Type
      typedef boost::uint64_t Int64_t;

    private:

      /// Packed pixel and segment keys into a photon key
      union Data
      {
        struct
        {
          Int32_t pixelKey   : 32; ///< The pixel key
          Int32_t segmentKey : 32; ///< The segment key
        } packed; ///< Representation as a packed struct
        Int64_t raw; ///< Raw data as a 64 bit int
      } m_data;

    public:

      /** Constructor from 64 bit int
       *  @param key The raw data key to use as the bit-packed data
       */
      PhotonKey( const Int64_t key = 0 ) { m_data.raw = key; }

      /** Constructor from segment and pixel numbers
       *
       *  @param pixelKey    The key for the associated RichRecPixel
       *  @param segmentKey  The key for the associated RichRecSegment
       */
      PhotonKey ( const Int32_t pixelKey,
                  const Int32_t segmentKey )
      {
        m_data.packed.pixelKey   = pixelKey;
        m_data.packed.segmentKey = segmentKey;
      }

      /// Destructor
      ~PhotonKey() {}

      /// Retrieve 32 bit integer key
      inline Int64_t key() const
      {
        return m_data.raw;
      }

      /// Int64_t operator
      inline operator Int64_t() const
      {
        return key();
      }

      /// Update 32 bit integer key
      inline void setKey(const Int64_t key)
      {
        m_data.raw = key;
      }

      /// Retrieve associated RichRecSegment key
      inline int segmentNumber() const
      {
        return m_data.packed.segmentKey;
      }

      /// Retrieve associated RichRecPixel key
      inline int pixelNumber() const
      {
        return m_data.packed.pixelKey;
      }

    public:

      /// Fill the ASCII output stream
      inline std::ostream& fillStream(std::ostream& s) const
      {
        return s << "{ " 
                 << " pixel = "   << pixelNumber()
                 << " segment = " << segmentNumber()
                 << " } ";
      }

      /// Implement textual ostream << method for Rich::Rec::TrackID
      friend inline std::ostream& operator << ( std::ostream& s,
                                                const PhotonKey& key )
      {
        return key.fillStream(s);
      }

    };

  }
}

#endif   // RichRecEvent_RichRecPhotonKey_H
