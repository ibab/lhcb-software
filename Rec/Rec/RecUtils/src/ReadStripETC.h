// $Id: ReadStripETC.h,v 1.2 2006-07-18 16:27:30 pkoppenb Exp $
#ifndef READSTRIPETC_H 
#define READSTRIPETC_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"

/** @class ReadStripETC ReadStripETC.h
 *  
 *  Reads in an input ETC and writes its contects in the TES for later saving 
 *  to DST. 
 *
 *  @author Patrick Koppenburg
 *  @date   2006-06-26
 */
class ReadStripETC : public GaudiTupleAlg {
public: 
  /// Standard constructor
  ReadStripETC( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~ReadStripETC( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  /// Note that all these methods do not pass const NTuplePtr because
  /// the item(...) method in it is not const. 
  StatusCode fillSelResults(NTuplePtr&) const ; ///< Fill SelResults
  StatusCode fillUsedSelections(NTuplePtr&); ///< Fill list of to be used selections

private:
  std::string m_inputCollectionName; ///< Input ETC name

  /// Name of selections for which one needs to write a SelResult. By default it is empty, which means 
  /// that one will write a SelResults for each one except the non-vetoed ones.
  /// As soon as this list is not empty, the vetoed list is ignored
  std::vector<std::string> m_selections ; 

  /// Name of the fields in the ETC for which one does not need to write out
  /// a SelResult object. This list is only used if m_selections is empty.
  std::vector<std::string> m_vetoedFields ; 

  /// Name of actual selections used determined in fisrt event
  std::vector<std::string> m_usedSelections ;

  /// Used selections have been filled
  bool m_usedSelectionsFilled ;
  
  long m_longType ; ///< ``Type'' code of ``long'' as defined in INTupleItem class  

};
#endif // READSTRIPETC_H
