#ifndef RichBase_h
#define RichBase_h 1

// Include files
#include <map>
#include<vector>
#include <cmath>

#include "RichPixelProperties.h"

#include "RichKernel/RichSmartID.h"
#include "Event/Kernel.h"
#include "GaudiKernel/ObjectList.h"
#include "GaudiKernel/ObjectVector.h"

class RichPixel;
class RichRegistry;

class RichBase {

public:

  RichBase( ) : m_MyState ( OPEN ) { }
  
  RichBase( RichBase & base ) : m_MyState ( base.m_MyState ) { }

  ~RichBase();

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

  void upDate( std::vector<RichSmartID> & pixelList );

  void cleanUp();

  long size() const { return m_activePixels.size(); }

  RichPixelProperties* DecodeUniqueID(const unsigned int &) const;

  typedef std::map<unsigned int, RichPixelProperties*> activemap;

  activemap::const_iterator begin() const;

  activemap::const_iterator end() const;

private: // data

  BaseState m_MyState;

  activemap m_activePixels;

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

inline RichBase& RichBase::operator=( const RichBase& b )
{
  m_MyState = b.m_MyState;
  return *this;
}

inline RichPixelProperties* RichBase::DecodeUniqueID (const unsigned int& idx ) const
{
  activemap::const_iterator it = m_activePixels.find( idx );
  return ( it != m_activePixels.end() ? (*it).second : 0 );
}

#endif
