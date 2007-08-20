// $Id: RegionSelectors.h,v 1.2 2007-08-20 12:56:33 jonrob Exp $
#ifndef TFKERNEL_REGIONSELECTORS_H
#define TFKERNEL_REGIONSELECTORS_H 1

// Event model
#include "Event/State.h"

namespace Tf
{

  /** @class XYSearchWindow RegionSelectors.h TfKernel/RegionSelectors.h
   *
   *  Simple class to store the min and max X and Y values for a search window
   *
   *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   *  @date   2007-08-20
   */
  class XYSearchWindow
  {
  public:
    /// Constructor from the min and max values
    XYSearchWindow( const float _minX, ///< The minimum x value for the search window
                    const float _maxX, ///< The maximum x value for the search window
                    const float _minY, ///< The minimum y value for the search window
                    const float _maxY  ///< The maximum y value for the search window
                    )
      : m_minX(_minX), m_maxX(_maxX), m_minY(_minY), m_maxY(_maxY) { }
  public:
    inline float minX() const { return m_minX; } ///< The minimum x value for the search window
    inline float maxX() const { return m_maxX; } ///< The maximum x value for the search window
    inline float minY() const { return m_minY; } ///< The minimum y value for the search window
    inline float maxY() const { return m_maxY; } ///< The maximum y value for the search window
  private:
    float m_minX, m_maxX, m_minY, m_maxY;
  };

  /** @class StateRegionSelector RegionSelectors.h TfKernel/RegionSelectors.h
   *
   *  Selector object for use in conjunction with the TStationHitManager class.
   *  Can be used to select hits with a given #signma region around a given
   *  track State.
   *
   *  Use a second order polynomial in z to compute the y value  and a first
   *  or polynominal in z for x.
   *
   *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   *  @date   2007-08-20
   */
  class StateRegionSelector
  {

  public:

    /// Constructor from an LHCb::State
    StateRegionSelector( const LHCb::State & aState, const double nSigma )
      : m_nSigma ( nSigma  )
    {
      m_y = aState.ty();
      c_y = aState.y() - m_y*aState.z();
      a_x = aState.qOverP()/42.;
      b_x = aState.tx() -2*aState.z()*a_x;
      c_x = aState.x() - aState.z()*(b_x + a_x*aState.z());
      dy  = nSigma*sqrt(aState.errY2());
      dx  = nSigma*sqrt(aState.errX2());
    }

    /// Destructor
    ~StateRegionSelector() { }

  public:

    /** Returns the (x,y) search window for the given z position
     *  @param z The z position to use
     *  @return The Search window
     */
    inline XYSearchWindow searchWindow( const double z ) const
    {
      const float y = z*((a_x*z)+b_x)+c_x;
      const float x = m_y*z+c_y;
      return XYSearchWindow( x-dx, x+dx, y-dy, y+dy );
    }

  private:

    /// No default constructor
    StateRegionSelector( ) { }

  private:

    /// #sigma selection window around the State
    double m_nSigma;

    // temporary cached parameters
    double m_y, c_y, a_x, b_x, c_x, dy, dx;

  };

}

#endif // TFKERNEL_REGIONSELECTORS_H
