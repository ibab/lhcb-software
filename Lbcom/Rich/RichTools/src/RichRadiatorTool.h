
//-----------------------------------------------------------------------------
/** @file RichRadiatorTool.h
 *
 *  Header file for tool : RichRadiatorTool
 *
 *  CVS Log :-
 *  $Id: RichRadiatorTool.h,v 1.2 2006-03-16 15:48:13 jonrob Exp $
 *
 *  @author Antonis Papanestis
 *  @date   2006-03-01
 */
//-----------------------------------------------------------------------------

#ifndef RICHTOOLS_RICHRADIATORTOOL_H
#define RICHTOOLS_RICHRADIATORTOOL_H 1

// base class
#include "RichKernel/RichToolBase.h"

#include "RichKernel/IRichRadiatorTool.h"            // Interface

#include "Kernel/RichRadiatorType.h"

#include "Kernel/Transform3DTypes.h"
#include "boost/array.hpp"

// from RichDet
#include "RichDet/DeRichRadiator.h"
#include "RichDet/RichRadIntersection.h"

/** @class RichRadiatorTool RichRadiatorTool.h
 *
 *
 *  @author Antonis Papanestis
 *  @date   2006-03-01
 */

class RichRadiatorTool : public RichToolBase, 
                         virtual public IRichRadiatorTool 
{

public:

  /// Standard constructor
  RichRadiatorTool( const std::string& type,
                    const std::string& name,
                    const IInterface* parent);

  virtual ~RichRadiatorTool( ); ///< Destructor

  // Initialization of the tool after creation
  virtual StatusCode initialize();

  /**
   * Finds the intersections (entry/exit) with radiator. For multiple solids
   * there can be more than one intersections
   *
   * @return The number of intersections.
   * @retval Zero if there is no intersction.
   */
  virtual unsigned int intersections( const Gaudi::XYZPoint& globalPoint,
                                      const Gaudi::XYZVector& globalVector,
                                      const Rich::RadiatorType radiator,
                                      std::vector<RichRadIntersection>&
                                      intersections ) const;

private:

  typedef std::vector<const DeRichRadiator*> RichRadiators;
  boost::array<RichRadiators, Rich::NRadiatorTypes> m_radiators;

  typedef std::vector<Gaudi::Transform3D> Transforms;
  boost::array<Transforms, Rich::NRadiatorTypes> m_transforms;

};

#endif // RICHTOOLS_RICHRADIATORTOOL_H
