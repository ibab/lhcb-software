// $Id: RegionSelectors.h,v 1.11 2007-08-28 12:03:58 jonrob Exp $
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
   *  Selector object for use in conjunction with the TStationHitManager and
   *  TTStationHitManager classes. 
   *
   *  This implementation selects hits within a given number of sigma region around a given
   *  track State. Uses a second order polynomial in z to compute the y value and a first
   *  or polynominal in z for x.
   *
   *  Example usage :- 
   *  @code
   *   LHCb::State * test_state = ....;
   *   const double nsigma = 3.0;
   *   StateRegionSelector selector( *test_state, nsigma );
   *   // hitMan is a pointer to a T or TT station hit manager
   *   hitMan->prepareHits(selector);
   *  @endcode 
   *
   *  In order to be used with the TStationHitManager or TTStationHitManager hit 
   *  manager classes, the selector objects must implement one specific method, 
   *  'searchWindow', which returns the search window  parameters 
   *  (min and max x and y) for a given z value.
   *
   *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   *  @date   2007-08-20
   */
  class StateRegionSelector
  {

  public:

    /** Constructor from an LHCb::Statech window size in number of sigma
     *  @param aState Reference to the LHCb::State object to search for hits around
     *  @param nSigma Size of the search window, in number of sigma * error.
     */
    StateRegionSelector( const LHCb::State & aState, const double nSigma )
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

    /** Returns the (x,y) search window for the given z position.
     *  @attention This is the one method all selectors must implement, if
     *  they are to be used in conjugtion with the TStationHitManager and
     *  TTStationHitManager classes.
     *  @param z The z position for which a search window should be created
     *  @return The Search window
     *  @see TStationHitManager
     *  @see TTStationHitManager
     */
    inline XYSearchWindow searchWindow( const double z ) const
    {
      const double x = (z*((a_x*z)+b_x)+c_x);
      const double y = (m_y*z+c_y);
      return XYSearchWindow( (float)(x-dx), (float)(x+dx), 
                             (float)(y-dy), (float)(y+dy) );
    }

  private:

    /// No default constructor
    StateRegionSelector( ) { }

  private:

    // temporary cached parameters
    double m_y, c_y, a_x, b_x, c_x, dy, dx;

  };

}

#endif // TFKERNEL_REGIONSELECTORS_H
