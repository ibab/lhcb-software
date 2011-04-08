
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

    // Initialization of the tool after creation
    virtual StatusCode initialize();

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

    /// UMS update
    StatusCode radiatorUpdate();

  private:

    typedef std::vector<const DeRichRadiator*> RichRadiators;
    /// Vector of DeRich radiator volumes for each Radiator type
    std::vector<RichRadiators> m_radiators;

  };

}

#endif // RICHTOOLS_RICHRADIATORTOOL_H
