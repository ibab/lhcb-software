
#ifndef RICHHPDIMAGEANALYSIS_HPDPIXEL_H
#define RICHHPDIMAGEANALYSIS_HPDPIXEL_H 1

#ifdef __INTEL_COMPILER    // Disable ICC remarks
  // non-pointer conversion from "int" to "char" may lose significant bits
  #pragma warning(disable:2259) 
  // floating-point equality and inequality comparisons are unreliable
  #pragma warning(disable:1572) 
#endif

#include <vector>

namespace Rich
{
  namespace HPDImage
  {

    /** @class Pixel RichHPDImageAnalysis/HPDPixel.h
     *
     *  Simple class to represent an HPD pixel (col,row)
     *
     *  @author Chris Jones
     *  @date   2011-03-05
     */
    class Pixel
    {

    public:

      /// Standard constructor
      Pixel( const int    _col       = -1,
             const int    _row       = -1,
             const double _intensity = 0.0 )
        : col(_col), row(_row), intensity(_intensity) 
      { }

      /// Copy Constructor
      Pixel( const Pixel & pix ) 
        : col(pix.col), row(pix.row), intensity(pix.intensity)
      { }

      ~Pixel( ) { } ///< Destructor

    public:

      /// Sorting operator
      inline bool operator <  ( const Pixel & pixel ) const
      { 
        const unsigned int scale = 10000000;
        return ( (scale * this->col) + this->row <
                 (scale * pixel.col) + pixel.row );
      }

      /// Equality operator
      inline bool operator == ( const Pixel & pixel ) const
      { 
        return this->col == pixel.col && this->row == pixel.row ;
      }

    public:

      int col;          ///< Pixel column
      int row;          ///< Pixel row
      double intensity; ///< Pixel 'intensity

    public:

      /// List of pixels
      typedef std::vector<Pixel> List;

    };

  }
}

#endif // RICHHPDIMAGEANALYSIS_HPDPIXEL_H
