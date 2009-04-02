#include <iostream>
#include <fstream>
#include <iterator>

#include <malloc.h>

#include <TSystem.h>
#include <TSysEvtHandler.h>
#include <TROOT.h>
#include <TApplication.h>
#include <TStyle.h>
#include <TString.h>
#include <TPRegexp.h>

#include "presenter.h"
#include "PresenterMainFrame.h"
#include "OnlineHistDB/OnlineHistDBEnv.h"
#include "OMAlib/OMAlib.h"

#include "../icons/presenter32.xpm"

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
  if (gROOT->IsBatch()) {
    std::cout << "error: LHCb Presenter cannot run in batch mode." << std::endl;
    exit(EXIT_FAILURE);
  }
  
  int windowWidth(1000);
  int windowHeight(600);
  int titleFontSize(0);

  std::string histdir("./");
  
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
       "starting operation mode:\n" "online, history or \n" "editor-online editor-offline")
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
      ("config-file,C", value<std::string>(), "configuration file")
      ("title-font-size,F", value<int>(&titleFontSize),
       "title font size for plots")      
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
    
    PresenterMainFrame presenterMainFrame("LHCb Online Presenter",
                                          savesetPath,
                                          referencePath,
                                          10, 10,
                                          windowWidth,
                                          windowHeight);
    presenterMainFrame.SetIconPixmap((char**)presenter32);

    if (startupSettings.count("verbosity")) {
      if ("silent" == startupSettings["verbosity"].as<std::string>()) {
        presenterMainFrame.setVerbosity(Silent);
      } else if ("verbose" == startupSettings["verbosity"].as<std::string>()) {
        presenterMainFrame.setVerbosity(Verbose);
      } else if ("debug" == startupSettings["verbosity"].as<std::string>()) {
        presenterMainFrame.setVerbosity(Debug);
      }
    } else {
      presenterMainFrame.setVerbosity(Silent);
    }
    if (startupSettings.count("login")) {
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
    } else {
      presenterMainFrame.setDatabaseMode(ReadOnly);
    }
    if (startupSettings.count("mode")) {
      if ("online" == startupSettings["mode"].as<std::string>()) {
        presenterMainFrame.setPresenterMode(Online);
      } else if ("history" == startupSettings["mode"].as<std::string>()) {
        presenterMainFrame.setPresenterMode(History);
      } else if ("editor-online" == startupSettings["mode"].as<std::string>()) {
        presenterMainFrame.setPresenterMode(EditorOnline);
      } else if ("editor-offline" == startupSettings["mode"].as<std::string>()) {
        presenterMainFrame.setPresenterMode(EditorOffline);        
      } else {
        std::cout << "warning: invalid mode setting, defaulting to Online" << std::endl;
        presenterMainFrame.setPresenterMode(Online);
      }
    } else {
      presenterMainFrame.setPresenterMode(Online);
    }


    if (startupSettings.count("title-font-size")) {
      presenterMainFrame.setTitleFontSize(startupSettings["title-font-size"].as<int>());
    }    
    if (startupSettings.count("startup-histograms")) {
      presenterMainFrame.setStartupHistograms(startupSettings["startup-histograms"].as< std::vector<std::string> >());
    }

    presenterApp.Run();
    exit(EXIT_SUCCESS);

  } catch(std::exception& e) {
    std::cout << e.what() << std::endl;
    exit(EXIT_FAILURE);
  }
}
#endif
