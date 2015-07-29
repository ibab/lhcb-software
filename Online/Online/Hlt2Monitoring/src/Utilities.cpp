// Include files

// boost
#include <boost/range/iterator_range.hpp>

// ROOT
#include <TDirectory.h>
#include <TFile.h>
#include <TKey.h>

// local
#include "Utilities.h"

namespace {
   using std::string;

   namespace fs = boost::filesystem;
}

//===============================================================================
void Monitoring::loadSavedHistograms(histos_t& histos, fs::path file,
                                     Monitoring::RunNumber run, std::string skip)
{
   std::function<void(TDirectory* dir, std::string path)> load
      = [skip, run, &load, &histos](TDirectory* dir, std::string path)->void {
      auto sl = skip.length();
      
      // Loop over keys in directory
      auto keys = dir->GetListOfKeys();
      TIter it{keys->MakeIterator()};
      while (TKey* key = static_cast<TKey*>(it.Next())) {
         if (!std::strncmp(key->GetClassName(), "TDirectory", 10)) {
            // If it's a directory, recurse
            auto dirPath = path;
            if (!dirPath.empty()) {
               dirPath += string{"/"};
            }
            dirPath += key->GetName();
            load(static_cast<TDirectory*>(dir->Get(key->GetName())), dirPath);
         } else if (!std::strcmp(key->GetClassName(), "TH1D")
                    && (skip.empty() ||
                        (strlen(key->GetName()) > sl
                         && std::strncmp(key->GetName() + strlen(key->GetName()) - sl,
                                         skip.c_str(), sl) != 0))) {
            // Else store the saved histograms in our memory storage.
            std::unique_ptr<TH1D> savedHisto{static_cast<TH1D*>(key->ReadObj())};
            savedHisto->SetDirectory(nullptr);
            const auto& hbn = histos.get<byName>();
            auto range = boost::make_iterator_range(hbn.equal_range(path + "/" + savedHisto->GetName()));
            auto it = std::find_if(begin(range), end(range), [run](const entry_t& entry) {
                  return entry.run == run;
               });
            if (it == end(range)) {
               // The type (rate or regular histo) comes from the title... ugly,
               // but least problematic alternative...
               std::string histoType = savedHisto->GetTitle();
               histos.insert({run, histoType, path, savedHisto.release()});
            } else {
               it->histo->Add(savedHisto.get());
            }
         }
      }
   };

   std::unique_ptr<TFile> inFile{new TFile(file.string().c_str(), "read")};
   string dir;
   load(inFile.get(), dir);
}
