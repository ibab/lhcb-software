//=============================================================================
/** @file DeRichPD.h
 *
 *  Header file for detector description class : DeRichPD
 *
 *  @author Chris Jones   christopher.rob.jones@cern.ch
 *  @date   2011-09-23
 */
//=============================================================================

#ifndef RICHDET_DERICHPD_H
#define RICHDET_DERICHPD_H 1

// STL
#include <memory>

// LHCbKernel
#include "Kernel/RichSmartID.h"

// RichDet
#include "RichDet/DeRichBase.h"
#include "RichDet/Rich1DTabFunc.h"
#include "RichDet/Rich1DTabProperty.h"

//=============================================================================
/** @class DeRichPD DeRichPD.h RichDet/DeRichPD.h
 *
 *  Base class for all photon detectors (HPDs, MaPMTs).
 *  Used by Brunel, Boole etc., so they are neutral to the PD technology.
 *
 *  @author Chris Jones  christopher.rob.jones@cern.ch
 *  @date   2011-09-23
 */
//=============================================================================
class DeRichPD : public DeRichBase
{

public:

  /// Standard constructor
  explicit DeRichPD( const std::string & name = "" );

  /// Destructor
  virtual ~DeRichPD( );

public:

  /** Retrieves the HPD Quantum Efficiency interpolation function.
   * For a given Photon Momentum in eV
   * returns the HPD Quantum Efficiency percentage.
   * @return pointer to the interpolation function for QuantumEff(PhotMom)
   */
  inline const Rich::TabulatedProperty1D* pdQuantumEff() const
  {
    return m_pdQuantumEffFunc.get();
  }

public: // virtual methods to be implemented by derived classes

  /** @brief Converts a RichSmartID to a point in global coordinates.
   *
   *  The point can be given either on the inside of the PD window (photocathode) if
   *  photoCathodeSide=true or on the outside including refraction correction if
   *  photoCathodeSide=false
   *
   *  @param[in]  smartID     The RichSmartID pixel channel ID
   *  @param[out] detectPoint The position in global coordinates
   *  @param[in]  photoCathodeSide Set to false to include refraction on HPD window
   *  @return StatusCode indicating if the conversion was successful or not
   *  @retval StatusCoe::SUCCESS Conversion was successful
   *  @retval StatusCode::FAILURE Conversion failed
   */
  virtual StatusCode detectionPoint ( const LHCb::RichSmartID smartID,
                                      Gaudi::XYZPoint& detectPoint,
                                      bool photoCathodeSide = false ) const = 0;

  /** @brief Converts an x,y point from the anode to the photocathode in the
   *         coordinate system of the HPD.
   *
   *  The point can be given either on the inside of the HPD window (photocathode) if
   *  photoCathodeSide=true or on the outside including refraction correction if
   *  photoCathodeSide=false
   *
   *  @param[in]  anodePoint The <x,y> pair on the anode in pixel coordinates
   *  @param[out] detectPoint The position in HPD coordinates
   *  @param[in]  photoCathodeSide Set to false to include refraction on HPD window
   *  @return StatusCode indicating if the conversion was successful or not
   *  @retval StatusCoe::SUCCESS Conversion was successful
   *  @retval StatusCode::FAILURE Conversion failed
   */
  virtual StatusCode detectionPoint ( const double fracPixelCol,
                                      const double fracPixelRow,
                                      Gaudi::XYZPoint& detectPoint,
                                      const bool photoCathodeSide = true ) const = 0;

protected: // to be initialised by derived classes
  
  ///< Interpolated property for HPD quantum efficiency
  mutable std::shared_ptr<const Rich::TabulatedProperty1D> m_pdQuantumEffFunc;
  
};

#endif // RICHDET_DERICHPD_H
