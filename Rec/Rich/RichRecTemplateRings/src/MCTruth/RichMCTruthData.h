// $Id: RichMCTruthData.h,v 1.2 2009-06-05 17:21:32 jonrob Exp $
#ifndef MCTRUTH_RICHMCTRUTHDATA_H
#define MCTRUTH_RICHMCTRUTHDATA_H 1

// Include files
// from Gaudi

#include "RichRingRec/RichRingRecToolBase.h"
#include "RichRingRec/IRichMCTruthData.h"            // Interface

namespace Rich
{
  namespace Rec
  {
    namespace TemplateRings
    {

      /** @class RichMCTruthData RichMCTruthData.h MCTruth/RichMCTruthData.h
       *
       *
       *  @author Sajan EASO
       *  @date   2007-06-26
       */
      class RichMCTruthData : public RichRingRecToolBase,
                              virtual public IRichMCTruthData {
      public:
        /// Standard constructor
        RichMCTruthData( const std::string& type,
                         const std::string& name,
                         const IInterface* parent);

        virtual ~RichMCTruthData( ); ///< Destructor

        void ResetMCTruthArrays();
        void StoreBGPid(int ns,  VD aBeta, VD aGamma, VD aPid );
        VD getTrueRingInfo(int rad , VI ahitlist, const Gaudi::XYZPoint & pdPointLocalTk,
                           double NominalMinradius, double NominalMaxradius  );

        void StoreTrueHit(VD rus, VD nus );

        double MCTrueRadiusValue(int i, int irad)  {  return m_MCTrueRadius [irad] [i];}
        double MCTrueNumHitValue(int i, int irad)  {  return m_MCTrueNumHit  [irad] [i];}
        double MCTrueBetaValue(int i, int irad)    {  return m_MCTrueBeta [irad] [i];}
        double MCTrueGammaValue(int i, int irad)   {  return m_MCTrueGamma [irad] [i];}
        double MCTruePidValue (int i, int irad)    {  return m_MCTruePid [irad] [i];}
        int numTrueTkInfoValue(int i)          {  return m_numTrueTkInfo [i];}

      protected:

      private:

        VVD m_MCTrueRadius;
        VVD m_MCTrueNumHit;
        VVD m_MCTrueBeta;
        VVD m_MCTrueGamma;
        VVD m_MCTruePid;
        //  VVD m_MCTrueMomentum;
        VI  m_numTrueTkInfo;

      };

    }
  }
}

#endif // MCTRUTH_RICHMCTRUTHDATA_H
