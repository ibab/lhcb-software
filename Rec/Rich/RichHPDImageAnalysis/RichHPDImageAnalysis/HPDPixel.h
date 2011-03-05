// $Id: $
#ifndef RICHHPDIMAGEANALYSIS_HPDPIXEL_H
#define RICHHPDIMAGEANALYSIS_HPDPIXEL_H 1

#include <vector>

namespace Rich
{
  namespace HPDImage
  {

    /** @class Pixel RichHPDImageAnalysis/HPDPixel.h
     *
     *  Simle class to represent an HPD pixel (col,row)
     *
     *  @author Chris Jones
     *  @date   2011-03-05
     */
    class Pixel
    {

    public:

      /// Standard constructor
      Pixel( const int _col = -1,
             const int _row = -1 )
        : col(_col), row(_row) { }

      ~Pixel( ) { } ///< Destructor

    public:

      int col; ///< Pixel column
      int row; ///< Pixel row

    public:

      /// List of pixels
      typedef std::vector<Pixel> List;

    };

  }
}

#endif // RICHHPDIMAGEANALYSIS_HPDPIXEL_H
