// $Id: RichRadCorrLocalPositions.h,v 1.3 2007-08-09 15:51:12 jonrob Exp $
#ifndef RICHRECBASE_RICHRADCORRLOCALPOSITIONS_H
#define RICHRECBASE_RICHRADCORRLOCALPOSITIONS_H 1

// STD
#include <ostream>
#include <vector>

// Gaudi
#include "GaudiKernel/Point3DTypes.h"

// Kernel
#include "Kernel/RichRadiatorType.h"
#include "RichKernel/BoostMemPoolAlloc.h"
//#include "RichKernel/BoostArray.h"

namespace Rich
{
  namespace Rec
  {

    /** @class RadCorrLocalPositions RichRecBase/RichRadCorrLocalPositions.h
     *
     *  Simple class to store three local coordinate positions, the corrected positions for each radiator
     *
     *  @author Chris Jones   (Christopher.Rob.Jones@cern.ch)
     *  @date   2007-03-28
     */

    class RadCorrLocalPositions : public Rich::BoostMemPoolAlloc<Rich::Rec::RadCorrLocalPositions>
    {

    public:

      /// Standard constructor
      RadCorrLocalPositions( ) : m_pos(Rich::NRadiatorTypes) { }

      /// Constructor from a default position
      RadCorrLocalPositions( const Gaudi::XYZPoint & point ) : m_pos(Rich::NRadiatorTypes,point) { }

      /// Destructor
      ~RadCorrLocalPositions( ) { }

      /// Get the position for the given radiator (const)
      inline const Gaudi::XYZPoint & position( const Rich::RadiatorType rad ) const
      {
        return m_pos[rad];
      }

      /// Get the position for the given radiator (non const)
      inline Gaudi::XYZPoint & position( const Rich::RadiatorType rad )
      {
        return m_pos[rad];
      }

      /// Set the position for the given radiator
      inline void setPosition( const Rich::RadiatorType rad,
                               const Gaudi::XYZPoint & point )
      {
        m_pos[rad] = point;
      }

      /// overload printout to ostream operator <<
      friend inline std::ostream& operator << ( std::ostream& s,
                                                const Rich::Rec::RadCorrLocalPositions & pos )
      {
        return s << "[ " 
                 << pos.position(Rich::Aerogel)  << ", " 
                 << pos.position(Rich::Rich1Gas) << ", " 
                 << pos.position(Rich::Rich2Gas)
                 << " ]";
      }

    private:

      /// The corrected local positions
      //boost::array<Gaudi::XYZPoint,Rich::NRadiatorTypes> m_pos;
      std::vector<Gaudi::XYZPoint> m_pos;

    };

  }
}

#endif // RICHRECBASE_RICHRADCORRLOCALPOSITIONS_H
