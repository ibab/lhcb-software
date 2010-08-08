#ifndef DBROOTHIST_H
#define DBROOTHIST_H 1

#include "HistogramIdentifier.h"

#include "dic.hxx"
#include "presenter.h"

#ifdef WIN32
#pragma warning( push )
#pragma warning( disable : 4800 )
#endif
#include <TH1.h>
#include <TPad.h>
#include <TDatime.h>
#ifdef WIN32
#pragma warning( pop )
#endif

class TImage;
class TImagePalette;
class DimInfo;
class DimInfoMonObject;
class MonRateRace;
class TPave;
class DimBrowser;
class PresenterMainFrame;
class OMAFitFunction;
class OnlineHistDB ;
class OnlineHistogram ;
class OMAlib ;

namespace boost {
  class recursive_mutex ;
};


class DbRootHist : public HistogramIdentifier
{
 public:
  
  
  enum ReferenceVisibility {
    Show = 0,
    Hide = 1
  };
  
  /// Constructor 
  DbRootHist(const std::string & identifier,
	     const std::string & dimServiceName,
	     int refreshTime , int instance,
	     OnlineHistDB* histogramDB,
	     OMAlib* analysisLib,
	     OnlineHistogram* onlineHist,
	     pres::MsgLevel verbosity,
	     DimBrowser* DimBr,
	     boost::recursive_mutex * oraMutex,
	     boost::recursive_mutex * dimMutex,
	     std::vector<std::string*> & tasksNotRunning,
	     boost::recursive_mutex * rootMutex);

  /// Copy constructor
  DbRootHist (const DbRootHist & );

  /// Equal operator 
  DbRootHist & operator= (const DbRootHist &);

  /// Destructor
  virtual ~DbRootHist();
    
  /// Initialize histogram
  virtual void initHistogram();
  
  /// Once we have our ROOT histos, this will be called to get new values
  virtual void fillHistogram();
  
  /// Enable editting of histo
  void enableEdit();
  
  /// Disable editting of histo
  void disableEdit();

  /// enable clear
  void enableClear();

  /// disable clear
  void disableClear() { m_cleared = false ; } ;

  /// set empty histo
  void beEmptyHisto();

  /// set regular histo
  void beRegularHisto() { m_isEmptyHisto = false ; }

  /// return true if empty histogram
  bool isEmptyHisto() { return m_isEmptyHisto; }
  
  /// Returns the instance number
  int instance() { return m_instance; }

  /// true if histogram is a analysis histogram
  bool isAnaHist() { return m_isAnaHist;}
  
  /// returns list of analysis sources
  const std::vector<DbRootHist*> *anaSources() { return &m_anaSources ; }

  /// Name of the creating algorithm
  std::string& creationAlgorithm() { return m_creationAlgorithm ; }

  /// Returns list of analysis parameters
  std::vector<float> *anaParameters() { return &m_parameters ; }
  
  /// Draw histogram
  virtual void draw( TCanvas * editorCanvas , double xlow , double ylow , 
		     double xup , double yup , bool fastHitMapDraw , 
		     TPad* overlayOnPad = NULL ) ;
  
  /// true if histogram is drawn on top of another one  
  bool isOverlap() {return m_isOverlap ; }

  /// Set overlap mode
  void setOverlapMode(pres::ServicePlotMode overlapMode);
  
  /// histogram identifier
  std::string identifier()  { return m_identifier ; }

  /// corresponding OnlineHistogram object
  OnlineHistogram* onlineHistogram() {return m_onlineHistogram;}

  /// set a link to an existing OnlineHistogram object, returns true on success
  virtual bool setOnlineHistogram( OnlineHistogram * newOnlineHistogram ) ;

  /// link to an existing ROOT TH1 object, returns true on success
  bool setRootHistogram( TH1* newRootHistogram ) ;
    
  /// returns link to DB session
  OnlineHistDB* dbSession() { return m_session; }
  
  /** updates ROOT TH1 display properties from Histogram DB 
   * (via OnlineHistogram object, normally called when connecting) 
   */
  void setTH1FromDB();

  /// fit histogram if requested in Histogram DB
  void fit();

  /// updates current drawing options from Histogram DB (via OnlineHistogram object)
  void setDrawOptionsFromDB(TPad* pad);

  /// saves current ROOT display options to OnlineHistogram object and to Histogram DB
  bool saveTH1ToDB(TPad* pad = NULL);

  /// normalize reference (if existing and requested) to current plot
  virtual void normalizeReference();
    
  /// reference histogram 
  void referenceHistogram( ReferenceVisibility visibility ) ;

  /// set reference histogram option
  void setReferenceOption( std::string refOption ) { 
    m_refOption = refOption ; }

  /// set histo in history trend mode
  void setHistoryTrendPlotMode(bool mode) { m_historyTrendPlotMode = mode; }
  
  /// set main presenter frame where to display histos
  void setPresenter(PresenterMainFrame* presenter) { 
    m_presenterApp = presenter;}
    
  /// returns eff service type
  pres::EffServiceType effServiceType() { return m_effServiceType; }
  
  /// Get the name of the associated root histogram
  std::string getName( ) { 
    return std::string( m_rootHistogram -> GetName() ) ; }
  
  /// Get the title of the associated root histogram
  std::string getTitle( ) { 
    return std::string( m_rootHistogram -> GetTitle() ) ; }
  
  /// Delete underlying ROOT histogram
  void deleteRootHistogram( ) {
    if ( 0 != m_rootHistogram ) {
      delete m_rootHistogram ;
      m_rootHistogram = NULL ;
    }
  }

  /// return false if no ROOT histograms are associated
  bool hasRootHistogram( ) { return ( NULL != m_rootHistogram ) ; } ;

  /// reset underlying ROOT histogram
  void rootHistogramReset( ) { m_rootHistogram -> Reset() ; } ;

  /// get ROOT histogram
  TH1 * getRootHistogram( ) { return m_rootHistogram ; } ;

  /// get histogram hosting pad
  TPad * getHostingPad( ) { return m_hostingPad ; } ;

  /// set hosting pad for an histogram
  void setHostingPad( TPad * thePad ) { m_hostingPad = thePad ; } ;

  /// produce virtual histogram on the fly
  TH1* makeVirtualHistogram(std::vector<TH1*> &sources);

 protected:
  TH1 * m_rootHistogram ; ///< pointer to the underlying ROOT histogram
  TString   m_histoRootName ; ///< generated ROOT histo name for identification
  TString   m_histoRootTitle ; ///< generated ROOT histo title from DIM gauchocomment
  std::string m_identifier ; ///< histogram identifier
  TPad * m_hostingPad ; ///< pointer to the histogram Pad  

  /// updates pad margins from  Histogram DB
  void setPadMarginsFromDB(TPad* &pad);


 private:
  TPad * m_drawPattern ; ///< pointer to pattern's pad
  DimInfo *  m_gauchocommentDimInfo ; ///< pointer to Gaucho comment DIM info
  TH1  * m_offsetHistogram ; ///< pointer to offset histogram
  int m_trendTimeScale ; ///< time scale of trend plots
  TDatime m_offsetTime ; ///< offset in time
  int m_trendBin ; ///< trend binning
  bool m_isAnaHist ; ///< is analysis histogram
  std::vector<DbRootHist*> m_anaSources; ///< source histograms for analysis histogram
  bool m_anaLoaded ; ///< analysis is loaded
  OMAlib * m_analysisLib ; ///< pointer to analysis library
  std::string m_creationAlgorithm ; ///< name of creation algorithm
  std::vector< std::string > m_sourcenames ; ///< names of the sources
  std::vector< float > m_parameters ; ///< list of parameters
  int  m_refreshTime ; ///< refresh time for DIM 
  bool m_cleared ; ///< flag for clear
  bool m_fastHitmapPlot ; ///< flag for fast hitmap plotting
  int m_instance ; ///< instance number
  int m_waitTime ; ///< wait time
  int m_serviceSize ; ///< size of service

  OnlineHistDB* m_session ; ///< link to hist db session
  OnlineHistogram* m_onlineHistogram ; ///< link to underlying online histogram
  bool m_isEmptyHisto ; ///< flag for empty histo
  int m_retryInit ; ///< retry count
  
  std::string m_refOption ; ///< reference options
  TImage * m_histogramImage ; ///< histogram image
  TImagePalette * m_prettyPalette ; ///< palette 
  TH1* m_reference ; ///< reference histogram
  std::string m_dimServiceName ; ///< dim service name
  DimInfo* m_dimInfo ; ///< pointer to dim info
  DimInfoMonObject* m_dimInfoMonObject ; ///< pointer to DIM info monitoring object
  MonRateRace* m_monRateRace ; ///< pointer to mon rate race
  pres::MsgLevel m_verbosity ; ///< verbosity of presenter
  DimBrowser * m_dimBrowser ; ///< pointer to DIM browser
  std::string m_partition ; ///< name of partition
  OMAFitFunction * m_fitfunction ; ///< pointer to fitting function

  TPave* m_titpave ; ///< pointer to title ROOT pave
  TPave* m_statpave ; ///< pointer to statistics ROOT pave
  
  bool m_historyTrendPlotMode ; ///< flag for history trending 
  bool m_isOverlap ; ///< flag for overlap
  boost::recursive_mutex * m_oraMutex ; ///< pointer for global oracle mutex
  boost::recursive_mutex * m_dimMutex ; ///< pointer for global dim mutex
  std::vector< std::string* > * m_tasksNotRunning ; ///< list of tasks not running
  boost::recursive_mutex * m_rootMutex ; ///< pointer to global root mutex

  PresenterMainFrame* m_presenterApp ; ///< link to presenter main frame
  pres::EffServiceType m_effServiceType ; ///< eff service type   
  
  /// clean analysis sources
  void cleanAnaSources();

  /// load analysis sources
  void loadAnaSources();

  /// update database option
  bool updateDBOption(std::string opt, void* value, bool isDefault);

  /// find the DIM service
  std::string findDimServiceName(const std::string & dimServiceType);

  /// build DIM service name
  std::string assembleCurrentDimServiceName();

  /** connect to DB session, returns true if the corresponding DB 
   *  histogram entry is found
   */
  bool connectToDB(OnlineHistDB* session, std::string page = "_NONE_",
		   int instance = 1);

  /// Apply default drawing options before getting them from the DB
  void applyDefaultDrawOptions( ) ;
};
#endif // DBROOTHIST_H
