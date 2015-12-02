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
class TText;

#include "Presenter/DisplayHistogram.h"
#include "Presenter/PresenterInformation.h"

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

struct PadContent {
  TPad* pad;
  bool  hasTitle;
  std::vector<DisplayHistogram*> objects;
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

  //== Add a simple histo on the page
  void addSimpleHisto ( std::string dimName, OnlineHistogram* onlH, OnlineHistDB* histDB, std::string partition="LHCb" );

  //== Set the DIM browser
  void setDimBrowser( DimBrowser* br )  { m_dimBrowser = br; }

  //== OnlineHistograms from DB
  std::vector<OnlineHistoOnPage*>& onlineHistos() { return m_onlineHistosOnPage; }

  //== Simple display of pages built by AddDimHistos
  void simpleDisplay (  TCanvas* editorCanvas, OMAlib* analysisLib );

  //== Prepare the list of histogram to access
  void prepareAccess( OnlineHistDB* histDB, std::string& partition );

  //== Load the histograms from DIM services
  void loadFromDIM( std::string& partition, bool update, std::string& message );

  //== Set reference plots
  void uploadReference( OMAlib* analysisLib, PresenterInformation& presInfo );
  void clearReference( );

  //== Give the (first) DisplayHistogram for the specified OnlineHistoOnPage
  DisplayHistogram* displayHisto( OnlineHistoOnPage* onl );

  //== Give the (first) DisplayHistogram for the specified OnlineHistogram
  DisplayHistogram* displayHisto( OnlineHistogram* onlHist );

  //== Give the (first) DisplayHistogram for the specified TH1 histogram
  DisplayHistogram* displayHisto( TH1* hist );

  //== Give the (first) DisplayHistogram for the specified TGraph
  DisplayHistogram* displayHisto( TGraph* hist );

  void processAnalysisHistos( std::vector<OnlineHistogram*>& anaHistos, OnlineHistDB* histDB );

  bool buildAnalysisHistos (OMAlib* analysisLib, bool update );

  void loadFromArchive( Archive* archive,
                        const std::string & timePoint,
                        const std::string & pastDuration,
                        bool hasChanged = false );

  //== Handle the offset histograms
  void setOffsetHistograms();
  void resetOffsetHistograms();

  void drawPage( TCanvas* canvas, OMAlib* analysisLib, bool fastHitMapDraw );

  void updateDrawingOptions();

  void fillTrendingPlots ( unsigned int startTime, unsigned int endTime, bool update=false );

  void prepareDisplayHistos();

  std::vector<DisplayHistogram*>& displayHistos() { return m_displayHistograms; }

  bool okForSave(); /// All histograms properly defined.

  void drawBanner( std::string name, std::string source );

  void updateBanner( std::string source="" );

  bool hasBanner() { return NULL != m_bannerPad; }
  void rescaleHists();

protected:

private:
  std::string m_name;     ///< Name of the page
  std::string m_shortDoc; ///< Short documentation
  DimBrowser* m_dimBrowser;
  std::vector<OnlineHistoOnPage*> m_onlineHistosOnPage;  ///< Histogram decription from the database
  std::vector<TaskHistos> m_tasks;
  std::vector<AnalysisHisto> m_analysis;
  std::vector<TrendingFile> m_trends;
  std::vector<PadContent> m_pads;
  std::vector<DisplayHistogram*> m_displayHistograms;
  TPad*       m_bannerPad;
  TText*      m_bannerPageName;
  TText*      m_bannerSource;
  std::string m_lastName;
  std::string m_lastTimePoint;
  std::string m_lastDuration;
  std::string m_partition;
};
#endif // PRESENTERPAGE_H
