
//-----------------------------------------------------------------------------
/** @file FastRingFitter.h
 *
 *  Header file for class : Rich::Rec::FastRingFitter
 *
 *  CVS Log :-
 *  $Id: FastRingFitter.h,v 1.2 2008-08-28 23:30:47 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-05-01
 */
//-----------------------------------------------------------------------------

#ifndef RichRecBase_FASTRINGFITTER_H
#define RichRecBase_FASTRINGFITTER_H 1

// std
#include <vector>

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
        /// Default Constructor
        Result() : Radius(0), XCenter(0), YCenter(0), Variance(0), Status(0) { }
      public:
        double  Radius;        ///< Fitted radius
        double  XCenter;       ///< Fitted x point of centre
        double  YCenter;       ///< Fitted y point of centre
        double  Variance;      ///< the variance estimate
        int     Status;        ///< Fit status code
      };

    public:

      ///< Default Constructor
      FastRingFitter() { }

      ///< Default Destructor
      ~FastRingFitter() {};

      /// Perform the ring fit with the current set of points
      bool   fit();

      /// full reset
      void   clear();

      /// Add a data (x,y) point to the fitter set
      void   addPoint( const double x, const double y );

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
