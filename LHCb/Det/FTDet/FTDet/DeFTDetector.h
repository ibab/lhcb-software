// $Id: $
#ifndef DEFTDETECTOR_H
#define DEFTDETECTOR_H 1

#include <memory>

// DetDesc
#include "DetDesc/DetectorElement.h"
#include "DetDesc/IGeometryInfo.h"

// Local
#include "DeFTStation.h"
#include "DeFTBiLayer.h"
#include "DeFTLayer.h"
#include "DeFTModule.h"
#include "DeFTFibreModule.h"
#include "DeFTFibreMat.h"

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

static const CLID CLID_DeFTDetector = 8601;

class DeFTDetector : public DetectorElement {

// Typedefs
typedef std::vector<DeFTStation*>     Stations;
typedef std::vector<DeFTBiLayer*>     BiLayers;
typedef std::vector<DeFTLayer*>       Layers;
typedef std::vector<DeFTModule*>      Modules;
typedef std::vector<DeFTFibreModule*> FibreModules;
typedef std::vector<DeFTFibreMat*>    FibreMats;

public: 

  /// Standard constructor
  DeFTDetector( const std::string& name = "" );
  
  /// Destructor
  ~DeFTDetector( ) override;

  /** Initialization method 
   *  @return Status of initialization
   */ 
  StatusCode initialize() override;

  /** Finalization method - delete objects created with new
   *  @return Status of finalization
   */ 
  StatusCode finalize();

  /** Retrieves reference to class identifier
   *  @return The class identifier for this class
   */
  const CLID& clID() const override;

  /** Another reference to class identifier
   *  @return The class identifier for this class
   */
  static const CLID& classID() { return CLID_DeFTDetector; }

  /** @return Ft version  */
  int version() const { return m_FTversion; }

  /** @return Vector of pointers to the FT Stations */
  const Stations& stations() const { return m_stations; }

  /** @return Vector of pointers to the FT BiLayers */
  const BiLayers& bilayers() const { return m_bilayers; }
  
  /** @return Vector of pointers to the FT Layers */
  const Layers&   layers()   const { return m_layers;   }
  
  /** @return Vector of pointers to the FT Modules */
  const Modules& modules() const { return m_modules; }

  /** @return Vector of pointers to the FT FibreModules */
  const FibreModules& fibremodules() const { return m_fibremodules; }

  /** @return Vector of pointers to the FT FibreMats */
  const FibreMats& fibremats() const { return m_fibremats; }

  /** */

  

  /** Find the FT Station where a global point is
   *  @return Pointer to the relevant Station
   */
  const DeFTStation* findStation(const Gaudi::XYZPoint& point) const;

  /** Find the FT BiLayer where a global point is
   *  @return Pointer to the relevant Layer
   */
  const DeFTBiLayer* findBiLayer(const Gaudi::XYZPoint& point) const;

  /** Find the FT Layer where a global point is
   *  @return Pointer to the relevant Layer
   */
  const DeFTLayer* findLayer(const Gaudi::XYZPoint& point) const;

  /** Find the FT Module where a global point is
   *  @return Pointer to the relevant Module
   */
  const DeFTModule* findModule(const Gaudi::XYZPoint& point) const;

  /** Find the FT FibreModule where a global point is
   *  @return Pointer to the relevant FibreModule
   */
  const DeFTFibreModule* findFibreModule(const Gaudi::XYZPoint& point) const;

  /** Find the FT FibreMat where a global point is
   *  @return Pointer to the relevant FibreMat
   */
  const DeFTFibreMat* findFibreMat(const Gaudi::XYZPoint& point) const;


  /** Find the FT Layer where a global point is
   *  @return Pointer to the relevant Layer
   */

  const DeFTFibreMat* findFibreMat( const LHCb::FTChannelID id ) const;

  // update and acquire any user geometry paramters
  StatusCode geometryUpdate ( );

  /// Make the Test algo a friend so that it can call private methods
  friend class DeFTTestAlg;

private: // private data members

  unsigned int m_FTversion = 0;
  Stations m_stations;           ///< vector of pointers to stations
  BiLayers m_bilayers;           ///< vector of pointers to Bilayers
  Layers m_layers;               ///< vector of pointers to layers
  Modules m_modules;             ///< vector of pointers to modules
  FibreModules m_fibremodules;   ///< vector of pointers to fibremodules
  FibreMats m_fibremats;         ///< vector of pointers to fibremats

  std::string m_FibreModuleNameSpec = "FibreModule";
  std::string m_FibreMatNameSpec = "Fibre";

  /// Use a single MsgStream instance (created in initialize)
  std::unique_ptr<MsgStream> m_msg;
  
}; //end of class

#endif // DEFTDETECTOR_H
