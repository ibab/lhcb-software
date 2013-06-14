// $Id: $
#ifndef DEFTFIBREMAT_H
#define DEFTFIBREMAT_H 1

// DetDesc
#include "DetDesc/DetectorElement.h"
#include "DetDesc/IGeometryInfo.h"

/** @class DeFTFibreMat DeFTFibreMat.h "FTDet/DeFTFibreMat.h"
 *
 *  This is the detector element class of the Fibre Tracker (FT) station.
 *  Currently it has no real function, but it has been created to maintain
 *  1-to-1 correspondence between the FT xml DDDB and the det. element
 *  classes in the Det/FTDet package.
 *
 *  @author Plamen Hopchev
 *  @date   2012-04-25
 */

static const CLID CLID_DeFTFibreMat = 8608;

class DeFTFibreMat : public DetectorElement {

public: 

  /// Standard constructor
  DeFTFibreMat( const std::string& name = "" );
  
  /// Destructor
  virtual ~DeFTFibreMat( );

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
  static const CLID& classID() { return CLID_DeFTFibreMat; }

};
#endif // DEFTFIBREMAT_H
