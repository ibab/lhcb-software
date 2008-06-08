
#include "Data.h"
#include "NimTypeRichModel.h"
#include "Utils/MessageHandler.h"
#include <fstream>

namespace Lester 
{

  void Data::doNothing() const throw() {}

  void Data::setFromFile(const std::string & file) 
  {
    hits.clear();
    secretCircs.clear();
    std::ifstream f(file.c_str());
    if ( f.is_open() )
    {
      double x(0), y(0);
      while ( f >> x ) 
      {
        f >> y;
        hits.push_back( Hit(x,y) );
      }
    }
    else 
    { 
      Lester::messHandle().fatal() << "FAILED to open data file '" 
                                   << file << "'" << Lester::endmsg;
      throw std::exception(); 
    }
  }

} // namespace

