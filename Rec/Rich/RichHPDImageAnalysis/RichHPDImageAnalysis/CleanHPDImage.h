
#ifndef RICHHPDIMAGEANALYSIS_CLEANHPDIMAGE_H
#define RICHHPDIMAGEANALYSIS_CLEANHPDIMAGE_H 1

#include <list>

#include "TH2D.h"

namespace Rich
{
  namespace HPDImage
  {

    /** @class Clean RichHPDImageAnalysis/CleanHPDImage.h
     *
     *  Applies some cleaning to HPD images.
     *  
     *  @author Chris Jones
     *  @date   2011-03-07
     */
    class Clean
    {

    public:

      /** @class Params RichHPDImageAnalysis/CleanHPDImage.h
       *
       *  Parameters for HPD Cleaner
       *  
       *  @author Chris Jones
       *  @date   2011-03-07
       */
      class Params
      {
      public:
        /// Default Constructor
        Params() 
          : hotBinFactor            ( 5.0  ), 
            centreRegionSize        ( 3.0  ),
            minBinContent           ( 1.0  ),
            giveRemovedPixAvCont    ( true ),
            neighbourFracForDeadPix ( 0.1  ),
            hotRowColFraction       ( 0.25 ),
            hotBiRowColFraction     ( 0.35 ),
            maxEventOcc             ( 0.5  )
        { }
      public:
        double hotBinFactor;
        double centreRegionSize;
        double minBinContent;
        bool giveRemovedPixAvCont;
        double neighbourFracForDeadPix;
        double hotRowColFraction;
        double hotBiRowColFraction;
        double maxEventOcc;
      };

    public:

      /// Standard constructor
      Clean( const TH2D* inH,
             const unsigned int nEvents = 0,
             const Params& params = Params() )
        : m_inHist  ( inH     ),
          m_params  ( params  ),
          m_nEvents ( nEvents )
      { 
        reset(); 
      }

      /// Destructor
      ~Clean( ) { }

    public:

      /// Access the inout histogram
      const TH2D* input() const { return m_inHist; }

      /// Run the filter
      TH2D* filter() const;

    private:
      
      /// Compute average bin content from neighbours
      double avFromNeighbours( const int COL,
                               const int ROW ) const;

      /// Add to list of excluded pixels
      void excludePixel( const int i, const int j ) const
      {
        m_excludedPixels.push_back( (i*100000)+j );
      }

      /// check if the given pixel is excluded
      bool isExcluded( const int i, const int j ) const
      {
        return std::find( m_excludedPixels.begin(),
                          m_excludedPixels.end(),
                          (i*100000)+j ) != m_excludedPixels.end();
      }

      /// Reset
      void reset() const 
      { 
        m_avPixCont = 0;
        m_excludedPixels.clear();
      }
   
    private:

      /// Pointer to original histogram
      const TH2D* m_inHist;

      /// Parameters
      Params m_params;

      /// Number of events in the histogram
      unsigned int m_nEvents;

    private: // working variables
      
      mutable double m_avPixCont; ///< Average pixel content

      mutable std::vector<unsigned int> m_excludedPixels;

    };

  }
}

#endif // RICHHPDIMAGEANALYSIS_CLEANHPDIMAGE_H
