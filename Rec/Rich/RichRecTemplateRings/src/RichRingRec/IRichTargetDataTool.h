// $Id: IRichTargetDataTool.h,v 1.2 2009-06-05 17:21:33 jonrob Exp $
#ifndef IRICHTARGETDATATOOL_H
#define IRICHTARGETDATATOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "RichRingRec/RichDataArrayDefinitions.h"

namespace Rich
{
  namespace Rec
  {
    namespace TemplateRings
    {

      static const InterfaceID IID_IRichTargetDataTool ( "IRichTargetDataTool", 1, 0 );

      /*** @class IRichTargetDataTool IRichTargetDataTool.h
       *
       *
       *  @author Sajan EASO
       *  @date   2007-05-17
       */
      class IRichTargetDataTool : virtual public IAlgTool {
      public:

        // Return the interface ID
        static const InterfaceID& interfaceID() { return IID_IRichTargetDataTool; }

        virtual   int  NumHitsTarget(int iRich)=0;
        virtual   double XHitInpValue(int i, int iRich)=0;
        virtual   double YHitInpValue(int i, int iRich)=0;




        virtual int  NumChRdTrackValue(int rad )=0;
        virtual double TrackXcInpValue(int i, int rad) =0;
        virtual double TrackYcInpValue(int i, int rad) =0;
        virtual double TrackRecMomValue(int i, int rad )=0;
        virtual CD CTrackInp (int i, int rad) =0;
        virtual int TrackInvIndexValue(int i, int rad )=0;

        virtual int RichDetNumFromHitZ(double GlobalZHit) =0;

        virtual void clearHitInputs() =0;
        virtual void StoreTargetHitData(VC rh, int rs, VI rinv )=0;
        virtual void setNumHitsInp(int aNum )=0;


        virtual void setNumChTrack(int aNumt)=0;
        virtual void clearTrackInputs() =0;
        virtual void StoreTargetTrackData(int ns, VC th, VI ti,VD mh )=0;

        virtual int  FindIndexValue(int aIndex, int rad )=0;


      protected:

      private:

      };

    }
  }
}

#endif // IRICHTARGETDATATOOL_H
