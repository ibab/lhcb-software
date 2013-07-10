
#ifndef RichBase_h
#define RichBase_h 1

// Include files
#include <map>
#include <vector>
#include <cmath>

#include "RichPixelProperties.h"

// Kernel
#include "RichKernel/RichHashMap.h"
#include "RichKernel/RichMap.h"
#include "Kernel/RichSmartID.h"

// gaudi
#include "GaudiKernel/ObjectList.h"
#include "GaudiKernel/ObjectVector.h"

namespace Rich
{
  namespace MC
  {
    namespace Digi
    {

      class RichPixel;
      class RichRegistry;

      class RichBase {

      public:

        RichBase( )
          : m_MyState ( OPEN )
        { }

        RichBase( RichBase & base ) :
          m_MyState ( base.m_MyState )
        { }

        ~RichBase() { cleanUp(); }

        enum BaseState
          {
            OPEN,        /*!< the base can be initalized */
            CLOSED,      /*!< the base cannot be initalized but channels can be setup */
            UNSETTABLE   /*!< the base is CLOSED and channels cannot be setup */
          };

      public:

        bool IsInOpenState() const
        {
          return ( m_MyState == OPEN ? true : false );
        }

        bool IsInSettableState() const
        {
          return ( m_MyState != UNSETTABLE ? true : false );
        }

        void CloseState()
        {
          if ( m_MyState == OPEN ) m_MyState = CLOSED;
        }

        void OpenState()
        {
          if ( m_MyState == CLOSED ) m_MyState = OPEN;
        }

        void UnsettableState()
        {
          m_MyState = UNSETTABLE;
        }

        void clear()
        {
          m_activePixels.clear();
        }

        void init(){};

        RichBase& operator=( const RichBase& );

        void upDate( const LHCb::RichSmartID::Vector & pixelList );

        void cleanUp();

        unsigned int size() const { return m_activePixels.size(); }

        RichPixelProperties* DecodeUniqueID(const LHCb::RichSmartID id) const;

        typedef Rich::HashMap< LHCb::RichSmartID::KeyType, RichPixelProperties*> activemap;
        //typedef Rich::Map< LHCb::RichSmartID::KeyType, RichPixelProperties*> activemap;

        activemap::const_iterator begin() const;

        activemap::const_iterator end() const;

      private: // data

        BaseState m_MyState;

        activemap m_activePixels;

        // crj : temporary single properties object whilst all pixels are treated the same
        RichPixelProperties m_pixProps;

        friend class RichRegistry;

      };

      inline RichBase::activemap::const_iterator RichBase::begin() const
      {
        return m_activePixels.begin();
      }

      inline RichBase::activemap::const_iterator RichBase::end() const
      {
        return m_activePixels.end();
      }

      inline RichBase& RichBase::operator=( const RichBase & b )
      {
        m_MyState = b.m_MyState;
        return *this;
      }

      inline RichPixelProperties* RichBase::DecodeUniqueID ( const LHCb::RichSmartID id ) const
      {
        activemap::const_iterator it = m_activePixels.find( id );
        return ( it != m_activePixels.end() ? (*it).second : 0 );
      }

    }
  }
}

#endif
