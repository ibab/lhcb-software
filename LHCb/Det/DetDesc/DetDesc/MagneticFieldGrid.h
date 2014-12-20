// $Id: MagneticFieldGrid.h,v 1.1 2009-09-01 14:50:30 wouter Exp $
#ifndef MAGFIELDGRID_H 
#define MAGFIELDGRID_H

#include <vector>
#include <array>
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/GenericMatrixTypes.h"

/** @class MagneticFieldGrid MagneticFieldGrid.h
 *  Simple class to hold a magnetic field grid. 
 *
 *  @author Marco Cattaneo, Wouter Hulsbergen
 *  @date   2008-07-26
 */

class MagneticFieldGridReader ;

namespace LHCb 
{
  class MagneticFieldGrid
  {
  public: 
    /// typedefs etc
    typedef Gaudi::XYZVector FieldVector ;
    typedef Gaudi::Matrix3x3 FieldGradient ;

    /// declare the field reader as friend so that we don't have to make a constructor
    friend class ::MagneticFieldGridReader;
    
    /// Standard constructor
    MagneticFieldGrid() ;
    
    /// Return the field vector fvec at the point xyz by interpolation
    /// on the grid.
    FieldVector fieldVector( const Gaudi::XYZPoint&  xyz ) const;
    
    /// Return the field vector fvec at the point xyz by interpolation
    /// on the grid.
    FieldGradient fieldGradient( const Gaudi::XYZPoint&  xyz ) const;
    
    /// Return the field vector fvec at the point xyz by choosing the
    /// closest point on the grid.
    FieldVector fieldVectorClosestPoint( const Gaudi::XYZPoint& xyz ) const ;
    
    /// Return the magnetic field scale factor
    double scaleFactor() const { return m_scaleFactor ; }

    /// Update the scale factor
    void setScaleFactor( double s ) {  m_scaleFactor = s ; }

  private:
    double m_scaleFactor ;
    using vec = ROOT::Math::DisplacementVector3D<ROOT::Math::Cartesian3D<float>>;
    std::vector<vec> m_Q; ///< Field map
    std::array<double,3> m_min_FL;            ///< Offset in x, y and z 
    std::array<double,3> m_Dxyz;              ///< Steps in x, y and z
    std::array<unsigned,3> m_Nxyz;            ///< Number of steps in x, y and z
  };


}

#endif // MAGFIELDGRID_H
