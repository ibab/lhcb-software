#ifndef TSASEEDINGHIT_H 
#define TSASEEDINGHIT_H 1

#include "GaudiKernel/SystemOfUnits.h"

#include "TfKernel/HitExtension.h"
#include "TfKernel/LineHit.h"
#include "TfKernel/OTHit.h"
#include "TfKernel/STHit.h"
#include "Kernel/MemPoolAlloc.h"

namespace Tf
{
  namespace Tsa
  {

    /** @class SeedingHit
     *  Extended hit class for Tsa algorithms
     *  @author S. Hansmann-Menzemer, W. Houlsbergen, C. Jones, K. Rinnert
     *  @date   2007-06-01
     */
    class SeedingHit : public HitExtension<Tf::LineHit>,
                       public LHCb::MemPoolAlloc<Tf::Tsa::SeedingHit>
    {

    public:

      /// Default constructor
      SeedingHit()
        : HitExtension<Tf::LineHit>(NULL),
          m_isOT        ( false ),
          m_driftRadius ( 0     ),
          m_onTrack(false)
      { }

      /// Constructor from a Tf::OTHit
      SeedingHit( const Tf::OTHit & otHit ) 
        : HitExtension<Tf::LineHit>(&otHit),
          m_isOT        ( true ),
          // just use value direct from OT hit
          //m_driftRadius ( otHit.driftDistance() )
          // use the the '77cm' value
          m_driftRadius(otHit.untruncatedDriftDistance( otHit.yMid() > 0 ?
                                                        otHit.yMid() - 77*Gaudi::Units::cm :
                                                        otHit.yMid() + 77*Gaudi::Units::cm ) ),
          m_onTrack(false)
      { }

      /// Constructor from a Tf::STHit
      SeedingHit( const Tf::STHit & stHit )
        : HitExtension<Tf::LineHit>(&stHit),
          m_isOT        ( false ),
          m_driftRadius ( 0     ),
          m_onTrack(false)
      { }

      /// Destructor
      ~SeedingHit() { }

      /// Is this hit OT or not
      inline bool isOT() const { return m_isOT; }

      /// Is this hit ST or not
      inline bool isST() const { return !m_isOT; }

      /// Access the OTChannelID for this head (default ID if not OT)
      LHCb::OTChannelID otChannelID() const
      {
        return ( isOT() ? hit()->lhcbID().otID() : LHCb::OTChannelID() );
      }

      /// Access the STChannelID for this head (default ID if not ST)
      LHCb::STChannelID stChannelID() const
      {
        return ( isST() ? hit()->lhcbID().stID() : LHCb::STChannelID() );
      }

      /// The drift distance
      inline double driftRadius() const { return m_driftRadius; }

      /// set that the hit is on track
      inline void setOnTrack(bool used) const { m_onTrack = used; }

      // return if on track
      inline bool onTrack() const { return m_onTrack; }

    private:

      bool m_isOT;         ///< Flag to indicate if the hit is OT or not (thus ST)
      double m_driftRadius; ///< The drift distance
      mutable bool m_onTrack;

    };

  }
}

#endif // TSASEEDINGHIT_H
