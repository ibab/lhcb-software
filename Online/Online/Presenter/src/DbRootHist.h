#ifndef DBROOTHIST_H
#define DBROOTHIST_H 1
#include "OnlineHistDB/OnlineRootHist.h"
#include "OMAlib/OMAlib.h"
#include "dic.hxx"
class TPad;
//class TImage;


class DbRootHist : public OnlineRootHist, public DimInfo
{
  public:
    DbRootHist(std::string identifier,
        std::string dimServiceName,
        int refreshTime,
        int localInstance,
        OnlineHistDB* histogramDB,
	OMAlib* AnaLib);

    DbRootHist (const DbRootHist & ); // copy constructor
    DbRootHist & operator= (const DbRootHist &); // Assignment operator

    virtual ~DbRootHist();

    // Histos live in pads, so each histo must have a home

    
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

    TH1*    rootHistogram;        // actual ROOT histo
    TPad*   hostingPad;           // Hosting pad for page layout
//    TImage* histogramImage;       // H2Ds are rendered as images 

  private:
    // TODO: have a stack of offset for bracketing
    TH1*      m_offsetHistogram;  // state of histo @ clr/rst

    bool      m_isAnaHist;
    std::vector<DbRootHist*> m_AnaSources; // source histograms for analysis histogram
    bool      m_anaLoaded;
    OMAlib*   m_analib;
    std::string m_CreationAlgorithm;
    std::vector<std::string> m_sourcenames;
    std::vector<float> m_parameters;

    int       m_refreshTime;      // DIM regular time interval  
    float*    m_histoDimData;     // dimbuffer
    void      infoHandler();      // overloaded from DimInfo
    TString   m_histoRootName;    // generated ROOT histo name
    bool      m_toRefresh;        // flag for refresh/static histo
    bool      m_cleared;          // flag for clear/integrate
    std::string m_hstype;
    std::string m_hname;
    int m_localInstance;
    int m_waitTime;
    int m_msgBoxReturnCode;

    void cleanAnaSources();
    void loadAnaSources();
    void initRootFromDim();
    void fillRootFromDim();
};

#endif // DBROOTHIST_H
