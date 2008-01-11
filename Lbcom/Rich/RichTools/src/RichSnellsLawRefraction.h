
//-----------------------------------------------------------------------------
/** @file RichSnellsLawRefraction.h
 *
 *  Header file for tool : Rich::SnellsLawRefraction
 *
 *  CVS Log :-
 *  $Id: RichSnellsLawRefraction.h,v 1.1 2008-01-11 12:04:32 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   03/01/2008
 */
//-----------------------------------------------------------------------------

#ifndef RICHTOOLS_RichSnellsLawRefraction_H
#define RICHTOOLS_RichSnellsLawRefraction_H 1

// Base class
#include "RichKernel/RichToolBase.h"

// interfaces
#include "RichKernel/IRichSnellsLawRefraction.h"
#include "RichKernel/IRichRefractiveIndex.h"
#include "RichKernel/IRichRadiatorTool.h"

// Math
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Plane3DTypes.h"

namespace Rich
{

  //-----------------------------------------------------------------------------
  /** @class SnellsLawRefraction RichSnellsLawRefraction.h
   *
   *  A tool which implements snells law refraction at Aerogel / Rich1Gas
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   03/01/2008
   */
  //-----------------------------------------------------------------------------

  class SnellsLawRefraction : public Rich::ToolBase,
                              virtual public Rich::ISnellsLawRefraction
  {

  public: // Methods for Gaudi Framework

    /// Standard constructor
    SnellsLawRefraction( const std::string& type,
                         const std::string& name,
                         const IInterface* parent );

    virtual ~SnellsLawRefraction( ); ///< Destructor

    // Initialization of the tool after creation
    virtual StatusCode initialize();

  public: // methods (and doxygen comments) inherited from interface

    // Correct the direction vector from refraction in going from Aerogel to Rich1Gas
    void aerogelToGas( Gaudi::XYZPoint& startPoint,
                       Gaudi::XYZVector &     dir,
                       const double photonEnergy ) const;

    // Correct the direction vector from refraction in going from Rich1Gas to aerogel
    void gasToAerogel( Gaudi::XYZVector &     dir,
                       const double photonEnergy ) const;

  private:

    /// Refractive index tool
    const IRefractiveIndex * m_refIndex;

    /// aerogel exit plane (for refraction correction)
    Gaudi::Plane3D m_aeroExitPlane;

    /// Vector normal to aerogel exit plane (for refraction correction)
    Gaudi::XYZVector m_aeroNormVect;

    /// z point for plane
    double m_minZaero;

  };

}

#endif // RICHTOOLS_RichSnellsLawRefraction_H
