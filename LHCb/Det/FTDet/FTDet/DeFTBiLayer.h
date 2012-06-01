// $Id: $
#ifndef DEFTBILAYER_H
#define DEFTBILAYER_H 1

// DetDesc
#include "DetDesc/DetectorElement.h"
#include "DetDesc/IGeometryInfo.h"

/** @class DeFTBiLayer DeFTBiLayer.h "FTDet/DeFTBiLayer.h"
 *
 *  This is the detector element class of the Fibre Tracker (FT) bi-layer.
 *  Currently it has no real function, but it has been created to maintain
 *  1-to-1 correspondence between the FT xml DDDB and the det. element
 *  classes in the Det/FTDet package.
 *
 *  @author Plamen Hopchev
 *  @date   2012-04-25
 */

static const CLID CLID_DeFTBiLayer = 18003;

class DeFTBiLayer : public DetectorElement {

public: 

  /// Standard constructor
  DeFTBiLayer( const std::string& name = "" );
  
  /// Destructor
  virtual ~DeFTBiLayer( );

  /** Retrieves reference to class identifier
   *  @return The class identifier for this class
   */
  const CLID& clID() const;

  /** Another reference to class identifier
   *  @return The class identifier for this class
   */
  static const CLID& classID() { return CLID_DeFTBiLayer; }

private: // private member functions

  /** Initialization method 
   *  @return Status of initialization
   */ 
  virtual StatusCode initialize();

};
#endif // DEFTBILAYER_H
