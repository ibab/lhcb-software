// $Id: SeedHit.h,v 1.7 2009-05-29 13:38:52 cattanem Exp $
#ifndef SEEDHIT_H
#define SEEDHIT_H 1

#include "TsaKernel/TsaSeedingHit.h"

#include "Kernel/OTChannelID.h"

#include "GaudiKernel/KeyedObject.h"

#include <vector>
#include <algorithm>
#include <iostream>

#include "LoKi/Range.h"

// Class ID definition
static const CLID CLID_TsaSeedHit = 19112;

namespace Tf
{
  namespace Tsa
  {

    // Namespace for locations in TDS
    namespace SeedHitLocation {
      static const std::string& Default = "/Event/Rec/Tsa/SeedHits";
    }

    /** @class SeedHit SeedHit.h
     *  An object to hold the information needed per hit for track seeding
     *
     *  @author Roger Forty
     *  @date   2005-11-28
     *
     *  @todo CRJ : In the longer term, this class should go and Tsa should use Tf::Tsa::SeedingHit directly
     */

    class SeedHit: public KeyedObject<int>
    {

    private:

      Tf::Tsa::SeedingHit * m_hit; ///< Pointer to the Tf data object

      // Stuff from original Tsa SeedHit
      std::vector<SeedHit*> m_skip;
      bool m_use1;
      bool m_use2;

      double m_x;               ///< x (or stereo ???) coordinate of wire
      double m_y;               ///< y coordinate of wire
      double m_z;               ///< z coordinate of wire

    private:

      /// No default constructor
      SeedHit() : KeyedObject<int>(), m_hit(NULL) { }

    public:

      explicit SeedHit( Tf::Tsa::SeedingHit * hit )
        : KeyedObject<int>(),
          m_hit(hit),
          m_use1(false),
          m_use2(false),
          m_x(hit->hit()->xMid()),
          m_y(0),
          m_z(hit->hit()->zMid())
      {
        m_skip.reserve(10);
      }

      explicit SeedHit( const SeedHit & hit )
        : KeyedObject<int>(),
          m_hit(hit.tfHit()),
          m_use1(hit.use1()),
          m_use2(hit.use2()),
          m_x(hit.x()),
          m_y(hit.y()),
          m_z(hit.z())
      { m_skip.reserve(10); }

      Tf::Tsa::SeedingHit * tfHit() const { return m_hit; }

      inline SeedHit* clone()
      {
        return new SeedHit(*this);
      }

    public:

      /// Retrieve pointer to class definition structure
      virtual const CLID& clID() const
      {
        return SeedHit::classID();
      }

      static const CLID& classID()
      {
        return CLID_TsaSeedHit;
      }

#ifndef _WIN32
      /// operator new
      static void* operator new ( size_t size )
      {
        return ( sizeof(SeedHit) == size ?
                 boost::singleton_pool<SeedHit, sizeof(SeedHit)>::malloc() :
                 ::operator new(size) );
      }

      /// placement operator new
      /// it is needed by libstdc++ 3.2.3 (e.g. in std::vector)
      /// it is not needed in libstdc++ >= 3.4
      static void* operator new ( size_t size, void* pObj )
      {
        return ::operator new (size,pObj);
      }

      /// operator delete
      static void operator delete ( void* p )
      {
        boost::singleton_pool<SeedHit, sizeof(SeedHit)>::is_from(p) ?
          boost::singleton_pool<SeedHit, sizeof(SeedHit)>::free(p) :
          ::operator delete(p);
      }

      /// placement operator delete
      /// not sure if really needed, but it does not harm
      static void operator delete ( void* p, void* pObj )
      {
        ::operator delete (p, pObj);
      }
#endif

    public:

      inline LHCb::LHCbID lhcbID() const { return m_hit->hit()->lhcbID(); }

      inline LHCb::STChannelID ITChan() const { return m_hit->stChannelID(); }

      inline LHCb::OTChannelID OTChan() const { return m_hit->otChannelID(); }

      inline bool sameStraw(const LHCb::OTChannelID chan) const
      {
        return chan == LHCb::OTChannelID( m_hit->otChannelID().geometry() );
      }

      inline bool sameChan(const LHCb::STChannelID chan) const
      {
        return chan == m_hit->stChannelID();
      }

      inline bool sameLayer(const LHCb::OTChannelID chan) const
      {
        return( chan.layer()   == m_hit->otChannelID().layer() &&
                chan.station() == m_hit->otChannelID().station() );
      }

      inline bool sameID(const LHCb::LHCbID id) const
      {
        return id == m_hit->hit()->lhcbID();
      }

    public: // getters

      inline double r() const { return tfHit()->driftRadius(); }
      inline double x() const { return m_x; }
      inline double y() const { return m_y; }
      inline double z() const { return m_z; }

      inline double xMax() const { return tfHit()->hit()->xMax(); }
      inline double xMin() const { return tfHit()->hit()->xMin(); }
      inline double yMax() const { return tfHit()->hit()->yMax(); }
      inline double yMin() const { return tfHit()->hit()->yMin(); }

      // CRJ : Not yet sure about this one
      inline double yMid() const { return tfHit()->hit()->yMid(); }

      inline bool OT() const { return tfHit()->isOT(); }

      std::vector<SeedHit*> & skip() { return m_skip; }

      inline bool use1() const { return m_use1; }
      inline bool use2() const { return m_use2; }

      inline bool onTrack() const { return tfHit()->onTrack(); }

      inline double length() const { return tfHit()->hit()->length(); }

    public: // setters

      inline void setX(double x) { m_x = x; }
      inline void setY(double y) { m_y = y; }
      inline void setZ(double z) { m_z = z; }

      void setSkip( const std::vector<SeedHit*>& value ) { m_skip = value; }

      void addToSkip(SeedHit* aHit) { m_skip.push_back(aHit); }

      inline void setUse1( const bool value ) { m_use1 = value; }
      inline void setUse2( const bool value ) { m_use2 = value; }
      inline void setUse ( const bool value ) { setUse1(value); setUse2(value); }

      inline void setOnTrack(bool used) const { tfHit()->setOnTrack(used); }

    public:

      /// Print this SeedHit in a human readable way
      std::ostream& fillStream(std::ostream& s) const;

      /// Implement ostream << method
      friend inline std::ostream& operator << ( std::ostream& s,
                                                const SeedHit& hit )
      {
        return hit.fillStream(s);
      }

    };

    typedef LoKi::Range_<std::vector<SeedHit*> > SeedHitRange;

    // Definition of keyed container for Tsa::Track
    //typedef std::vector<SeedHit*> SeedHits;
    typedef KeyedContainer<SeedHit, Containers::HashMap> SeedHits;

  }
}

#endif // SEEDHIT_H
