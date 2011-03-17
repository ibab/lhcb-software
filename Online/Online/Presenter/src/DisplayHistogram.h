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
  TImage* m_histogramImage ; ///< histogram image
  TImagePalette* m_prettyPalette ; ///< palette 
  TGraph* m_timeGraph;
  double* m_timeArray;
  double* m_valueArray;
};
#endif // DISPLAYHISTOGRAM_H
