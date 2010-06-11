// $Id: RunInfo.h,v 1.1 2010-06-11 13:00:10 ggiacomo Exp $
#ifndef OMALIB_RUNINFO_H 
#define OMALIB_RUNINFO_H 1

#include <dim/dic.hxx>

/** @class RunInfo RunInfo.h OMAlib/RunInfo.h
 *  
 *
 *  @author Giacomo GRAZIANI
 *  @date   2010-06-09
 */

class AnalysisTask;

class RunInfo : public DimInfo {
public: 
  RunInfo( AnalysisTask* Ana); 
  virtual ~RunInfo( ) {}
  void infoHandler();
protected:

  AnalysisTask* m_analysis;

};
#endif // OMALIB_RUNINFO_H
