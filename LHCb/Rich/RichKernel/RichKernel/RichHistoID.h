
//-----------------------------------------------------------------------------
/** @file RichHistoID.h
 *
 *  Header file for RICH utility class : Rich::HistogramID
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   27/10/2005
 */
//-----------------------------------------------------------------------------

#ifndef RICHKERNEL_RICHHISTOID_H
#define RICHKERNEL_RICHHISTOID_H 1

// STL
#include <string>

// Kernel
#include "RichKernel/RichPackedPIDInfo.h"

// Gaudi
#include "GaudiAlg/HistoID.h"

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

  public:

    /** Constructor with RICH specific information
     *  @param id    Histogram string ID
     *  @param det   The RICH detector
     *  @param side  The side of the RICH detector
     *  @param rad   The radiator medium
     *  @param pid   The mass hypothesis
     */
    HistogramID( const std::string &         id,
                 const Rich::DetectorType   det = Rich::InvalidDetector,
                 const Rich::Side          side = Rich::InvalidSide,
                 const Rich::RadiatorType   rad = Rich::InvalidRadiator,
                 const Rich::ParticleIDType pid = Rich::Unknown )
      : m_id(id), m_data(det,side,rad,pid)
    { }

    /** Constructor with RICH specific information
     *  @param id    Histogram string ID
     *  @param det   The RICH detector
     *  @param side  The side of the RICH detector
     *  @param rad   The radiator medium
     *  @param pid   The mass hypothesis
     */
    HistogramID( const std::string &         id,
                 const Rich::Side          side,
                 const Rich::RadiatorType   rad = Rich::InvalidRadiator,
                 const Rich::ParticleIDType pid = Rich::Unknown )
      : m_id(id), m_data( ( rad == Rich::Rich2Gas ? Rich::Rich2 :
                            rad == Rich::Rich1Gas ? Rich::Rich1 :
                            rad == Rich::Aerogel  ? Rich::Rich1 :
                            Rich::InvalidDetector ),
                          side, rad, pid )
    { }
    
    /** Constructor with RICH specific information
     *  @param id    Histogram string ID
     *  @param rad   The radiator medium
     *  @param pid   The mass hypothesis
     */
    HistogramID( const std::string &         id,
                 const Rich::RadiatorType   rad,
                 const Rich::ParticleIDType pid = Rich::Unknown )
      : m_id(id), m_data(Rich::InvalidDetector,Rich::InvalidSide,rad,pid)
    { }

    /** Constructor with RICH specific information
     *  @param id    Histogram string ID
     *  @param pid   The mass hypothesis
     */
    HistogramID( const std::string &         id,
                 const Rich::ParticleIDType pid )
      : m_id(id), m_data( Rich::InvalidDetector,
                          Rich::InvalidSide,
                          Rich::InvalidRadiator,
                          pid )
    { }

  public:

    /// Access the Detector type
    inline Rich::DetectorType   det() const { return m_data.det(); }

    /// Access the radiator medium
    inline Rich::RadiatorType   rad() const { return m_data.rad(); }

    /// Access the side of the RICH
    inline Rich::Side          side() const { return m_data.side(); }

    /// Access the Particle ID hypothesis
    inline Rich::ParticleIDType pid() const { return m_data.pid(); }

  public:

    /// Returns the appropriate histogram ID
    inline const std::string& id() const
    {  
      return m_id;
    }

    /// Returns the packed Rich PID info
    inline const Rich::PackedPIDInfo& packedData() const
    {  
      return m_data;
    }

    /// Returns the appropriate histogram ID
    std::string fullid() const;

    /// Get the full title, including Detector Info
    std::string fullTitle( const std::string & title ) const;

  public:

    /// implicit conversion to an std::string
    inline operator std::string() const 
    { 
      return fullid(); 
    }

    /// Implicit conversion to a GaudiAlg::HistoID
    inline operator GaudiAlg::HistoID() const 
    {
      return GaudiAlg::HistoID(fullid()); 
    }

  private:

    std::string m_id;             ///< The ID string
    Rich::PackedPIDInfo m_data;   ///< Packed Rich Info

  };

  /// Shortcut for reduced typing where useful ...
  using HID = HistogramID;

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
