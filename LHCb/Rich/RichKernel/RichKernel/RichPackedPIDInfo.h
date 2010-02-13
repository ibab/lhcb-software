
//-----------------------------------------------------------------------------
/** @file RichPackedPIDInfo.h
 *
 *  Header file for RICH utility class : Rich::PackedPIDInfo
 *
 *  CVS Log :-
 *  $Id: RichPackedPIDInfo.h,v 1.2 2010-02-13 13:20:30 jonrob Exp $
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

  private:

    /// Packed PID information
    union Data
    {
      struct Packed
      {
        int rich : 8;   ///< The RICH type
        int rad  : 8;   ///< The radiator type
        int side : 8;   ///< The RICH detector 'side'
        int pid  : 8;   ///< The Mass Hypothesis
      } packed;         ///< Representation as a packed struct
      unsigned int raw; ///< Representation as an unsigned int
    } data; 

  public:

    /// Set the detector type
    inline void setDet( const Rich::DetectorType det )
    {
      data.packed.rich = static_cast<int>(det);
    }

    /// Set the radiator type
    inline void setRad( const Rich::RadiatorType rad )
    {
      data.packed.rad = static_cast<int>(rad);
    }

    /// Set the detector side type
    inline void setSide( const Rich::Side side )
    {
      data.packed.side = static_cast<int>(side);
    }

    /// Set the particle mass hypothesis
    inline void setPid( const Rich::ParticleIDType pid )
    {
      data.packed.pid = static_cast<int>(pid);
    }

  public:

    /// Get the detector type
    inline Rich::DetectorType det( ) const
    {
      return static_cast<Rich::DetectorType>(data.packed.rich);
    }

    /// Get the radiator type
    inline Rich::RadiatorType rad( ) const
    {
      return static_cast<Rich::RadiatorType>(data.packed.rad);
    }

    /// Get the side type
    inline Rich::Side side( ) const
    {
      return static_cast<Rich::Side>(data.packed.side);
    }

    /// Get the particle mass hypothesis
    inline Rich::ParticleIDType pid( ) const
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
    inline unsigned int raw() const { return data.raw; }

    /// implicit conversion to an int
    inline operator unsigned int() const
    {
      return this->raw();
    }

  };

}
#endif // RICHKERNEL_RICHPACKEDPIDINFO_H
