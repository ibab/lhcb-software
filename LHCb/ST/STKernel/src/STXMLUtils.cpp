#include "Kernel/STXMLUtils.h"

// Date and time from boost
#include "boost/date_time/gregorian/gregorian.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"

#include <iostream>

namespace {

  static const std::string openComment = "<!-- ";
  static const std::string closeComment = " -->";
}

void ST::XMLUtils::authorComment(std::ostringstream& stream, 
                                 const std::string& author) {
  comment(stream, "Author", author);
}

void ST::XMLUtils::dateComment(std::ostringstream& stream) {
  boost::gregorian::date d(boost::gregorian::day_clock::universal_day());
  boost::posix_time::ptime
    now(boost::posix_time::microsec_clock::local_time());
  boost::posix_time::time_duration tod(now.time_of_day());
  stream << openComment << d << " - " << tod.hours() << ":" << std::setw(2)
         << std::setfill('0') << tod.minutes() << ":" << tod.seconds() << closeComment << std::endl;
}

void ST::XMLUtils::tagComment(std::ostringstream& stream, 
                              const std::string& tag) {
  comment(stream, "Version:", tag);
}

void ST::XMLUtils::descriptionComment(std::ostringstream& stream, 
                              const std::string& description) {
  comment(stream, "Description:", description);
}

void ST::XMLUtils::comment(std::ostringstream& stream, 
                           const std::string& name, 
                           const std::string& value){
  stream << openComment << name <<  ": " << value << closeComment << std::endl;
}

void ST::XMLUtils::fullComment(std::ostringstream& stream,
                              const std::string& author, 
                              const std::string& tag,
		              const std::string desc){
  dateComment(stream);
  authorComment(stream, author);
  tagComment(stream, tag);  
  descriptionComment(stream, desc);
}


void ST::XMLUtils::replaceChars(std::string& conString) {
  std::cout << "Add comment " <<std::endl;
  std::string blank = " ";
  replace(conString, ",", blank);
  replace(conString, "[", blank);
  replace(conString, "]", blank);
  replace(conString, "\"/", "\"");
}

void ST::XMLUtils::replace(std::string& conString, 
                           std::string in, 
                           std::string out ) {

  std::string::size_type pos = 0;
  while (pos != std::string::npos){
    pos = conString.find(in,pos);
    if (pos != std::string::npos) {
      conString.replace(pos,in.size(),out);
    }
  } //pos
}
