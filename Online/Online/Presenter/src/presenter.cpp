#include <iostream>
#include <fstream>
#include <iterator>

#include <malloc.h>

#include <TSystem.h>
#include <TSysEvtHandler.h>
#include <TApplication.h>
#include <TStyle.h>
#include <TString.h>
#include <TPRegexp.h>

//#include "presenter.h"
#include "PresenterMainFrame.h"
#include "OnlineHistDB/OnlineHistDBEnv.h"
//#include "OnlineHistDB/OnlineHistogram.h"
//#include "ParallelWait.h"
#include "OMAlib/OMAlib.h"
//#include "HistogramIdentifier.h"
//#include "dim/dic.hxx"
//#include "OnlineHistDB/OnlineHistDB.h"
//#include "OMAlib/OMAlib.h"
//#include "DbRootHist.h"

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

using namespace pres;
using namespace boost::program_options;
using namespace boost::filesystem;

void setSystemEnvironment(const char* environmentVariable, const char* value) {
// inspired by WhateverBox, innotek/Sun Microsystems/Whomever
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
//  return 0;
//  return errno;
}
#else
if (!setenv(environmentVariable, value, 1)) {
//  return ok;
//  return errno;
}
#endif
}

//TODO: ACLiC
#define STANDALONE

#ifdef STANDALONE

int main(int argc, char* argv[])
{
  TApplication presenterApp("Presenter", NULL, NULL);
  gROOT->Reset("a");
  gSystem->ResetSignal(kSigBus, true);
  gSystem->ResetSignal(kSigSegmentationViolation, true);
  gSystem->ResetSignal(kSigIllegalInstruction, true);
  gSystem->ResetSignal(kSigFloatingException, true);

  pres::MsgLevel messageLevelCli;
  std::string histdir("./");
  
  int windowWidth(1000);
  int windowHeight(600);  
  
  const char* histdirEnv = getenv(s_histdir.c_str());
  if (NULL != histdirEnv) {
    histdir = histdirEnv;
    if (!histdir.empty()) {
      histdir.append(s_slash);
    }
  }
  
  std::string referencePath(histdir+OnlineHistDBEnv_constants::StdRefRoot);
  std::string savesetPath(histdir+OnlineHistDBEnv_constants::StdSavesetsRoot);
    
  const char* referencePathEnv = getenv(s_referencePath.c_str());
  if (NULL != referencePathEnv) {
    referencePath = histdir + referencePathEnv;
    if (!referencePath.empty()) {
      referencePath.append(s_slash);
    }
  } else {
//    referencePath = ".";
  }
  
  const char* savesetPathEnv = getenv(s_savesetPath.c_str());
  if (NULL != savesetPathEnv) {
    savesetPath = histdir + savesetPathEnv;
    if (!savesetPath.empty()) {
      savesetPath.append(s_slash);
    }
  } else {
//    savesetPath = ".";
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
      ("login", value<std::string>()->default_value("read-only"),
       "\"no\" when not to login to histogram\n" "database on startup\n"
       "read-only or read-write")
      ("window-width,W", value<int>(&windowWidth)->default_value(1000),
       "window width")
      ("window-height,H", value<int>(&windowHeight)->default_value(600),
       "window height")
      ("verbosity,V", value<std::string>()->default_value("silent"),
       "verbosity level:\n" "silent, verbose or debug")
      ("dim-dns-node,D", value<std::string>(), "DIM DNS node name")
      ("reference-path,R", value<std::string>()->default_value(referencePath), "reference path")
      ("saveset-path,S", value<std::string>()->default_value(savesetPath), "saveset path")
      ("tnsnames-path,O", value<std::string>(), "tnsnames.ora file")
      ("databases,DB", value<std::string>()->default_value(s_histdb),"known databases")
      ("database-credentials,DBC", value<std::string>()->default_value(s_histReaderPair),"database credentials")
      ("logbook-settings,L", value<std::string>(), "logbook configuration")
      ("hide-alarm-list,A", value<bool>(), "hide alarm list")
      ("config-file,C", value<std::string>(), "configuration file")
      ("key-file,K", value<std::string>(), "TCK list file")
      ("image-path,I", value<std::string>()->default_value(gSystem->TempDirectory()), "image dump directory")
      ("partition,P", value<std::string>()->default_value(s_lhcbPartionName.Data()), "partition name")
      ("dump-format,F", value<std::string>()->default_value("png"), "dump format")
    ;

    // program argument -> histo list
    options_description hidden("Histogram list");
    hidden.add_options()
     ("startup-histograms,D", value< std::vector<std::string> >(),
      "startup histograms")
    ;
      
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
      ifstream configFile(startupSettings["config-file"].as<std::string>().c_str());
      store(parse_config_file(configFile, config_file_options), startupSettings);
    } else {
      ifstream configFile("presenter.cfg");
      store(parse_config_file(configFile, config_file_options), startupSettings);
    }
    notify(startupSettings);

    if (startupSettings.count("help")) {
      std::cout << "Usage: presenter.exe [options] [histogram id(s)]..." << std::endl
       << visible << std::endl
       << "histogram id(s) should be DIM servicenames" << std::endl
       << std::endl
       << "For bug reporting instructions, please see:" << std::endl
       << "<URL:http://lhcb-online.web.cern.ch/lhcb-online/dataquality/default.html>." << std::endl
       << "For Presenter specific bug reporting instructions, please see:" << std::endl
       << "<URL:http://lhcb-online.web.cern.ch/lhcb-online/dataquality/presenter/default.htm>." << std::endl;
      exit(EXIT_SUCCESS);
    }

    if (startupSettings.count("version")) {
      std::cout << "LHCb Presenter version: " << s_presenterVersion << std::endl <<
              "Histogram Database version: " << OnlineHistDBEnv_constants::version << std::endl <<
              "Analysis Library version: " << OMAconstants::version << std::endl <<
              "Histogram Database schema: " << OnlineHistDBEnv_constants::DBschema << std::endl;
      exit(EXIT_SUCCESS);
    }
    
    if (startupSettings.count("saveset-path")) {
      savesetPath = startupSettings["saveset-path"].as<std::string>();
    }
    
    if (!exists(path(savesetPath))) {
      std::cout << "error: saveset-path doesn't exist" << std::endl;
      exit(EXIT_FAILURE);
    }

    if (startupSettings.count("reference-path")) {      
      referencePath = startupSettings["reference-path"].as<std::string>();
    }

    if (!exists(path(referencePath))) {
      std::cout << "error: reference-path doesn't exist" << std::endl;
      exit(EXIT_FAILURE);
    }

    if (startupSettings.count("tnsnames-path")) {
      setSystemEnvironment(s_tnsAdminEnv.c_str(),
             startupSettings["tnsnames-path"].as<std::string>().c_str());
    }

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

    if (startupSettings.count("mode")) {
      if ("online" == startupSettings["mode"].as<std::string>()) {
        presenterMainFrame.setPresenterMode(Init);  
        presenterMainFrame.setPresenterMode(Online);
      } else if ("history" == startupSettings["mode"].as<std::string>()) {
        presenterMainFrame.setPresenterMode(Init);
        presenterMainFrame.setPresenterMode(History);
      } else if ("editor-online" == startupSettings["mode"].as<std::string>()) {
        presenterMainFrame.setPresenterMode(Init);        
        presenterMainFrame.setPresenterMode(EditorOnline);
      } else if ("editor-offline" == startupSettings["mode"].as<std::string>()) {
        presenterMainFrame.setPresenterMode(Init);        
        presenterMainFrame.setPresenterMode(EditorOffline);        
      } else if ("batch" == startupSettings["mode"].as<std::string>()) {        
        presenterMainFrame.setPresenterMode(Batch);        
      } else {
        std::cout << "warning: invalid mode setting, defaulting to Online" << std::endl;
        presenterMainFrame.setPresenterMode(Init);    
        presenterMainFrame.setPresenterMode(Online);
      }
    } else {
      presenterMainFrame.setPresenterMode(Init);      
      presenterMainFrame.setPresenterMode(Online);
    }

    if (startupSettings.count("verbosity")) {
      if ("silent" == startupSettings["verbosity"].as<std::string>()) {
        messageLevelCli = Silent;
      } else if ("verbose" == startupSettings["verbosity"].as<std::string>()) {
        messageLevelCli = Verbose;        
      } else if ("debug" == startupSettings["verbosity"].as<std::string>()) {
        messageLevelCli = Debug;        
      }
    } else {
      messageLevelCli = Silent;
    }
    presenterMainFrame.setVerbosity(messageLevelCli);
    
    if (startupSettings.count("partition")) {
      presenterMainFrame.setPartition(startupSettings["partition"].as<std::string>());
    }    


//TODO: remove batch escape when DB                                             
      if (startupSettings.count("login") &&
        ("batch" != startupSettings["mode"].as<std::string>())) {
      if ("no" == startupSettings["login"].as<std::string>()) {
        presenterMainFrame.setDatabaseMode(LoggedOut);
      } else if ("read-write" == startupSettings["login"].as<std::string>()) {
        presenterMainFrame.setDatabaseMode(ReadWrite);
      } else if ("read-only" == startupSettings["login"].as<std::string>()) {
        presenterMainFrame.setDatabaseMode(ReadOnly);
      } else {
        std::cout << "warning: invalid database login setting, defaulting to ReadOnly" << std::endl;
        presenterMainFrame.setDatabaseMode(ReadOnly);
      }
    } else if ("batch" != startupSettings["mode"].as<std::string>()) {
      presenterMainFrame.setDatabaseMode(ReadOnly);
    } else {
      presenterMainFrame.setDatabaseMode(LoggedOut);
    }

    
    if (startupSettings.count("logbook-settings")) {
      presenterMainFrame.setLogbookConfig(startupSettings["logbook-settings"].as<std::string>());
    }

    if (startupSettings.count("hide-alarm-list") &&
        (true == startupSettings["hide-alarm-list"].as<bool>())) {
        presenterMainFrame.toggleShowAlarmList();
    }

    if (startupSettings.count("startup-histograms")) {
      presenterMainFrame.setStartupHistograms(startupSettings["startup-histograms"].as< std::vector<std::string> >());
    }

    if (startupSettings.count("image-path")) {
      if (!exists(path(savesetPath))) {
        std::cout << "error: image-path doesn't exist" << std::endl;
        exit(EXIT_FAILURE);
      } else {
        presenterMainFrame.setImagePath(startupSettings["image-path"].as<std::string>());
      }
    }

    if (startupSettings.count("dump-format")) {
      if ("png" == startupSettings["dump-format"].as<std::string>() ||
          "root" == startupSettings["dump-format"].as<std::string>() ||
          "eps" == startupSettings["dump-format"].as<std::string>() ||
          "svg" == startupSettings["dump-format"].as<std::string>() ||
          "pdf" == startupSettings["dump-format"].as<std::string>() ||
          "C" == startupSettings["dump-format"].as<std::string>()  ) {
        presenterMainFrame.setDumpFormat(startupSettings["dump-format"].as<std::string>());
      } 
    }

    if ((Batch != presenterMainFrame.presenterMode())) {
      if (gROOT->IsBatch()) {
        std::cout << "error: LHCb Presenter cannot run in batch mode." << std::endl;
        exit(EXIT_FAILURE);
      }
      presenterApp.Run();
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
            serviceID = serviceID.substr(serviceID.find_first_of(s_underscore),
                                         serviceID.length());
            serviceID = presenterMainFrame.currentPartition() + serviceID;
            std::string* histo = new std::string(serviceID);
            startupHistograms.push_back(*histo);
          }
        }
        keyFile.close();
        presenterMainFrame.setPresenterMode(Init);
        presenterMainFrame.setStartupHistograms(startupHistograms); 
      }
      presenterMainFrame.startPageRefresh();
      presenterApp.Run();
    }
  exit(EXIT_SUCCESS);
  } catch(std::exception& e) {
    std::cout << e.what() << std::endl;
    exit(EXIT_FAILURE);
  }
}
#endif
