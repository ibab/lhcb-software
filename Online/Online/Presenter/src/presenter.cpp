// $Id: presenter.cpp,v 1.84 2010-10-01 12:54:12 frankb Exp $
// STL
#include <iostream>
#include <fstream>
#include <malloc.h>

// ROOT
#include <TSystem.h>
#include <TSysEvtHandler.h>
#include <TApplication.h>
#include <TStyle.h>
#include <TString.h>
#include <TPRegexp.h>

// boost
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/recursive_mutex.hpp>

// Gaudi
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/AlgTool.h"

// Trending
#include "Trending/ITrendingTool.h"

// Online
#include "OnlineHistDB/OnlineHistDBEnv.h"
#include "OMAlib/OMAlib.h"

// Local
#include "PresenterMainFrame.h"

using namespace boost::program_options;
using namespace boost::filesystem;

// global variables (mutex)
namespace PresenterMutex {
  boost::mutex           listMutex ;
  boost::mutex           archiveMutex ;
  boost::recursive_mutex oraMutex ;
  boost::recursive_mutex dimMutex ;
  boost::recursive_mutex rootMutex ;
};

// global variables (Gaudi tools)
namespace PresenterGaudi {
  ITrendingTool * trendingTool ;
};


void setSystemEnvironment(const char* environmentVariable, const char* value) {
#if defined(_WIN32)
  const size_t envVarLen = strlen(environmentVariable);
  const size_t envVarValue = strlen(value);
  char *tmpEnvVar = (char *)alloca(envVarLen + envVarValue + 2 + !*value);
  memcpy(tmpEnvVar, environmentVariable, envVarLen);
  tmpEnvVar[envVarLen] = '=';
  if (*value) {
    memcpy(tmpEnvVar + envVarLen + 1, value, envVarValue + 1);
  } else {
    tmpEnvVar[envVarLen + 1] = ' ';
    tmpEnvVar[envVarLen + 2] = '\0';
  }

  if (!putenv(tmpEnvVar)) {
  }
#else
  if (!setenv(environmentVariable, value, 1)) {
  }
#endif
}

#define STANDALONE

#ifdef STANDALONE

int main(int argc, char* argv[]) {
  // load trending tool
  ISvcLocator * iface = Gaudi::svcLocator() ;
  IService * isvc ;
  iface -> getService( "ToolSvc" , isvc ) ;
  const IInterface * a3( isvc ) ;
  const std::string & name( "TrendingTool" ) ;
  IAlgTool * intf = AlgTool::Factory::create(name, name, name, a3);
  PresenterGaudi::trendingTool = dynamic_cast< ITrendingTool * >( intf ) ;

  // ROOT startup
  TApplication presenterApp("Presenter", NULL, NULL);
  gROOT->Reset("a");
  gSystem->ResetSignal(kSigBus, true);
  gSystem->ResetSignal(kSigSegmentationViolation, true);
  gSystem->ResetSignal(kSigIllegalInstruction, true);
  gSystem->ResetSignal(kSigFloatingException, true);

  pres::MsgLevel messageLevelCli;
  std::string histdir("./");

  int windowWidth;
  int windowHeight;

  const char* histdirEnv = getenv( "HISTDIR" );
  if (NULL != histdirEnv) {
    histdir = histdirEnv;
    if (!histdir.empty()) histdir.append( pres::s_slash ) ;
  }

  std::string referencePath(histdir+OnlineHistDBEnv_constants::StdRefRoot);
  std::string savesetPath(  histdir+OnlineHistDBEnv_constants::StdSavesetsRoot);
  std::string savesetFile( "" ) ;
  std::string startupPage( "" ) ;

  const char* referencePathEnv = getenv( "HISTREFPATH" );
  if (NULL != referencePathEnv) {
    referencePath = histdir + referencePathEnv;
    if (!referencePath.empty()) referencePath.append( pres::s_slash ) ;
  }

  const char* savesetPathEnv = getenv( "HISTSAVESETSPATH" );
  if (NULL != savesetPathEnv) {
    savesetPath = histdir + savesetPathEnv;
    if (!savesetPath.empty()) savesetPath.append( pres::s_slash );
  }

  try {
    // cli
    options_description cliOptions("General options");
    cliOptions.add_options()
      ("help,h", "display help message")
      ("version,v", "print version information")
      ;
    // cli + .cfg file
    options_description config("Configuration (=default)");
    config.add_options()
      ("mode,M", value<std::string>()->default_value("online"),
       "starting operation mode:\n" "online, history \n" "editor-online editor-offline \n or batch")
      ("login,l", value<std::string>()->default_value("read-only"),
       "\"no\" when not to login to histogram\n" "database on startup\n"
       "read-only or read-write")
      ("window-width,W",  value<int>(&windowWidth)->default_value(1270), "window width")
      ("window-height,H", value<int>(&windowHeight)->default_value(980), "window height")
      ("verbosity,V", value<std::string>()->default_value("silent"),
       "verbosity level:\n" "silent, verbose or debug")
      ("dim-dns-node,D", value<std::string>(), "DIM DNS node name")
      ("reference-path,R", value<std::string>()->default_value(referencePath), "reference path")
      ("saveset-path,S", value<std::string>()->default_value(savesetPath), "saveset path")
      // To open a ROOT file at startup. Set also mode to history to load histo from files.
      ("saveset-file,f", value<std::string>()->default_value("") , "saveset file to open" )
      ("tnsnames-path,O", value<std::string>(), "tnsnames.ora file")
      ("databases,d", value<std::string>()->default_value(s_histdb),"known databases")
      ("database-credentials,k", value<std::string>()->default_value(s_histReaderPair),
       "database credentials")
      ("logbook-settings,L", value<std::string>(), "logbook configuration")
      ("problem-settings,x", value<std::string>(), "Problem Database configuration")
      ("rundb-settings,r", value<std::string>() , "Run Database configuration" )
      ("enable-alarm-display,a", value<bool>()->default_value(true), "enable alarm display")
      ("hide-alarm-list", value<bool>(), "hide alarm list")
      ("config-file,C", value<std::string>(), "configuration file")
      ("key-file,K", value<std::string>(), "TCK list file")
      ("image-path,I", value<std::string>()->default_value(gSystem->TempDirectory()),
       "image dump directory")
      ("partition,P", value<std::string>()->default_value( pres::s_lhcbPartitionName.Data()),
       "partition name")
      ("dump-format,F", value<std::string>()->default_value("png"), "dump format")
      ("startup-page,p" , value<std::string>()->default_value(""), "page to display at startup" )
      ("editing-allowed,E", value<bool>()->default_value( true ), "Allow editing of database" )
      ("offline-context,K", value<bool>()->default_value( false ), "Running in offline context, for file access" )
      ("offline-event-type,Z", value<std::string>()->default_value( "EXPRESS" ), "Event type offline" )
      ("offline-processing,W", value<std::string>()->default_value( "Reco10" ), "Processing version" )
      ("shift-crew,w", value<bool>()->default_value( false ), "Version fo rteh shift crew" )
      ;

    // program argument -> histo list
    options_description hidden("Histogram list");
    hidden.add_options()
      ("startup-histograms,G", value< std::vector<std::string> >(), "startup histograms");

    options_description cmdline_options;
    cmdline_options.add(cliOptions).add(config).add(hidden);

    options_description config_file_options;
    config_file_options.add(config).add(hidden);

    options_description visible("LHCb Online Presenter allowed options");
    visible.add(cliOptions).add(config);

    positional_options_description p;
    p.add("startup-histograms", -1);

    variables_map startupSettings;
    store(command_line_parser(argc, argv).
          options(cmdline_options).positional(p).run(), startupSettings);

    if (startupSettings.count("config-file")) {
      std::ifstream configFile(startupSettings["config-file"].as<std::string>().c_str());
      store(parse_config_file(configFile, config_file_options), startupSettings);
    } else {
      std::ifstream configFile("presenter.cfg");
      store(parse_config_file(configFile, config_file_options), startupSettings);
    }
    notify(startupSettings);

    if (startupSettings.count("help")) {
      std::cout << "Usage: presenter.exe [options] [histogram id(s)]..." << std::endl
                << visible << std::endl
                << "histogram id(s) should be DIM servicenames" << std::endl
                << std::endl;
      exit(EXIT_SUCCESS);
    }

    if (startupSettings.count("version")) {
      std::cout << "LHCb Presenter version: " << s_presenterVersion << std::endl
                << "Histogram Database version: " << OnlineHistDBEnv_constants::version << std::endl
                << "Analysis Library version: " << OMAconstants::version << std::endl
                << "Histogram Database schema: " << OnlineHistDBEnv_constants::DBschema
                << std::endl;
      exit(EXIT_SUCCESS);
    }

    if (startupSettings.count("saveset-path"))
      savesetPath = startupSettings["saveset-path"].as<std::string>();
    if (!exists(path(savesetPath))) {
      std::cout << "error: saveset-path doesn't exist" << std::endl;
      exit(EXIT_FAILURE);
    }

    if (startupSettings.count( "startup-page" ) )
      startupPage = startupSettings["startup-page"].as<std::string>();

    if (startupSettings.count( "saveset-file" ) ) {
      savesetFile = startupSettings["saveset-file"].as< std::string >() ;
      if ( ( "" != savesetFile ) && ( ! exists( path( savesetFile ) ) ) )
        std::cout << "Error: saveset-file doesn't exist" << std::endl ;
    }

    if (startupSettings.count("reference-path"))
      referencePath = startupSettings["reference-path"].as<std::string>();
    if (!exists(path(referencePath))) {
      std::cout << "error: reference-path doesn't exist" << std::endl;
      exit(EXIT_FAILURE);
    }

    if (startupSettings.count("tnsnames-path"))
      setSystemEnvironment(s_tnsAdminEnv.c_str(),
                           startupSettings["tnsnames-path"].as<std::string>().c_str());

    const char* dimDnsNodeEnv = getenv(s_dimDnsNodeEnv.c_str());
    if (startupSettings.count("dim-dns-node")) {
      setSystemEnvironment(s_dimDnsNodeEnv.c_str(),
                           startupSettings["dim-dns-node"].as<std::string>().c_str());
    } else if (NULL != dimDnsNodeEnv) {
      setSystemEnvironment(s_dimDnsNodeEnv.c_str(), dimDnsNodeEnv);
    } else {
      setSystemEnvironment(s_dimDnsNodeEnv.c_str(), "127.0.0.1");
    }
    if (startupSettings.count("window-width")) {
      windowWidth = startupSettings["window-width"].as<int>();
    }
    if (startupSettings.count("window-height")) {
      windowHeight = startupSettings["window-height"].as<int>();
    }

    PresenterMainFrame presenterMainFrame("LHCb Online Presenter",
                                          savesetPath,
                                          referencePath,
                                          10, 10,
                                          windowWidth,
                                          windowHeight);

    presenterMainFrame.setKnownDatabases(startupSettings["databases"].as<std::string>(),
                                         startupSettings["database-credentials"].as<std::string>());


    if (startupSettings.count("problem-settings")) {
      presenterMainFrame.setPbdbConfig(startupSettings["problem-settings"].as<std::string>());
    }

    if ( startupSettings.count( "rundb-settings" ) ) {
      presenterMainFrame.setRundbConfig( startupSettings["rundb-settings"].as<std::string>() ) ;
    }

    if (startupSettings.count("editing-allowed") ) {
      std::cout << "Set editing-allowed to " << startupSettings["editing-allowed"].as<bool>() << std::endl;
      presenterMainFrame.enableEditing(startupSettings["editing-allowed"].as<bool>());
    }
    if ( startupSettings.count( "offline-context" ) ) {
      bool offline = startupSettings["offline-context"].as<bool>();
      std::string processing = startupSettings["offline-processing"].as<std::string>();
      std::string eventType  = startupSettings["offline-event-type"].as<std::string>();

      presenterMainFrame.setOfflineContext( offline, processing, eventType );
    }

    if (startupSettings.count("verbosity")) {
      if ("silent" == startupSettings["verbosity"].as<std::string>()) {
        messageLevelCli = pres::Silent;
      } else if ("verbose" == startupSettings["verbosity"].as<std::string>()) {
        messageLevelCli = pres::Verbose;
      } else if ("debug" == startupSettings["verbosity"].as<std::string>()) {
        messageLevelCli = pres::Debug;
      }
    } else {
      messageLevelCli = pres::Silent;
    }
    presenterMainFrame.setVerbosity(messageLevelCli);

    if (startupSettings.count("mode")) {
      std::cout << "Setting mode = " << startupSettings["mode"].as<std::string>() << std::endl;
      if ("online" == startupSettings["mode"].as<std::string>()) {
        presenterMainFrame.setPresenterMode( pres::Init ) ;
        presenterMainFrame.setPresenterMode( pres::Online ) ;
      } else if ("history" == startupSettings["mode"].as<std::string>()) {
        presenterMainFrame.setPresenterMode( pres::Init ) ;
        presenterMainFrame.setPresenterMode( pres::History ) ;
      } else if ("editor-online" == startupSettings["mode"].as<std::string>()) {
        presenterMainFrame.setPresenterMode( pres::Init ) ;
        presenterMainFrame.setPresenterMode( pres::EditorOnline ) ;
      } else if ("editor-offline" == startupSettings["mode"].as<std::string>()) {
        presenterMainFrame.setPresenterMode( pres::Init ) ;
        presenterMainFrame.setPresenterMode( pres::EditorOffline ) ;
      } else if ("batch" == startupSettings["mode"].as<std::string>()) {
        presenterMainFrame.setPresenterMode( pres::Batch ) ;
      } else {
        std::cout << "warning: invalid mode setting, defaulting to Online" << std::endl;
        presenterMainFrame.setPresenterMode( pres::Init ) ;
        presenterMainFrame.setPresenterMode( pres::Online ) ;
      }
    } else {
      presenterMainFrame.setPresenterMode( pres::Init ) ;
      presenterMainFrame.setPresenterMode( pres::Online ) ;
    }

    if (startupSettings.count("partition")) {
      presenterMainFrame.setPartition(startupSettings["partition"].as<std::string>());
    }

    if (startupSettings.count("shift-crew")) {
      presenterMainFrame.setShiftCrew(startupSettings["shift-crew"].as<bool>());
    }

    //== Database mode
    if (startupSettings.count("login") &&
        ("batch" != startupSettings["mode"].as<std::string>())) {
      if ("no" == startupSettings["login"].as<std::string>()) {
        presenterMainFrame.setDatabaseMode( pres::LoggedOut ) ;
      } else if ("read-write" == startupSettings["login"].as<std::string>()) {
        presenterMainFrame.setDatabaseMode( pres::ReadWrite ) ;
      } else if ("read-only" == startupSettings["login"].as<std::string>()) {
        presenterMainFrame.setDatabaseMode( pres::ReadOnly ) ;
      } else {
        std::cout << "warning: invalid database login setting, defaulting to ReadOnly" << std::endl;
        presenterMainFrame.setDatabaseMode( pres::ReadOnly ) ;
      }
    } else if ("batch" != startupSettings["mode"].as<std::string>()) {
      presenterMainFrame.setDatabaseMode( pres::ReadOnly ) ;
    } else {
      presenterMainFrame.setDatabaseMode( pres::LoggedOut ) ;
    }
    presenterMainFrame.reconfigureGUI(); // reconfigure as we have the database information.

    if (startupSettings.count("logbook-settings"))
      presenterMainFrame.setLogbookConfig(startupSettings["logbook-settings"].as<std::string>());

    if (startupSettings.count("hide-alarm-list") &&
        (true == startupSettings["hide-alarm-list"].as<bool>()))
      presenterMainFrame.toggleShowAlarmList();

    if (startupSettings.count("enable-alarm-display") )
      presenterMainFrame.enableAlarmDisplay(startupSettings["enable-alarm-display"].as<bool>());

    if (startupSettings.count("startup-histograms"))
      presenterMainFrame.setStartupHistograms(startupSettings["startup-histograms"].as< std::vector<std::string> >());

    if ( savesetFile != "" ) {
      presenterMainFrame.setSavesetFileName( savesetFile ) ;
      std::vector<std::string> startupHistograms ;
      startupHistograms.push_back( savesetFile ) ;
      presenterMainFrame.setStartupHistograms( startupHistograms ) ;
      presenterMainFrame.refreshHistogramSvcList( pres::s_withTree ) ;
    }

    if ( ! startupPage.empty() ) {
      presenterMainFrame.setCurrentPageName( startupPage ) ;
      presenterMainFrame.loadSelectedPageFromDB( startupPage , pres::s_startupFile,
                                                 savesetFile ) ;
      if ( presenterMainFrame.presenterMode() == pres::Online ) presenterMainFrame.startPageRefresh();
    }

    if (startupSettings.count("image-path")) {
      if (!exists(path( startupSettings["image-path"].as<std::string>() ))) {
        std::cout << "error: image-path doesn't exist" << std::endl;
        exit(EXIT_FAILURE);
      } else
        presenterMainFrame.setImagePath(startupSettings["image-path"].as<std::string>());
    }

    if (startupSettings.count("dump-format")) {
      if ("png" == startupSettings["dump-format"].as<std::string>() ||
          "root" == startupSettings["dump-format"].as<std::string>() ||
          "eps" == startupSettings["dump-format"].as<std::string>() ||
          "svg" == startupSettings["dump-format"].as<std::string>() ||
          "pdf" == startupSettings["dump-format"].as<std::string>() ||
          "gif" == startupSettings["dump-format"].as<std::string>() ||
          "ps" == startupSettings["dump-format"].as<std::string>() ||
          "C" == startupSettings["dump-format"].as<std::string>()  ) {
        presenterMainFrame.setDumpFormat(startupSettings["dump-format"].as<std::string>());
      }
    }

    if (( pres::Batch != presenterMainFrame.presenterMode( ) ) ) {
      if (gROOT->IsBatch()) {
        std::cout << "error: LHCb Presenter cannot run in batch mode." << std::endl;
        exit(EXIT_FAILURE);
      }
      presenterApp.Run( kTRUE );
    } else {
      if (startupSettings.count("key-file")) {
        std::ifstream keyFile(startupSettings["key-file"].as<std::string>().c_str(), std::ios::in);
        if(!keyFile.is_open()) {
          std::cout << "Please provide a file listing of the services to dump" << std::endl;
          exit(EXIT_FAILURE);
        }
        std::string serviceID;
        std::vector<std::string> startupHistograms;
        while (getline(keyFile, serviceID)) {
          if (!serviceID.empty()) {
            serviceID = serviceID.substr(serviceID.find_first_of( pres::s_underscore),
                                         serviceID.length());
            serviceID = presenterMainFrame.currentPartition() + serviceID;
            std::string* histo = new std::string(serviceID);
            startupHistograms.push_back(*histo);
          }
        }
        keyFile.close();
        presenterMainFrame.setPresenterMode( pres::Init );
        presenterMainFrame.setStartupHistograms(startupHistograms);
      }
      presenterMainFrame.startPageRefresh();
      presenterApp.Run( kTRUE );
    }
  } catch(std::exception& e) {
    std::cout << e.what() << std::endl;
  }
  // Release trending tool
  delete PresenterGaudi::trendingTool ;
  exit(EXIT_SUCCESS);
}
#endif
