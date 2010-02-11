
//-----------------------------------------------------------------------------
/** @file RichHistoBase.h
 *
 *  Header file for RICH base class : Rich::HistoBase
 *
 *  CVS Log :-
 *  $Id: RichHistoBase.h,v 1.3 2010-02-11 19:32:28 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2009-07-27
 */
//-----------------------------------------------------------------------------

#ifndef RICHKERNEL_RICHHISTOBASE_H
#define RICHKERNEL_RICHHISTOBASE_H 1

#include <vector>

#include "RichKernel/RichCommonBase.h"
#include "RichKernel/RichHistoID.h"
#include "RichKernel/RichHashMap.h"
#include "Kernel/RichDetectorType.h"
#include "Kernel/RichRadiatorType.h"

// boost
#include "boost/array.hpp"

// AIDA
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
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

    /// Get the full title, including Detector Info
    inline std::string fullTitle( const Rich::HistogramID & id,
                                  const std::string &    title ) const
    {
      std::string _title("");
      if ( id.det()  != Rich::InvalidDetector ) _title += Rich::text(id.det())  + " ";
      if ( id.rad()  != Rich::InvalidRadiator ) _title += Rich::text(id.rad())  + " ";
      if ( id.side() != Rich::InvalidSide     ) _title += Rich::text(id.side()) + " ";
      if ( id.pid()  != Rich::Unknown         ) _title += Rich::text(id.pid())  + " ";
      return _title + title;
    }

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
     * @param low     Lower histogram edge
     * @param high    Upper histogram edge
     * @param bins    Number of bins
     *
     * @return Pointer to booked histogram
     */
    AIDA::IHistogram1D* richHisto1D( const Rich::HistogramID & id,
                                     const std::string &    title,
                                     const double             low,
                                     const double            high,
                                     const unsigned long     bins );

    /** Book a 2D histogram
     *
     * @param id      Histogram identifier
     * @param title   Histogram title
     * @param lowX    Lower histogram edge in X
     * @param highX   Upper histogram edge in X
     * @param lowX    Lower histogram edge in Y
     * @param highX   Upper histogram edge in Y
     * @param binsX   Number of bins in X
     * @param binsX   Number of bins in Y
     *
     * @return Pointer to booked histogram
     */
    AIDA::IHistogram2D* richHisto2D( const Rich::HistogramID & id,
                                     const std::string &     title,
                                     const double             lowX,
                                     const double            highX,
                                     const unsigned long     binsX,
                                     const double             lowY,
                                     const double            highY,
                                     const unsigned long     binsY );

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
    AIDA::IProfile1D* richProfile1D( const Rich::HistogramID & id,
                                     const std::string &    title,
                                     const double             low,
                                     const double            high,
                                     const unsigned long     bins );

    //-----------------------------------------------------------------------------------------

  private: // Types for histogram lookup

    /// Mapping between IDs and 1D histogram pointers
    typedef Rich::HashMap< std::string, AIDA::IHistogram1D * > Map1DH;

    /// Mapping between IDs and 2D histogram pointers
    typedef Rich::HashMap< std::string, AIDA::IHistogram2D * > Map2DH;

    /// Mapping between IDs and 1D Profile histogram pointers
    typedef Rich::HashMap< std::string, AIDA::IProfile1D * > Map1DP;

  private:

    /// Flag to indicate if histograms have been booked or not
    bool m_histosAreBooked;

    /// 1D histo map
    Map1DH m_1dhmap;

    /// 2D histo map
    Map2DH m_2dhmap;

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
    inline AIDA::IHistogram1D * richHisto1D( const Rich::HistogramID & id )
    {
      Map1DH::iterator iH = m_1dhmap.find(id.id());
      if ( iH == m_1dhmap.end() )
      {
        this->Exception( "Cannot find pre-booked histogram '"+id.id()+"'" );
      }
      return iH->second;
    }

    /** Access 2D histogram by id
     *
     * @param id      Histogram identifier
     *
     * @return Pointer to booked histogram
     */
    inline AIDA::IHistogram2D * richHisto2D( const Rich::HistogramID & id )
    {
      Map2DH::iterator iH = m_2dhmap.find(id.id());
      if ( iH == m_2dhmap.end() )
      {
        this->Exception( "Cannot find pre-booked histogram '"+id.id()+"'" );
      }
      return iH->second;
    }

    /** Access 1D profile histogram by id
     *
     * @param id      Histogram identifier
     *
     * @return Pointer to booked histogram
     */
    inline AIDA::IProfile1D * richProfile1D( const Rich::HistogramID & id )
    {
      Map1DP::iterator iH = m_1dpmap.find(id.id());
      if ( iH == m_1dpmap.end() )
      {
        this->Exception( "Cannot find pre-booked histogram '"+id.id()+"'" );
      }
      return iH->second;
    }

    //-----------------------------------------------------------------------------------------

  };

}

#endif // RICHKERNEL_RICHHISTOBASE_H
