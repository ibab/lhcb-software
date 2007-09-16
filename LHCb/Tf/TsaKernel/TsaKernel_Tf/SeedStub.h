// $Id: SeedStub.h,v 1.1 2007-09-16 09:44:45 mneedham Exp $
#ifndef SEEDSTUB_H
#define SEEDSTUB_H 1

#include "GaudiKernel/KeyedObject.h"

#include "GaudiKernel/boost_allocator.h"

//class SeedHit;
#include "TsaKernel_Tf/SeedHit.h"

// Class ID definition
static const CLID CLID_TsaSeedStub = 19113;

// Namespace for locations in TDS
namespace SeedStubLocation {
  static const std::string& Default = "/Event/Rec/Tsa/SeedStubs";
}

namespace Tf
{
  namespace Tsa
  {

    /** @class SeedStub SeedStub.h
     *  An object to hold the information needed per IT stub for track seeding
     *
     *  @author Roger Forty
     *  @date   2006-5-4
     */

    class SeedStub: public KeyedObject<int> {
    public:

      /// Constructor
      SeedStub( );

      SeedStub( double x, double y, double z1, double z2, double sx, double sy,
                SeedHit* xHit1, SeedHit* xHit2, SeedHit* yHit1, SeedHit* yHit2,
                bool ownHits = true );

      ~SeedStub( );

      void setLive( const bool value );
      void setX( const double value );
      void setY( const double value );
      void setZ1( const double value );
      void setZ2( const double value );
      void setSx( const double value );
      void setSy( const double value );

      bool live() const;
      double x() const;
      double y() const;
      double z1() const;
      double z2() const;
      double sx() const;
      double sy() const;
      SeedHit* xHit1() const;
      SeedHit* xHit2() const;
      SeedHit* yHit1() const;
      SeedHit* yHit2() const;


#ifndef _WIN32
      /// operator new
      static void* operator new ( size_t size )
      {
        return ( sizeof(SeedStub) == size ?
                 boost::singleton_pool<SeedStub, sizeof(SeedStub)>::malloc() :
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
        boost::singleton_pool<SeedStub, sizeof(SeedStub)>::is_from(p) ?
          boost::singleton_pool<SeedStub, sizeof(SeedStub)>::free(p) :
          ::operator delete(p);
      }

      /// placement operator delete
      /// not sure if really needed, but it does not harm
      static void operator delete ( void* p, void* pObj )
      {
        ::operator delete (p, pObj);
      }
#endif

    private:

      bool m_live;
      double m_x;
      double m_y;
      double m_z1;
      double m_z2;
      double m_sx;
      double m_sy;
      SeedHit* m_xHit1;
      SeedHit* m_xHit2;
      SeedHit* m_yHit1;
      SeedHit* m_yHit2;
      bool m_ownHits;

    };

    inline SeedStub::SeedStub( ): KeyedObject<int>(),
                                  m_live ( 1 ),
                                  m_x ( 0. ),
                                  m_y ( 0. ),
                                  m_z1 ( 0. ),
                                  m_z2 ( 0. ),
                                  m_sx ( 0. ),
                                  m_sy ( 0. ),
                                  m_xHit1 ( 0 ),
                                  m_xHit2 ( 0 ),
                                  m_yHit1 ( 0 ),
                                  m_yHit2 ( 0 ),
                                  m_ownHits(false) { }

    inline SeedStub::SeedStub(double x, double y, double z1, double z2, double sx, double sy,
                              SeedHit* xHit1, SeedHit* xHit2, SeedHit* yHit1, SeedHit* yHit2, bool ownHits )
      : KeyedObject<int>(),
        m_live ( true ),
        m_x ( x),
        m_y ( y ),
        m_z1 ( z1 ),
        m_z2 ( z2 ),
        m_sx ( sx ),
        m_sy ( sy ),
        m_xHit1 (xHit1  ),
        m_xHit2 ( xHit2 ),
        m_yHit1 ( yHit1 ),
        m_yHit2 ( yHit2 ),
        m_ownHits ( ownHits ) { }

    /// Destructor
    inline SeedStub::~SeedStub( )
    {
      if ( m_ownHits )
      {
        if ( m_xHit1 != 0 ) delete m_xHit1;
        if ( m_xHit2 != 0 ) delete m_xHit2;
        if ( m_yHit1 != 0 ) delete m_yHit1;
        if ( m_yHit2 != 0 ) delete m_yHit2;
      }
    }

    inline void SeedStub::setLive( const bool value ) {
      m_live = value;
    }

    inline void SeedStub::setX( const double value ) {
      m_x = value;
    }

    inline void SeedStub::setY( const double value ) {
      m_y = value;
    }

    inline void SeedStub::setZ1( const double value ) {
      m_z1 = value;
    }

    inline void SeedStub::setZ2( const double value ) {
      m_z2 = value;
    }

    inline void SeedStub::setSx( const double value ) {
      m_sx = value;
    }

    inline void SeedStub::setSy( const double value ) {
      m_sy = value;
    }

    //inline void SeedStub::setXHit1( SeedHit* value ) {
    //  m_xHit1 = value;
    //}

    //inline void SeedStub::setXHit2( SeedHit* value ) {
    // m_xHit2 = value;
    //}

    //inline void SeedStub::setYHit1( SeedHit* value ) {
    //  m_yHit1 = value;
    //}

    //inline void SeedStub::setYHit2( SeedHit* value ) {
    //  m_yHit2 = value;
    //}

    inline bool SeedStub::live() const {
      return m_live;
    }

    inline double SeedStub::x() const {
      return m_x;
    }

    inline double SeedStub::y() const {
      return m_y;
    }

    inline double SeedStub::z1() const {
      return m_z1;
    }

    inline double SeedStub::z2() const {
      return m_z2;
    }

    inline double SeedStub::sx() const {
      return m_sx;
    }

    inline double SeedStub::sy() const {
      return m_sy;
    }

    inline SeedHit* SeedStub::xHit1() const {
      return m_xHit1;
    }

    inline SeedHit* SeedStub::xHit2() const {
      return m_xHit2;
    }

    inline SeedHit* SeedStub::yHit1() const {
      return m_yHit1;
    }

    inline SeedHit* SeedStub::yHit2() const {
      return m_yHit2;
    }

    //Defintion of keyed container for Tsa::Track
    typedef KeyedContainer<SeedStub, Containers::HashMap> SeedStubs;

  }
}

#endif // SEEDSTUB_H
