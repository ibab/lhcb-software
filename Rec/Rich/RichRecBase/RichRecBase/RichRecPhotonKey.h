
//-----------------------------------------------------------------------------
/** @file RichRecPhotonKey.h
 *
 * Header file for utility class : RichRecPhotonKey
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date   2003-06-20
 */
//-----------------------------------------------------------------------------

#ifndef RichRecBase_RichRecPhotonKey_H
#define RichRecBase_RichRecPhotonKey_H 1

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

      /// Type for packed fields
      typedef boost::uint16_t PackedType;

      /// Type for overall word
      typedef boost::uint32_t KeyType;

    private:

      /// Packed pixel and segment keys into a photon key
      union Data
      {
        struct
        {
          PackedType pixelKey   : 16; ///< The pixel key
          PackedType segmentKey : 16; ///< The segment key
        } packed; ///< Representation as a packed struct
        KeyType raw; ///< Raw data
      } m_data;

    public:

      /** Constructor from a full int
       *  @param key The raw data key to use as the bit-packed data
       */
      PhotonKey( const KeyType key = 0 ) { m_data.raw = key; }

      /** Constructor from segment and pixel numbers
       *
       *  @param pixelKey    The key for the associated RichRecPixel
       *  @param segmentKey  The key for the associated RichRecSegment
       */
      PhotonKey ( const PackedType pixelKey,
                  const PackedType segmentKey )
      {
        m_data.packed.pixelKey   = pixelKey;
        m_data.packed.segmentKey = segmentKey;
      }

      /// Copy Constructor
      //PhotonKey( const PhotonKey & k ) { m_data.raw = k.key(); }

    public:

      /// Retrieve the full key
      inline KeyType key() const
      {
        return m_data.raw;
      }

      /// KeyType operator
      inline operator KeyType() const
      {
        return key();
      }

      /// Update the full key
      inline void setKey( const KeyType key )
      {
        m_data.raw = key;
      }

    public:

      /// Retrieve associated RichRecSegment key
      inline PackedType segmentNumber() const
      {
        return m_data.packed.segmentKey;
      }

      /// Retrieve associated RichRecPixel key
      inline PackedType pixelNumber() const
      {
        return m_data.packed.pixelKey;
      }

    public:

      /// Operator ==
      inline bool operator== ( const PhotonKey & k ) const
      { 
        return k.key() == this->key(); 
      }      

      /// Operator !=
      inline bool operator!= ( const PhotonKey & k ) const
      { 
        return k.key() != this->key(); 
      }

      /// Operator <
      inline bool operator<  ( const PhotonKey & k ) const
      { 
        return k.key() < this->key(); 
      }

      /// Operator >
      inline bool operator>  ( const PhotonKey & k ) const
      { 
        return k.key() > this->key(); 
      }

    public:

      /// Fill the ASCII output stream
      inline std::ostream& fillStream(std::ostream& s) const
      {
        return s << "{" 
                 << " pixel = "   << pixelNumber()
                 << " segment = " << segmentNumber()
                 << " photon = "  << key()
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

#endif   // RichRecBase_RichRecPhotonKey_H
