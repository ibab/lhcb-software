#ifndef TRACKEXTRAPOLATORS_TRFIELDEXTRAPOLATORBASE_H 
#define TRACKEXTRAPOLATORS_TRFIELDEXTRAPOLATORBASE_H 


// Include files
// -------------
// from TrackExtrapolators
#include "TrackExtrapolator.h"

class ILHCbMagnetSvc ;
#include "DetDesc/MagneticFieldGrid.h"
#include "Kernel/ILHCbMagnetSvc.h"

/** @class TrackFieldExtrapolatorBase TrackFieldExtrapolatorBase.h TrackExtrapolators/TrackFieldExtrapolatorBase.h
 *
 *  A TrackFieldExtrapolatorBase is a TrackExtrapolator with access to the magnetic field
 *
 *  @author Wouter Hulsbergen
 *  @date   16/07/2009
 */

class TrackFieldExtrapolatorBase: public TrackExtrapolator
{

public:
  typedef Gaudi::XYZVector FieldVector ;
  typedef Gaudi::Matrix3x3 FieldGradient ;
  /// constructor
  TrackFieldExtrapolatorBase( const std::string& type, 
			      const std::string& name, 
			      const IInterface* parent );

  /// destructor
  ~TrackFieldExtrapolatorBase() override;

  /// initialize (picks up the field service)
  StatusCode initialize()  override;

  /// finalize
  StatusCode finalize()  override;

  /// access to the field
  FieldVector fieldVector( const Gaudi::XYZPoint& position ) const {
    ++m_numFieldCalls ;
    return (m_fieldGrid->*m_fieldFunction)( position ) ;
  }
  
  /// access to the field gradient
  FieldGradient fieldGradient( const Gaudi::XYZPoint& position ) const {
    return m_fieldGrid->fieldGradient( position ) ;
  }
  
private:
  const LHCb::MagneticFieldGrid* m_fieldGrid ; ///< Pointer to the magnetic field grid
  Gaudi::XYZVector (LHCb::MagneticFieldGrid::*m_fieldFunction)(const Gaudi::XYZPoint&)const ;
  mutable size_t m_numFieldCalls ;

  SmartIF<ILHCbMagnetSvc> m_fieldSvc ;           ///< Pointer to the magnetic field service
  std::string m_fieldSvcName;                  ///< Name of the field service
  bool m_useGridInterpolation ;                ///< Flag whether to interpolate on the grid or not
};

#endif // TRACKEXTRAPOLATORS_TRLINEAREXTRAPOLATOR_H
