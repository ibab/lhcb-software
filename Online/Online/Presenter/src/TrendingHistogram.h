// $Id: TrendingHistogram.h,v 1.1 2010-08-08 15:12:33 robbep Exp $
#ifndef PRESENTER_TRENDINGHISTOGRAM_H_
#define PRESENTER_TRENDINGHISTOGRAM_H_

// STL
#include <string>
#include <vector>

// presenter
#include "presenter.h"

// Base class
#include "DbRootHist.h"

// forward declaration
class OnlineHistDB ;
class OMAlib ;
class DimBrowser ;
class TGraph ;
class TCanvas ;
namespace boost {
  class recursive_mutex ;
};

/** @class TrendingHistogram TrendingHistogram.h
 *
 *  Trending histogram class, inherits from ::DbRootHist
 *
 *  @author Patrick Robbe
 *  @date   2010-07-10
 */
class TrendingHistogram : public DbRootHist
{
 public:
  /// mode of trending
  enum TrendingMode { 
    Last5Minutes , 
    Last30Minutes , 
    Last2Hours , 
    LastDay , 
    LastWeek , 
    LastYear , 
    Last10Years , 
    History };

  /// default constructor
  TrendingHistogram( const std::string & identifier,
		     const std::string & dimServiceName,
		     int refreshTime,
		     int instance,
		     OnlineHistDB* histogramDB,
		     OMAlib* analysisLib,
		     OnlineHistogram* onlineHist,
		     pres::MsgLevel verbosity,
		     DimBrowser* DimBr,
		     boost::recursive_mutex * oraMutex,
		     boost::recursive_mutex * dimMutex,
		     std::vector<std::string*> & tasksNotRunning,
		     boost::recursive_mutex * rootMutex , 
		     int startTime , 
		     int endTime , 
		     TrendingMode mode ) ;

  virtual ~TrendingHistogram( ) ; ///< Destructor
  
  /// Draw the histogram 
  virtual void draw(TCanvas* editorCanvas, double xlow, double ylow, 
  		    double xup, double yup, 
  		    bool fastHitMapDraw, TPad* overlayOnPad = NULL ) ;

  /// Initialize the histogram
  virtual void initHistogram( ) ;

  /// Fill the histogram
  virtual void fillHistogram( ) ;

  /// Refresh histogram
  void refresh( ) ;

  /// Change the mode
  void setMode( TrendingMode mode ) { m_trendingMode = mode ; } ;

  /// Normalize reference
  virtual void normalizeReference() { ; } ;
  
 private:
  std::string m_trendFileName ; ///< Name of the trending file
  std::string m_tagName ;       ///< Name of the tag
  TGraph * m_timeGraph ;        ///< Graph object to draw histogram

  double_t * m_timeArray  ; ///< internal array of time
  double_t * m_valueArray ; ///< internal array of values
  int        m_size       ; ///< size of the above arrays

  int m_startTime             ; ///< time of the beginning of the interval
  int m_endTime               ; ///< time of the end of the interval
  TrendingMode m_trendingMode ; ///< mode for trending

  /// Transfer graphics options from the root histogram
  void copyAttributes( ) ;

  /// Time offset for ROOT display
  static const int s_timeOffset ;
};

#endif /*PRESENTER_TRENDINGHISTOGRAM_H_*/
