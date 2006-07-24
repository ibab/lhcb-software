// $Id: TsaSPCache.h,v 1.1.1.1 2006-07-24 14:56:45 mneedham Exp $
#ifndef _TsaSPCache_H
#define _TsaSPCache_H

/** @class TsaSPCache TsaSPCache.h
* Data cache for fast access to spacepoints in a station
* @author M. Needham
*/

#include "TsaKernel/SpacePoint.h"
#include "TsaKernel/Types.h"

class TsaSPCache{

public:

  /// Constructer
  TsaSPCache(const double deltaTx):
   m_deltaTx(deltaTx) {}

  /// destructer
  ~TsaSPCache(){}

  /// configure
  void configure(Tsa::SpacePoints::iterator first, 
                 Tsa::SpacePoints::iterator last);

  /// rest
  void reset();  

  /// total range  
  Tsa::SpacePointRange range() const;

  /// current range
  Tsa::SpacePointRange currentRange(const double& tX) const;

private:

  /// return begin
  Tsa::SpacePoints::iterator cBegin(const double& tX) const;

  /// return end
  Tsa::SpacePoints::iterator cEnd(const double& tX) const;


  Tsa::SpacePoints::iterator m_begin;
  Tsa::SpacePoints::iterator m_end;
  double m_deltaTx;  
  mutable Tsa::SpacePoints::iterator m_cachedFirst;
  mutable Tsa::SpacePoints::iterator m_cachedLast;

};

inline Tsa::SpacePointRange TsaSPCache::range() const{
  return Tsa::SpacePointRange(m_begin,m_end);
}

inline Tsa::SpacePointRange TsaSPCache::currentRange(const double& tX) const{
  return Tsa::SpacePointRange(cBegin(tX),cEnd(tX));
}

inline Tsa::SpacePoints::iterator TsaSPCache::cBegin(const double& tX) const{
  // update cached begin
  double tXStart = tX-m_deltaTx;
  if ((m_cachedFirst == m_end)||(*m_cachedFirst)->tX() > tXStart) m_cachedFirst = m_begin; 
  while ((m_cachedFirst != m_end) 
     &&( (*m_cachedFirst)->tX() < tXStart)){
     ++m_cachedFirst;
  }

  return m_cachedFirst;
}

inline Tsa::SpacePoints::iterator TsaSPCache::cEnd(const double& tX) const{
  // return cached end
  double tXStop = tX+m_deltaTx;
  if ((m_cachedLast == m_end)||(*m_cachedLast)->tX() > tXStop) m_cachedLast = m_begin; 
  while (( m_cachedLast != m_end) 
     &&( (*m_cachedLast)->tX() < tXStop)){
     ++m_cachedLast;
  }
  return m_cachedLast;
}

#endif // _TsaSPCache_H
