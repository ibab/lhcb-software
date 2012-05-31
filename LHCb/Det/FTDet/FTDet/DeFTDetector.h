// $Id: $
#ifndef DEFTDETECTOR_H
#define DEFTDETECTOR_H 1

// DetDesc
#include "DetDesc/DetectorElement.h"
#include "DetDesc/IGeometryInfo.h"

// Local
#include "DeFTStation.h"
#include "DeFTBiLayer.h"
#include "DeFTLayer.h"


/** @class DeFTDetector DeFTDetector.h "FTDet/DeFTDetector.h"
 *
 *  This is the top-level detector element class of the Fibre Tracker (FT).
 *  It has member variables holding pointers to the detector sub-structure
 *  det. elements (Stations, BiLayers and Layers). Provides methods for
 *  the correspondence geometrical point <--> FTChannelID. In most cases
 *  the actual implementation of these methods is made in the sub-structure
 *  det.elements with largest granularity, i.e. in DeFTLayer.
 *  See FTDet/DeFTLayer.h for the quarter, SiPM and cell/channel numbering
 *  conventions.
 *
 *  @author Plamen Hopchev
 *  @date   2012-04-25
 */

namespace DeFTDetectorLocation{
  // FT location defined in the XmlDDDB
  static const std::string& Default = "/dd/Structure/LHCb/AfterMagnetRegion/T/FT";
}

static const CLID CLID_DeFTDetector = 18001;

class DeFTDetector : public DetectorElement {

public: 

  // Typedefs
  typedef std::vector<DeFTStation*> Stations;
  typedef std::vector<DeFTBiLayer*> BiLayers;
  typedef std::vector<DeFTLayer*> Layers;

  /// Standard constructor
  DeFTDetector( const std::string& name = "" );
  
  /// Destructor
  virtual ~DeFTDetector( );

  /** Retrieves reference to class identifier
   *  @return The class identifier for this class
   */
  const CLID& clID() const;

  /** Another reference to class identifier
   *  @return The class identifier for this class
   */
  static const CLID& classID() { return CLID_DeFTDetector; }

  /** Initialization method 
   *  @return Status of initialization
   */ 
  virtual StatusCode initialize();

  /** @return Vector of pointers to the FT Stations */
  const Stations& stations() const;

  /** @return Vector of pointers to the FT BiLayers */
  const BiLayers& biLayers() const;

  /** @return Vector of pointers to the FT Layers */
  const Layers&   layers()   const;

  /** Find the FT Station where a global point is
   *  @return Pointer to the relevant Station
   */
  const DeFTStation* findStation(const Gaudi::XYZPoint& aPoint) const;

  /** Find the FT BiLayer where a global point is
   *  @return Pointer to the relevant BiLayer
   */
  const DeFTBiLayer* findBiLayer(const Gaudi::XYZPoint& aPoint) const;

  /** Find the FT Layer where a global point is
   *  @return Pointer to the relevant Layer
   */
  const DeFTLayer*   findLayer(  const Gaudi::XYZPoint& aPoint) const;

  //TO-DO (when needed): Similar functions to find S/B/L where an FTChannelID is

protected:

private:

  Stations m_stations;     ///< vector of pointers to stations
  BiLayers m_bilayers;     ///< vector of pointers to bilayers
  Layers m_layers;         ///< vector of pointers to layers
  double m_angleMagnitude; ///< magnitude of the stereo angle of the u/v layers (in radians)
  bool m_printInitInfo;    ///< switch for prinitng of the layer properties during initialize

}; //end of class

/// Methods to get the sub-structure detector elements
inline const DeFTDetector::Stations& DeFTDetector::stations() const { return m_stations; }
inline const DeFTDetector::BiLayers& DeFTDetector::biLayers() const { return m_bilayers; }
inline const DeFTDetector::Layers&   DeFTDetector::layers()   const { return m_layers;   }

#endif // DEFTDETECTOR_H
