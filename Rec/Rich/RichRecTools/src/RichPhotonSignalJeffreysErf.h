
//-----------------------------------------------------------------------------
/** @file PhotonSignalJeffreysErf.h
 *
 *  Header file for tool : Rich::Rec::PhotonSignalJeffreysErf
 *
 *  CVS Log :-
 *  $Id: RichPhotonSignalJeffreysErf.h,v 1.1 2007-06-22 14:35:58 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RICHPHOTONSIGNALJEFFREYSERF_H
#define RICHRECTOOLS_RICHPHOTONSIGNALJEFFREYSERF_H 1

// base class
#include "RichPhotonSignalGaussProb.h"

namespace Rich
{
  namespace Rec
  {

    //-----------------------------------------------------------------------------
    /** @class PhotonSignalJeffreysErf RichPhotonSignalJeffreysErf.h
     *
     *  Tool to calculate for a given photon the probabilities of it
     *  being a signal or scattered photon, and its predicted contribution
     *  to its associated RichRecPixel, under a certain mass hypothesis.
     *
     *  An attempt to deal with outliers by using a signal probability function
     *  that convolutes the simple Gaussian probability with a range of resolutions.
     *
     *  See http://scripts.iucr.org/cgi-bin/paper?he0278 for details
     *
     *  @attention This tool is under development
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   14/06/2007
     */
    //-----------------------------------------------------------------------------

    class PhotonSignalJeffreysErf : public Rich::Rec::PhotonSignalGaussProb
    {

    public: // Methods for Gaudi Framework

      /// Standard constructor
      PhotonSignalJeffreysErf( const std::string& type,
                               const std::string& name,
                               const IInterface* parent );

      /// Destructor
      virtual ~PhotonSignalJeffreysErf() {}

    protected: // methods

      /// compute the actual signal probability
      virtual double signalProbFunc( const double thetaDiff,
                                     const double thetaExpRes ) const;

    private:

      /// Max error scale factor
      double m_errScale;

    };

  }
}

#endif // RICHRECTOOLS_RICHPHOTONSIGNALJEFFREYSERF_H
