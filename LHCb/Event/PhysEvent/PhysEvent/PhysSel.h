

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



#ifndef PhysEvent_PhysSel_H
#define PhysEvent_PhysSel_H 1

// Include files
#include <map>
#include "LHCbEvent/CLHEPStreams.h"
#include "GaudiKernel/DataObject.h"
#include "PhysEvent/PhysSelDecay.h"


// Class ID definition
  static const CLID& CLID_PhysSel = 860;

/** @class PhysSel PhysSel.h 
 *
 *  Stores the results of the decay selection package
 *
 *  @author Gloria Corti
 *  created Mon Feb  4 20:21:56 2002
 *
 */

class PhysSel: public DataObject
{

public: 

  /// std constructor
  PhysSel()
  {
  setVersion(1);
  }

  /// std destructor
  ~PhysSel();

  /// Retrieve pointer to class definition structure
  virtual const CLID& clID() const; 
  static const CLID& classID(); 

  /// Remove all PhysSelDecays entries
  void removePhysSelDecays();

  /// Add single PhysSelDecay to map
  void addPhysSelDecay(std::string& name, PhysSelDecay*& value);

  /// Remove single PhysSelDecay entry from map via its name
  void removePhysSelDecay(std::string& name);

  /// Retrieve list of names for which a PhysSelDecay has been stored
  void whichPhysSelDecays(std::vector<std::string>& nameList);

  /// Retrieve the PhysSelDecay results for a decay channel by its name
  void retrievePhysSelDecay(std::string& name, const PhysSelDecay*& value);

  /// Retrieve results as a set of true/false for a decay channel specifying its name
  void results(std::string& name, bool& mc, bool& trk, bool& pqual, bool& sel, bool& agr, bool& tag);

  /// Return MC true result for a specific decay channel
  bool decayIsInMCTree(std::string& name);

  /// Return acceptance result for a specific decay channel
  bool decayHasTrkRecon(std::string& name);

  /// Return physics quality result for a specific decay channel
  bool decayHasTrkPQual(std::string& name);

  /// Return selection result for a specific decay channel
  bool decayIsSelected(std::string& name);

  /// Return agreement results  for a specific decay channel
  bool decaySelectedIsMC(std::string& name);

  /// Return flavour tag results for a specific decay channel
  bool decayIsFlavourTagged(std::string& name);

  /// Serialize the object for writing
  StreamBuffer& serialize(StreamBuffer& s) const;

  /// Serialize the object for reading
  StreamBuffer& serialize(StreamBuffer& s);

  /// Fill the ASCII output stream
  std::ostream& fillStream(std::ostream& s) const;


protected: 


private: 

  std::map<std::string,PhysSelDecay*> m_table; ///<    Map with decay selection results

};

// -----------------------------------------------------------------------------
//   end of class
// -----------------------------------------------------------------------------


inline const CLID& PhysSel::clID() const 
{
  return PhysSel::classID();
}

inline const CLID& PhysSel::classID()
{
  return CLID_PhysSel;
}


#endif   ///PhysEvent_PhysSel_H
