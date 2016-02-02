
//-----------------------------------------------------------------------------
/** @file RichHistoBase.h
 *
 *  Header file for RICH base class : Rich::HistoBase
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2009-07-27
 */
//-----------------------------------------------------------------------------

#ifndef RICHKERNEL_RICHHISTOBASE_H
#define RICHKERNEL_RICHHISTOBASE_H 1

#include <vector>
#include <string>

// Local
#include "RichKernel/RichCommonBase.h"
#include "RichKernel/RichHistoID.h"
#include "RichKernel/RichMap.h"
#include "RichKernel/RichHashMap.h"

// boost
#include "boost/array.hpp"

// AIDA
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
//#include "AIDA/IHistogram3D.h"
#include "AIDA/IProfile1D.h"
#include "AIDA/IProfile2D.h"

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
               ISvcLocator *svc = nullptr );

    /// Destructor
    virtual ~HistoBase( );

  private:

    /// Histogram Constructor initisalisations
    void initRichHistoConstructor();

    /// Tuple Constructor initisalisations
    void initRichTupleConstructor();

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

  protected:

    /// short name for bin labels
    using BinLabels = std::vector<std::string>;

  protected:

    /** Book a 1D histogram
     *
     * @param id         Histogram identifier
     * @param title      Histogram title
     * @param low        Lower histogram edge
     * @param high       Upper histogram edge
     * @param bins       Number of bins
     * @param xAxisLabel Label for the X Axis
     * @param yAxisLabel Label for the Y Axis
     * @param binLabels  Labels for the X axis bins
     *
     * @return Pointer to booked histogram
     */
    AIDA::IHistogram1D* richHisto1D( const Rich::HistogramID &  id,
                                     const std::string &     title,
                                     const double              low,
                                     const double             high,
                                     const unsigned int       bins,
                                     const std::string&      xAxisLabel = "",
                                     const std::string&      yAxisLabel = "",
                                     const BinLabels& binLabels = BinLabels() ) const;

    /** Book a 2D histogram
     *
     * @param id         Histogram identifier
     * @param title      Histogram title
     * @param lowX       Lower histogram edge in X
     * @param highX      Upper histogram edge in X
     * @param binsX      Number of bins in X
     * @param lowY       Lower histogram edge in Y
     * @param highY      Upper histogram edge in Y
     * @param binsY      Number of bins in Y
     * @param xAxisLabel Label for the X Axis
     * @param yAxisLabel Label for the Y Axis
     * @param zAxisLabel Label for the Z Axis
     * @param xBinLabels Labels for the X axis bins
     * @param yBinLabels Labels for the Y axis bins
     *
     * @return Pointer to booked histogram
     */
    AIDA::IHistogram2D* richHisto2D( const Rich::HistogramID &  id,
                                     const std::string &     title,
                                     const double             lowX,
                                     const double            highX,
                                     const unsigned int      binsX,
                                     const double             lowY,
                                     const double            highY,
                                     const unsigned int      binsY,
                                     const std::string&      xAxisLabel = "",
                                     const std::string&      yAxisLabel = "",
                                     const std::string&      zAxisLabel = "",
                                     const BinLabels& xBinLabels = BinLabels(),
                                     const BinLabels& yBinLabels = BinLabels() ) const;

    /** Book a 1D profile histogram
     *
     * @param id         Histogram identifier
     * @param title      Histogram title
     * @param low        Lower hisstogram edge
     * @param high       Upper histogram edge
     * @param bins       Number of bins
     * @param xAxisLabel Label for the X Axis
     * @param yAxisLabel Label for the Y Axis
     * €param binLabels  Labels for the X axis bins
     *
     * @return Pointer to booked histogram
     */
    AIDA::IProfile1D* richProfile1D( const Rich::HistogramID & id,
                                     const std::string &    title,
                                     const double             low,
                                     const double            high,
                                     const unsigned int      bins,
                                     const std::string&      xAxisLabel = "",
                                     const std::string&      yAxisLabel = "",
                                     const BinLabels& binLabels = BinLabels() ) const;

    /** Book a 2D profile histogram
     *
     * @param id         Histogram identifier
     * @param title      Histogram title
     * @param lowX       Lower histogram edge in X
     * @param highX      Upper histogram edge in X
     * @param binsX      Number of bins in X
     * @param lowY       Lower histogram edge in Y
     * @param highY      Upper histogram edge in Y
     * @param binsY      Number of bins in Y
     * @param xAxisLabel Label for the X Axis
     * @param yAxisLabel Label for the Y Axis
     * @param zAxisLabel Label for the Z Axis
     * @param xBinLabels Labels for the X axis bins
     * @param yBinLabels Labels for the Y axis bins
     *
     * @return Pointer to booked histogram
     */
    AIDA::IProfile2D* richProfile2D( const Rich::HistogramID &  id,
                                     const std::string &     title,
                                     const double             lowX,
                                     const double            highX,
                                     const unsigned int      binsX,
                                     const double             lowY,
                                     const double            highY,
                                     const unsigned int      binsY,
                                     const std::string&      xAxisLabel = "",
                                     const std::string&      yAxisLabel = "",
                                     const std::string&      zAxisLabel = "",
                                     const BinLabels& xBinLabels = BinLabels(),
                                     const BinLabels& yBinLabels = BinLabels() ) const;

    //-----------------------------------------------------------------------------------------

  private: // Types for histogram lookup

    /** @class HistoMap RichKernel/RichHistoBase.h
     *
     *  Private class to implement mapping between RICH classes and histograms
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   2009-07-27
     */
    template< class HTYPE >
    class HistoMap
    {
    public:
      using StringToHist = Rich::HashMap < std::string, HTYPE * >;
    private:
      using Map = Rich::Map < Rich::PackedPIDInfo::Pack32_t, StringToHist >;
    private:
      Map map;
    public:
      inline StringToHist & getmap( const Rich::PackedPIDInfo& info ) 
      { 
        return map[info.raw()];
      }
    };

    /// Mapping between IDs and 1D histogram pointers
    using Map1DH = HistoMap< AIDA::IHistogram1D >;

    /// Mapping between IDs and 2D histogram pointers
    using Map2DH = HistoMap< AIDA::IHistogram2D >;

    /// Mapping between IDs and 1D Profile histogram pointers
    using Map1DP = HistoMap< AIDA::IProfile1D >;

    /// Mapping between IDs and 2D Profile histogram pointers
    using Map2DP = HistoMap< AIDA::IProfile2D >;

  private:

    /// Flag to indicate if histograms have been booked or not
    mutable bool m_histosAreBooked{false};

    /// 1D histo map
    mutable Map1DH m_1dhmap;

    /// 2D histo map
    mutable Map2DH m_2dhmap;

    /// 1D Profile map
    mutable Map1DP m_1dpmap;

    /// 2D Profile map
    mutable Map2DP m_2dpmap;

  protected:

    //-----------------------------------------------------------------------------------------

    /** Access 1D histogram by id
     *
     * @param id      Histogram identifier
     *
     * @return Pointer to booked histogram
     */
    inline AIDA::IHistogram1D * richHisto1D( const Rich::HistogramID & id ) const
    {
      const auto & map = m_1dhmap.getmap(id.packedData());
      const auto    iH = map.find(id.id());
      if ( iH == map.end() )
      {
        this->Exception( "Cannot find pre-booked histogram '"+id.fullid()+"'" );
      }
      return iH->second;
    }

    /** Access 2D histogram by id
     *
     * @param id      Histogram identifier
     *
     * @return Pointer to booked histogram
     */
    inline AIDA::IHistogram2D * richHisto2D( const Rich::HistogramID & id ) const
    {
      const auto & map = m_2dhmap.getmap(id.packedData());
      const auto    iH = map.find(id.id());
      if ( iH == map.end() )
      {
        this->Exception( "Cannot find pre-booked histogram '"+id.fullid()+"'" );
      }
      return iH->second;
    }

    /** Access 1D profile histogram by id
     *
     * @param id      Histogram identifier
     *
     * @return Pointer to booked histogram
     */
    inline AIDA::IProfile1D * richProfile1D( const Rich::HistogramID & id ) const
    {
      const auto & map = m_1dpmap.getmap(id.packedData());
      const auto    iH = map.find(id.id());
      if ( iH == map.end() )
      {
        this->Exception( "Cannot find pre-booked histogram '"+id.fullid()+"'" );
      }
      return iH->second;
    }

    /** Access 2D profile histogram by id
     *
     * @param id      Histogram identifier
     *
     * @return Pointer to booked histogram
     */
    inline AIDA::IProfile2D * richProfile2D( const Rich::HistogramID & id ) const
    {
      const auto & map = m_2dpmap.getmap(id.packedData());
      const auto    iH = map.find(id.id());
      if ( iH == map.end() )
      {
        this->Exception( "Cannot find pre-booked histogram '"+id.fullid()+"'" );
      }
      return iH->second;
    }

    //-----------------------------------------------------------------------------------------

  };

}

#endif // RICHKERNEL_RICHHISTOBASE_H
