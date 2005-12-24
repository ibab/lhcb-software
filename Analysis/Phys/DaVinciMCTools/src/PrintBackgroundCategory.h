#ifndef PRINTBACKGROUNDCATEGORY_H 
#define PRINTBACKGROUNDCATEGORY_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DVAlgorithm.h"
#include <map>

/** @class PrintBackgroundCategory PrintBackgroundCategory.h
 *  
 *  \brief Loops over all particles in the given InputLocation and 
 *  checks the BackgroundCategory.
 *
 *  Prints the reconstructed tree(s) with property PrintTree.
 * 
 *  Dump all the MC particles trees with property DumpMCEvent.
 *
 *  Categories to be filtered (kept) can be added with the 
 *  FilterCategoryNames and FilterCategoryTypes properties. One can use
 *  either one or the other, or both, to define the categories to be filtered.
 *
 *  The logical test (keep or reject) can be flipped with the FlipResult
 *  property.
 *
 *  @author Luis Fernandez & Jeremie Borel
 *  @date 2005-12-16
 */
class PrintBackgroundCategory : public DVAlgorithm {
  struct bkgStat
  {
    bkgStat(): name(""), countCndate(0), countEvent(0), filterIt(0), tmpFlag(false) {};
    std::string name;
    int countCndate, countEvent;
    int filterIt; // -1 depends on whether the user filters on something or not. 0 do not filter (reject), 1 filter it (keep).
    bkgStat& operator++();
    void reset(){ tmpFlag=false; };
  private:
    bool tmpFlag;
  };  
  typedef std::map< int,bkgStat > bkgstats ;
  
public: 
  /// Standard constructor
  PrintBackgroundCategory( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PrintBackgroundCategory( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize();    ///< Algorithm finalization

protected:

private:
  std::map<int, std::string> m_cat; // temp object, should be defined in IBackgroundCategroy
  
  StatusCode increment_stats(IBackgroundCategory::categories);
  StatusCode print_stats();
  
  IBackgroundCategory *m_bkgCategory;
  bkgstats m_stats;

  IDebugTool *m_debug;
  
  bool m_PrintTree; ///< Print reconstructed tree.
  bool m_DumpMCEvent; ///< Dump whole MC event.

  int  m_acceptedevents; ///< Number of events with a candidate.
  int  m_candidates; ///< Number of candidates seen by the algorithm.
  
  bool m_flipResult; ///< Any event that would be rejected is kept and vice versa.
  std::vector<std::string> m_stringCat; ///< Categories the user wants to keep.
  std::vector<int> m_intCat; ///< Categories the user wants to keep.
};
#endif // PRINTBACKGROUNDCATEGORY_H
