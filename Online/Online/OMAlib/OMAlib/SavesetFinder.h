// $Id: SavesetFinder.h,v 1.2 2008-11-11 13:39:08 ggiacomo Exp $
#ifndef OMALIB_SAVESETFINDER_H 
#define OMALIB_SAVESETFINDER_H 1

/** @class SavesetFinder SavesetFinder.h OMAlib/SavesetFinder.h
 *  
 *
 *  @author Giacomo Graziani
 *  @date   2008-03-11
 */

#include <string>
#include <dim/dic.hxx>


namespace OMAconstants {
  const std::string SavesetServicePath = "/SAVESETLOCATION";
}

class AnalysisTask;

class SavesetFinder : public DimInfo {
public: 
  /// Standard constructor
  SavesetFinder( AnalysisTask* Ana, 
                 std::string &TaskName,
                 std::string &PartitionName);

  virtual ~SavesetFinder( ); ///< Destructor
  
  virtual void infoHandler();
protected:
  AnalysisTask* m_analysis;
  std::string m_taskname;
private:
  std::string m_lastSaveset;
};
#endif // OMALIB_SAVESETFINDER_H
