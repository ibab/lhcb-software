// $Id: presenter.h,v 1.109 2010-10-26 18:45:29 robbep Exp $
#ifndef PRESENTER_H_
#define PRESENTER_H_

#include <string>

#ifdef WIN32
#pragma warning( push )
#pragma warning( disable : 4800 )
#endif
#include <TPRegexp.h>
#include <TString.h>
#include <TROOT.h>
#ifdef WIN32
#pragma warning( pop )
#endif

#ifdef _WIN32
 #define NOMSG
 #define WIN32_LEAN_AND_MEAN
#endif

void setSystemEnvironment(const char* environmentVariable, const char* value);

static const std::string s_presenterVersion("v2r4");
// environment variable for archive mount point (i.e. prefix to paths)
static const std::string s_configToken(";");

static const std::string s_histdb("HISTDB");
static const std::string s_histReader("HIST_READER");
static const std::string s_histWriter("HIST_WRITER");
static const std::string s_histReaderPair("HIST_READER:reader");

static const std::string s_dimDnsNodeEnv("DIM_DNS_NODE");
static const std::string s_tnsAdminEnv("TNS_ADMIN");

// 4 slots+signals: cint hates namespaces
enum FilterCriteria {
  FoldersAndPages = 0,
  Tasks = 1,
  Subsystems = 2,
  HistogramsWithAnalysis = 3,
  AnalysisHistograms = 4,
  AllHistograms = 5
};
 

namespace pres {
  enum MsgLevel {
    Silent  = 0,
    Verbose = 1,
    Debug   = 2
  };

  enum ExceptionLevel {
    NoExceptions     = 0,
    SevereExceptions = 1,
    AllExceptions    = 2
  };

  enum DatabaseMode {
    ReadOnly = 0,
    ReadWrite = 1,
    LoggedOut = 2,
    UserSelected = 3
  };

  enum PresenterMode {
    Online = 0,
    History = 1,
    EditorOnline = 2,
    EditorOffline = 3,
    Batch = 4,
    Init = 5
  };
  
  enum DisplayMode {
    None =0,
    Page = 1,
    Alarm = 2
  };


  enum SavesetType {
    OnlineFile = 0,
    OfflineFile = 1
  };
  
  enum EffServiceType {
    MonRate = 0,
    TCKinfo = 1
  };
  
  enum ServicePlotMode {
    separate = 0,
    overlap = 1,
    invisible = 2
  };

  static const bool s_withHistograms    = true;
  static const bool s_withoutHistograms = false;
  
  static const bool s_withTree          = true;
  static const bool s_withoutTree       = false;

  static const bool s_checkTreeItems    = true;
  static const bool s_uncheckTreeItems  = false;
  
  static const bool s_timeInterval      = true;
  static const bool s_runInterval       = false;

  static const std::string s_FILE_URI("file://");
  
  static const std::string s_DimWildcard("*");
  static const std::string s_Mon("Mon");

//  The format parameter specifies the contents of the structure in the form
//  T:N[;T:N]*[;T] where T is the item type:
//  (I)nteger, (C)arachter, (L)ong, (S)hort, (F)loat, (D)ouble,
//  X(tra long) and N is the number of such items.
//  The type alone at the end means all following items are of the same type.
//  Example: "I:3;F:2;C" means 3 Integers, 2 Floats and Characters until the end.


  static const std::string s_DimInteger("I");
  static const std::string s_DimCharacter("C");
  static const std::string s_DimLong("L");
  static const std::string s_DimShort("S");
  static const std::string s_DimFloat("F");
  static const std::string s_DimDouble("D");

//  static TPRegexp s_DimCNTRegexp("^([ILFD])(?:(:\\d+))?");
  static TPRegexp s_DimHltCntRegexp("^D:2;C");
  static TPRegexp s_DimHltTckRegexp("^I:1;C");

  static const std::string s_hltNodePrefix("HLT");
  static const std::string s_PAGE("PAGE");
  static const std::string s_TASK("TASK");
  static const std::string s_ALGORITHM("ALGORITHM");
  static const std::string s_SET("SET");
  static const std::string s_LEVEL("LEVEL");
  static const std::string s_setSwitch("_$");
  static const std::string s_underscore("_");
  static const std::string s_slash("/"); // also for DB folders...
  static const std::string s_byRunDir("ByRun");  

// Move to OMAMessage:
  static const std::string s_ALARM("ALARM");
  static const std::string s_WARNING("WARNING");
  static const std::string s_INFO("INFO");
  static const std::string s_TREND("TRE");
  
  static const std::string s_adder("Adder");
  static const std::string s_eff_monRate("monRate");
  static const std::string s_eff_MonitorSvc("MonitorSvc");
  static const std::string s_eff_TCK("TCK");
  static const std::string s_eff_init("Initialising...");
  static const std::string s_default_tck("default");
  static const std::string s_eor("-EOR");
  static const std::string s_efftask("GauchoJob");

  // boost::filesystem::slash

// ^(H1D|H2D|P1D|HPD|P2D|CNT)?/?([^/_]+_)?([^/_]*)(_[^/]*)?/([^/]*)/(([^_]*)(_\\$)?(.*))$
// w partition: ^(H1D|H2D|P1D|HPD|P2D|CNT)?/?([^/_]+_)?([^/_]+_)?([^/_]*)(_[^/]*)?/([^/]*)/(([^_]*)(_\\$)?(.*))$
// test cases:
//  CC-PC/CCPCAlg/TELL1Mult3_$T3/L2/Q1/myTell1
//  H1D/node00101_taskname_1/CCPCAlg/TELL1Mult3_$T3/L2/Q1/myTell1
//  H2D/node00101_taskname_1/CCPCAlg/TELL1Mult3_$T3/L2/Q1/myTell1
//  CC-PC/CCPCAlg/TELL1Mult1_$T3/L2/Q0/myTell1
//  CC-PC/er_CCPCAlg/TELL1Mult1_$T3/L2/Q0/myTell1
//  CC-PC/CCPCAlg/TELL1Mult1_1//sdf/sf/sCC-PC/CCPCAlg/myTell1
//  CC-PC/CCPCAlg/T3/L2/Q1/myTell1
//  H2D/RichOnlAna/Ana_R21PixelRC/His_$tory_0
//  H2D/RichiOnlAna/Ana_R20PixelRC/Histo/ry_diff_0
//  H2D/LHCb_MONA0805_L0CaloDAQMon_00/L0CaloDecoderMonitoring/L0CaloDecoderMonitoring/10001
//  H2D/LHCb_MONA0805_L0CaloDAQMon_00/L0CaloDecoderMonitoring/L0CaloDecoderMonitoring/10002
//  H2D/LHCb_MONA0805_L0CaloDAQMon_00/L0CaloDecoderMonitoring/L0CaloDecoderMonitoring/10003
//  H2D/LHCb_MONA0805_L0CaloDAQMon_00/L0CaloDecoderMonitoring/L0CaloDecoderMonitoring/10004
//  H2D/LHCb_MONA0805_L0CaloDAQMon_00/L0CaloDecoderMonitoring/L0CaloDecoderMonitoring/10005
//  H2D/LHCb_MONA0805_L0CaloDAQMon_00/L0CaloEmulatorMonitoring/L0CaloEmulatorMonitoring/10001
//  H2D/LHCb_MONA0805_L0CaloDAQMon_00/L0CaloEmulatorMonitoring/L0CaloEmulatorMonitoring/10002
//  H2D/LHCb_MONA0805_L0CaloDAQMon_00/L0CaloEmulatorMonitoring/L0CaloEmulatorMonitoring/10003
//  H2D/LHCb_MONA0805_L0CaloDAQMon_00/L0CaloEmulatorMonitoring/L0CaloEmulatorMonitoring/10004
//  Partition_Adder_1/GauchoJob/MonitorSvc/monRate/Hlt1Hlt1L0GlobalPi0DecisionLineAccepts

// I guess TPRegexp wrapper is buggy: other pcre-s can match the pattern below (-1 match is OK):
// so let's process the UTGID separately: ^([^_/]+)_([^_/]+)_([^_/]+)_?([^_/]+)?$
// OK
  static TPRegexp s_histogramUrlRegexp("^(MonP1|MonH1D|MonH2D|H1D|H2D|P1D|HPD|P2D|CNT)?/?([^/]+)/([^/]+)/(([^/]+)/?(.*))$");
  
//  RICH1_Adder_1
//  HLTA0101_Adder_1
//  LHCb_MONA0805_L0CaloDAQMon_00  
  static TPRegexp s_histogramUTGIDRegexp("^([^_/]+)_([^_/]+)_([^_/]+)_?([^_/]+)?$");  
  
  static TPRegexp s_histogramNameRegexp("^([^_]*)(_\\$)?(.*)$");

  // MuonDAQMon-20091010T113224.root
  // MuonDAQMon-345-20091010T113224.root
//  static TPRegexp s_fileDateRegexp("^(.*)-(\\d{8}T\\d{6})(-EOR)?\\.root$");
  static TPRegexp s_fileDateRegexp("^([^-]+)(-\\d+)?-(\\d{8}T\\d{6})((-EOR)?)\\.root$");
  static TPRegexp s_fileRunRegexp("^(.+)-run(\\d+)\\.root$");
 
//  Brunel_45044_00004599_00000034_1_Hist.root
//  Brunel_Ex_45044_00004598_00000003_1_Hist.root
//  DaVinci_45044_00004599.root
//  Brunel_45044_00004599.root
//  DaVinci_45044_00004599_00000034_2_Hist.root
//  DaVinci_Ex_45044_00004598_00000003_2_Hist.root
//  DaVinci_FULL_45256_00004636_00001476_2_Hist.root
//  DaVinci_FULL_47933_00004636_Merged.root
//  Brunel_EX_48014_00004633_00000018_1_Hist.root
//  Brunel_FULL_44878_00004636_00001223_1_Hist.root
//  Brunel_FULL_45027_00004636_Merged.root
//  Brunel_FULL_45055_00004636_00001229_1_Hist.root

//  static TPRegexp s_offlineJobRegexp("^([^_]+)_(Ex)?_?(\\d+)_(\\d+)_?(\\d+)?_?(\\d+)?_?(Hist)?\\.root$");
  static TPRegexp s_offlineJobRegexp("^(Brunel|DaVinci|Boole|Gauss|BrunelDaVinci)(_.+|)\\.root$"); 

  // Tunables:
  static const int s_estimatedDimServiceCount = 1000;
  static const int s_estimatedHistosOnPage = 200;
  static const int s_pageRefreshRate = 10000;  // 10 seconds...
  static const int s_maxWindowWidth = 32767;
  static const int s_maxWindowHeight = 32767;
  static const int s_maxPageNameLength = 1024;

  static const int s_defStatOptions = 1110;

  static const TString s_lhcbPartitionName("LHCb");

  static const std::string s_rootFileExtension(".root");
  static const std::string s_savesetToken("-");
  
  static const std::string s_Area("AREA");
  static const std::string s_Entries("ENTR");
  static const std::string s_NoReference("NOREF");
  static const std::string s_NoNormalization("NONE");
  
  static const std::string s_Now("Now");
  static const std::string s_startupFile("startupFile");
    
}
#endif /*PRESENTER_H_*/
