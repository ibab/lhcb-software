// $Id: SavesetFinder.h,v 1.1 2008-03-11 18:23:26 ggiacomo Exp $
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
  const std::string SavesetServicePath = "/SAVER/SAVESETLOCATION/";
}

class AnalysisTask;

class SavesetFinder : public DimInfo {
public: 
  /// Standard constructor
  SavesetFinder( AnalysisTask* Ana, std::string TaskName); 

  virtual ~SavesetFinder( ); ///< Destructor
  
  virtual void infoHandler();

protected:
  AnalysisTask* m_analysis;
  std::string m_taskname;
private:

};
#endif // OMALIB_SAVESETFINDER_H
