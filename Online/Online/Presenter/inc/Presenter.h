#ifndef PRESENTER_H_
#define PRESENTER_H_

namespace Presenter {
  static const bool Hide = false; 
  static const bool Show = true;
  static const bool WithHistogram = true;
  static const bool WithoutHistogram = false;
  static const int  DIM = 0;
  static const int  Name = 1;
  static const int  Type = 2;
  static const int  Algorithm = 3;
  static const int  Task = 4;
  static const int  UTGID = 5;
  
  static const std::string Unknown="Unknown";
  static const std::string H1D="H1D";
  static const std::string H2D="H2D";
  static const std::string HPD="HPD";
  static const std::string CI4="CI4";
  static const std::string CF4="CF4";
  static const std::string CD8="CD8";
  static const std::string CC1="CC1";  
  
}
#endif /*PRESENTER_H_*/
