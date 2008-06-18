// $Id: HltBackgroundCategory.h,v 1.3 2008-06-18 19:12:55 pkoppenb Exp $
#ifndef HLTBACKGROUNDCATEGORY_H 
#define HLTBACKGROUNDCATEGORY_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class HltBackgroundCategory HltBackgroundCategory.h
 *  
 *  Read Hlt summary and looks if reconstructed candidated is associated
 *  to a true decay.
 *
 *  As there are many candidates per event and selection only the candidate for each selection
 *  and event with the lowest background category (i.e. closest to signal) is considered.
 *
 *  If option "FillAll" is set to true, all categories are taken. But there's still only
 *  one endResult per selection and event.
 *
 * Option PrintTree allows to print the tree of the candidate and the mc particle is applicable.
 *
 *  @author Patrick Koppenburg
 *  @date   2008-02-04
 */
class IHltSummaryTool ;
class IBackgroundCategory ;
class IPrintDecayTreeTool ;
class IPrintMCDecayTreeTool ;

class HltBackgroundCategory : public GaudiAlgorithm {
public: 
  /// Standard constructor
  HltBackgroundCategory( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltBackgroundCategory( );  ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  bool m_printTree ;               ///< print tree
  bool m_fillAll ;                  ///< fill all candidates, not only best
  IHltSummaryTool* m_summaryTool ; ///< HLT summary tool
  IBackgroundCategory* m_bkg     ; //< Background category tool
  IAlgorithmCorrelations* m_algoCorr ;  ///< Correlation tool
  IPrintDecayTreeTool* m_print ;        ///< print Particles
  IPrintMCDecayTreeTool* m_printMC ;    ///< print MCParticles
  
};
#endif // HLTBACKGROUNDCATEGORY_H
