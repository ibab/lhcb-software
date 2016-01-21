
//-----------------------------------------------------------------------------
/** @file RichRadiatorTool.h
 *
 *  Header file for tool : Rich::RadiatorTool
 *
 *  @author Antonis Papanestis
 *  @date   2006-03-01
 */
//-----------------------------------------------------------------------------

#ifndef RICHTOOLS_RICHRADIATORTOOL_H
#define RICHTOOLS_RICHRADIATORTOOL_H 1

// STL
#include <vector>
#include <functional>
#include <algorithm>
#include <sstream>

// base class
#include "RichKernel/RichToolBase.h"

// RichKernel
#include "RichKernel/IRichRadiatorTool.h"

// LHCbKernel
#include "Kernel/RichRadiatorType.h"
#include "Kernel/RichRadIntersection.h"

// Math
#include "GaudiKernel/Transform3DTypes.h"

// from RichDet
#include "RichDet/DeRichRadiator.h"
#include "RichDet/DeRichMultiSolidRadiator.h"
#include "RichDet/DeRichAerogelRadiator.h"

namespace Rich
{

  //-----------------------------------------------------------------------------
  /** @class RadiatorTool RichRadiatorTool.h
   *
   *  Tool to find the intersections with a given radiator volume
   *
   *  @author Antonis Papanestis
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   2006-03-01
   */
  //-----------------------------------------------------------------------------

  class RadiatorTool : public Rich::ToolBase,
                       virtual public IRadiatorTool
  {

  public:

    /// Standard constructor
    RadiatorTool( const std::string& type,
                  const std::string& name,
                  const IInterface* parent);

    virtual ~RadiatorTool( ); ///< Destructor

    // Finalization of the tool
    virtual StatusCode finalize();

  public:

    /** @brief Finds the intersections of a given vector from a given point (entry/exit) with radiator.
     *
     * For multiple radiators (e.g. the aerogel tiles) there can be more than one intersections
     *
     * @param globalPoint   The start point for the intersection extraplotion
     * @param globalVector  The direction vector for the intersection extraplotion
     * @param radiator      The radiator to find the intersections in
     * @param intersections The found intersections
     *
     * @return The number of intersections
     */
    unsigned int intersections( const Gaudi::XYZPoint& globalPoint,
                                const Gaudi::XYZVector& globalVector,
                                const Rich::RadiatorType radiator,
                                Rich::RadIntersection::Vector& intersections ) const;

  private:

    /// Load the given radiator detector element
    void loadRadiator( const Rich::RadiatorType radiator );

    StatusCode updateRich1Gas();    ///< UMS update for Rich1Gas
    StatusCode updateRich2Gas();    ///< UMS update for Rich2Gas
    StatusCode updateAerogel();     ///< UMS update for Aerogel

  private: // classes

    /// Functor to sort RichSmartIDs by Rich then panel numbers
    class SortByDistFromBeam : std::binary_function< const DeRichRadiator*, 
                                                     const DeRichRadiator*, bool >
    {
    public:
      /** Sort operator for the rich radiators
       *
       *  Sorts into order of distance from beam line (x,y) = (0,0)
       *
       *  @param r1 First radiator
       *  @param r2 Second radiator
       *
       *  @return bool indicating if r1 should be listed before r2
       */
      inline bool operator() ( const DeRichRadiator* r1, const DeRichRadiator* r2 ) const
      {
        const Gaudi::XYZPoint c1 ( r1->geometry()->toGlobal(Gaudi::XYZVector(0,0,0)) );
        const Gaudi::XYZPoint c2 ( r2->geometry()->toGlobal(Gaudi::XYZVector(0,0,0)) );
        return ( std::pow(c1.x(),2) + std::pow(c1.y(),2) <
                 std::pow(c2.x(),2) + std::pow(c2.y(),2) );
      }
    };

  private:

    /// Type for list of radiator volumes
    typedef std::vector<const DeRichRadiator*> RichRadiators;

    /// Vector of DeRich radiator volumes for each Radiator type
    std::vector<RichRadiators> m_radiators;

    /// Aerogel full Tile IDs to exclude
    std::vector<int> m_excludedAeroTiles;

  };

}

#endif // RICHTOOLS_RICHRADIATORTOOL_H
