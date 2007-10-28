#ifndef DBROOTHIST_H
#define DBROOTHIST_H 1
#include "OnlineHistDB/OnlineRootHist.h"
#include "HistogramIdentifier.h"
#include "OMAlib/OMAlib.h"
#include "dic.hxx"
class TPad;
//class TImage;

class DbRootHist : public OnlineRootHist, public DimInfo,
  public HistogramIdentifier
{
  public:
    DbRootHist(std::string identifier, std::string dimServiceName,
               int refreshTime, int localInstance, OnlineHistDB* histogramDB,
               OMAlib* anaLib);

    DbRootHist (const DbRootHist & );
    DbRootHist & operator= (const DbRootHist &);

    virtual ~DbRootHist();

    virtual bool setdbHist(OnlineHistogram*  oh);

    // Binning is only known from DIM, so create ROOT histo with DIM
    void initHistogram();

    // Once we have our ROOT histos, this will be called to get new values
    // TODO: if binning changes on the fly, reinit ...MonObj?
    void fillHistogram();

    // Enable / disable pad with SetEditable and move
    void enableEdit();
    void disableEdit();

    // Clearing histos with an offset value and restoring them
    void enableClear();
    void disableClear();
    bool isCleared() { return m_cleared; }

    std::string histoRootName() {return std::string(m_histoRootName.Data());}
    std::string hstype() { return m_hstype; }
    int instance() { return m_localInstance; }

    // actual ROOT histo
    TH1*    rootHistogram;
    // Histos live in pads, so each histo must have a home
    TPad*   hostingPad;
    // H2Ds are rendered as images
//    TImage* histogramImage;


  private:
    DimInfo* m_gauchocommentDimInfo;
    // TODO: have a stack of offset for bracketing
    // state of histo @ clr/rst
    TH1*      m_offsetHistogram;

//    HistogramIdentifier m_histogramIdentifier;

    bool      m_isAnaHist;
    // source histograms for analysis histogram
    std::vector<DbRootHist*> m_anaSources;
    bool      m_anaLoaded;
    OMAlib*   m_analib;
    std::string m_creationAlgorithm;
    std::vector<std::string> m_sourcenames;
    std::vector<float> m_parameters;

    // DIM regular time interval
    int       m_refreshTime;
    // dimbuffer
    float*    m_histoDimData;
    // overloaded from DimInfo
    void      infoHandler();
    // generated ROOT histo name for identification
    TString   m_histoRootName;
    // generated ROOT histo title from DIM gauchocomment
    TString   m_histoRootTitle;
    // flag for refresh/static histo
    bool      m_toRefresh;
    // flag for clear/integrate
    bool      m_cleared;
    std::string m_hstype;
    std::string m_hname;
    int m_localInstance;
    int m_waitTime;
    int m_msgBoxReturnCode;
    int m_serviceSize;

    void cleanAnaSources();
    void loadAnaSources();
    void initRootFromDim();
    void fillRootFromDim();
};

#endif // DBROOTHIST_H
