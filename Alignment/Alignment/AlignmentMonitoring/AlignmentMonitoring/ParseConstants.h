#ifndef ALIGNMENT_ALIGNMENTMONITORING_PARSECONSTANTS_H
#define ALIGNMENT_ALIGNMENTMONITORING_PARSECONSTANTS_H

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
