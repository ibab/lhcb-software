

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



#ifndef RichRecBase_RichGlobalPIDTrack_H
#define RichRecBase_RichGlobalPIDTrack_H 1

// Include files
#include "Kernel/CLHEPStreams.h"
#include "Event/KeyedObject.h"
#include "GaudiKernel/ObjectList.h"
#include "GaudiKernel/ObjectVector.h"
#include "RichRecBase/RichGlobalPIDTkQuality.h"
#include "Event/RichRecTrack.h"
#include "Kernel/SerializeStl.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/StreamBuffer.h"


// Forward declarations
class RichGlobalPID;


// Class ID definition
  static const CLID& CLID_RichGlobalPIDTrack = 12432;

// Namespace for locations in TDS
namespace RichGlobalPIDTrackLocation {
  static const std::string& Default = "Rec/Rich/GlobalPIDTracks";
}


/** @class RichGlobalPIDTrack RichGlobalPIDTrack.h 
 *
 *  Working track object for Rich Global PID
 *
 *  @author Chris Jones     Christopher.Rob.Jones@cern.ch
 *  created Sun Mar  2 19:00:05 2003
 *
 */

class RichGlobalPIDTrack: public KeyedObject<long>
{

public: 

  friend class RichGlobalPIDTrack_dict;

  /// Default Constructor
  RichGlobalPIDTrack()
    : m_trQuality(Rich::GlobalPID::Unknown)
  {
 
  }

  /// Destructor 
  virtual ~RichGlobalPIDTrack() {}

  /// Retrieve pointer to class definition structure
  virtual const CLID& clID() const; 
  static const CLID& classID(); 

  /// Retrieve Integer code describing the quality of this track for the Global likelihood method
  const Rich::GlobalPID::TkQuality& trQuality() const; 

  /// Retrieve Integer code describing the quality of this track for the Global likelihood method (non-const)
  Rich::GlobalPID::TkQuality& trQuality();

  /// Update Integer code describing the quality of this track for the Global likelihood method
  void setTrQuality(const Rich::GlobalPID::TkQuality& value);

  /// Retrieve SmartRef to RichRecTrack (const)
  const RichRecTrack* richRecTrack() const;

  /// Retrieve SmartRef to RichRecTrack (non-const)
  RichRecTrack* richRecTrack();

  /// Update SmartRef to RichRecTrack
  void setRichRecTrack(const SmartRef<RichRecTrack>& value);

  /// Retrieve Associated GlobalPID object (const)
  const RichGlobalPID* globalPID() const;

  /// Retrieve Associated GlobalPID object (non-const)
  RichGlobalPID* globalPID();

  /// Update Associated GlobalPID object
  void setGlobalPID(const SmartRef<RichGlobalPID>& value);

  /// Serialize the object for writing
  virtual StreamBuffer& serialize(StreamBuffer& s) const;

  /// Serialize the object for reading
  virtual StreamBuffer& serialize(StreamBuffer& s);

  /// Fill the ASCII output stream
  virtual std::ostream& fillStream(std::ostream& s) const;

protected: 

private: 

  Rich::GlobalPID::TkQuality m_trQuality; ///< Integer code describing the quality of this track for the Global likelihood method
  SmartRef<RichRecTrack> m_richRecTrack; ///< SmartRef to RichRecTrack
  SmartRef<RichGlobalPID> m_globalPID; ///< Associated GlobalPID object

};

// -----------------------------------------------------------------------------
//   end of class
// -----------------------------------------------------------------------------


// Including forward declarations
#include "Event/RichGlobalPID.h"


inline const CLID& RichGlobalPIDTrack::clID() const 
{
  return RichGlobalPIDTrack::classID();
}

inline const CLID& RichGlobalPIDTrack::classID()
{
  return CLID_RichGlobalPIDTrack;
}

inline const Rich::GlobalPID::TkQuality& RichGlobalPIDTrack::trQuality() const 
{
  return m_trQuality;
}

inline Rich::GlobalPID::TkQuality& RichGlobalPIDTrack::trQuality()
{
  return m_trQuality;
}

inline void RichGlobalPIDTrack::setTrQuality(const Rich::GlobalPID::TkQuality& value)
{
  m_trQuality = value; 
}

inline const RichRecTrack* RichGlobalPIDTrack::richRecTrack() const
{
   return m_richRecTrack;
}

inline RichRecTrack* RichGlobalPIDTrack::richRecTrack() 
{
   return m_richRecTrack;
}

inline void RichGlobalPIDTrack::setRichRecTrack(const SmartRef<RichRecTrack>& value)
{
   m_richRecTrack = value;
}

inline const RichGlobalPID* RichGlobalPIDTrack::globalPID() const
{
   return m_globalPID;
}

inline RichGlobalPID* RichGlobalPIDTrack::globalPID() 
{
   return m_globalPID;
}

inline void RichGlobalPIDTrack::setGlobalPID(const SmartRef<RichGlobalPID>& value)
{
   m_globalPID = value;
}

inline StreamBuffer& RichGlobalPIDTrack::serialize(StreamBuffer& s) const 
{
  KeyedObject<long>::serialize(s);
  s << m_trQuality
    << m_richRecTrack(this)
    << m_globalPID(this);
  return s;
}

inline StreamBuffer& RichGlobalPIDTrack::serialize(StreamBuffer& s)
{
  KeyedObject<long>::serialize(s);
  s >> m_trQuality
    >> m_richRecTrack(this)
    >> m_globalPID(this);
  return s;
}

inline std::ostream& RichGlobalPIDTrack::fillStream(std::ostream& s) const
{
  s << "{ "
    << " trQuality:\t" << m_trQuality << " } ";
  return s;
}

//Defintion of keyed container for RichGlobalPIDTrack
typedef KeyedContainer<RichGlobalPIDTrack, Containers::HashMap> RichGlobalPIDTracks;
// typedef for std::vector of RichGlobalPIDTrack
typedef std::vector<RichGlobalPIDTrack*> RichGlobalPIDTrackVector;

#endif   ///RichRecBase_RichGlobalPIDTrack_H
