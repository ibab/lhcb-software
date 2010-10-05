#ifndef MOVEDATAOBJECT_H 
#define MOVEDATAOBJECT_H 1

// Include files
// from Gaudi
#include "MicroDST/MicroDSTAlgorithm.h"

/** @class MoveDataObject MoveDataObject.h
 *  
 *  Move a DataObject from one TES location to another.
 *  
 *  <b>Example</b> Move RecVertices from "/Event/SpecialRec/Vertex/V0" to
 *  "/Event/MyLocation/SpecialRec/Vertex/V0". The object is removed from
 *  the original location, so never do this unless you are sure nothing else
 *  is going to try to acces it!
 *
 *  @code
 *
 *  mySeq = GaudiSequencer("SomeSequence")
 *  moveV0=MoveDataObject('MoveV0',
 *                        InputLocations = ['/Event/SpecialRec/Vertex/V0'] 
 *                        OutputPrefix = 'MyLocation')
 *  mySeq.Members += [copyPV]
 *  @endcode
 *  
 *  @author Juan Palacios
 *  @date   2010-10-05
 */
class MoveDataObject : public MicroDSTAlgorithm {
public: 
  /// Standard constructor
  MoveDataObject( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MoveDataObject( ); ///< Destructor

  virtual StatusCode execute   ();    ///< Algorithm execution

private:

  void executeLocation(const std::string& particleLocation);
  
};
#endif // MOVEDATAOBJECT_H
