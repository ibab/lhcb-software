
//-----------------------------------------------------------------------------
/** @file RichPackedPIDInfo.h
 *
 *  Header file for RICH utility class : Rich::PackedPIDInfo
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
    using Pack8_t  = boost::int8_t;

    /// Type for 32 bit raw word
    using Pack32_t = boost::uint32_t;

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
      } packed;             ///< Representation as a packed struct
      Pack32_t raw{0};      ///< Representation as an unsigned int (init to 0)
    } data; 

  public:

    /// Set the detector type
    inline void setDet( const Rich::DetectorType det ) noexcept
    {
      data.packed.rich = (Pack8_t)(det);
    }

    /// Set the radiator type
    inline void setRad( const Rich::RadiatorType rad ) noexcept
    {
      data.packed.rad = (Pack8_t)(rad);
    }

    /// Set the detector side type
    inline void setSide( const Rich::Side side ) noexcept
    {
      data.packed.side = (Pack8_t)(side);
    }

    /// Set the particle mass hypothesis
    inline void setPid( const Rich::ParticleIDType pid ) noexcept
    {
      data.packed.pid = (Pack8_t)(pid);
    }

  public:

    /// Get the detector type
    inline Rich::DetectorType det() const noexcept
    {
      return (Rich::DetectorType)(data.packed.rich);
    }

    /// Get the radiator type
    inline Rich::RadiatorType rad() const noexcept
    {
      return (Rich::RadiatorType)(data.packed.rad);
    }

    /// Get the side type
    inline Rich::Side side() const noexcept
    {
      return (Rich::Side)(data.packed.side);
    }

    /// Get the particle mass hypothesis
    inline Rich::ParticleIDType pid() const noexcept
    {
      return (Rich::ParticleIDType)(data.packed.pid);
    }

  public:

    /// Standard constructor
    PackedPIDInfo( ) = default;

    /// Default destructor
    ~PackedPIDInfo() = default;

    /// Default Copy Constructor
    PackedPIDInfo( const PackedPIDInfo& ) = default;

    /// Default Copy operator
    PackedPIDInfo& operator=( const PackedPIDInfo& ) = default;

    /// Default Move Constructor
    PackedPIDInfo( PackedPIDInfo&& ) = default;

    /// Default Move operator
    PackedPIDInfo& operator=( PackedPIDInfo&& ) = default;

    /** Constructor from Rich data
     *  @param det  The RICH detector
     *  @param side The RICH detector side
     *  @param rad  The radiator media
     *  @param pid  The Mass Hypothesis
     */
    PackedPIDInfo( const Rich::DetectorType   det,
                   const Rich::Side          side = Rich::InvalidSide,
                   const Rich::RadiatorType   rad = Rich::InvalidRadiator,
                   const Rich::ParticleIDType pid = Rich::Unknown ) noexcept
    {
      setDet  ( det  );
      setRad  ( rad  );
      setSide ( side );
      setPid  ( pid  );
    }

  public:

    /// Return as a raw unsigned int
    inline Pack32_t raw() const noexcept { return data.raw; }

    /// implicit conversion to an int
    inline operator Pack32_t() const noexcept { return raw(); }

  };

}

#endif // RICHKERNEL_RICHPACKEDPIDINFO_H
