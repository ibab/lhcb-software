// $Id: SeedPnt.h,v 1.3 2006-09-05 15:48:51 mneedham Exp $
#ifndef SEEDPNT_H 
#define SEEDPNT_H 1

/** @class SeedPnt SeedPnt.h
 *  An object to hold the information needed per point for track seeding
 *
 *  @author Roger Forty
 *  @date   2006-5-4
 */

class SeedHit;

namespace LHCb{
  class OTChannelID;
  class STChannelID;
}

class SeedPnt {
public: 

  /// Constructor
  SeedPnt();

  SeedPnt(SeedHit* hit,  const int sign = 0);

  ~SeedPnt( ) {} ///< Destructor

 
  void setSign( const int value );
  void flipSign();
  void setHit( SeedHit* value );
  void setCoord( const double value ) ; 
  void setZ( const double value ) ;
  void setSkip( const bool value ); 

  int sign() const ; 
  SeedHit* hit() const;
  double coord() const ; 
  double z() const; 
  bool skip() const; 

  bool sameStraw(const LHCb::OTChannelID chan) const;

  bool sameChan(const LHCb::STChannelID chan) const;

private:

  int m_sign;   
  double m_coord;
  double m_z;
  bool m_skip;
  SeedHit* m_hit;   

};


#include "Kernel/OTChannelID.h"
#include "Kernel/STChannelID.h"
#include "SeedHit.h"

// Default Constructor
inline SeedPnt::SeedPnt( ) :
 m_sign ( 0 ),
 m_coord ( 0. ),
 m_z ( 0. ),
 m_skip ( 0 ),
 m_hit(0) {}

inline SeedPnt::SeedPnt(SeedHit* hit,  const int sign) :
 m_sign ( sign),
 m_coord ( 0. ),
 m_z ( 0. ),
 m_skip ( 0 ),
 m_hit(hit){}

inline void SeedPnt::setSign( const int value ) { 
  m_sign = value; 
}

inline void SeedPnt::flipSign(){
  m_sign *= -1;
}

inline void SeedPnt::setHit( SeedHit* value ) { 
 m_hit = value; 
}

inline void SeedPnt::setCoord( const double value ) { 
  m_coord = value; 
}

inline void SeedPnt::setZ( const double value ) { 
  m_z = value; 
}

inline void SeedPnt::setSkip( const bool value ) {  
  m_skip = value; 
}

inline int SeedPnt::sign() const { 
  return m_sign; 
}

inline SeedHit* SeedPnt::hit() const{ 
  return m_hit; 
}

inline double SeedPnt::coord() const { 
  return m_coord; 
}

inline double SeedPnt::z() const { 
  return m_z;
}

inline bool SeedPnt::skip() const { 
  return m_skip; 
}
   
inline bool SeedPnt::sameStraw(const LHCb::OTChannelID chan) const{
  return hit()->sameStraw(chan);
}

inline bool SeedPnt::sameChan(const LHCb::STChannelID chan) const{
  return hit()->sameChan(chan);
}


#endif // SEEDPNT_H
