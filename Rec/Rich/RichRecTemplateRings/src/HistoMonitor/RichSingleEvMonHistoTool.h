// $Id: RichSingleEvMonHistoTool.h,v 1.1.1.1 2009-03-04 12:01:45 jonrob Exp $
#ifndef HISTOMONITOR_RICHSINGLEEVMONHISTOTOOL_H 
#define HISTOMONITOR_RICHSINGLEEVMONHISTOTOOL_H 1

// Include files
// from Gaudi
#include "RichRingRec/RichRingRecToolBase.h"
#include "RichRingRec/IRichSingleEvMonHistoTool.h"            // Interface
//#include "TH1D.h"
//#include "TH2D.h"
//#include "TObjArray.h"

// Forward declarations
class IHistogram1D;
class IHistogram2D;


/** @class RichSingleEvMonHistoTool RichSingleEvMonHistoTool.h HistoMonitor/RichSingleEvMonHistoTool.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2007-06-18
 */
class RichSingleEvMonHistoTool : public RichRingRecToolBase, 
                                 virtual public IRichSingleEvMonHistoTool {
public: 
  /// Standard constructor
  RichSingleEvMonHistoTool( const std::string& type, 
                            const std::string& name,
                            const IInterface* parent);

  virtual ~RichSingleEvMonHistoTool( ); ///< Destructor

  void PlotHitsAndTracksInEv(int irad);
  void PlotTargetAndTemplate();
  void PlotInvCorr( VVD aRPInv);




  //  void InitDebugHistoSetup();
  // void  WriteOutDebugHistogramsToAuxiliaryFile();

  
    
    
protected:

private:



  // AIDA interface does not have all the functions like SetBinContent, needed
  // hence using ROOT directly. changed to simple filling of histo for now.
  //  IHistogram2D*   m_HistoInvCorrSingleTrack;
  //  IHistogram1D*    m_TestHistoMonitor;
  //  TH2D*   m_HistoInvCorrSingleTrack;
  //  TH1D*    m_TestHistoMonitor;
  // TObjArray*  m_RichOutHistogramList;  
  //
  //
  //  bool m_WriteOutDebugHistoFlag;
  // std::string m_DebugHistoOutputFileName;
  
  IHistogram2D*  m_HistoInvCorrSingleTrack;
  IHistogram1D* m_TestHistoMonitor;
  
};
#endif // HISTOMONITOR_RICHSINGLEEVMONHISTOTOOL_H
