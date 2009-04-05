#ifndef PRESENTER_H_
#define PRESENTER_H_

#include <string>
#include <TPRegexp.h>
#include <TString.h>

#ifdef _WIN32
 #define NOMSG
 #define WIN32_LEAN_AND_MEAN
#endif

void setSystemEnvironment(const char* environmentVariable, const char* value);

static const std::string s_presenterVersion("v0r17");
// environment variable for archive mount point (i.e. prefix to paths)
static const std::string s_histdir("HISTDIR");
static const std::string s_referencePath("HISTREFPATH");
static const std::string s_savesetPath("HISTSAVESETSPATH");
static const std::string s_dimDnsNodeEnv("DIM_DNS_NODE");
static const std::string s_tnsAdminEnv("TNS_ADMIN");

// 4 slots+signals: cint hates namespaces
enum FilterCriteria
{
  FoldersAndPages = 0,
  Tasks = 1,
  Subsystems = 2,
  HistogramsWithAnalysis = 3,
  AnalysisHistograms = 4,
  AllHistograms = 5
};

namespace pres
{
  enum MsgLevel
  {
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
    EditorOffline = 3
  };

  static const bool s_withHistograms    = true;
  static const bool s_withoutHistograms = false;
  
  static const bool s_withTree          = true;
  static const bool s_withoutTree       = false;

  static const bool s_overlapping       = true;
  static const bool s_separate          = false;

  static const bool s_checkTreeItems    = true;
  static const bool s_uncheckTreeItems  = false;
  
  static const bool s_timeInterval    = true;
  static const bool s_runInterval     = false;  

  // Conventions (see HistogramIdentifier for parsing)
  static const std::string s_H1D("H1D");
  static const std::string s_H2D("H2D");
  static const std::string s_P1D("P1D");
  static const std::string s_HPD("HPD"); // legacy compat == P1D
  static const std::string s_P2D("P2D");
  static const std::string s_CNT("CNT");
  static const std::string s_gauchocomment("/gauchocomment");
  static const std::string s_hltNodePrefix("HLT");
  static const std::string s_PAGE("PAGE");
  static const std::string s_TASK("TASK");
  static const std::string s_ALGORITHM("ALGORITHM");
  static const std::string s_SET("SET");
  static const std::string s_LEVEL("LEVEL");
  static const std::string s_setSwitch("_$");
  static const std::string s_underscore("_");
  static const std::string s_slash("/"); // also for DB folders...
  
// Move to OMAMessage:
  static const std::string s_ALARM("ALARM");
  static const std::string s_WARNING("WARNING");
  static const std::string s_INFO("INFO");
  
  static const std::string s_adder("Adder");
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

// I guess TPRegexp wrapper is buggy: other pcre-s can match the pattern below (-1 match is OK):
//  static TPRegexp s_histogramUrlRegexp("^(H1D|H2D|P1D|HPD|P2D)?/?([^/_]+)_([^/_]+)_([^/]+)_([^/]+)/([^/]+)/(([^_]+)(_\\$)?(.*))$");
// so let's process the UTGID separately:
// TODO: make this lazier...
//  static TPRegexp s_histogramUrlRegexpEFF("^(MonP1|MonH1F|MonH1D|MonH2F|MonH2D|H1D|H2D|P1D|HPD|P2D)?/?([^/]+)/([^/]+)/([^/]+)/(([^_]+)(_\\$)?(.*))$");
//  static TPRegexp s_histogramUrlRegexp("^(MonP1|MonH1F|MonH1D|MonH2F|MonH2D|H1D|H2D|P1D|HPD|P2D)?/?([^/]+)/([^/]+)/(([^_]+)(_\\$)?(.*))$");
  static TPRegexp s_histogramUrlRegexpEFF("^(MonP1|MonH1D|MonH2D|H1D|H2D|P1D|HPD|P2D)?/?([^/]+)/([^/]+)/([^/]+)/(([^_]+)(_\\$)?(.*))$");
  static TPRegexp s_histogramUrlRegexp("^(MonP1|MonH1D|MonH2D|H1D|H2D|P1D|HPD|P2D)?/?([^/]+)/([^/]+)/(([^_]+)(_\\$)?(.*))$");
//  static TPRegexp s_histogramUTGIDRegexp("^([^/_]+)?_?([^/_]+)_([^/]+)_([^/]+)$");
  static TPRegexp s_histogramUTGIDRegexp("^(([^/_]+)_)([^/_]+)_([^/]+)_([^/]+)$");
  static TPRegexp s_histogramUTGIDRegexpEFF("^([^/_]+)_([^/]+)_([^/]+)$");
//  static TPRegexp s_fileDateRegexp("(.*)-(\\d{8}T\\d{6})(?:-EOR)\\.root$");
 static TPRegexp s_fileDateRegexp("^(.*)-(\\d{8}T\\d{6})(-EOR)?\\.root$");

  // Tunables:
  static const int s_estimatedDimServiceCount = 1000;
  static const int s_estimatedHistosOnPage = 200;
  static const int s_pageRefreshRate = 2000;
  static const int s_maxWindowWidth = 32767;
  static const int s_maxWindowHeight = 32767;

  static const TString s_histReader("HIST_READER");
  static const std::string s_histReaderPw("reader");
  static const TString s_histWriter("HIST_WRITER");
  static const std::string s_histWriterPw("histeggia194");  
  static const TString s_lhcbMonGiacomo("LHCB_MON_GIACOMO");

  static const TString s_oradev10("devdb10");
  static const TString s_histdb("HISTDB");
  static const TString s_shistdb("SHISTDB");
  static const TString s_lhcbPartionName("LHCb");

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
