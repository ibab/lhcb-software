
//   **************************************************************************
//   *                                                                        *
//   *                      ! ! ! A T T E N T I O N ! ! !                     *
//   *                                                                        *
//   *  This file was created automatically by GaudiObjDesc, please do not    *
//   *  delete it or edit it by hand.                                         *
//   *                                                                        *
//   *  If you want to change this file, first change the corresponding       *
//   *  xml-file and rerun the tools from GaudiObjDesc (or run make if you    *
//   *  are using it from inside a Gaudi-package).                            *
//   *                                                                        *
//   **************************************************************************

#ifndef HltEvent_MultiTrack_H
#define HltEvent_MultiTrack_H 1

// Include files
#include "Event/Track.h"
#include "GaudiKernel/NamedRange.h"
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/ObjectVector.h"
#ifdef _WIN32
// Avoid conflict of Windows macro with std::max
  #ifndef NOMINMAX
    #define NOMINMAX
  #endif
#endif
#include "GaudiKernel/boost_allocator.h"
#include "GaudiKernel/SmartRef.h"
#include <vector>
#include <ostream>

// Forward declarations

namespace Hlt
{

  // Forward declarations
  
  // Class ID definition
  static const CLID CLID_MultiTrack = 7564;
  
  // Namespace for locations in TDS
  namespace MultiTrackLocation {
    static const std::string& Default = "Hlt/MultiTracks";
  }
  

  /** @class MultiTrack MultiTrack.h
   *
   * Representation of di-track for Hlt 
   *
   * @author alexander.mazurov@gmail.com
   * created Mon Jun 14 15:57:48 2010
   * 
   */

  class MultiTrack: public ContainedObject
  {
  public:

    /// typedef for std::vector of MultiTrack
    typedef std::vector<MultiTrack*> Vector;
    typedef std::vector<const MultiTrack*> ConstVector;
  
  /// typedef for ObjectVector of MultiTrack
  typedef ObjectVector<MultiTrack> Container;
  
    /// The container type for shared di-tracks (without ownership)
    typedef SharedObjectsContainer<Hlt::MultiTrack> Selection;
    /// For uniform access to containers in TES (KeyedContainer,SharedContainer) 
    typedef Gaudi::NamedRange_<ConstVector> Range;
  
    /// Default Constructor
    MultiTrack() {}
  
    /// Default Destructor
    virtual ~MultiTrack() {}
  
    // Retrieve pointer to class definition structure
    virtual const CLID& clID() const;
    static const CLID& classID();
  
    /// Fill the ASCII output stream
   virtual std::ostream& fillStream(std::ostream& s) const;
  
    /// Retrieve (const)  Reference to the first track
    const LHCb::Track* first() const;
  
    /// Retrieve  Reference to the first track
    LHCb::Track* first();
  
    /// Update  Reference to the first track
    void setFirst(const SmartRef<LHCb::Track>& value);
  
    /// Update (pointer)  Reference to the first track
    void setFirst(const LHCb::Track* value);
  
    /// Retrieve (const)  Reference to the second track
    const LHCb::Track* second() const;
  
    /// Retrieve  Reference to the second track
    LHCb::Track* second();
  
    /// Update  Reference to the second track
    void setSecond(const SmartRef<LHCb::Track>& value);
  
    /// Update (pointer)  Reference to the second track
    void setSecond(const LHCb::Track* value);
  
  
  #ifndef GOD_NOALLOC
    /// operator new
    static void* operator new ( size_t size )
    {
      return ( sizeof(MultiTrack) == size ?
               boost::singleton_pool<MultiTrack, sizeof(MultiTrack)>::malloc() :
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
      boost::singleton_pool<MultiTrack, sizeof(MultiTrack)>::is_from(p) ?
      boost::singleton_pool<MultiTrack, sizeof(MultiTrack)>::free(p) :
      ::operator delete(p);
    }
  
    /// placement operator delete
    /// not sure if really needed, but it does not harm
    static void operator delete ( void* p, void* pObj )
    {
      ::operator delete (p, pObj);
    }
  #endif
  protected:

  private:

    SmartRef<LHCb::Track> m_first;  ///< Reference to the first track
    SmartRef<LHCb::Track> m_second; ///< Reference to the second track
  
  }; // class MultiTrack

  /// Definition of vector container type for MultiTrack
  typedef ObjectVector<MultiTrack> MultiTracks;
  
  inline std::ostream& operator<< (std::ostream& str, const MultiTrack& obj)
  {
    return obj.fillStream(str);
  }
  
} // namespace LHCb;

// -----------------------------------------------------------------------------
// end of class
// -----------------------------------------------------------------------------

// Including forward declarations

inline const CLID& Hlt::MultiTrack::clID() const
{
  return Hlt::MultiTrack::classID();
}

inline const CLID& Hlt::MultiTrack::classID()
{
  return CLID_MultiTrack;
}

inline std::ostream& Hlt::MultiTrack::fillStream(std::ostream& s) const
{
  return s;
}


inline const LHCb::Track* Hlt::MultiTrack::first() const
{
  return m_first;
}

inline LHCb::Track* Hlt::MultiTrack::first()
{
  return m_first;
}

inline void Hlt::MultiTrack::setFirst(const SmartRef<LHCb::Track>& value)
{
  m_first = value;
}

inline void Hlt::MultiTrack::setFirst(const LHCb::Track* value)
{
  m_first = value;
}

inline const LHCb::Track* Hlt::MultiTrack::second() const
{
  return m_second;
}

inline LHCb::Track* Hlt::MultiTrack::second()
{
  return m_second;
}

inline void Hlt::MultiTrack::setSecond(const SmartRef<LHCb::Track>& value)
{
  m_second = value;
}

inline void Hlt::MultiTrack::setSecond(const LHCb::Track* value)
{
  m_second = value;
}


#endif ///HltEvent_MultiTrack_H
