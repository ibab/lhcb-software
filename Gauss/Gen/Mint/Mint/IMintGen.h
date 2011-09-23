#ifndef IMINTGEN_HH
#define IMINTGEN_HH
// author: Matt Coombes
// status:  17 Aug 2011

namespace MINT{
class IMintGen{
 public:
  virtual void SetInputTextFile(std::string inputFile) = 0;
  virtual void Initalize(const std::vector<int> &) = 0;

	// Decay Event in parent Rest Frame
  virtual std::vector<std::vector<double> > DecayEventRFVec() = 0;
};
}// namespace MINT

#endif
//
