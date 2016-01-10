//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Feb 15 16:04:15 2007 by ROOT version 5.14/00b
// from TTree protoPtuple/ProtoParticle PID Information
// found on file: dlls-newRichQE-tuple.root
//////////////////////////////////////////////////////////

#ifndef GlobalPID_h
#define GlobalPID_h

#include <iostream>
#include <map>
#include <vector>
#include <cmath>

//#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TH2F.h>
//#include "TSystem.h"
//#include "TPad.h"

/// ROOT class for producing PID plots from a TTree
class GlobalPID
{

public: // definitions

  /// MC type
  enum PIDType
    {
      UnknownParticle = -2,
      NoMCInfo = -1,
      Electron = 0,
      Muon     = 1,
      Pion     = 2,
      Kaon     = 3,
      Proton   = 4
    };

  /// Tracktype
  enum TrkType
    {
      TypeUnknown = 0,
      Velo,
      VeloR,
      Long,
      Upstream,
      Downstream,
      Ttrack
    };

  // Variable type
  enum Variable
    {
      NoVar,
      richDLLe,
      richDLLmu,
      richDLLpi,
      richDLLk,
      richDLLp,
      combDLLe,
      combDLLmu,
      combDLLpi,
      combDLLk,
      combDLLp,
      muonMuLL,
      muonBkgLL,
      caloPIDe,
      ecalPIDe,
      hcalPIDe,
      prsPIDe,
      bremPIDe,
      caloPIDmu,
      ecalPIDmu,
      hcalPIDmu,
      muonNShared
    };

  typedef std::pair<unsigned int,unsigned int> SelData;
  typedef std::map< PIDType, SelData > PIDData;
  typedef std::map< double, PIDData > PIDStepData;

  typedef std::map<PIDType,unsigned int> AcceptanceTracks;

public: // classes

  /// Utility class used to configure the job
  class Configuration
  {
  public:
    Configuration()
      : minIDeff(0),maxIDeff(100),minMisIDeff(0),maxMisIDeff(100),
        minP(2000), maxP(100000), minPt(500), maxPt(100000), 
        minGhostProb(0), maxGhostProb(0.9),
        minTrackLikelihood(-40), maxTrackLikelihood(999),
        minCut(-15), maxCut(15),
        //minAngle(-9999),maxAngle(9999),
        minAngle(30),maxAngle(300),
        mustHaveAerogel(false),mustHaveR1Gas(false),mustHaveR2Gas(false),mustHaveAnyRICH(false),
        mustHaveECAL(false),mustHaveHCAL(false),mustHavePRS(false),mustHaveBREM(false),
        mustHaveECALorPRS(false),
        mustHaveMUON(false),
        mustHaveECALorPRSorRICH(false),
        nSteps(41),
        idType(Kaon), misidType(Pion),
        trackType(Long),
        var1(richDLLk), var2(richDLLpi),
        secvar1(NoVar), secvar2(NoVar),
        secondaryCut(0),
        logX(false),logY(true),
        maxLabels(4),
        title("UnDefined Title"),
        subtitle(""),
        recreateCombinedDLLs(false),
        superImpose(false),writeCutValues(true),
        color(kBlack),
        textSize(0.018),
        imageFile(""),
        cutType(">"),
        secCutType(">"),
        useFixedGraphRange(true),
        minGraphX(60), maxGraphX(100), minGraphY(1), maxGraphY(40)
    { }
  public:
    double minIDeff,maxIDeff,minMisIDeff,maxMisIDeff;
    double minP, maxP, minPt, maxPt;
    double minGhostProb, maxGhostProb;
    double minTrackLikelihood, maxTrackLikelihood;
    double minCut, maxCut;
    double minAngle, maxAngle;
    bool mustHaveAerogel,mustHaveR1Gas,mustHaveR2Gas,mustHaveAnyRICH;
    bool mustHaveECAL,mustHaveHCAL,mustHavePRS,mustHaveBREM,mustHaveECALorPRS;
    bool mustHaveMUON,mustHaveECALorPRSorRICH;
    unsigned int nSteps;
    GlobalPID::PIDType idType, misidType;
    GlobalPID::TrkType trackType;
    GlobalPID::Variable var1, var2, secvar1, secvar2;
    double secondaryCut;
    bool logX,logY;
    int maxLabels;
    std::string title, subtitle;
    bool recreateCombinedDLLs;
    bool superImpose;
    bool writeCutValues;
    Color_t color;
    double textSize;
    std::string imageFile;
    std::string cutType, secCutType;
    bool useFixedGraphRange;
    double minGraphX,maxGraphX,minGraphY,maxGraphY;
  };

public:

  // Core root stuff
  //-------------------------------------------------------------
  GlobalPID ( );
  GlobalPID ( const std::string & filename );
  virtual ~GlobalPID();

  void loadTTree( const std::string & filename );

  void resetFile();

  //virtual Int_t    Cut(Long64_t entry);
  virtual Int_t    GetEntry(Long64_t entry);
  virtual Long64_t LoadTree(Long64_t entry);
  virtual void     Init(TTree *tree);
  virtual Bool_t   Notify();
  virtual void     Show(Long64_t entry = -1);

  //-------------------------------------------------------------

public: // User methods

  /// Main method to mae the Eff. V Pur. curve
  void makeCurve( const Long64_t nTracks = -1 );

  /// Save the current canvases to file. Optional argument is the image type
  void saveFigures( const std::string & type = "pdf" );

  /// Reset the config object
  void configReset();

  /// Reset the object
  void reset();

  /// make code
  void makeCode();

private: // methods

  /// Returns the MC type
  GlobalPID::PIDType getMcType() const;

  /// Does the current entry pass the track selection ?
  bool passTrackSelection() const;

  /// Does the current entry pass the PID detector selection ?
  bool passPIDDetSelection() const;

  /// Basic PID det sel
  bool passBasicPIDDet() const;

  /// Returns the value for the given variable
  double variable( const GlobalPID::Variable var ) const;

  /// Returns the PID type as a string
  std::string name( const GlobalPID::PIDType type ) const;

  /// Returns the Variable type as a string
  std::string name( const GlobalPID::Variable var ) const;

  /// Returns the Track type as a string
  std::string name( const GlobalPID::TrkType type ) const;

  /// Returns a file name for the image of the plot
  std::string imageName() const;

  /// Save the image to disk
  void saveImage ( TCanvas * canvas,
                   const std::string& filename );

  /// Fill a description into a text box on the given canvas
  void fillLabelBox() const;

  /// Recreate the combined DLL values
  void recreateCombinedDLL();

  /// The overall CALO electron PID measure
  double caloTotalPIDe() const;

  /// The overall CALO mu PID measure
  double caloTotalPIDmu() const;

private: // inline methods

  /// Secondary cut
  inline bool passSecondaryCut() const
  {
    return ( config.secvar1 == NoVar ? true :
             ( config.secCutType == ">" ?
               (variable(config.secvar1)-variable(config.secvar2)) > config.secondaryCut :
               (variable(config.secvar1)-variable(config.secvar2)) < config.secondaryCut )
             );
  }

  /// The Muon DLL
  inline double muonDLL() const
  {
    return ( 1 == MuonIsMuon ? MuonMuLL  : -99999 );
  }

  /// The Muon N Shared Hits
  inline double muonSharedHits() const
  {
    return ( 1 == MuonInAcc ? MuonNShared  : 0 );
  }

  /// The Combined Muon DLL
  inline double combinedMuonDLL() const
  {
    return ( 1 == MuonIsMuon ? CombDLLmu : -99999 );
  }

  /// Returns the currently configured selection variable
  inline double selectionVar()
  {
    if (config.recreateCombinedDLLs) recreateCombinedDLL();
    return variable(config.var1) - variable(config.var2);
  }

  /// RICH Aerogel is available
  inline bool hasAerogelInfo() const
  {
    return RichUsedAero != 0;
  }

  /// RICH1 Gas info is available
  inline bool hasRich1Gas() const
  {
    return RichUsedR1Gas != 0;
  }

  /// RICH2 Gas info is available
  inline bool hasRich2Gas() const
  {
    return RichUsedR2Gas != 0;
  }

  /// Any RICH info available
  inline bool hasRichInfo() const
  {
    //const PIDType mcType = getMcType();
    //if ( ( mcType == Electron && RichAboveElThres == 0 ) ||
    //     ( mcType == Muon     && RichAboveMuThres == 0 ) ||
    //     ( mcType == Pion     && RichAbovePiThres == 0 ) ||
    //     ( mcType == Kaon     && RichAboveKaThres == 0 ) ||
    //     ( mcType == Proton   && RichAbovePrThres == 0 ) ) return false;
    return ( hasAerogelInfo() || hasRich1Gas() || hasRich2Gas() );
  }
  /// CALO Ecal info ?
  inline bool hasEcalInfo() const
  {
    return InAccEcal == 1;
  }
  /// CALO Hcal Info ?
  inline bool hasHcalInfo() const
  {
    return InAccHcal == 1;
  }
  /// CALO Prs Info
  inline bool hasPrsInfo() const
  {
    return InAccPrs == 1;
  }
  /// CALO Brem Info ?
  inline bool hasBremInfo() const
  {
    return InAccBrem == 1;
  }

  /// Has Muon Info ?
  inline bool hasMuonInfo() const
  {
    //return MuonInAcc != 0;
    //return MuonInAcc != 0 && ( getMcType() != Muon || MuonIsMuon != 0 );
    return ( getMcType() != Muon || MuonIsMuon != 0 );
  }

  // returns the poison error
  inline double poisError( const double top, const double bot ) const
  {
    return std::sqrt((top/bot)*(1.-top/bot)/bot);
  }

  // test the cut
  bool testCut( const double var, const double cut ) const
  {
    return ( config.cutType == ">" ? var > cut : var < cut );
  }

public :

  /// Utility class to add labels to the eff v pur curves
  class Label
  {
  public:
    Label( double _x, double _y, const std::string & _label )
      : x(_x), y(_y), label(_label) { }
    double x,y;
    std::string label;
  };
  typedef std::vector<Label> Labels;

public: // data

  Configuration   config;

private: // data

  typedef std::map<std::string,TCanvas*> CanvasData;

  CanvasData figures;

  // reset track count
  mutable AcceptanceTracks tksInAcc;

  TTree          *fChain;   //!pointer to the analyzed TTree or TChain
  Int_t           fCurrent; //!current Tree number in a TChain
  TFile          *m_file;

  // Declaration of leaf types
  Double_t        TrackP;
  Double_t        TrackPt;
  Double_t        TrackChi2PerDof;
  Int_t           TrackNumDof;
  Int_t           TrackType;
  Int_t           TrackHistory;
  Double_t        TrackGhostProb;
  Double_t        TrackLikelihood;
  Double_t        TrackCloneDist;
  Double_t        RichDLLe;
  Double_t        RichDLLmu;
  Double_t        RichDLLpi;
  Double_t        RichDLLk;
  Double_t        RichDLLp;
  Double_t        RichDLLbt;
  Bool_t          RichUsedAero;
  Bool_t          RichUsedR1Gas;
  Bool_t          RichUsedR2Gas;
  Bool_t          RichAboveElThres;
  Bool_t          RichAboveMuThres;
  Bool_t          RichAbovePiThres;
  Bool_t          RichAboveKaThres;
  Bool_t          RichAbovePrThres;
  Int_t           RichBestPID;
  Double_t        MuonBkgLL;
  Double_t        MuonMuLL;
  Double_t        MuonNShared;
  Bool_t          MuonIsLooseMuon;
  Bool_t          MuonIsMuon;
  Double_t        MuonInAcc;
  Double_t        InAccSpd;
  Double_t        InAccPrs;
  Double_t        InAccEcal;
  Double_t        InAccHcal;
  Double_t        InAccBrem;
  Double_t        CaloTrMatch;
  Double_t        CaloElectronMatch;
  Double_t        CaloBremMatch;
  Double_t        CaloChargedSpd;
  Double_t        CaloChargedPrs;
  Double_t        CaloChargedEcal;
  Double_t        CaloSpdE;
  Double_t        CaloPrsE;
  Double_t        CaloEcalChi2;
  Double_t        CaloClusChi2;
  Double_t        CaloBremChi2;
  Double_t        CaloEcalE;
  Double_t        CaloHcalE;
  Double_t        CaloTrajectoryL;
  Double_t        EcalPIDe;
  Double_t        HcalPIDe;
  Double_t        PrsPIDe;
  Double_t        BremPIDe;
  Double_t        EcalPIDmu;
  Double_t        HcalPIDmu;
  Double_t        CombDLLe;
  Double_t        CombDLLmu;
  Double_t        CombDLLpi;
  Double_t        CombDLLk;
  Double_t        CombDLLp;
  Double_t        ProbNNe;
  Double_t        ProbNNmu;
  Double_t        ProbNNpi;
  Double_t        ProbNNk;
  Double_t        ProbNNp;
  Double_t        ProbNNghost;
  Double_t        VeloCharge;
  Int_t           MCParticleType;
  Double_t        MCParticleP;
  Double_t        MCParticlePt;
  Double_t        MCVirtualMass;

  // List of branches
  TBranch        *b_TrackP;   //!
  TBranch        *b_TrackPt;   //!
  TBranch        *b_TrackChi2PerDof;   //!
  TBranch        *b_TrackNumDof;   //!
  TBranch        *b_TrackType;   //!
  TBranch        *b_TrackHistory;   //!
  TBranch        *b_TrackGhostProb;   //!
  TBranch        *b_TrackLikelihood;   //!
  TBranch        *b_TrackCloneDist;   //!
  TBranch        *b_RichDLLe;   //!
  TBranch        *b_RichDLLmu;   //!
  TBranch        *b_RichDLLpi;   //!
  TBranch        *b_RichDLLk;   //!
  TBranch        *b_RichDLLp;   //!
  TBranch        *b_RichDLLbt;   //!
  TBranch        *b_RichUsedAero;   //!
  TBranch        *b_RichUsedR1Gas;   //!
  TBranch        *b_RichUsedR2Gas;   //!
  TBranch        *b_RichAboveElThres;   //!
  TBranch        *b_RichAboveMuThres;   //!
  TBranch        *b_RichAbovePiThres;   //!
  TBranch        *b_RichAboveKaThres;   //!
  TBranch        *b_RichAbovePrThres;   //!
  TBranch        *b_RichBestPID;   //!
  TBranch        *b_MuonBkgLL;   //!
  TBranch        *b_MuonMuLL;   //!
  TBranch        *b_MuonNShared;   //!
  TBranch        *b_MuonIsLooseMuon;   //!
  TBranch        *b_MuonIsMuon;   //!
  TBranch        *b_MuonInAcc;   //!
  TBranch        *b_InAccSpd;   //!
  TBranch        *b_InAccPrs;   //!
  TBranch        *b_InAccEcal;   //!
  TBranch        *b_InAccHcal;   //!
  TBranch        *b_InAccBrem;   //!
  TBranch        *b_CaloTrMatch;   //!
  TBranch        *b_CaloElectronMatch;   //!
  TBranch        *b_CaloBremMatch;   //!
  TBranch        *b_CaloChargedSpd;   //!
  TBranch        *b_CaloChargedPrs;   //!
  TBranch        *b_CaloChargedEcal;   //!
  TBranch        *b_CaloSpdE;   //!
  TBranch        *b_CaloPrsE;   //!
  TBranch        *b_CaloEcalChi2;   //!
  TBranch        *b_CaloClusChi2;   //!
  TBranch        *b_CaloBremChi2;   //!
  TBranch        *b_CaloEcalE;   //!
  TBranch        *b_CaloHcalE;   //!
  TBranch        *b_CaloTrajectoryL;   //!
  TBranch        *b_EcalPIDe;   //!
  TBranch        *b_HcalPIDe;   //!
  TBranch        *b_PrsPIDe;   //!
  TBranch        *b_BremPIDe;   //!
  TBranch        *b_EcalPIDmu;   //!
  TBranch        *b_HcalPIDmu;   //!
  TBranch        *b_CombDLLe;   //!
  TBranch        *b_CombDLLmu;   //!
  TBranch        *b_CombDLLpi;   //!
  TBranch        *b_CombDLLk;   //!
  TBranch        *b_CombDLLp;   //!
  TBranch        *b_ProbNNe;   //!
  TBranch        *b_ProbNNmu;   //!
  TBranch        *b_ProbNNpi;   //!
  TBranch        *b_ProbNNk;   //!
  TBranch        *b_ProbNNp;   //!
  TBranch        *b_ProbNNghost;   //!
  TBranch        *b_VeloCharge;   //!
  TBranch        *b_MCParticleType;   //!
  TBranch        *b_MCParticleP;   //!
  TBranch        *b_MCParticlePt;   //!
  TBranch        *b_MCVirtualMass;   //!

};

#endif
