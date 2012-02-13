// $Id: $
#ifndef FILESTAGER_FILESTAGERFUNCTIONS_H 
#define FILESTAGER_FILESTAGERFUNCTIONS_H 1

namespace FileStager {
   bool createPFN( std::string& remote, std::string& command, bool stageLocal = false );
   bool createLFN( std::string& remote, std::string& command );
}

#endif // FILESTAGER_FILESTAGERFUNCTIONS_H
