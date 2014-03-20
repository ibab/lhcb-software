// $Id: $
#ifndef DEFTLAYER_H
#define DEFTLAYER_H 1

// DetDesc
#include "DetDesc/DetectorElement.h"
#include "DetDesc/IGeometryInfo.h"


static const CLID CLID_DeFTLayer = 8604;


class DeFTLayer : public DetectorElement {

public: 

  /// Standard constructor
  DeFTLayer( const std::string& name = "" );
  
  /// Destructor
  virtual ~DeFTLayer( );

  /** Initialization method 
   *  @return Status of initialization
   */ 
  virtual StatusCode initialize();

  /** Retrieves reference to class identifier
   *  @return The class identifier for this class
   */
  const CLID& clID() const;

  /** Another reference to class identifier
   *  @return The class identifier for this class
   */
  static const CLID& classID() { return CLID_DeFTLayer; }

};

// -----------------------------------------------------------------------------
//   end of class
// -----------------------------------------------------------------------------

#endif // DEFTLAYER_H
