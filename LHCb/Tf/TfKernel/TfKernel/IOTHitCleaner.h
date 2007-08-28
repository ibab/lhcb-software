// $Id: IOTHitCleaner.h,v 1.4 2007-08-28 12:03:58 jonrob Exp $
#ifndef TFTOOLS_IOTHITCLEANER_H
#define TFTOOLS_IOTHITCLEANER_H 1

// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/SystemOfUnits.h"

// From Tf
#include "TfKernel/OTHit.h"
//#include "TfKernel/Region.h"

/// Interface ID
static const InterfaceID IID_IOTHitCleaner ( "IOTHitCleaner", 1, 0 );

namespace Tf
{

  /** @class IOTHitCleaner IOTHitCleaner.h TfKernel/IOTHitCleaner.h
   *
   *  Interface to tool that 'cleans' ranges of OTHits
   *
   *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   *  @date   2007-07-20
   */
  class IOTHitCleaner : virtual public IAlgTool
  {

  public:

    /// Return the interface ID
    static const InterfaceID& interfaceID() { return IID_IOTHitCleaner; }

  public:

    /** Clean the given range of OT hits
     *  @param[in]  input The range of OT hits to clean
     *  @param[out] output The selected hits
     *  @return The number of removed hits
     */
    template<class INPUTDATA >
    inline unsigned int cleanHits( const INPUTDATA & input,
                                   OTHits& output ) const
    {
      return cleanHits ( input.begin(), input.end(), output );
    }

    /** Clean the given range of hits
     *  @param[in] begin Iterator to the start of a range of OT hits
     *  @param[in] end   Iterator to the start of a range of OT hits
     *  @param[out] output The selected hits
     *  @return The number of removed hits
     */
    virtual unsigned int cleanHits( const OTHits::const_iterator begin,
                                    const OTHits::const_iterator end,
                                    OTHits& output ) const = 0;

  public:
    // CRJ : Why are these here ?

    /// This will only select neighbouring straws in the same module
    class AdjacentByStrawNumber
      : std::binary_function< const OTHit* , const OTHit* , bool >
    {
    public:
      /// Comparision operator
      inline bool operator() (const OTHit* lhs, const OTHit* rhs)
      {
        // the monolayer calculation is currently the slowest
        return
          abs((int)(lhs->straw()-rhs->straw()))==1 &&
          (&(lhs->module()) == &(rhs->module())) &&
          (lhs->monolayer() == rhs->monolayer()) ;
      }
    };

    /// This will also select neighbouring straws on different modules
    class AdjacentByCoordinate
      : std::binary_function< const OTHit* , const OTHit* , bool >
    {
    public:
      /// a bit more than the pitch, but less than twice the pitch, to
      /// allow for misalignments.
      AdjacentByCoordinate(const float maxdistance = 8*Gaudi::Units::mm )
        : m_d(maxdistance) {}
      /// Comparision operator
      inline bool operator() (const OTHit* lhs, const OTHit* rhs)
      {
        return
          fabs(lhs->xT() - rhs->xT()) < m_d &&
          lhs->regionID() == rhs->regionID() ;
      }
    private:
      float m_d ; ///< The maximum distance for separation
    };

  };

}

#endif // TFTOOLS_IOTHITCLEANER_H
