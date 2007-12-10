// $Id: CopyStripETC.h,v 1.2 2007-12-10 16:27:46 ibelyaev Exp $
// ============================================================================
#ifndef COPYSTRIPETC_H 
#define COPYSTRIPETC_H 1
// ============================================================================
// Include file
// ============================================================================
// Gaudi
// ============================================================================
#include "GaudiAlg/GaudiTupleAlg.h"
// ============================================================================
/** @class CopyStripETC CopyStripETC.h
 *  
 *  Copies the contents of the input ETC to the output ETC
 *  (that is identical to the input except for the pointers to events
 *  that refer to the output DST).
 *
 *  @author Patrick Koppenburg
 *  @date   2006-06-26
 */
class CopyStripETC : public GaudiTupleAlg 
{
public: 
  /// Standard constructor
  CopyStripETC( const std::string& name, ISvcLocator* pSvcLocator );
  
  virtual ~CopyStripETC( ); ///< Destructor
  
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  
protected:
  /// Note that all these methods do not pass const NTuplePtr because
  /// the item(...) method in it is not const. 
  StatusCode fillOutputETC(NTuplePtr&) const ; ///< Fill output ETC
  
private:
  std::string m_inputCollectionName; ///< Input ETC name
  std::string m_outputCollectionName; ///< Output ETC name
  
  long m_intType ; ///< ``Type'' code of ``int'' as defined in INTupleItem class  

};
// ============================================================================
// The END 
// ============================================================================
#endif // COPYSTRIPETC_H
// ============================================================================

