// $Id: SeedHit.h,v 1.4 2006-09-05 15:48:51 mneedham Exp $
#ifndef SEEDHIT_H 
#define SEEDHIT_H 1

/** @class SeedHit SeedHit.h
 *  An object to hold the information needed per hit for track seeding
 *
 *  @author Roger Forty
 *  @date   2005-11-28
 */

#include "Kernel/OTChannelID.h"
#include "TsaKernel/OTCluster.h"
#include "TsaKernel/STCluster.h"
#include "GaudiKernel/boost_allocator.h"
#include "GaudiKernel/KeyedObject.h"

#include <vector>
#include <algorithm>

class SeedTrack;

// Class ID definition
static const CLID& CLID_SeedHit = 19112;

// Namespace for locations in TDS
namespace SeedHitLocation {
  static const std::string& Default = "/Event/Rec/Tsa/SeedHits";
}


class SeedHit: public KeyedObject<int> {
public: 

  /// Constructor
  SeedHit( );

  SeedHit(Tsa::OTCluster* clus);

  SeedHit(Tsa::STCluster* clus);

  SeedHit* clone();

  ~SeedHit( ) {} ///< Destructor

  /// Retrieve pointer to class definition structure
  virtual const CLID& clID() const;
  static const CLID& classID();

  void setR( const double value );
  void setX( const double value );
  void setY( const double value );
  void setZ( const double value );
  void setUse1( const bool value );
  void setUse2( const bool value );
  void setITChan( const LHCb::STChannelID  value );
  void setOTChan( const LHCb::OTChannelID  value );
  void setOT( bool value );
  void setClus( Tsa::Cluster*  value );
  void setSkip( const std::vector<SeedHit*>& value ) ;
  void setSeeds( const std::vector<SeedTrack*>& value ) ;

  double r() const;
  double x() const;
  double y() const;
  double z() const ;
  bool use1() const;
  bool use2() const;
  LHCb::STChannelID ITChan() const;
  LHCb::OTChannelID OTChan() const;
  bool OT() const;
  Tsa::Cluster* clus() const;

  double xMin() const;
  double xMax() const;
 
  std::vector<SeedHit*>& skip();
  std::vector<SeedTrack*>& seeds() ;

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
 
    bool sameStraw(const LHCb::OTChannelID chan) const;
  
    bool sameChan(const LHCb::STChannelID chan) const;

    void addToSkip(SeedHit* aHit);

  

private:

  SeedHit(const SeedHit& other); 

  double m_r;     // radius of hit from wire
  double m_x;     // x (or stereo) coordinate of wire
  double m_y;     // y coordinate of wire
  double m_z;     // z coordinate of wire
  bool m_use1;
  bool m_use2;
  LHCb::STChannelID m_ITChan; // ChannelID of hit
  LHCb::OTChannelID m_OTChan; // ChannelID of hit
  bool m_OT;
  Tsa::Cluster* m_clus;

  double m_xMin;
  double m_xMax; 

  std::vector<SeedHit*> m_skip; 
  std::vector<SeedTrack*> m_seeds; 
  
};

#include <algorithm>


inline SeedHit::SeedHit( ):
 KeyedObject<int>(),
 m_r ( 0. ),
 m_x ( 0. ),
 m_y ( 0. ),
 m_z ( 0. ),
 m_use1 ( 0 ),
 m_use2 ( 0 ),
 m_ITChan ( 0 ),
 m_OTChan ( 0 ),
 m_OT ( 0 ),
 m_clus ( 0 ) {}


inline SeedHit::SeedHit(Tsa::STCluster* clus):
 KeyedObject<int>(), 
 m_r ( 0. ),
 m_x ( clus->xMid() ),
 m_y ( 0. ),
 m_z ( clus->zMid() ),
 m_use1 ( 0 ),
 m_use2 ( 0 ),
 m_ITChan (clus->channelID()  ),
 m_OTChan ( 0 ),
 m_OT ( 0 ),
 m_clus ( clus ) {

  double xMin = clus->beginPoint().x();
  double xMax = clus->endPoint().x();

  if (xMin > xMax) std::swap(xMin, xMax);

  m_xMin = xMin;
  m_xMax = xMax;

  m_skip.reserve(10);
  m_seeds.reserve(10);

}


inline SeedHit::SeedHit(Tsa::OTCluster* clus):
 KeyedObject<int>(), 
 m_r ( clus->driftRadius() ),
 m_x ( clus->xMid() ),
 m_y ( 0. ),
 m_z ( clus->zMid() ),
 m_use1 ( 0 ),
 m_use2 ( 0 ),
 m_ITChan ( 0 ),
 m_OTChan (clus->channel()  ),
 m_OT ( 1 ),
 m_clus ( clus ) {

  double xMin = clus->beginPoint().x();
  double xMax = clus->endPoint().x();

  if (xMin > xMax) std::swap(xMin, xMax);

  m_xMin = xMin;
  m_xMax = xMax;

  m_skip.reserve(10);
  m_seeds.reserve(10);

}

inline SeedHit::SeedHit( const SeedHit& other ):
 KeyedObject<int>(),
 m_r ( 0. ),
 m_x ( other.m_x ),
 m_y ( other.m_y ),
 m_z ( other.m_z ),
 m_use1 ( 0 ),
 m_use2 ( 0 ),
 m_ITChan ( other.m_ITChan ),
 m_OTChan ( other.m_OTChan ),
 m_OT ( other.m_OT ),
 m_clus ( other.m_clus ), 
 m_xMin( other.m_xMin),
 m_xMax( other.m_xMax) {
}

inline SeedHit* SeedHit::clone(){
 return new SeedHit(*this);
}

inline const CLID& SeedHit::clID() const
{
  return SeedHit::classID();
}

inline const CLID& SeedHit::classID()
{
  return CLID_SeedHit;
}

inline void SeedHit::setR( const double value ) { 
  m_r = value; 
}

inline void SeedHit::setX( const double value ) { 
  m_x = value; 
}

inline void SeedHit::setY( const double value ) { 
  m_y = value; 
}

inline void SeedHit::setZ( const double value ) { 
  m_z = value; 
}

inline void SeedHit::setUse1( const bool value ) { 
  m_use1 = value; 
}

inline void SeedHit::setUse2( const bool value ) { 
  m_use2 = value;
}

inline void SeedHit::setITChan( const LHCb::STChannelID  value ) { 
  m_ITChan = value; 
}

inline void SeedHit::setOTChan( const LHCb::OTChannelID  value ) { 
  m_OTChan = value; 
}

inline void SeedHit::setOT( const bool value ) { 
  m_OT = value; 
}

inline void SeedHit::setClus( Tsa::Cluster*  value ) { 
 m_clus = value; 
}


inline void SeedHit::setSkip( const std::vector<SeedHit*>& value ) { 
m_skip = value; 
}

inline void SeedHit::setSeeds( const std::vector<SeedTrack*>& value ) { 
  m_seeds = value; 
}

inline double SeedHit::r() const { 
  return m_r; 
}

inline double SeedHit::x() const { 
 return m_x; 
}

inline double SeedHit::y() const { 
 return m_y; 
}

inline double SeedHit::z() const { 
  return m_z; 
}

inline bool SeedHit::use1() const { 
  return m_use1;
}

inline bool SeedHit::use2() const { 
  return m_use2; 
}

inline LHCb::STChannelID SeedHit::ITChan() const { 
  return m_ITChan; 
}

inline LHCb::OTChannelID SeedHit::OTChan() const { 
  return m_OTChan; 
}

inline Tsa::Cluster* SeedHit::clus() const { 
 return m_clus; 
}

inline bool SeedHit::OT() const { 
  return m_OT;
}

inline double SeedHit::xMin() const{
  return m_xMin;
}

inline double SeedHit::xMax() const{
  return m_xMax;
}

inline std::vector<SeedHit*>& SeedHit::skip() { 
  return m_skip; 
}

inline std::vector<SeedTrack*>& SeedHit::seeds() { 
  return m_seeds; 
}

inline bool SeedHit::sameStraw(const LHCb::OTChannelID chan) const{
  return chan == LHCb::OTChannelID(m_OTChan.geometry());
}

inline bool SeedHit::sameChan(const LHCb::STChannelID chan) const{
  return chan == LHCb::STChannelID(m_ITChan);
}

inline void SeedHit::addToSkip(SeedHit* aHit){
  m_skip.push_back(aHit);
}

//Defintion of keyed container for Tsa::Track
typedef KeyedContainer<SeedHit, Containers::HashMap> SeedHits;

#include "LoKi/Range.h"
typedef LoKi::Range_<std::vector<SeedHit*> > SeedHitRange;

#endif // SEEDHIT_H
