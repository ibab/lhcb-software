#include <iostream>
#include <fstream>
#include <iterator>

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

//TODO: ACLiC
#define STANDALONE

#ifdef STANDALONE

int main(int argc, char* argv[])
{
  TApplication presenterApp("Presenter", NULL, NULL);
  gROOT->Reset("a");
  if (gROOT->IsBatch()) {
    std::cout << "error: LHCb Presenter cannot run in batch mode." << std::endl;
    exit(EXIT_FAILURE);
  }

  int windowWidth(1000);
  int windowHeight(600);

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
       "starting operation mode:\n" "online, history or editor")
      ("login", value<std::string>()->default_value("read-only"),
       "\"no\" when not to login to histogram\n" "database on startup\n"
       "read-only or read-write")
      ("window-width,W", value<int>(&windowWidth)->default_value(1000),
       "window width")
      ("window-height,H", value<int>(&windowHeight)->default_value(600),
       "window height")
      ("verbosity,V", value<std::string>()->default_value("silent"),
       "verbosity level:\n" "silent, verbose or debug")
//      ("dim-dns-node,D", value<std::string>(), "DIM DNS node name")
      ("reference-path,R", value<std::string>(), "relative reference path")
      ("saveset-path,S", value<std::string>(), "relative saveset path")
      ("config-file,C", value<std::string>(), "configuration file")
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

    PresenterMainFrame presenterMainFrame("LHCb Online Presenter", 10, 10,
                                          windowWidth,
                                          windowHeight);
    presenterMainFrame.SetIconPixmap(presenter32);

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
      } else if ("editor" == startupSettings["mode"].as<std::string>()) {
        presenterMainFrame.setPresenterMode(Editor);
      } else {
        std::cout << "warning: invalid mode setting, defaulting to Online" << std::endl;
        presenterMainFrame.setPresenterMode(Online);
      }
    } else {
      presenterMainFrame.setPresenterMode(Online);
    }
    std::string groupMountPoint("");
    const char* groupdirEnv = getenv(s_groupdir.c_str());
    if (NULL != groupdirEnv) {
      groupMountPoint = groupdirEnv;
      if (!groupMountPoint.empty()) {
        groupMountPoint.append(s_slash);
        presenterMainFrame.setArchiveRoot(groupMountPoint);
      }
    }
   // TODO: make GROUPDIR effect more visible, echo path on exit
    if (startupSettings.count("saveset-path")) {
      path savesetPath(startupSettings["saveset-path"].as<std::string>());
      if (exists(path(groupMountPoint)/savesetPath)) {
        presenterMainFrame.setSavesetPath(savesetPath.file_string());
      } else {
        std::cout << "error: saveset-path doesn't exist" << std::endl;
        exit(EXIT_FAILURE);
      }
    }
    if (startupSettings.count("reference-path")) {
      path referencePath(startupSettings["reference-path"].as<std::string>());
      if (exists(path(groupMountPoint)/referencePath)) {
        presenterMainFrame.setReferencePath(referencePath.file_string());
      } else {
        std::cout << "error: reference-path doesn't exist" << std::endl;
        exit(EXIT_FAILURE);
      }
    }
    if (startupSettings.count("startup-histograms")) {
      presenterMainFrame.setStartupHistograms(startupSettings["startup-histograms"].as< std::vector<std::string> >());
    }

    presenterMainFrame.setDimDnsNode("");

    presenterApp.Run();
    exit(EXIT_SUCCESS);

  } catch(std::exception& e) {
    std::cout << e.what() << std::endl;
    exit(EXIT_FAILURE);
  }
}
#endif
