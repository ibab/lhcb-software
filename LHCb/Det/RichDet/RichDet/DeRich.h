/** @file DeRich.h
 *
 *  Header file for detector description class : DeRich
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */

#ifndef RICHDET_DERICH_H
#define RICHDET_DERICH_H 1

// STL
#include <vector>
#include <array>
#include <memory>

// DetDesc
#include "RichDet/Rich1DTabProperty.h"

// Mathcore
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Plane3DTypes.h"

// LHCbKernel
#include "Kernel/RichDetectorType.h"
#include "Kernel/RichSide.h"

// Local
#include "RichDet/DeRichBase.h"
#include "RichDet/RichMirrorSegPosition.h"
#include "RichDet/DeRichLocations.h"
#include "RichDet/RichDetConfigType.h"

class DeRichPDPanel;

/** @class DeRich DeRich.h
 *
 * Class for generic info about the Rich(1/2) system.
 * DeRich is an abstract class contaning the common parts of the DeRich1
 * and DeRich2 classes.
 *
 * @author Antonis Papanestis a.papanestis@rl.ac.uk
 */
class DeRich: public DeRichBase
{

public:

  /**
   * Constructor for this class
   */
  DeRich( const std::string & name = "" );

  /**
   * Default destructor
   */
  virtual ~DeRich();

  /**
   * This is where most of the geometry is read and variables initialised
   * @return Status of initialisation
   * @retval StatusCode::FAILURE Initialisation failed, program should
   * terminate
   */
  virtual StatusCode initialize();

  /**
   * Returns the nominal centre of curvature of the spherical mirror for
   * this Rich
   *
   * @param side Which side: top, bottom (Rich1), left, right (Rich2)
   * @return The nominal centre of curvature
   */
  virtual const Gaudi::XYZPoint& nominalCentreOfCurvature(const Rich::Side side) const = 0;

  /**
   * Returns the nominal normal vector of the flat mirror plane for this Rich
   *
   * @param side Which side: top, bottom (Rich1), left, right (Rich2)
   * @return The nominal normal vector
   */
  virtual const Gaudi::XYZVector& nominalNormal(const Rich::Side side) const = 0;

  /**
   * Returns the nominal flat mirror plane for this Rich
   *
   * @param side Which side: top, bottom (Rich1), left, right (Rich2)
   * @return The nominal flat mirror plane
   */
  virtual const Gaudi::Plane3D& nominalPlane(const Rich::Side side) const = 0;

  /**
   * Check on which side of this Rich lies this point
   *
   * @param point A point in the global coordinate system
   * @return The side for this point
   */
  virtual Rich::Side side( const Gaudi::XYZPoint& point ) const = 0;

  /**
   * Returns the detector type for this Rich
   *
   * @return The detector type
   */
  inline Rich::DetectorType rich() const noexcept
  {
    return m_rich;
  }

  /**
   * Returns the RichSystem
   *
   * @return The DeRichSystem object
   */
  inline DeRichSystem* deRichSystem() const noexcept
  {
    return deRichSys();
  }

  /**
   * Returns the nominal spherical mirror radius for this Rich
   *
   * @return The nominal spherical mirror radius
   */
  inline double sphMirrorRadius() const noexcept
  {
    return m_sphMirrorRadius;
  }

  /// Returns the Photon Detector config type 0=hpd, 1=pmt
  inline Rich::RichPhDetConfigType RichPhotoDetConfig() const noexcept
  {
    return m_RichPhotoDetConfig;
  }

  /** Returns the Rich Geometry config type 0=current optics, 1 upgrade optics, 2 horizontal rich1-upgrade optics.
   *  CRJ - This should be an enum, not an int ....
   *        Should also be cleaned up now the horizontal RICH1 is no longer an option
   */
  inline int RichGeometryConfig() const noexcept
  {
    return m_RichGeometryConfig;
  }

  // returns phodetector array config 0=standardpmt, 1=grandpmt, 2= mixture of stand+grandpmt
  inline int Rich2PhotoDetectorArrayConfig() const noexcept
  {
    return m_Rich2PhotoDetectorArrayConfig;
  }

  /// Use large PMTs
  inline bool Rich2UseGrandPmt () const noexcept
  {
    return m_Rich2UseGrandPmt;
  }

  /// Use large+small  PMTs
  inline bool Rich2UseMixedPmt () const noexcept
  {
    return m_Rich2UseMixedPmt;
  }

  /**
   * Returns a pointer to the tabulated property that holds the refractive
   * index for the gas window for this Rich
   *
   * @return Pointer to gas window refIndex
   */
  inline const Rich::TabulatedProperty1D* gasWinRefIndex() const noexcept
  {
    return m_gasWinRefIndex.get();
  }

  /**
   * Returns a pointer to the tabulated property that holds the absorption
   * length of the gas window for this Rich
   *
   * @return Pointer gas window absorption length
   */
  inline const Rich::TabulatedProperty1D* gasWinAbsLength() const noexcept
  {
    return m_gasWinAbsLength.get();
  }

  /**
   * Returns a pointer to the tabulated property that holds the nominal quantum
   * efficiency of an Photodetector
   *
   * @return Pointer to quantum efficiency (can be null)
   */
  inline const std::shared_ptr<const Rich::TabulatedProperty1D>&
  nominalPDQuantumEff() const noexcept
  {
    if ( !m_nominalPDQuantumEff ) { loadNominalQuantumEff(); }
    return m_nominalPDQuantumEff;
  }

  /**
   * Returns a pointer to the tabulated property that holds the nominal reflectivity
   * of the spherical mirror.
   *
   * @return Pointer to nominal spherical mirror reflectivity
   */
  inline const Rich::TabulatedProperty1D* nominalSphMirrorRefl() const noexcept
  {
    return m_nominalSphMirrorRefl.get();
  }

  /**
   * Returns a pointer to the tabulated property that holds the nominal reflectivity
   * of the flat mirror.
   *
   * @return Pointer to nominal flat mirror reflectivity
   */
  inline const Rich::TabulatedProperty1D* nominalSecMirrorRefl() const noexcept
  {
    return m_nominalSecMirrorRefl.get();
  }

  /**
   * Method to find the row/column of a spherical mirror segment.  It can
   * be used to test if the mirror segment is at the edge or not
   * @return Position (row/column) for this spherical mirror segment
   */
  virtual Rich::MirrorSegPosition sphMirrorSegPos( const int mirrorNumber ) const;

  /**
   * Method to find the row/column of a flat mirror segment. It can be used to
   * test if the mirror segment is at the edge or not
   *
   * @return Position (row/column) for this flat mirror segment
   */
  virtual Rich::MirrorSegPosition secMirrorSegPos( const int mirrorNumber ) const;

  /// sensitive volume identifier for hpd version. to be phased out
  virtual int sensitiveVolumeID(const Gaudi::XYZPoint& globalPoint) const;

public:

  /// Access PD Panels on demand
  DeRichPDPanel * pdPanel( const Rich::Side panel ) const;

private:

  /// Access the name for a given panel
  virtual const std::string panelName( const Rich::Side panel ) const;

  /// Load on demand the nominal HPD Q.E.
  const Rich::TabulatedProperty1D * loadNominalHPDQuantumEff() const;

  /// Load on demand the nominal PMT Q.E.
  const Rich::TabulatedProperty1D * loadNominalPMTQuantumEff() const;

  /// Load on demand the nominal PD Q.E.
  void loadNominalQuantumEff() const;

protected:

  /// rich type
  Rich::DetectorType m_rich = Rich::InvalidDetector;

  /// The nominal radius of the spherical mirror
  double m_sphMirrorRadius{0};

  /// RICH PhotoDetector Configuration
  Rich::RichPhDetConfigType m_RichPhotoDetConfig = Rich::HPDConfig;

  /// Rich Geometry Configuration
  int m_RichGeometryConfig{0};

  /** RICH2 PhotoDetector Configuration
   * This is specific to RICH2, but is part of the overall RICH geometry config.
   */
  int m_Rich2PhotoDetectorArrayConfig{0};

  /** Use large PMTs in RICH2
   * This is specific to RICH2, but the flags are in RICH1 part of DB since
   * all the pmts are created there together. So at the moment giving the capability to access it from
   * RICH1 or RICH2 detector elements as per convenience and saving cpu time. There is also the possibility of
   * different array config in RICH1 in future. So keeping all the flags here makes it easier to
   * navigate through them.
   * The mixed pmt means some pmts are large and others are with standard size.
   */
  bool m_Rich2UseGrandPmt{false};

  /// Use mixed PMTs
  bool m_Rich2UseMixedPmt{false};

  /// refractive index of the quartz gas window
  std::unique_ptr<const Rich::TabulatedProperty1D> m_gasWinRefIndex;

  /// absorption length of the quartz gas window
  std::unique_ptr<const Rich::TabulatedProperty1D> m_gasWinAbsLength;

  /// spherical mirror reflectivity
  std::unique_ptr<const Rich::TabulatedProperty1D> m_nominalSphMirrorRefl;

  /// flat mirror reflectivity
  std::unique_ptr<const Rich::TabulatedProperty1D> m_nominalSecMirrorRefl;

private: // data

  /// Pointers to the PD panels of this Rich detector
  mutable std::array<DeRichPDPanel*,Rich::NRiches> m_PDPanels{{nullptr,nullptr}};

  /// flag to test if the xml supports mirror position info
  bool m_positionInfo{false};

  int m_sphMirrorSegRows{0};  ///< number of spherical mirror rows
  int m_sphMirrorSegCols{0};  ///< number of spherical mirror columns
  int m_secMirrorSegRows{0};  ///< number of secondary mirror rows
  int m_secMirrorSegCols{0};  ///< number of secondary mirror columns

  /// PD quantum efficiency
  mutable std::shared_ptr<const Rich::TabulatedProperty1D> m_nominalPDQuantumEff;

};

#endif    // RICHDET_DERICH_H
