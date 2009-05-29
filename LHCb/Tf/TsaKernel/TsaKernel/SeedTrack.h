// $Id: SeedTrack.h,v 1.4 2009-05-29 13:38:52 cattanem Exp $
#ifndef SEEDTRACK_H
#define SEEDTRACK_H 1

#include "GaudiKernel/KeyedObject.h"
#ifndef _WIN32
#include "GaudiKernel/boost_allocator.h"
#endif

#include <algorithm>

#include "TsaKernel/SeedPnt.h"
#include "TsaKernel/SeedHit.h"

namespace LHCb{
  class LHCbID;
}

#include <boost/array.hpp>

// Class ID definition
static const CLID CLID_TsaSeedTrack = 19111;

namespace Tf
{
  namespace Tsa
  {

    // Namespace for locations in TDS
    namespace SeedTrackLocation {
      static const std::string& Default = "/Event/Rec/Tsa/SeedTracks";
    }

    /** @class SeedTrack SeedTrack.h
     *  An object to hold the information needed per track for track seeding
     *
     *  @author Roger Forty
     *  @date   2006-1-27
     */

    class SeedTrack: public KeyedObject<int> {

    public:
      /// Constructor
      SeedTrack();

      /// Constructor with x points
      SeedTrack(const std::vector<SeedPnt>& xPnts, int sector, double dth = 0);

      /// Constructor with x and y points
      SeedTrack(const std::vector<SeedPnt>& xPnts,
                const std::vector<SeedPnt>& yPnts, int sector);

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
      void setSector( const int value );
      void setDth( const double value );
      void setLik( const double value );
      void setXPnts( const std::vector<SeedPnt>& value );
      void setYPnts( const std::vector<SeedPnt>& value );

      bool select() const;
      bool live() const;
      int nx() const;
      int ny() const;
      int nHit() const;

      double x0() const;
      double y0() const;
      double sx() const;
      double sy() const;
      double tx() const;
      double xChi2() const;
      double yChi2() const;
      int sector() const;
      double dth() const;
      double lik() const;

      std::vector<SeedPnt>& xPnts();
      std::vector<SeedPnt>& yPnts() ;
      const std::vector<SeedPnt>& xPnts() const ;
      const std::vector<SeedPnt>& yPnts() const;

      std::vector<SeedPnt> pnts() const;
      std::vector<SeedPnt> usedPnts() const;

      bool onTrack(const LHCb::LHCbID id) const;

      bool xHitOnTrack(const LHCb::LHCbID id) const;

      bool yHitOnTrack(const LHCb::LHCbID id) const;

      bool onTrack(const LHCb::LHCbID id, const int layer) const;

      double x(const double z, const double z0) const;
      double xSlope(const double z, const double z0) const ;
      double y(const double z, const double z0) const;

      double xErr( const int i ) const ;
      double yErr( const int i ) const ;

      void setXerr( const int i, const double value );
      void setYerr( const int i, const double value );

      void setXParams(const double& tx, const double& sx, const double& x0  );

      void setYParams(const double& sy, const double& y0  );


    public:

      /// Print this SeedHit in a human readable way
      std::ostream& fillStream(std::ostream& s) const;

      /// Implement ostream << method
      friend inline std::ostream& operator << ( std::ostream& s,
                                                const SeedTrack& track )
      {
        return track.fillStream(s);
      }

    public:

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

      void addToXPnts(const SeedPnt& pnt);

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
      int m_sector;

      double m_lik;
      double m_dth;
      std::vector<SeedPnt> m_xPnts;
      std::vector<SeedPnt> m_yPnts;

      typedef boost::array<double,6> CovX;
      typedef boost::array<double,3> CovY;
      CovX m_xErr;   // covariance matrix elements (11, 12, 13, 22, 23, 33) of X
      CovY m_yErr;  // covariance matrix elements (11, 12, 22) of Y fit

    };

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
      m_sector ( -1 ),
      m_lik ( 0. ),
      m_dth ( 0. )
    {

      for (CovX::iterator iterX = m_xErr.begin(); iterX != m_xErr.end(); ++iterX){
        *iterX = 0;
      } // iterX

      for (CovY::iterator iterY = m_yErr.begin(); iterY != m_yErr.end(); ++iterY){
        *iterY = 0;
      } // iter

      m_yPnts.reserve(24);

    }

    /// Constructor
    inline SeedTrack::SeedTrack(const std::vector<SeedPnt>& xPnts, int sector, double dth) :
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
      m_sector ( sector),
      m_lik ( 0. ),
      m_dth (dth ),
      m_xPnts(xPnts)
    {

      for (CovX::iterator iterX = m_xErr.begin(); iterX != m_xErr.end(); ++iterX){
        *iterX = 0;
      } // iterX

      for (CovY::iterator iterY = m_yErr.begin(); iterY != m_yErr.end(); ++iterY){
        *iterY = 0;
      } // iter


      m_yPnts.reserve(24);

    }


    /// Constructor
    inline SeedTrack::SeedTrack(const std::vector<SeedPnt>& xPnts,
                                const std::vector<SeedPnt>& yPnts, int sector) :
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
      m_sector ( sector),
      m_lik ( 0. ),
      m_dth ( 0. ),
      m_xPnts(xPnts),
      m_yPnts(yPnts)
    {

      for (CovX::iterator iterX = m_xErr.begin(); iterX != m_xErr.end(); ++iterX){
        *iterX = 0;
      } // iterX

      for (CovY::iterator iterY = m_yErr.begin(); iterY != m_yErr.end(); ++iterY){
        *iterY = 0;
      } // iter

    }


    inline const CLID& SeedTrack::clID() const
    {
      return SeedTrack::classID();
    }

    inline const CLID& SeedTrack::classID()
    {
      return CLID_TsaSeedTrack;
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

    inline void SeedTrack::setNy( const int value ) {
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

    inline void SeedTrack::setXParams(const double& tx , const double& sx , const double& x0) {

      m_tx = tx;
      m_sx = sx;
      m_x0 = x0;
    }


    inline void SeedTrack::setYParams(const double& sy , const double& y0) {

      m_sy = sy;
      m_y0 = y0;
    }

    inline void SeedTrack::setXChi2( const double value ) {
      m_xChi2 = value;
    }

    inline void SeedTrack::setYChi2( const double value ) {
      m_yChi2 = value;
    }

    inline void SeedTrack::setSector( const int value ) {
      m_sector = value;
    }

    inline void SeedTrack::setDth( const double value ) {
      m_dth = value;
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

    inline bool SeedTrack::select() const {
      return m_select;
    }

    inline bool SeedTrack::live() const {
      return m_live;
    }

    inline int SeedTrack::nx() const {
      return m_nx;
    }

    inline int SeedTrack::nHit() const {
      return m_nx +m_ny;
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

    inline int SeedTrack::sector() const {
      return m_sector;
    }

    inline double SeedTrack::dth() const {
      return m_dth;
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

    inline void SeedTrack::addToXPnts(const SeedPnt& pnt){
      m_xPnts.push_back(pnt);
    }

    inline bool SeedTrack::pntByIncreasingZ::operator() (const SeedPnt& first,  const SeedPnt& second ) const {
      //-------------------------------------------------------------------------
      //  Define the sort sequence for hits
      //-------------------------------------------------------------------------
      return (first.z() < second.z());
    }

    inline bool SeedTrack::onTrack(const LHCb::LHCbID id, const int layer) const{
      return (layer == 1 || layer == 2 ? yHitOnTrack(id) : xHitOnTrack(id));
    }

    //Defintion of keyed container for Tsa::Track
    typedef KeyedContainer<SeedTrack, Containers::HashMap> SeedTracks;

  }
}

#endif // SEEDTRACK_H
