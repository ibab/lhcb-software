#ifndef FILEREADER_HPP
#define FILEREADER_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

class FileReader {
 public:
  FileReader(const char* filename);
  FileReader(const std::string& filename);

  bool nextLine();
  
  bool nextTxtLine();

  template < typename T >
  T getField(const int n) {
    isValid_ = true;
    endLine_ = false;
    std::istringstream ist(line);
    this->skip_fields(ist, n-1);
    T rval;
    ist >> rval;
    
    if (ist.fail())
	    endLine_ = true;
    
    return rval;
  }
  
  bool inputFailed() const;
  bool endOfLine() const;
  bool endOfFile() const;
  bool isValid() const;

  void print_line()
  {
	  std::cout << line << std::endl;
  }
  
 private:
  void skip_fields(std::istringstream& ist, const int n);
  std::ifstream file;
  std::string line;
  bool isValid_;
  bool endLine_;
  bool endOfFile_;
};

#endif