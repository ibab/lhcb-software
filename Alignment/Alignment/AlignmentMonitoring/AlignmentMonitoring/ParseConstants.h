#ifndef PARSECONSTANTS_H
#define PARSECONSTANTS_H

#include <map>
#include <string>

namespace Alignment
{
  namespace AlignmentMonitoring
  {
    class ParseConstants
    {
    public:
      // Constructor
      ParseConstants();
      // Destructor
      ~ParseConstants() {}
      // Methods
      std::map<std::string, double> Parse(const char* filename);
    private:
    };
  }// namespace AlignmentMonitoring
}// namespace Alignment

#endif
