
//-----------------------------------------------------------------------------
/** @file FastRingFitter.h
 *
 *  Header file for class : Rich::Rec::FastRingFitter
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-05-01
 */
//-----------------------------------------------------------------------------

#ifndef RichRecBase_FASTRINGFITTER_H
#define RichRecBase_FASTRINGFITTER_H 1

// std
#include <vector>
#include <ostream>

namespace LHCb
{
  class RichRecRing;
}

namespace Rich
{
  namespace Rec
  {

    /** @class FastRingFitter RichRecBase/FastRingFitter.h
     *
     *  Fast fitting routine using a iterational linear regression
     *  method (ILRM). Reference: N.Chernov, G.A.Ososkov, Computer
     *  Physics Communication 33 (1984) 329-333.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   2003-05-01
     */
    class FastRingFitter
    {

    public:

      /** @class Result RichRecBase/FastRingFitter.h
       *
       *  Results of the FastRingFitter
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   2003-05-01
       */
      class Result
      {

      public:

        /// overload printout to ostream operator <<
        friend inline std::ostream& operator << ( std::ostream& s,
                                                  const Result & result )
        {
          return s << "[ Status="  << result.Status
                   << " Radius="   << result.Radius
                   << " XCenter="   << result.XCenter
                   << " YCenter="   << result.YCenter
                   << " Variance=" << result.Variance
                   << " ]";
        }

      public:

        double  Radius{0};        ///< Fitted radius
        double  XCenter{0};       ///< Fitted x point of centre
        double  YCenter{0};       ///< Fitted y point of centre
        double  Variance{0};      ///< the variance estimate
        int     Status{0};        ///< Fit status code

      };

    public:

      ///< Default Constructor
      FastRingFitter() { }

      ///< Constructor from a RichRecRing
      explicit FastRingFitter( const LHCb::RichRecRing & ring ) 
      { addPoints(ring); }

      /// Perform the ring fit with the current set of points
      const Result& fit();

      /// full reset
      void clear();

      /// Add a data (x,y) point to the fitter data set
      void addPoint( const double x, const double y );

      /// Add the points from a RichRecRing to the fitter data set
      void addPoints( const LHCb::RichRecRing & ring );

      /// Number of data points
      unsigned int numberOfPoints() const;

      /// returns the fit results
      const Result& result() const; 

    private: // data

      std::vector<double> m_X; ///< x-coordinates of points
      std::vector<double> m_Y; ///< y-coordinates of points

      /// The results
      Result m_result;

    };

    inline void FastRingFitter::addPoint( const double x, const double y )
    {
      m_X.push_back(x);
      m_Y.push_back(y);
    }

    inline unsigned int FastRingFitter::numberOfPoints() const
    {
      return m_X.size();
    }

    inline const FastRingFitter::Result& FastRingFitter::result() const
    {
      return m_result;
    }

  }
}

#endif // RichRecBase_FASTRINGFITTER_H
