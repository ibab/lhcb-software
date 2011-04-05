// $Id: $
#ifndef DISPLAYHISTOGRAM_H 
#define DISPLAYHISTOGRAM_H 1

// Include files
class OnlineHistogram;
class OMAlib;
class TH1;
class TPad;
class TImagePalette;
class TImage;
class TGraph;
class TPaveText;
class TPaveStats;
#include <map>

/** @class DisplayHistogram DisplayHistogram.h
 *  This class contains the information on a histogram, for display
 *
 *  @author Olivier Callot
 *  @date   2011-02-09
 */
class DisplayHistogram {
public: 
  /// Standard constructor
  DisplayHistogram( OnlineHistogram* hist ); 

  virtual ~DisplayHistogram( ); ///< Destructor

  OnlineHistogram* histo() { return m_onlineHist; }
  void setOnlineHistogram( OnlineHistogram* hist );

  void setShortName( std::string name ) { m_shortName = name; }
  std::string shortName() { return m_shortName; }

  TH1* rootHist() { return m_rootHistogram; }
  void setRootHist( TH1* rootH ) { m_rootHistogram = rootH; }

  std::string rootName();

  void deleteRootHist();

  void createDummyHisto( );

  TH1* referenceHist() { return m_referenceHist; }
  void setReferenceHistogram( TH1* ref );
  
  void normalizeReference();

  void loadFromMonObject( std::string& location, bool update );

  void draw( TCanvas * editorCanvas , double xlow , double ylow , 
             double xup , double yup , OMAlib* analysisLib,
             bool fastHitMapDraw , TPad* overlayOnPad = NULL ) ;

  TPad* hostingPad() { return m_hostingPad; }

  void setDisplayOptions();  ///< Pass theOnlineHistogram options to the Root histo

  void setDrawingOptions( TPad* pad );

  void fit( OMAlib* analysisLib );

  bool hasOption( const char* str, int* iopt );
  bool hasOption( const char* str, float* fopt );
  bool hasOption( const char* str, std::string* sopt );

  void setOffsetHistogram();
  void resetOffsetHistogram();

  void prepareForDisplay();

  void createGraph( std::vector<std::pair<int,double> > values, bool update );

  void copyFrom( TH1* src );
 
  TGraph* graph() { return m_timeGraph; }
 
  TObject* myObject();

  void clearOverlap() { m_nOverlap = 0; }
  void increaseOverlap() { m_nOverlap++; }
  int nOverlap() { return m_nOverlap; }

  void saveOptionsToDB( TPad* pad );
  bool updateDBOption( std::string opt, void* value, bool isDefault);

  bool isOverlap() { return m_isOverlap; }
  bool hasTitle() { return m_hasTitle; }

  void loadOptions();
  void setShowReference( bool show ) { m_showReference = show; }

  bool isDummy() { return m_isDummy; }
  void setDummy( bool dum ) { m_isDummy = dum; }
protected:

private:
  OnlineHistogram* m_onlineHist; ///< the online histogram.
  std::string m_shortName;
  bool m_isOverlap;
  bool m_isTrendPlot;
  TH1* m_rootHistogram;   ///< pointer to the underlying ROOT histogram
  TH1* m_offsetHistogram; ///< pointer to offset histogram
  TH1* m_referenceHist;   ///< reference histogram
  TPad* m_hostingPad;     ///< TPad for this histogram
  TPaveText* m_titPave;
  TPaveStats* m_statPave;
  TImage* m_histogramImage ; ///< histogram image
  TImagePalette* m_prettyPalette ; ///< palette 
  TGraph* m_timeGraph;
  TGraph* m_minGraph;
  TGraph* m_maxGraph;
  double* m_timeArray;
  double* m_valueArray;
  int     m_nOverlap;
  bool    m_hasTitle;
  bool    m_optionsAreLoaded;
  bool    m_showReference;
  bool    m_isDummy;
  std::map<std::string, int>         m_intOptions;
  std::map<std::string, float>       m_floatOptions;
  std::map<std::string, std::string> m_stringOptions;
};
#endif // DISPLAYHISTOGRAM_H
