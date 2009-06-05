// $Id: RichRingRecMass.h,v 1.2 2009-06-05 17:21:33 jonrob Exp $
#ifndef RINGFIT_RICHRINGRECMASS_H
#define RINGFIT_RICHRINGRECMASS_H 1

// Include files
// from Gaudi
#include "RichRingRec/RichRingRecToolBase.h"
#include "RichRingRec/IRichRingRecMass.h"            // Interface

namespace Rich
{
  namespace Rec
  {
    namespace TemplateRings
    {

      /** @class RichRingRecMass RichRingRecMass.h RingFit/RichRingRecMass.h
       *
       *
       *  @author Sajan EASO
       *  @date   2007-07-06
       */
      class RichRingRecMass : public RichRingRecToolBase,
                              virtual public IRichRingRecMass {
      public:
        /// Standard constructor
        RichRingRecMass( const std::string& type,
                         const std::string& name,
                         const IInterface* parent);

        virtual ~RichRingRecMass( ); ///< Destructor

        void RichReconMassForTrackSegment(int itk, int irad );
        StatusCode bestReconstructedMassForRichTrack(VI SegmentIndex ,
                                                     int currentTrackIndex);

        int SelectRadiatorFromSegReconLight( VD SegMomVect, VD SegBGVect, VD SegRadiusVect,
                                             VD SegMassVect, VB radiatorPresent, int arcount);
        int SelectRadiatorFromSegReconHeavy( VD SegMomVect, VD SegBGVect, VD SegRadiusVect,
                                             VD SegMassVect,VB radiatorPresent, int arcount);
        int SelectFromPairOfBGA (double aBG1, double aBG2, double aMom ,
                                 double aMass1, double aMass2, int isp1, int isp2);
        int SelectFromPairOfBGB (double aBG1, double aBG2, double aMom ,
                                 double aMass1, double aMass2, int isp1, int isp2);


      protected:

      private:

      };

    }
  }
}

#endif // RINGFIT_RICHRINGRECMASS_H
