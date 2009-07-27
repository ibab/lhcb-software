
//-----------------------------------------------------------------------------
/** @file RichHistoBase.h
 *
 *  Header file for RICH base class : Rich::HistoBase
 *
 *  CVS Log :-
 *  $Id: RichHistoBase.h,v 1.1 2009-07-27 20:29:01 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2009-07-27
 */
//-----------------------------------------------------------------------------

#ifndef RICHKERNEL_RICHHISTOBASE_H
#define RICHKERNEL_RICHHISTOBASE_H 1

#include <vector>

#include "RichKernel/RichCommonBase.h"
#include "RichKernel/RichHashMap.h"
#include "Kernel/RichDetectorType.h"
#include "Kernel/RichRadiatorType.h"

// boost
#include "boost/array.hpp"

// AIDA
#include "AIDA/IHistogram1D.h"
#include "AIDA/IProfile1D.h"

namespace Rich
{

  //-----------------------------------------------------------------------------
  /** @class HistoBase RichKernel/RichHistoBase.h
   *
   *  Common base class for all RICH histogramming tools and algorithms.
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   2009-07-27
   */
  //-----------------------------------------------------------------------------

  template <class PBASE>
  class HistoBase : public CommonBase<PBASE>
  {

  public:

    /// Standard algorithm-like constructor
    HistoBase( const std::string& name,
               ISvcLocator* pSvcLocator );

    /// Standard tool-like constructor
    HistoBase( const std::string& type,
               const std::string& name,
               const IInterface* parent );

    /// Standard Converter-like Constructor
    HistoBase( long storage_type,
               const CLID &class_type,
               ISvcLocator *svc = NULL );

    /// Destructor
    virtual ~HistoBase( );

  public:

    /// System initialize
    virtual StatusCode sysInitialize();

    // Initialization of the algorithm after creation
    virtual StatusCode initialize();

    // Finalization of the algorithm before deletion
    virtual StatusCode finalize();

  protected:

    /// Number of bins for 1D histograms
    inline unsigned int nBins1D() const { return m_nBins1D; }

    /// Number of bins for 2D histograms
    inline unsigned int nBins2D() const { return m_nBins2D; }

  private:

    unsigned int m_nBins1D; ///< Number of bins for 1D histograms
    unsigned int m_nBins2D; ///< Number of bins for 2D histograms

  protected:

    /** @brief Place to book all histograms which must be present after initialisation
     *
     *  Useful for online monitoring where booking on-demand does not play well with
     *  the online histogram systems
     *
     *  This method is automatically called during initialisation. It is not neccessary
     *  to call it by hand. Simply implement this method in any monitoring code which
     *  needs to pre-book histograms.
     *
     *  @return StatusCode indicating if booking was successful
     */
    virtual StatusCode prebookHistograms();

    //-----------------------------------------------------------------------------------------

    /** Book a 1D histogram
     *
     * @param id      Histogram identifier
     * @param title   Histogram title
     * @param low     Lower hisstogram edge
     * @param high    Upper histogram edge
     * @param bins    Number of bins
     *
     * @return Pointer to booked histogram
     */
    AIDA::IHistogram1D* richHisto1D( const std::string &       id,
                                     const std::string &    title,
                                     const double             low,
                                     const double            high,
                                     const unsigned long     bins );

    /** Book a 1D profile histogram
     *
     * @param id      Histogram identifier
     * @param title   Histogram title
     * @param low     Lower hisstogram edge
     * @param high    Upper histogram edge
     * @param bins    Number of bins
     *
     * @return Pointer to booked histogram
     */
    AIDA::IProfile1D* richProfile1D( const std::string &       id,
                                     const std::string &    title,
                                     const double             low,
                                     const double            high,
                                     const unsigned long     bins );

    //-----------------------------------------------------------------------------------------

    /** Book a 1D histogram for the given radiator
     *
     * @param rad     The radiator type
     * @param id      Histogram identifier
     * @param title   Histogram title
     * @param low     Lower hisstogram edge
     * @param high    Upper histogram edge
     * @param bins    Number of bins
     *
     * @return Pointer to booked histogram
     */
    AIDA::IHistogram1D* richHisto1D( const Rich::RadiatorType rad,
                                     const std::string &       id,
                                     const std::string &    title,
                                     const double             low,
                                     const double            high,
                                     const unsigned long     bins );

    /** Book a 1D profile histogram for the given radiator
     *
     * @param rad     The radiator type
     * @param id      Histogram identifier
     * @param title   Histogram title
     * @param low     Lower hisstogram edge
     * @param high    Upper histogram edge
     * @param bins    Number of bins
     *
     * @return Pointer to booked histogram
     */
    AIDA::IProfile1D* richProfile1D( const Rich::RadiatorType rad,
                                     const std::string &       id,
                                     const std::string &    title,
                                     const double             low,
                                     const double            high,
                                     const unsigned long     bins );

    //-----------------------------------------------------------------------------------------

    /** Book a 1D histogram for the given RICH
     *
     * @param rich    The RICH
     * @param id      Histogram identifier
     * @param title   Histogram title
     * @param low     Lower hisstogram edge
     * @param high    Upper histogram edge
     * @param bins    Number of bins
     *
     * @return Pointer to booked histogram
     */
    AIDA::IHistogram1D* richHisto1D( const Rich::DetectorType rich,
                                     const std::string &       id,
                                     const std::string &    title,
                                     const double             low,
                                     const double            high,
                                     const unsigned long     bins );

    /** Book a 1D profile histogram for the given RICH
     *
     * @param rich    The RICH
     * @param id      Histogram identifier
     * @param title   Histogram title
     * @param low     Lower hisstogram edge
     * @param high    Upper histogram edge
     * @param bins    Number of bins
     *
     * @return Pointer to booked histogram
     */
    AIDA::IProfile1D* richProfile1D( const Rich::DetectorType rich,
                                     const std::string &       id,
                                     const std::string &    title,
                                     const double             low,
                                     const double            high,
                                     const unsigned long     bins );

    //-----------------------------------------------------------------------------------------

  private: // Types for histogram lookup

    /// Mapping between IDs and 1D histogram pointers
    typedef Rich::HashMap< std::string, AIDA::IHistogram1D * > Map1DH;

    /// One 1D map per radiator
    typedef boost::array< Map1DH, Rich::NRadiatorTypes > RadMaps1DH;

    /// One 1D map per rich
    typedef boost::array< Map1DH, Rich::NRiches > RichMaps1DH;

    /// Mapping between IDs and 1D Profile histogram pointers
    typedef Rich::HashMap< std::string, AIDA::IProfile1D * > Map1DP;

    /// One 1D profile map per radiator
    typedef boost::array< Map1DP, Rich::NRadiatorTypes > RadMaps1DP;

    /// One 1D profile map per rich
    typedef boost::array< Map1DP, Rich::NRiches > RichMaps1DP;

  private:

    /// Flag to indicate if histograms have been booked or not
    bool m_histosAreBooked;

    /// 1D histos for each radiator
    RadMaps1DH m_rad1dhmaps;

    /// 1D profile histos for each radiator
    RadMaps1DP m_rad1dpmaps;

    /// 1D histos for each radiator
    RichMaps1DH m_rich1dhmaps;

    /// 1D profile histos for each radiator
    RichMaps1DP m_rich1dpmaps;

    /// 1D histo map
    Map1DH m_1dhmap;

    /// 1D Profile map
    Map1DP m_1dpmap;

  protected:

    //-----------------------------------------------------------------------------------------

    /** Access 1D histogram by id
     *
     * @param id      Histogram identifier
     *
     * @return Pointer to booked histogram
     */
    inline AIDA::IHistogram1D * richHisto1D( const std::string &       id )
    {
      Map1DH::iterator iH = m_1dhmap.find(id);
      if ( iH == m_1dhmap.end() )
      {
        this->Exception( "Cannot find pre-booked histogram '"+id+"'" );
      }
      return iH->second;
    }

    /** Access 1D profile histogram by id
     *
     * @param id      Histogram identifier
     *
     * @return Pointer to booked histogram
     */
    inline AIDA::IProfile1D * richProfile1D( const std::string &       id )
    {
      Map1DP::iterator iH = m_1dpmap.find(id);
      if ( iH == m_1dpmap.end() )
      {
        this->Exception( "Cannot find pre-booked histogram '"+id+"'" );
      }
      return iH->second;
    }

    //-----------------------------------------------------------------------------------------

    /** Access 1D histogram by radiator type and id
     *
     * @param rad     The radiator type
     * @param id      Histogram identifier
     *
     * @return Pointer to booked histogram
     */
    inline AIDA::IHistogram1D * richHisto1D( const Rich::RadiatorType rad,
                                             const std::string &       id )
    {
      Map1DH::iterator iH = (m_rad1dhmaps[rad]).find(id);
      if ( iH == (m_rad1dhmaps[rad]).end() )
      {
        this->Exception( "Cannot find pre-booked histogram '"+id+"' for "+Rich::text(rad) );
      }
      return iH->second;
    }

    /** Access 1D profile histogram by radiator type and id
     *
     * @param rad     The radiator type
     * @param id      Histogram identifier
     *
     * @return Pointer to booked histogram
     */
    inline AIDA::IProfile1D * richProfile1D( const Rich::RadiatorType rad,
                                             const std::string &       id )
    {
      Map1DP::iterator iH = (m_rad1dpmaps[rad]).find(id);
      if ( iH == (m_rad1dpmaps[rad]).end() )
      {
        this->Exception( "Cannot find pre-booked histogram '"+id+"' for "+Rich::text(rad) );
      }
      return iH->second;
    }

    //-----------------------------------------------------------------------------------------

    /** Access 1D histogram by RICH type and id
     *
     * @param rich    The RICH type
     * @param id      Histogram identifier
     *
     * @return Pointer to booked histogram
     */
    inline AIDA::IHistogram1D * richHisto1D( const Rich::DetectorType rich,
                                             const std::string &       id )
    {
      Map1DH::iterator iH = (m_rich1dhmaps[rich]).find(id);
      if ( iH == (m_rich1dhmaps[rich]).end() )
      {
        this->Exception( "Cannot find pre-booked histogram '"+id+"' for "+Rich::text(rich) );
      }
      return iH->second;
    }

    /** Access 1D profile histogram by RICH type and id
     *
     * @param rich    The RICH type
     * @param id      Histogram identifier
     *
     * @return Pointer to booked histogram
     */
    inline AIDA::IProfile1D * richProfile1D( const Rich::DetectorType rich,
                                             const std::string &       id )
    {
      Map1DP::iterator iH = (m_rich1dpmaps[rich]).find(id);
      if ( iH == (m_rich1dpmaps[rich]).end() )
      {
        this->Exception( "Cannot find pre-booked histogram '"+id+"' for "+Rich::text(rich) );
      }
      return iH->second;
    }

    //-----------------------------------------------------------------------------------------

  };

}

#endif // RICHKERNEL_RICHHISTOBASE_H
