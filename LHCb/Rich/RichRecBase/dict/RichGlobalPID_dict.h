

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



#ifndef RichRecBase_RichGlobalPID_H
#define RichRecBase_RichGlobalPID_H 1

// Include files
#include "Kernel/CLHEPStreams.h"
#include "GaudiKernel/ObjectList.h"
#include "GaudiKernel/ObjectVector.h"
#include "Event/RichPID.h"
#include "Kernel/SerializeStl.h"
#include "GaudiKernel/StreamBuffer.h"


// Class ID definition
  static const CLID& CLID_RichGlobalPID = 12434;

// Namespace for locations in TDS
namespace RichGlobalPIDLocation {
  static const std::string& Default = "Rec/Rich/GlobalPIDs";
}


/** @class RichGlobalPID RichGlobalPID.h 
 *
 *  Rich particle identification object for the global track based algorithm
 *
 *  @author Chris Jones   (Christopher.Rob.Jones@cern.ch)
 *  created Sun Mar  2 19:00:05 2003
 *
 */

class RichGlobalPID: public RichPID
{

public: 

  friend class RichGlobalPID_dict;

  /// Default constructor
  RichGlobalPID()
  {
this->setGlobal(1);
  }

  /// Destructor 
  virtual ~RichGlobalPID() {}

  /// Retrieve pointer to class definition structure
  virtual const CLID& clID() const; 
  static const CLID& classID(); 

  /// Serialize the object for writing
  virtual StreamBuffer& serialize(StreamBuffer& s) const;

  /// Serialize the object for reading
  virtual StreamBuffer& serialize(StreamBuffer& s);

  /// Fill the ASCII output stream
  virtual std::ostream& fillStream(std::ostream& s) const;

protected: 

private: 


};

// -----------------------------------------------------------------------------
//   end of class
// -----------------------------------------------------------------------------


inline const CLID& RichGlobalPID::clID() const 
{
  return RichGlobalPID::classID();
}

inline const CLID& RichGlobalPID::classID()
{
  return CLID_RichGlobalPID;
}

inline StreamBuffer& RichGlobalPID::serialize(StreamBuffer& s) const 
{
  RichPID::serialize(s);
  return s;
}

inline StreamBuffer& RichGlobalPID::serialize(StreamBuffer& s)
{
  RichPID::serialize(s);
  return s;
}

inline std::ostream& RichGlobalPID::fillStream(std::ostream& s) const
{
  return s;
}

//Defintion of keyed container for RichGlobalPID
typedef KeyedContainer<RichGlobalPID, Containers::HashMap> RichGlobalPIDs;
// typedef for std::vector of RichGlobalPID
typedef std::vector<RichGlobalPID*> RichGlobalPIDVector;

#endif   ///RichRecBase_RichGlobalPID_H
