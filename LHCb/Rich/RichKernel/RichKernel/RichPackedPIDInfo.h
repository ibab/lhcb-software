
//-----------------------------------------------------------------------------
/** @file RichPackedPIDInfo.h
 *
 *  Header file for RICH utility class : Rich::PackedPIDInfo
 *
 *  CVS Log :-
 *  $Id: RichPackedPIDInfo.h,v 1.3 2010-02-13 15:10:47 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/02/2010
 */
//-----------------------------------------------------------------------------

#ifndef RICHKERNEL_RICHPACKEDPIDINFO_H
#define RICHKERNEL_RICHPACKEDPIDINFO_H 1

// Kernel
#include "Kernel/RichRadiatorType.h"
#include "Kernel/RichDetectorType.h"
#include "Kernel/RichParticleIDType.h"
#include "Kernel/RichSide.h"

// boost
#include "boost/cstdint.hpp"

#ifdef __INTEL_COMPILER         // Disable ICC remark
  #pragma warning(disable:2259) // Non-pointer conversion may lose significant bits
  #pragma warning(push)
#endif

namespace Rich
{

  /** @class PackedPIDInfo RichKernel/RichPackedPIDInfo.h
   *
   *  Class that packs the various RICH detector and PID enums into a single
   *  int-like class, that can be used for things like fast map indexing.
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   12/02/2010
   */
  class PackedPIDInfo
  {

  public:

    /// Type for 8 bit packed word
    typedef boost::int8_t   Pack8_t;

    /// Type for 32 bit raw word
    typedef boost::uint32_t Pack32_t;

  private:

    /// Packed PID information
    union Data
    {
      struct Packed
      {
        Pack8_t rich : 8;   ///< The RICH type
        Pack8_t rad  : 8;   ///< The radiator type
        Pack8_t side : 8;   ///< The RICH detector 'side'
        Pack8_t pid  : 8;   ///< The Mass Hypothesis
      } packed;         ///< Representation as a packed struct
      Pack32_t raw; ///< Representation as an unsigned int
    } data; 

  public:

    /// Set the detector type
    inline void setDet( const Rich::DetectorType det )
    {
      data.packed.rich = static_cast<Pack8_t>(det);
    }

    /// Set the radiator type
    inline void setRad( const Rich::RadiatorType rad )
    {
      data.packed.rad = static_cast<Pack8_t>(rad);
    }

    /// Set the detector side type
    inline void setSide( const Rich::Side side )
    {
      data.packed.side = static_cast<Pack8_t>(side);
    }

    /// Set the particle mass hypothesis
    inline void setPid( const Rich::ParticleIDType pid )
    {
      data.packed.pid = static_cast<Pack8_t>(pid);
    }

  public:

    /// Get the detector type
    inline Rich::DetectorType det() const
    {
      return static_cast<Rich::DetectorType>(data.packed.rich);
    }

    /// Get the radiator type
    inline Rich::RadiatorType rad() const
    {
      return static_cast<Rich::RadiatorType>(data.packed.rad);
    }

    /// Get the side type
    inline Rich::Side side() const
    {
      return static_cast<Rich::Side>(data.packed.side);
    }

    /// Get the particle mass hypothesis
    inline Rich::ParticleIDType pid() const
    {
      return static_cast<Rich::ParticleIDType>(data.packed.pid);
    }

  public:

    /// Standard constructor
    PackedPIDInfo( ) { data.raw = 0; }

    /** Constructor from Rich data
     *  @param det  The RICH detector
     *  @param side The RICH detector side
     *  @param rad  The radiator media
     *  @param pid  The Mass Hypothesis
     */
    PackedPIDInfo( const Rich::DetectorType   det,
                   const Rich::Side          side = Rich::InvalidSide,
                   const Rich::RadiatorType   rad = Rich::InvalidRadiator,
                   const Rich::ParticleIDType pid = Rich::Unknown )
    {
      data.raw = 0;
      setDet  ( det  );
      setRad  ( rad  );
      setSide ( side );
      setPid  ( pid  );
    }

    /// Destructor
    ~PackedPIDInfo( ) { }

  public:

    /// Return as a raw unsigned int
    inline Pack32_t raw() const { return data.raw; }

    /// implicit conversion to an int
    inline operator Pack32_t() const { return raw(); }

  };

}
#ifdef __INTEL_COMPILER         // End disable ICC remark
  #pragma warning(pop)
#endif

#endif // RICHKERNEL_RICHPACKEDPIDINFO_H
