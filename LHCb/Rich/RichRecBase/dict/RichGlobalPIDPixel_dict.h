

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



#ifndef RichRecBase_RichGlobalPIDPixel_H
#define RichRecBase_RichGlobalPIDPixel_H 1

// Include files
#include <vector>
#include "Kernel/CLHEPStreams.h"
#include "Event/KeyedObject.h"
#include "GaudiKernel/ObjectList.h"
#include "GaudiKernel/ObjectVector.h"
#include "Event/RichRecPixel.h"
#include "Kernel/SerializeStl.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/StreamBuffer.h"


// Class ID definition
  static const CLID& CLID_RichGlobalPIDPixel = 12433;

// Namespace for locations in TDS
namespace RichGlobalPIDPixelLocation {
  static const std::string& Default = "Rec/Rich/GlobalPIDPixels";
}


/** @class RichGlobalPIDPixel RichGlobalPIDPixel.h 
 *
 *  Working pixel object for Global PID
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  created Sun Mar  2 19:00:05 2003
 *
 */

class RichGlobalPIDPixel: public KeyedObject<long>
{

public: 

  friend class RichGlobalPIDPixel_dict;

  /// Default Constructor 
  RichGlobalPIDPixel() 
    : m_backgroundEstimates() {}

  /// Destructor 
  virtual ~RichGlobalPIDPixel() {}

  /// Retrieve pointer to class definition structure
  virtual const CLID& clID() const; 
  static const CLID& classID(); 

  /// Retrieve Background estimate for this pixel, for each event iteration
  const std::vector<float>& backgroundEstimates() const; 

  /// Retrieve Background estimate for this pixel, for each event iteration (non-const)
  std::vector<float>& backgroundEstimates();

  /// Update Background estimate for this pixel, for each event iteration
  void setBackgroundEstimates(const std::vector<float>& value);

  /// Retrieve Associated RichRecPixel (const)
  const RichRecPixel* richRecPixel() const;

  /// Retrieve Associated RichRecPixel (non-const)
  RichRecPixel* richRecPixel();

  /// Update Associated RichRecPixel
  void setRichRecPixel(const SmartRef<RichRecPixel>& value);

  /// Serialize the object for writing
  virtual StreamBuffer& serialize(StreamBuffer& s) const;

  /// Serialize the object for reading
  virtual StreamBuffer& serialize(StreamBuffer& s);

  /// Fill the ASCII output stream
  virtual std::ostream& fillStream(std::ostream& s) const;

protected: 

private: 

  std::vector<float> m_backgroundEstimates; ///< Background estimate for this pixel, for each event iteration
  SmartRef<RichRecPixel> m_richRecPixel; ///< Associated RichRecPixel

};

// -----------------------------------------------------------------------------
//   end of class
// -----------------------------------------------------------------------------


inline const CLID& RichGlobalPIDPixel::clID() const 
{
  return RichGlobalPIDPixel::classID();
}

inline const CLID& RichGlobalPIDPixel::classID()
{
  return CLID_RichGlobalPIDPixel;
}

inline const std::vector<float>& RichGlobalPIDPixel::backgroundEstimates() const 
{
  return m_backgroundEstimates;
}

inline std::vector<float>& RichGlobalPIDPixel::backgroundEstimates()
{
  return m_backgroundEstimates;
}

inline void RichGlobalPIDPixel::setBackgroundEstimates(const std::vector<float>& value)
{
  m_backgroundEstimates = value; 
}

inline const RichRecPixel* RichGlobalPIDPixel::richRecPixel() const
{
   return m_richRecPixel;
}

inline RichRecPixel* RichGlobalPIDPixel::richRecPixel() 
{
   return m_richRecPixel;
}

inline void RichGlobalPIDPixel::setRichRecPixel(const SmartRef<RichRecPixel>& value)
{
   m_richRecPixel = value;
}

inline StreamBuffer& RichGlobalPIDPixel::serialize(StreamBuffer& s) const 
{
  KeyedObject<long>::serialize(s);
  s << m_backgroundEstimates
    << m_richRecPixel(this);
  return s;
}

inline StreamBuffer& RichGlobalPIDPixel::serialize(StreamBuffer& s)
{
  KeyedObject<long>::serialize(s);
  s >> m_backgroundEstimates
    >> m_richRecPixel(this);
  return s;
}

inline std::ostream& RichGlobalPIDPixel::fillStream(std::ostream& s) const
{
  s << "{ "
    << " backgroundEstimates:\t" << m_backgroundEstimates << " } ";
  return s;
}

//Defintion of keyed container for RichGlobalPIDPixel
typedef KeyedContainer<RichGlobalPIDPixel, Containers::HashMap> RichGlobalPIDPixels;
// typedef for std::vector of RichGlobalPIDPixel
typedef std::vector<RichGlobalPIDPixel*> RichGlobalPIDPixelVector;

#endif   ///RichRecBase_RichGlobalPIDPixel_H
