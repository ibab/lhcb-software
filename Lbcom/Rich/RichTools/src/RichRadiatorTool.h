
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

// base class
#include "RichKernel/RichToolBase.h"

// RichKernel
#include "RichKernel/IRichRadiatorTool.h"
#include "RichKernel/BoostArray.h"

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

    typedef std::vector<const DeRichRadiator*> RichRadiators;
    boost::array<RichRadiators, Rich::NRadiatorTypes> m_radiators;

    typedef std::vector<Gaudi::Transform3D> Transforms;
    boost::array<Transforms, Rich::NRadiatorTypes> m_transforms;

  };

}

#endif // RICHTOOLS_RICHRADIATORTOOL_H
