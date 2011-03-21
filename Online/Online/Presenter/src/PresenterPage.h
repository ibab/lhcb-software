// $Id: $
#ifndef PRESENTERPAGE_H 
#define PRESENTERPAGE_H 1

// Include files
class OnlineHistoOnPage;
class OnlineHistogram;
class OnlineHistDB;
class OMAlib;
class Archive;
class DimBrowser;
class TCanvas;

#include "DisplayHistogram.h"

struct TaskHistos {
  std::string name;
  std::string location;
  bool dead;
  std::vector<DisplayHistogram> histos;
};

struct AnalysisHisto {
  DisplayHistogram* displayHisto;
  std::string algorithm;
  std::vector<float> params;
  std::vector<std::string> histoNames;
  std::vector<OnlineHistogram*> onlineHistos;
};

struct TrendingFile {
  std::string fileName;
  int  startTime;
  int  endTime;
  std::vector<DisplayHistogram> histos;
};

/** @class PresenterPage PresenterPage.h
 *  This describes the current page in the Presenter
 *
 *  @author Olivier Callot
 *  @date   2011-02-09
 */
class PresenterPage {
public: 
  /// Standard constructor
  PresenterPage( ); 

  virtual ~PresenterPage( ); ///< Destructor

  //== Name of the page
  std::string name() const { return m_name; }
  void setName( std::string name ) { m_name =  name; }

  //== Short documentation.
  std::string shortDoc() const { return m_shortDoc; }
  void setShortDoc( std::string doc ) { m_shortDoc = doc; }

  //== Reset and clear the page
  void clear();

  //== Set the DIM browser
  void setDimBrowser( DimBrowser* br )  { m_dimBrowser = br; }

  //== OnlineHistograms from DB
  std::vector<OnlineHistoOnPage*>& onlineHistos() { return m_onlineHistosOnPage; }
  
  //== Prepare the list of histogram to access
  void prepareAccess( OnlineHistDB* histDB, std::string& partition );

  //== Load the histograms from DIM services
  void loadFromDIM( std::string& partition, bool update );

  //== Set reference plots
  void uploadReference( OMAlib* analysisLib, int startRun, std::string tck );
  void clearReference( );

  //== Give the (first) DisplayHistogram for the specified OnlineHistoOnPage
  DisplayHistogram* displayHisto( OnlineHistoOnPage* onl );

  //== Give the (first) DisplayHistogram for the specified OnlineHistogram
  DisplayHistogram* displayHisto( OnlineHistogram* onlHist );

  //== Give the (first) DisplayHistogram for the specified OnlineHistogram
  DisplayHistogram* displayHisto( TH1* hist );

  bool buildAnalysisHistos (OMAlib* analysisLib, bool update );

  void loadFromArchive( Archive* archive, 
                        const std::string & timePoint,
                        const std::string & pastDuration );

  //== Handle the offset histograms
  void setOffsetHistograms();
  void resetOffsetHistograms();

  void drawPage( TCanvas* canvas, OMAlib* analysisLib, bool fastHitMapDraw );

  void updateDrawingOptions();

  void fillTrendingPlots ( int startTime, int endTime, bool update=false );

protected:

private:
  std::string m_name;     ///< Name of the page
  std::string m_shortDoc; ///< Short documentation
  DimBrowser* m_dimBrowser;
  std::vector<OnlineHistoOnPage*> m_onlineHistosOnPage;  ///< Histogram decription from the database
  std::vector<TaskHistos> m_tasks;
  std::vector<AnalysisHisto> m_analysis;
  std::vector<TrendingFile> m_trends;
};
#endif // PRESENTERPAGE_H
