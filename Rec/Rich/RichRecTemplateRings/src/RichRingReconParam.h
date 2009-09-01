// $Id: RichRingReconParam.h,v 1.4 2009-09-01 17:15:14 seaso Exp $
#ifndef RICHRINGRECONPARAM_H
#define RICHRINGRECONPARAM_H 1

// Include files
// from Gaudi
#include "RichRingRec/RichRingRecToolBase.h"
#include "RichRingRec/IRichRingReconParam.h"            // Interface

namespace Rich
{
  namespace Rec
  {
    namespace TemplateRings
    {

      /** @class RichRingReconParam RichRingReconParam.h
       *
       *
       *  @author Sajan EASO
       *  @date   2007-06-12
       */
      class RichRingReconParam : public RichRingRecToolBase,
                                 virtual public IRichRingReconParam {
      public:
        /// Standard constructor
        RichRingReconParam( const std::string& type,
                            const std::string& name,
                            const IInterface* parent);

        virtual ~RichRingReconParam( ); ///< Destructor


        double YAgelShift()  {  return m_YAgelShift;}
        int MinRadiator()  {  return m_MinRadiator;}
        int MaxRadiator()  {  return m_MaxRadiator;}
        bool  ActivateSingleEvHisto(){  return  m_ActivateSingleEvHisto;}
        int MinTrackNumForDebug(){    return m_MinTrackNumForDebug;}
        int MaxTrackNumForDebug(){    return m_MaxTrackNumForDebug;}
        bool ActivateSingleTrackDebug()  {  return m_ActivateSingleTrackDebug;}
        std::string RingRecHistoPath()  {  return m_RingRecHistoPath;}
        bool ActivateRandomPhiLocalScaling()  {  return m_ActivateRandomPhiLocalScaling;}
        bool ActivateMCCompareMassPrint()  {  return m_ActivateMCCompareMassPrint;}
        bool ActivateMaxNumberOfTrackSegmentsInRadiator()
        {  return m_ActivateMaxNumberOfTrackSegmentsInRadiator;}
        int MaxNumTrackSegmentsInAerogel()
        {    return m_MaxNumTrackSegmentsInAerogel;}
        int MaxNumTrackSegmentsInR1gas()
        {    return m_MaxNumTrackSegmentsInR1gas;}
        int MaxNumTrackSegmentsInR2gas()
        {    return m_MaxNumTrackSegmentsInR2gas;}
        //  bool WriteOutAuxiliaryDebugHisto()  {    return m_WriteOutAuxiliaryDebugHisto;}
        // std::string RichDebugAuxHistoFileName()  {    return m_RichDebugAuxHistoFileName;}

        double MinTrackMomentumSelectInAerogel() {return m_MinTrackMomentumSelectInAerogel;}
        
          
        double MinTrackMomentumSelectInRich1Gas(){return m_MinTrackMomentumSelectInRich1Gas;}
        
        double MinTrackMomentumSelectInRich2Gas(){return m_MinTrackMomentumSelectInRich2Gas;}
        

        StatusCode  initialize();


      protected:

      private:


        double m_YAgelShift;


        int  m_MinRadiator;
        int  m_MaxRadiator;
        bool m_ActivateSingleEvHisto;
        int m_MinTrackNumForDebug;
        int m_MaxTrackNumForDebug;
        bool m_ActivateSingleTrackDebug;
        std::string m_RingRecHistoPath;

        //  bool m_WriteOutAuxiliaryDebugHisto;
        // std::string m_RichDebugAuxHistoFileName;
        bool m_ActivateRandomPhiLocalScaling;
        bool m_ActivateMCCompareMassPrint;
        bool m_ActivateMaxNumberOfTrackSegmentsInRadiator;
        int  m_MaxNumTrackSegmentsInAerogel;
        int  m_MaxNumTrackSegmentsInR1gas;
        int  m_MaxNumTrackSegmentsInR2gas;

        double m_MinTrackMomentumSelectInAerogel;
        double m_MinTrackMomentumSelectInRich1Gas;
        double m_MinTrackMomentumSelectInRich2Gas;
        



      };

    }
  }
}

#endif // RICHRINGRECONPARAM_H
