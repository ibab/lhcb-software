
//-----------------------------------------------------------------------------
/** @file RichHistoID.h
 *
 *  Header file for RICH utility class : Rich::HistogramID
 *
 *  CVS Log :-
 *  $Id: RichHistoID.h,v 1.7 2010-02-11 19:32:28 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   27/10/2005
 */
//-----------------------------------------------------------------------------

#ifndef RICHKERNEL_RICHHISTOID_H
#define RICHKERNEL_RICHHISTOID_H 1

// STL
#include <functional>
#include <string>

// Kernel
#include "Kernel/RichRadiatorType.h"
#include "Kernel/RichDetectorType.h"
#include "Kernel/RichParticleIDType.h"
#include "Kernel/RichSide.h"

namespace Rich
{

  //-----------------------------------------------------------------------------
  /** @class HistogramID RichKernel/RichHistoID.h
   *
   *  Utility class providing a convenient ID object for histograms, depending on
   *  various RICH criteria.
   *
   *  @code
   *
   *    // Create an ID object for Rich::RadiatorType rad
   *    Rich::HistogramID hid("shortName",rad);
   *
   *    // Can also be used directly with GaudiHistos methods i.e.
   *    const double ckPull = ....;
   *    const Rich::RadiatorType rad = ...;
   *    plot1D( ckPull, Rich::HistogramID("ckPull",rad), "(Rec-Exp)/Res Cktheta", -5, 5 );
   *
   *  @endcode
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   10/02/2010
   */
  //-----------------------------------------------------------------------------
  class HistogramID
  {

  private:

    /// Form the full ID
    inline void constructID()
    {
      if ( m_pid  != Rich::Unknown         ) { m_id = Rich::text(m_pid)  + "/" + m_id; }
      if ( m_side != Rich::InvalidSide     ) { m_id = Rich::text(m_side) + "/" + m_id; }
      if ( m_rad  != Rich::InvalidRadiator ) { m_id = Rich::text(m_rad)  + "/" + m_id; }
      if ( m_det  != Rich::InvalidDetector ) { m_id = Rich::text(m_det)  + "/" + m_id; }
    }

  public:

    /// Constructor from a string, with no detector information
    HistogramID( const std::string & id )
      : m_id   ( id                    ),
        m_det  ( Rich::InvalidDetector ),
        m_rad  ( Rich::InvalidRadiator ),
        m_side ( Rich::InvalidSide     ),
        m_pid  ( Rich::Unknown         )
    { }

    /** Constructor with RICH specific information
     *  @param id    Histogram string ID
     *  @param det   The RICH detector
     */
    HistogramID( const std::string & id,
                 Rich::DetectorType  det )
      : m_id   ( Rich::text(det) + "/" + id ),
        m_det  ( det                   ), 
        m_rad  ( Rich::InvalidRadiator ),
        m_side ( Rich::InvalidSide     ), 
        m_pid  ( Rich::Unknown         )
    { }

    /** Constructor with RICH specific information
     *  @param id    Histogram string ID
     *  @param rad   The radiator medium
     */
    HistogramID(  const std::string & id,
                  Rich::RadiatorType  rad )
      : m_id   ( Rich::text(rad) + "/" + id ),
        m_det  ( Rich::InvalidDetector ), 
        m_rad  ( rad                   ),
        m_side ( Rich::InvalidSide     ), 
        m_pid  ( Rich::Unknown         )
    { }

    /** Constructor with RICH specific information
     *  @param id    Histogram string ID
     *  @param det   The RICH detector
     *  @param side  The side of the RICH detector
     *  @param rad   The radiator medium
     *  @param pid   The mass hypothesis
     */
    HistogramID( const std::string &   id,
                 Rich::DetectorType   det,
                 Rich::Side          side,
                 Rich::RadiatorType   rad = Rich::InvalidRadiator,
                 Rich::ParticleIDType pid = Rich::Unknown )
      : m_id   ( id   ), 
        m_det  ( det  ), 
        m_rad  ( rad  ),
        m_side ( side ),
        m_pid  ( pid  )
    {
      constructID();
    }

    /** Constructor with RICH specific information
     *  @param id    Histogram string ID
     *  @param rad   The radiator medium
     *  @param pid   The mass hypothesis
     */
    HistogramID( const std::string &   id,
                 Rich::RadiatorType   rad,
                 Rich::ParticleIDType pid )
      : m_id   ( Rich::text(rad) + "/" + Rich::text(pid) + "/" + id ),
        m_det  ( Rich::InvalidDetector ), 
        m_rad  ( rad                   ),
        m_side ( Rich::InvalidSide     ), 
        m_pid  ( pid                   )
    { }

  public:

    /// Returns the appropriate histogram ID
    inline const std::string& id() const
    {
      return m_id;
    }

    /// implicit conversion to an std::string
    inline operator std::string() const
    {
      return this->id();
    }

    /// Implicit conversion to a GaudiAlg::HistoID
    inline operator GaudiAlg::HistoID() const
    {
      return GaudiAlg::HistoID(this->id());
    }

  public:

    /// Access the Detector type
    inline Rich::DetectorType   det() const { return m_det; }
    /// Access the radiator medium
    inline Rich::RadiatorType   rad() const { return m_rad; }
    /// Access the side of the RICH
    inline Rich::Side          side() const { return m_side; }
    /// Access the Particle ID hypothesis
    inline Rich::ParticleIDType pid() const { return m_pid; }

  private:

    std::string m_id;           ///< The ID string
    Rich::DetectorType m_det;   ///< The RICH detector
    Rich::RadiatorType m_rad;   ///< The radiator medium
    Rich::Side m_side;          ///< The detector 'side' (up,down,left,right)
    Rich::ParticleIDType m_pid; ///< The Particle hypothesis

  };

  /// Shortcut for reduced typing where useful ...
  typedef HistogramID HID;

  //-----------------------------------------------------------------------------
  /** @class HistoID RichKernel/RichHistoID.h
   *
   *  Utility class providing a convenient ID object for histograms, depending on
   *  various RICH criteria.
   *
   *  @code
   *
   *    // Create an ID object
   *    Rich::HistoID hid;
   *
   *    // Create the histo ID for a histogram in a given radiator
   *    // id will be "[rad]/shortName" where [rad] is either aerogel, rich1Gas or rich2Gas.
   *    std::string id = hid(rad,"shortName");
   *
   *    // Can also be used directly with GaudiHistos methods i.e.
   *    const double ckPull = ....;
   *    const Rich::RadiatorType rad = ...;
   *    plot1D( ckPull, hid(rad,"ckPull"), "(Rec-Exp)/Res Cktheta", -5, 5 );
   *
   *  @endcode
   *
   *  @attention Kept for backwards compatibility. Superceded by Rich::HistogramID
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   05/04/2002
   */
  //-----------------------------------------------------------------------------
  class HistoID
  {

  public:

    /// Standard constructor
    HistoID() { }

    /** Create histo ID for given radiator type and histogram tag
     *
     *  @param rad RICH radiator type
     *  @param tag Histogram tag
     *
     *  @return Full histogram identifier
     */
    inline Rich::HistogramID operator() ( const Rich::RadiatorType rad,
                                          const std::string & tag ) const
    {
      return Rich::HistogramID(tag,rad);
    }

    /** Create histo ID for given RICH detector and histogram tag
     *
     *  @param det RICH detector type
     *  @param tag Histogram tag
     *
     *  @return Full histogram identifier
     */
    inline Rich::HistogramID operator() ( const Rich::DetectorType det,
                                          const std::string & tag ) const
    {
      return Rich::HistogramID(tag,det);
    }

    /** Create histo ID for given RICH detector, HPD panel and histogram tag
     *
     *  @param det  RICH detector type
     *  @param side The HPD panel
     *  @param tag  Histogram tag
     *
     *  @return Full histogram identifier
     */
    inline Rich::HistogramID operator() ( const Rich::DetectorType det,
                                          const Rich::Side side,
                                          const std::string & tag ) const
    {
      return Rich::HistogramID(tag,det,side);
    }

    /** Create histo ID for given radiator type, mass hypothesis and histogram tag
     *
     *  @param rad RICH radiator type
     *  @param pid Particle mass hypothesis
     *  @param tag Histogram tag
     *
     *  @return Full histogram identifier
     */
    inline Rich::HistogramID operator() ( const Rich::RadiatorType rad,
                                          const Rich::ParticleIDType pid,
                                          const std::string & tag ) const
    {
      return Rich::HistogramID(tag,rad,pid);
    }

  };

} // RICH

#endif // RICHKERNEL_RICHHISTOID_H
