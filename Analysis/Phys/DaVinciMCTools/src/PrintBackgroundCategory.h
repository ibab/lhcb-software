#ifndef PRINTBACKGROUNDCATEGORY_H 
#define PRINTBACKGROUNDCATEGORY_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DVAlgorithm.h"

/** @class PrintBackgroundCategory PrintBackgroundCategory.h
 *  
 *  Loops over all particles in the given InputLocation and checks the 
 *  BackgroundCategory
 *
 *  @todo: add counters for each category ...
 * 
 *  @author Luis Fernandez
 *  @date   2005-12-16
 */
class PrintBackgroundCategory : public DVAlgorithm {
  typedef std::map<IBackgroundCategory::categories,int> bkgstats ;
public: 
  /// Standard constructor
  PrintBackgroundCategory( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PrintBackgroundCategory( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize();    ///< Algorithm finalization

protected:

private:

  StatusCode increment_stats(IBackgroundCategory::categories);
  StatusCode print_stats();

  IBackgroundCategory *m_bkgCategory;
  bkgstats m_stats;
  int m_entries;

};
#endif // PRINTBACKGROUNDCATEGORY_H
