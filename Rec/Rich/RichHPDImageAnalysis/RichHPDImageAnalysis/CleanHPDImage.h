
#ifndef RICHHPDIMAGEANALYSIS_CLEANHPDIMAGE_H
#define RICHHPDIMAGEANALYSIS_CLEANHPDIMAGE_H 1

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
        Params() 
          : hotBinFractor           ( 5.0  ), 
            centreRegionSize        ( 3.0  ),
            minBinContent           ( 1.0  ),
            giveRemovedPixAvCont    ( true ),
            neighbourFracForDeadPix ( 0.1  ),
            hotRowColFraction       ( 0.5  )
        { }
      public:
        double hotBinFractor;
        double centreRegionSize;
        double minBinContent;
        bool giveRemovedPixAvCont;
        double neighbourFracForDeadPix;
        double hotRowColFraction;
      };

    public:

      /// Standard constructor
      Clean( const TH2D* inH,
             const Params& params = Params() )
        : m_inHist ( inH    ),
          m_params ( params )
      { }

      /// Destructor
      ~Clean( ) { }

    public:

      /// Access the inout histogram
      const TH2D* input() const { return m_inHist; }

      /// Run the filter
      TH2D* filter() const;

    private:

      typedef std::vector<int> Ints;

    private:
      
      /// Compute average bin content from neighbours
      double avFromNeighbours(  const int COL,
                                const int ROW ) const;

    private:

      /// Pointer to original histogram
      const TH2D* m_inHist;

      /// Parameters
      Params m_params;

    };

  }
}

#endif // RICHHPDIMAGEANALYSIS_CLEANHPDIMAGE_H
