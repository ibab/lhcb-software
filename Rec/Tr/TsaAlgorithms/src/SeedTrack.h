// $Id: SeedTrack.h,v 1.2 2006-08-17 08:36:08 mneedham Exp $
#ifndef SEEDTRACK_H 
#define SEEDTRACK_H 1

#include "GaudiKernel/KeyedObject.h"
#include "GaudiKernel/boost_allocator.h"

#include <algorithm>

class SeedPnt;
class SeedHit;

#include <boost/array.hpp>

/** @class SeedTrack SeedTrack.h
 *  An object to hold the information needed per track for track seeding
 *
 *  @author Roger Forty
 *  @date   2006-1-27
 */

  // Class ID definition
  static const CLID& CLID_SeedTrack = 19111;

  // Namespace for locations in TDS
  namespace SeedTrackLocation {
    static const std::string& Default = "/Event/Rec/Tsa/SeedTracks";
  }


class SeedTrack: public KeyedObject<int> {

public:
  /// Constructor
  SeedTrack();

  /// Constructor with points
  SeedTrack(const std::vector<SeedPnt>& xPnts);
    
  ~SeedTrack( ) {} ///< Destructor


  /// Retrieve pointer to class definition structure
  virtual const CLID& clID() const;
  static const CLID& classID();


  void setSelect( const bool value );
  void setLive( const bool value) ; 
  void setNx( const int value ); 
  void setNy( const int value ); 
  void setX0( const double value ); 
  void setY0( const double value ); 
  void setSx( const double value ); 
  void setSy( const double value ); 
  void setTx( const double value ); 
  void setXChi2( const double value ) ; 
  void setYChi2( const double value ); 
  void setMatch( const int value ); 
  void setLik( const double value );
  void setXPnts( const std::vector<SeedPnt>& value );
  void setYPnts( const std::vector<SeedPnt>& value ); 
  void setLinks( const std::vector<SeedTrack*>& value); 

  bool select() const;
  bool live() const;
  int nx() const;
  int ny() const;
  double x0() const;
  double y0() const;
  double sx() const;
  double sy() const;
  double tx() const;
  double xChi2() const;
  double yChi2() const;
  double lik() const;

  std::vector<SeedPnt>& xPnts();
  std::vector<SeedPnt>& yPnts() ;
  const std::vector<SeedPnt>& xPnts() const ;
  const std::vector<SeedPnt>& yPnts() const;

  std::vector<SeedPnt> pnts() const;
  std::vector<SeedPnt> usedPnts() const;

  std::vector<SeedTrack*>& links() ;
  std::vector<SeedTrack*>& yLinks();
  
  double x(const double z, const double z0) const;  
  double xSlope(const double z, const double z0) const ;
  double y(const double z, const double z0) const;
    
  double xErr( const int i ) const ;
  double yErr( const int i ) const ; 

  void setXerr( const int i, const double value ); 
  void setYerr( const int i, const double value );

#ifndef _WIN32
    /// operator new
    static void* operator new ( size_t size )
    {
      return ( sizeof(SeedTrack) == size ?
               boost::singleton_pool<SeedTrack, sizeof(SeedTrack)>::malloc() :
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
      boost::singleton_pool<SeedTrack, sizeof(SeedTrack)>::is_from(p) ?
      boost::singleton_pool<SeedTrack, sizeof(SeedTrack)>::free(p) :
      ::operator delete(p);
    }

    /// placement operator delete
    /// not sure if really needed, but it does not harm
    static void operator delete ( void* p, void* pObj )
    {
      ::operator delete (p, pObj);
    }
#endif

    void addToYPnts(const SeedPnt& pnt);

private:

  class pntByIncreasingZ  {
  public:
    bool operator() (const SeedPnt& first, const SeedPnt& second ) const;
  };

  bool m_select;  
  bool m_live;  
  int m_nx;  
  int m_ny;  
  double m_x0;  
  double m_y0;  
  double m_sx;   
  double m_sy;   
  double m_tx;   
  double m_xChi2; 
  double m_yChi2; 
  int m_match;
  double m_lik;  
  std::vector<SeedPnt> m_xPnts;
  std::vector<SeedPnt> m_yPnts;
  std::vector<SeedTrack*> m_links; 
  std::vector<SeedTrack*> m_yLinks; 

  typedef boost::array<double,6> CovX;
  typedef boost::array<double,3> CovY;
  CovX m_xErr;   // covariance matrix elements (11, 12, 13, 22, 23, 33) of X
  CovY m_yErr;  // covariance matrix elements (11, 12, 22) of Y fit
  std::vector<SeedHit> m_hits;      // X hits on the track

};


#include "SeedPnt.h"
#include "SeedHit.h"

/// Constructor
inline SeedTrack::SeedTrack( ) : 
     KeyedObject<int>(),
      m_select ( 0 ),
      m_live ( 1 ),
      m_nx ( 0 ),
      m_ny ( 0 ),
      m_x0 ( 0. ),
      m_y0 ( 0. ),
      m_sx ( 0. ),
      m_sy ( 0. ),
      m_tx ( 0. ),
      m_xChi2 ( 0. ),
      m_yChi2 ( 0. ),
     m_lik ( 0. )
{

 for (CovX::iterator iterX = m_xErr.begin(); iterX != m_xErr.end(); ++iterX){
   *iterX = 0;
 } // iterX

 for (CovY::iterator iterY = m_yErr.begin(); iterY != m_yErr.end(); ++iterY){
   *iterY = 0;
 } // iter

 m_yPnts.reserve(24);
 m_links.reserve(10);
 m_yLinks.reserve(10);

}


/// Constructor
inline SeedTrack::SeedTrack(const std::vector<SeedPnt>& xPnts) : 
     KeyedObject<int>(),
      m_select ( 0 ),
      m_live ( 1 ),
      m_nx ( 0 ),
      m_ny ( 0 ),
      m_x0 ( 0. ),
      m_y0 ( 0. ),
      m_sx ( 0. ),
      m_sy ( 0. ),
      m_tx ( 0. ),
      m_xChi2 ( 0. ),
      m_yChi2 ( 0. ),
      m_lik ( 0. ),
      m_xPnts(xPnts)
{

 for (CovX::iterator iterX = m_xErr.begin(); iterX != m_xErr.end(); ++iterX){
   *iterX = 0;
 } // iterX

 for (CovY::iterator iterY = m_yErr.begin(); iterY != m_yErr.end(); ++iterY){
   *iterY = 0;
 } // iter

 
 m_yPnts.reserve(24);
 m_links.reserve(10);
 m_yLinks.reserve(10);

}

inline const CLID& SeedTrack::clID() const
{
  return SeedTrack::classID();
}

inline const CLID& SeedTrack::classID()
{
  return CLID_SeedTrack;
}


inline void SeedTrack::setSelect( const bool value ) { 
  m_select = value; 
}

inline void SeedTrack::setLive( const bool value ) {
 m_live = value; 
}
  
inline void SeedTrack::setNx( int value ) { 
 m_nx = value; 
}
 
inline void SeedTrack::setNy( const int value ) 
{
  m_ny = value; 
}

inline void SeedTrack::setX0( const double value ) { 
  m_x0 = value; 
}

inline void SeedTrack::setY0( const double value ) { 
  m_y0 = value; 
}

inline void SeedTrack::setSx( const double value ) { 
  m_sx = value; 
}
  
inline void SeedTrack::setSy( const double value ) { 
  m_sy = value; 
}
  
inline void SeedTrack::setTx( const double value ) { 
  m_tx = value; 
}

inline void SeedTrack::setXChi2( const double value ) { 
  m_xChi2 = value; 
}
  
inline void SeedTrack::setYChi2( const double value ) {
 m_yChi2 = value; 
}
  
inline void SeedTrack::setLik( const double value ) { 
  m_lik = value; 
}

inline void SeedTrack::setXPnts( const std::vector<SeedPnt>& value ) { 
  m_xPnts = value; 
}

inline void SeedTrack::setYPnts( const std::vector<SeedPnt>& value ) {   
  m_yPnts = value; 
}

inline void SeedTrack::setLinks( const std::vector<SeedTrack*>& value ) { 
  m_links = value; 
}

inline bool SeedTrack::select() const { 
 return m_select; 
}
  
inline bool SeedTrack::live() const { 
  return m_live; 
}

inline int SeedTrack::nx() const { 
  return m_nx; 
}

inline int SeedTrack::ny() const { 
  return m_ny; 
}

inline double SeedTrack::x0() const { 
  return m_x0; 
}

inline double SeedTrack::y0() const { 
  return m_y0; 
}

inline double SeedTrack::sx() const { 
  return m_sx; 
}

inline double SeedTrack::sy() const { 
  return m_sy; 
}

inline double SeedTrack::tx() const { 
  return m_tx; 
}

inline double SeedTrack::xChi2() const { 
 return m_xChi2; 
}
  
inline double SeedTrack::yChi2() const { 
 return m_yChi2; 
}

inline double SeedTrack::lik() const { 
  return m_lik; 
}
  
inline std::vector<SeedPnt>& SeedTrack::xPnts() { 
  return m_xPnts; 
}

inline std::vector<SeedPnt>& SeedTrack::yPnts() {
  return m_yPnts;
}

inline const std::vector<SeedPnt>& SeedTrack::xPnts() const{ 
  return m_xPnts; 
}

inline const std::vector<SeedPnt>& SeedTrack::yPnts() const{
  return m_yPnts;
}

inline std::vector<SeedTrack*>& SeedTrack::links() { 
  return m_links; 
}

inline std::vector<SeedTrack*>& SeedTrack::yLinks() { 
  return m_yLinks; 
}
    
inline double SeedTrack::y(const double z, const double z0) const{
  return y0() + sy()*(z-z0);
}

inline double SeedTrack::x(const double z, const double z0) const{
  return x0() + sx()*(z-z0) + tx()*(z-z0)*(z-z0);
}

inline double SeedTrack::xSlope(const double z, const double z0) const{
  return sx() + 2.0*tx()*(z-z0);
}

inline std::vector<SeedPnt> SeedTrack::pnts() const{
  std::vector<SeedPnt> tmp;
  tmp.reserve(m_xPnts.size() + m_yPnts.size());
  tmp.insert(tmp.begin(), m_xPnts.begin(), m_xPnts.end());
  tmp.insert(tmp.begin(), m_yPnts.begin(), m_yPnts.end());
  std::sort(tmp.begin(), tmp.end(), pntByIncreasingZ());
  return tmp;
}

inline std::vector<SeedPnt> SeedTrack::usedPnts() const{
  std::vector<SeedPnt> allPnts = pnts();
  std::vector<SeedPnt> tmpPnts;
  tmpPnts.reserve(allPnts.size());
  for (std::vector<SeedPnt>::iterator iter = allPnts.begin(); iter != allPnts.end(); ++iter){
    if ((*iter).skip() == false) tmpPnts.push_back(*iter);
  } //iter

  return tmpPnts;
}

inline double SeedTrack::xErr( const int i ) const { 
  return (i < CovX::static_size ? m_xErr[i]  : 9999.0); 
}

inline double SeedTrack::yErr( const int i ) const { 
  return ( i < CovY::static_size ? m_yErr[i] : 9999.0); 
}

inline void SeedTrack::setXerr( const int i, const double value ) { 
  if (i < CovX::static_size) m_xErr[i] = value; 
}

inline void SeedTrack::setYerr( const int i, const double value ) { 
  if (i < CovX::static_size) m_yErr[i] = value; 
}

inline void SeedTrack::addToYPnts(const SeedPnt& pnt){
  m_yPnts.push_back(pnt); 
}

inline bool SeedTrack::pntByIncreasingZ::operator() (const SeedPnt& first,  const SeedPnt& second ) const {
//-------------------------------------------------------------------------
//  Define the sort sequence for hits
//-------------------------------------------------------------------------
  return (first.z() < second.z());
}


//Defintion of keyed container for Tsa::Track
typedef KeyedContainer<SeedTrack, Containers::HashMap> SeedTracks;


#endif // SEEDTRACK_H
