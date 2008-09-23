// $Id: OldFilterDesktop.h,v 1.1 2008-09-23 13:52:23 pkoppenb Exp $
#ifndef FILTERDESKTOP_H
#define FILTERDESKTOP_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DVAlgorithm.h"

/** @class OldFilterDesktop OldFilterDesktop.h
 *
 *  Filters particles from Desktop using the ByPIDFilterCriterion tool
 *
 *  The following example options select K* in a 100 MeV mass window
 *  with chi2<100 made of pions and kaons with PVIP > 2sigma and
 *  where the pions have Pt>300 MeV;
 *  
 *  @code
 * 
 *  OldFilterDesktop.PhysDesktop.InputLocations = {"Phys/StdLooseKstar2KPi"};
 *  OldFilterDesktop.Filter.Code = "(ADMASS('K*(892)0')<100*MeV) 
 *                             & (VFASPF(VCHI2/VDOF)<100)
 *                             & (2==NINTREE(MIPCHI2DV(PRIMARY)>4))
 *                             & (INTREE(( ABSID=='pi+') & (PT>300*MeV)))"
 *
 *  @endcode
 *
 *  Note that the code looks for the string ```: ''', so the space after the colon
 *  is mandatory. This allows to support the syntax
 *  
 *  @code
 * 
 *  OldFilterDesktop.Filter.Selections = { "K*(892)0 : LoKi::Hybrid::FilterCriterion/KstarFilter" };
 *
 *  @endcode
 *
 *  @author Patrick KOPPENBURG
 *  @date   2004-11-26
 */

class IPlotTool;
class OldFilterDesktop : public DVAlgorithm {

public:
  /// Standard constructor
  OldFilterDesktop( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~OldFilterDesktop( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  StatusCode makePlots(const LHCb::Particle::ConstVector&,IPlotTool*); ///< make plots

private:
  IFilterCriterion* m_selection ;  ///< pointer to criterion

  /// what filter to use
  std::string m_filterCriterion ;
  /// Name for the input plot tool (switch on with HistoProduce = true,
  /// switch off with InputPlotTool = "none" )
  std::string m_inputPlotTool;
  /// Name for the output plot tool (switch on with HistoProduce = true,
  ///  switch off with OutputPlotTool = "none" )
  std::string m_outputPlotTool;
  /// Histogram path name for input plot tool. Default is "I" + algorithm instance name.
  std::string m_inputPlotsPath;
  /// Histogram path name for output plot tool. Default is "O" + algorithm instance name.
  std::string m_outputPlotsPath;

  std::string m_outputLocation ;    ///< Non-standard Output location, gets read by PhysDesktop

  /// Plots
  IPlotTool* m_inputPlots;
  IPlotTool* m_outputPlots;

  bool m_cloneFinalState ; ///< Clone final state particles instead of input mother
  bool m_cloneDaughters ;  ///< Clone immediate daughters instead of input mother
  bool m_cloneTree ;  ///< Clone whole tree including input mother

};
#endif // FILTERDESKTOP_H
