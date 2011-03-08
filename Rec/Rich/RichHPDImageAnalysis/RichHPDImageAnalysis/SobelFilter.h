
#ifndef RICHHPDIMAGEANALYSIS_SOBELFILTER_H
#define RICHHPDIMAGEANALYSIS_SOBELFILTER_H 1

#include "TH2D.h"

namespace Rich
{
  namespace HPDImage
  {

    /** @class SobelFilter SobelFilter.h RichHPDImageAnalysis/SobelFilter.h
     *
     *  Applies a Sobel filter to a 2D histogram
     *   
     *  @author Chris Jones
     *  @date   2011-03-07
     */
    class SobelFilter
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
        Params() 
          : centreRegionSize ( 7.5  )
        { }
      public:
        double centreRegionSize;
      };

    public:

      /// Standard constructor
      SobelFilter( const TH2D* inH,
                   const Params& params = Params())
        : m_inHist ( inH ),
          m_params ( params )
      { }

      /// Destructor
      ~SobelFilter( ) { }

    public:

      /// Access the inout histogram
      const TH2D* input() const { return m_inHist; }

      /// Run the filter
      TH2D* filter() const;

    private:

      /// Pointer to original histogram
      const TH2D* m_inHist;

      /// Parameters
      Params m_params;

    };

  }
}

#endif // RICHHPDIMAGEANALYSIS_SOBELFILTER_H
