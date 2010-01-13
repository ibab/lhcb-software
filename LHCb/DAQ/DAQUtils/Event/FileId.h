// $Id: FileId.h,v 1.1 2009/10/09 15:52:33 panmanj Exp $
#ifndef FILEID_H 
#define FILEID_H 1

// Include files
#include "GaudiAlg/GaudiAlgorithm.h"

/** @class FileId FileId.h
 *   
 *  Converts fileID to and from 4 unsigned ints
 *
 *  @author Jaap Panman
 *  @date   2009-10-09
 */
namespace LHCb 
{
  class FileId {
  public: 
    /// Standard constructor
    FileId( ); 

    virtual ~FileId( ); ///< Destructor

    virtual std::vector<unsigned int> fileIDstr2int(std::string& s);
    virtual std::string fileIDint2str(std::vector<unsigned int>& b);

  protected:

  private:

  };
}


namespace LHCb 
{
  //=============================================================================
  // Standard constructor, initializes variables
  //=============================================================================
  FileId::FileId(  ) {
   
  }
  //=============================================================================
  // Destructor
  //=============================================================================
  FileId::~FileId() {} 

  //=============================================================================
  //  fileIDstr2int
  //=============================================================================
  std::vector<unsigned int> FileId::fileIDstr2int(std::string& s) {
    // converts fileID like 2A8D7CDC-D4AD-DE11-A29B-001E4F3E59B1 into 4 ulongs

    std::string IDstr=s;
    std::size_t idx = std::string::npos;
    while ( (idx = IDstr.find('-')) != std::string::npos ) IDstr.erase( idx , 1 );
    // prepare integer representation
    std::vector<unsigned int> b;
    for ( int i = 0; i < 4; i++ ) {
      unsigned long fileID = std::strtoul( (IDstr.substr(24-i*8,8)).c_str(), NULL, 16 );
      b.push_back( fileID );
    }
    return b;
  }

  //=============================================================================
  //  fileIDint2str
  //=============================================================================
  std::string FileId::fileIDint2str(std::vector<unsigned int>& b) {
    // converts 4 ulongs to fileID string

    std::stringstream st;
    st.setf(std::ios::uppercase);
    st << std::hex << b[3] << std::hex << b[2] << std::hex << b[1] << std::hex << b[0];
    std::string s;
    s += st.str(); 
    s.insert(8,"-");
    s.insert(13,"-");
    s.insert(18,"-");
    s.insert(23,"-");
    return s;
  }

  //=============================================================================

}

#endif // FILEID_H
