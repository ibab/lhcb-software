#include <iostream>

#include "MisalignmentParser.h"

int main()
{
  MisalignmentParser p("foo.opts");

  std::vector<std::string> dets;
  dets.push_back("/Det/BBQ");
  dets.push_back("/Det/BBQ/Meat");
  dets.push_back("/Det/BBQ/Charcoal");
  dets.push_back("/Det/BBQ/Sausage");

  for (std::vector<std::string>::const_iterator it = dets.begin();
      dets.end() != it; ++it) {
    std::vector<double> shifts, rotations;
    MisalignmentParser::AlignmentParameters ap = p.getAlignmentParameters(*it);
    std::cout << "Detector element " << *it << " misaligned: " <<
      ap->isMisaligned() << std::endl;
    shifts = ap->getShifts();
    rotations = ap->getRotations();
    std::cout << "\tShifts: " << shifts[0] << " " << shifts[1] << " " <<
      shifts[2] << std::endl;
    std::cout << "\tRotations: " << rotations[0] << " " << rotations[1] << " " <<
      rotations[2] << std::endl << std::endl;
  }

  return 0;
}
