// $Id: RichLocalDefaultTemplateData.h,v 1.2 2009-06-05 17:21:34 jonrob Exp $
#ifndef TEMPLATEDATA_RICHLOCALDEFAULTTEMPLATEDATA_H
#define TEMPLATEDATA_RICHLOCALDEFAULTTEMPLATEDATA_H 1

// Include files
#include "GaudiKernel/Point3DTypes.h"
#include "RichRingRec/RichDataArrayDefinitions.h"

namespace Rich
{
  namespace Rec
  {
    namespace TemplateRings
    {

      /** @class RichLocalDefaultTemplateData RichLocalDefaultTemplateData.h TemplateData/RichLocalDefaultTemplateData.h
       *
       *
       *  @author Sajan EASO
       *  @date   2008-11-26
       */
      class RichLocalDefaultTemplateData {
      public:

        virtual ~RichLocalDefaultTemplateData( ); ///< Destructor

        static  RichLocalDefaultTemplateData* getRichLocalDefaultTemplateDataInstance();
        void DefineLocalDefaultTemplateData();

        double aerogelDefaultTemplate_X(int i){
          double aX=0;
          if(i < (int) m_aerogelDefaultTemplate_X.size() ) aX= m_aerogelDefaultTemplate_X[i];
          return aX;
        }
        double aerogelDefaultTemplate_Y(int i){
          double aY=0;
          if(i < (int) m_aerogelDefaultTemplate_Y.size() ) aY= m_aerogelDefaultTemplate_Y[i];
          return aY;
        }

        double rich1GasDefaultTemplate_X(int i){
          double bX=0;
          if(i < (int) m_rich1GasDefaultTemplate_X.size() ) bX= m_rich1GasDefaultTemplate_X[i];
          return bX;
        }
        double rich1GasDefaultTemplate_Y(int i){
          double bY=0;
          if(i < (int) m_rich1GasDefaultTemplate_Y.size() ) bY= m_rich1GasDefaultTemplate_Y[i];
          return bY;
        }

        double rich2GasDefaultTemplate_X(int i){
          double cX=0;
          if(i < (int) m_rich2GasDefaultTemplate_X.size() ) cX= m_rich2GasDefaultTemplate_X[i];
          return cX;
        }
        double rich2GasDefaultTemplate_Y(int i){
          double cY=0;
          if(i < (int) m_rich2GasDefaultTemplate_Y.size() ) cY= m_rich2GasDefaultTemplate_Y[i];
          return cY;
        }

        int aerogelDefaultTemplateSize()  {  return (int) m_aerogelDefaultTemplate_X.size() ;}
        int rich1GasDefaultTemplateSize()  {  return (int) m_rich1GasDefaultTemplate_X.size();}
        int rich2GasDefaultTemplateSize()  {  return (int) m_rich2GasDefaultTemplate_X.size();}

        int getDefaultRichTemplateSize(int irad)
        {
          int aSize=0;
          if (irad == 0) aSize =  aerogelDefaultTemplateSize();
          if (irad == 1) aSize = rich1GasDefaultTemplateSize();
          if (irad == 2) aSize = rich2GasDefaultTemplateSize();

          return aSize;

        }
        double getCurRichRadiatorTemplateDataX(int irad, int index )
        {
          double aXVal=0;
          if (irad == 0) aXVal=aerogelDefaultTemplate_X(index);
          if (irad == 1) aXVal=rich1GasDefaultTemplate_X(index);
          if (irad == 2) aXVal=rich2GasDefaultTemplate_X(index);
          return aXVal;

        }

        double getCurRichRadiatorTemplateDataY(int irad, int index )
        {
          double aYVal=0;
          if (irad == 0) aYVal=aerogelDefaultTemplate_Y(index);
          if (irad == 1) aYVal=rich1GasDefaultTemplate_Y(index);
          if (irad == 2) aYVal=rich2GasDefaultTemplate_Y(index);
          return aYVal;

        }

        double aerogelTrackDefaultTemplate_X()  {  return m_aerogelTrackDefaultTemplate_X;}
        double aerogelTrackDefaultTemplate_Y()  {  return m_aerogelTrackDefaultTemplate_Y;}

        double rich1GasTrackDefaultTemplate_X()  {  return m_rich1GasTrackDefaultTemplate_X;}
        double rich1GasTrackDefaultTemplate_Y()  {  return m_rich1GasTrackDefaultTemplate_Y;}

        double rich2GasTrackDefaultTemplate_X()  {  return m_rich2GasTrackDefaultTemplate_X;}
        double rich2GasTrackDefaultTemplate_Y()  {  return m_rich2GasTrackDefaultTemplate_Y;}

        double getCurrentTrackDefaultTemplate_X(int irad ){

          double aTX=0.0;
          if (irad == 0) aTX=aerogelTrackDefaultTemplate_X();
          if (irad == 1) aTX=rich1GasTrackDefaultTemplate_X();
          if (irad == 2) aTX=rich2GasTrackDefaultTemplate_X();
          return aTX;
        }

        double getCurrentTrackDefaultTemplate_Y(int irad ){

          double aTY=0.0;
          if (irad == 0) aTY=aerogelTrackDefaultTemplate_Y();
          if (irad == 1) aTY=rich1GasTrackDefaultTemplate_Y();
          if (irad == 2) aTY=rich2GasTrackDefaultTemplate_Y();
          return aTY;
        }


      protected:

      private:
        /// Standard constructor
        RichLocalDefaultTemplateData( );

        static RichLocalDefaultTemplateData* m_RichLocalDefaultTemplateDataInstance;


        VD m_aerogelDefaultTemplate_X;
        VD m_aerogelDefaultTemplate_Y;
        VD m_rich1GasDefaultTemplate_X;
        VD m_rich1GasDefaultTemplate_Y;
        VD m_rich2GasDefaultTemplate_X;
        VD m_rich2GasDefaultTemplate_Y;

        double m_aerogelTrackDefaultTemplate_X;
        double m_aerogelTrackDefaultTemplate_Y;
        double m_rich1GasTrackDefaultTemplate_X;
        double m_rich1GasTrackDefaultTemplate_Y;
        double m_rich2GasTrackDefaultTemplate_X;
        double m_rich2GasTrackDefaultTemplate_Y;


      };

    }
  }
}

#endif // TEMPLATEDATA_RICHLOCALDEFAULTTEMPLATEDATA_H
