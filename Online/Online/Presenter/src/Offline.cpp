#include <iostream>

#include "Offline.h" 

using namespace pres;
using namespace std;
using namespace boost::filesystem;
using namespace boost::posix_time;

//ClassImp(Offline)

Offline::Offline():
  m_path(path("/home/psomogyi/cmtuser/Online_v4r2/Online/Presenter/v0r5")) {
}
Offline::~Offline() 
{  
}
path Offline::findFile(const path & dirPath, const string & fileName)
{
  path foundFile("");
  if (exists(dirPath)) {
    directory_iterator end_itr; // default construction yields past-the-end
    for (directory_iterator itr(dirPath); itr != end_itr; ++itr) {
      if (is_directory(itr->status())) {
        foundFile = findFile(itr->path(), fileName); 
        if ("" != foundFile) { return foundFile; }
      } else if (itr->path().leaf() == fileName) {
        foundFile = itr->path();
        return foundFile;
      }
    }
  }
  return foundFile;
}
void Offline::listDirectory()
{
 
 //m_taskname+"-"+yearstr+"-"+monthstr+"-"+daystr+"T"+hourstr+"-"+minstr+"-"+secstr+".root"
 
 path fileFound = findFile(m_path, "GauchoJob-26-11-2007T16-9-21.root");
  
 cout << " does contain the file:  " << fileFound << endl;

  time_duration td(1,2,3,4);
  // bool is_not_a_date_time() const
  cout << to_simple_string(td) << endl;
}
