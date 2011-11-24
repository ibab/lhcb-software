// $Id: $
#ifndef RICHDET_DERICHPMT_H
#define RICHDET_DERICHPMT_H 1

// Include files
// DetDesc
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/IPVolume.h"

// LHCbKernel
#include "Kernel/ILHCbMagnetSvc.h"

// RichDet
#include "RichDet/DeRichPD.h"

// GSL
#include "gsl/gsl_math.h"

// STL
#include <cmath>

// External declarations
extern const CLID CLID_DERichPMT;

//=============================================================================
/** @class DeRichPMT DeRichPMT.h RichDet/DeRichPMT.h
 *
 *  PMT Photodetector
 *
 *  @author Sajan Easo
 *  @date   2011-10-10
 */
//=============================================================================

class DeRichPMT : public DeRichPD
{

public:

  /// Standard constructor
  DeRichPMT(const std::string & name = "" );

  virtual ~DeRichPMT( ); ///< Destructor

  /**
   * Retrieves reference to class identifier
   * @return the class identifier for this class
   */
  const CLID& clID() const { return classID(); }

  /**
   * Retrieves reference to class identifier
   * @return the class identifier for this class
   */
  static const CLID& classID();

  /**
   * This is where most of the geometry is read and variables initialised
   * @return Status of initialisation
   * @retval StatusCode::FAILURE Initialisation failed, program should
   * terminate
   */
  virtual StatusCode initialize();

  // @brief Converts a RichSmartID to a point in global coordinates.
  virtual StatusCode detectionPoint ( const LHCb::RichSmartID smartID,
                                      Gaudi::XYZPoint& detectPoint,
                                      bool photoCathodeSide = false ) const;

  // Converts an x,y point from the anode to the photocathode in the
  // coordinate system of the PMT.
  virtual StatusCode detectionPoint ( const double fracPixelCol,
                                      const double fracPixelRow,
                                      Gaudi::XYZPoint& detectPoint,
                                      const bool photoCathodeSide = true ) const;

  /** Converts a RichSmartID to a point on the anode in global coordinates.
   *  @param[in] smartID The RichSmartID for the PMT channel
   *  @return The detection point on the anode in global coordinates
   */
  Gaudi::XYZPoint detPointOnAnode ( const LHCb::RichSmartID smartID ) const;

private:

  StatusCode getPMTParameters();
  StatusCode initPMTQuantumEff();
  StatusCode updateGeometry();
  Gaudi::XYZPoint getAnodeHitCoordFromPixelNum( const double fracPixelCol,
                                                const double fracPixelRow ) const;


  /// Clean up interpolators
  void cleanUpInterps();

private:

  IDetectorElement* m_dePmtAnode; ///< The PMT Anode detector element
  int m_number;                    ///<Pmt number (should be the same as copy number)
  std::string  m_PmtQELocation;

  double m_PmtAnodeXSize;
  double m_PmtAnodeYSize;
  double m_PmtAnodeZSize;

  double m_PmtAnodeLocationInPmt;
  double m_PmtPixelXSize;
  double m_PmtPixelYSize;
  double m_PmtPixelGap;
  double m_PmtEffectivePixelXSize;
  double m_PmtEffectivePixelYSize;
  double m_PmtAnodeHalfThickness;
  double m_PmtNumPixCol;
  double m_PmtNumPixRow;
  double m_PmtQwZSize;
  double m_QwToAnodeZDist ;

};

#endif // RICHDET_DERICHPMT_H
