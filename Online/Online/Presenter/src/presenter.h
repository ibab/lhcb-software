#ifndef PRESENTER_H_
#define PRESENTER_H_

#include <string>
#include <TPRegexp.h>
#include <TString.h>

static const std::string PRESENTER_VERSION("v0r6");
// environment variable for archive mount point (i.e. prefix to paths)
static const std::string GROUPDIR("GROUPDIR");

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
    Editor = 2,
  };

  static const bool WithHistograms    = true;
  static const bool WithoutHistograms = false;

  static const bool CheckTreeItems    = true;
  static const bool UncheckTreeItems  = false;

  // Conventions (see HistogramIdentifier for parsing)
  static const std::string H1D("H1D");
  static const std::string H2D("H2D");
  static const std::string P1D("P1D");
  static const std::string HPD("HPD"); // legacy compat == P1D
  static const std::string P2D("P2D");
  static const std::string CNT("CNT");
  static const std::string GAUCHOCOMMENT("/gauchocomment");
  static const std::string PAGE("PAGE");
  static const std::string TASK("TASK");
  static const std::string ALGORITHM("ALGORITHM");
  static const std::string SET("SET");
  static const std::string LEVEL("LEVEL");
  static const std::string SET_SWITCH("_$");
  static const std::string Slash("/"); // also for DB folders...
  // boost::filesystem::slash

// ^(H1D|H2D|P1D|HPD|P2D|CNT)?/?([^/_]+_)?([^/_]*)(_[^/]*)?/([^/]*)/(([^_]*)(_\\$)?(.*))$
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
  static TPRegexp HistogramUrlRegexp("^(H1D|H2D|P1D|HPD|P2D|CNT)?/?([^/_]+_)?([^/_]*)(_[^/]*)?/([^/]*)/(([^_]*)(_\\$)?(.*))$");

  static TPRegexp FileDateRegexp("(.*)-(\\d{8}T\\d{6})\\.root$");

  // Tunables:
  static const int EstimatedDimServiceCount = 1000;
  static const int EstimatedHistosOnPage = 200;
  static const int TimeoutOfMainTimer = 2000;
  static const int MaxWindowWidth = 32767;
  static const int MaxWindowHeight = 32767;

  static const TString HIST_READER("HIST_READER");
  static const std::string HIST_READER_KRED("reader");
  static const TString HIST_WRITER("HIST_WRITER");
  static const TString LHCB_MON_GIACOMO("LHCB_MON_GIACOMO");
  static const std::string HIST_WRITER_KRED("histeggia194");

  static const TString LBORA01("lbora01:1528/HISTOGRAMDB");
  static const TString ORADEV10("oradev10.cern.ch:10520/D10");
  static const TString HISTDB("HISTDB");

  static const std::string RootFileExtension(".root");
  static const std::string SavesetToken("-");
}
#endif /*PRESENTER_H_*/
