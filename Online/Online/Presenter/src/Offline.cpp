#include <iostream>

#include "Offline.h" 

using namespace pres;
using namespace std;
using namespace boost::filesystem;
using namespace boost::posix_time;

//ClassImp(Offline)

Offline::Offline():
  m_path(path("/home/psomogyi/cmtuser/Online_v4r2/Online/Presenter/v0r5/test")) {
}
Offline::~Offline() 
{  
}
path Offline::findFile(const path & dirPath,    // in this directory,
                       const string & fileName) // search for this name
{
  path foundFile("");
  if (!exists(dirPath)) { return foundFile; }
  directory_iterator end_itr; // default construction yields past-the-end
  for (directory_iterator itr(dirPath); itr != end_itr; ++itr) {
    if (is_directory(itr->status())) {
      if ("" != findFile(itr->path(), fileName)) { return foundFile; }
    } else if (itr->path().leaf() == fileName) {
      foundFile = itr->path();
      return foundFile;
    }
  }
  return foundFile;
}
void Offline::listDirectory()
{
 path fileFound = findFile(m_path, "GauchoJob26112007_16:9:21.roo");
  
 cout << " does contain the file:  " << fileFound << endl;

  time_duration td(1,2,3,4);
  // bool is_not_a_date_time() const
  cout << to_simple_string(td) << endl;

}
