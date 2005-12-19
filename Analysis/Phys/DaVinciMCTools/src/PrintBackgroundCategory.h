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
 *  TODO: add counters for each category ...
 * 
 *  @author Luis Fernandez
 *  @date   2005-12-16
 */
class PrintBackgroundCategory : public DVAlgorithm {
public: 
  /// Standard constructor
  PrintBackgroundCategory( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PrintBackgroundCategory( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:
  IBackgroundCategory *m_bkgCategory;

};
#endif // PRINTBACKGROUNDCATEGORY_H
