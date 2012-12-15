// $Id: RemoveDuplicates.h,v 1.1 2012-10-11 11:56:34 elsasser Exp $
#ifndef REMOVEDUPLICATES_H 
#define REMOVEDUPLICATES_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DaVinciAlgorithm.h"

/** @class RemoveDuplicates RemoveDuplicates.h
 *  
 *  Filter the local storage to remove all duplicated candidates
 *
 *  @author Christian U. Elsasser
 *  @date   2012-10-11
 * 
 * 
 */


//===========================================================================
///A public DaVinciAlgorithm using the LHCb-ID hash to remove duplicates.
/** RemoveDuplicates is a useful tool to be called on stripped candidates.
 * The Tool removes duplicates (built from the same final state particles) 
 * within stripped candidates based on LHCb-ID hash.
 * If the candidates have the same hash only one is kept.
 * Output container for further processing is Phys/<toolname>/Particles.
 */
//===========================================================================

class RemoveDuplicates : public DaVinciAlgorithm {

public: 
  /// Standard constructor
  RemoveDuplicates( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RemoveDuplicates( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  
private:
  // globals

  int m_NpartIn; ///<number of particles so far sent to the alg
  int m_NpartOut; ///<number of unique particles so far written out by the alg
  
  

};
#endif // REMOVEDUPLICATES_H
