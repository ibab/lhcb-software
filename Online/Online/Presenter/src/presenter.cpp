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
    cout << "error: LHCb Presenter cannot run in batch mode." << endl;
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
      ("mode,M", value<string>()->default_value("online"),
       "starting operation mode:\n" "online, history or editor")
      ("login", value<string>()->default_value("read-only"),
       "\"no\" when not to login to histogram\n" "database on startup\n"
       "read-only or read-write")
      ("window-width,W", value<int>(&windowWidth)->default_value(1000),
       "window width")
      ("window-height,H", value<int>(&windowHeight)->default_value(600),
       "window height")
      ("verbosity,V", value<string>()->default_value("silent"),
       "verbosity level:\n" "silent, verbose or debug")
      ("dim-dns-node,D", value<string>(), "DIM DNS node name")
      ("reference-path,R", value<string>(), "relative reference path")
      ("saveset-path,S", value<string>(), "relative saveset path")
      ("config-file,C", value<string>(), "configuration file")
    ;

    // program argument -> histo list
    options_description hidden("Histogram list");
    hidden.add_options()
     ("startup-histograms,D", value< vector<string> >(),
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
      ifstream configFile(startupSettings["config-file"].as<string>().c_str());
      store(parse_config_file(configFile, config_file_options), startupSettings);
    } else {
      ifstream configFile("presenter.cfg");
      store(parse_config_file(configFile, config_file_options), startupSettings);
    }
    notify(startupSettings);

    if (startupSettings.count("help")) {
      cout << "Usage: presenter.exe [options] [histogram id(s)]..." << endl
       << visible << endl
       << "histogram id(s) should be DIM servicenames" << endl
       << endl
       << "For bug reporting instructions, please see:" << endl
       << "<URL:http://lhcb-online.web.cern.ch/lhcb-online/dataquality/default.html>." << endl
       << "For Presenter specific bug reporting instructions, please see:" << endl
       << "<URL:http://lhcb-online.web.cern.ch/lhcb-online/dataquality/presenter/default.htm>." << endl;
      exit(EXIT_SUCCESS);
    }

    if (startupSettings.count("version")) {
      cout << "LHCb Presenter version: " << s_presenterVersion << endl <<
              "Histogram Database version: " << OnlineHistDBEnv_constants::version << endl <<
              "Analysis Library version: " << OMAconstants::version << endl <<
              "Histogram Database schema: " << OnlineHistDBEnv_constants::DBschema << endl;
      exit(EXIT_SUCCESS);
    }

    PresenterMainFrame presenterMainFrame("LHCb Online Presenter", 10, 10,
                                          windowWidth,
                                          windowHeight);
    presenterMainFrame.SetIconPixmap(presenter32);

    if (startupSettings.count("verbosity")) {
      if ("silent" == startupSettings["verbosity"].as<string>()) {
        presenterMainFrame.setVerbosity(Silent);
      } else if ("verbose" == startupSettings["verbosity"].as<string>()) {
        presenterMainFrame.setVerbosity(Verbose);
      } else if ("debug" == startupSettings["verbosity"].as<string>()) {
        presenterMainFrame.setVerbosity(Debug);
      }
    } else {
      presenterMainFrame.setVerbosity(Silent);
    }
    if (startupSettings.count("login")) {
      if ("no" == startupSettings["login"].as<string>()) {
        presenterMainFrame.setDatabaseMode(LoggedOut);
      } else if ("read-write" == startupSettings["login"].as<string>()) {
        presenterMainFrame.setDatabaseMode(ReadWrite);
      } else if ("read-only" == startupSettings["login"].as<string>()) {
        presenterMainFrame.setDatabaseMode(ReadOnly);
      } else {
        cout << "warning: invalid database login setting, defaulting to ReadOnly" << endl;
        presenterMainFrame.setDatabaseMode(ReadOnly);
      }
    } else {
      presenterMainFrame.setDatabaseMode(ReadOnly);
    }
    if (startupSettings.count("mode")) {
      if ("online" == startupSettings["mode"].as<string>()) {
        presenterMainFrame.setPresenterMode(Online);
      } else if ("history" == startupSettings["mode"].as<string>()) {
        presenterMainFrame.setPresenterMode(History);
      } else if ("editor" == startupSettings["mode"].as<string>()) {
        presenterMainFrame.setPresenterMode(Editor);
      } else {
        cout << "warning: invalid mode setting, defaulting to Online" << endl;
        presenterMainFrame.setPresenterMode(Online);
      }
    } else {
      presenterMainFrame.setPresenterMode(Online);
    }
    string groupMountPoint("");
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
      path savesetPath(startupSettings["saveset-path"].as<string>());
      if (exists(path(groupMountPoint)/savesetPath)) {
        presenterMainFrame.setSavesetPath(savesetPath.file_string());
      } else {
        cout << "error: saveset-path doesn't exist" << endl;
        exit(EXIT_FAILURE);
      }
    }
    if (startupSettings.count("reference-path")) {
      path referencePath(startupSettings["reference-path"].as<string>());
      if (exists(path(groupMountPoint)/referencePath)) {
        presenterMainFrame.setReferencePath(referencePath.file_string());
      } else {
        cout << "error: reference-path doesn't exist" << endl;
        exit(EXIT_FAILURE);
      }
    }
    if (startupSettings.count("startup-histograms")) {
      presenterMainFrame.setStartupHistograms(startupSettings["startup-histograms"].as< vector<string> >());
    }

    presenterMainFrame.setDimDnsNode("");

    presenterApp.Run();
    exit(EXIT_SUCCESS);

  } catch(exception& e) {
    cout << e.what() << endl;
    exit(EXIT_FAILURE);
  }
}
#endif
