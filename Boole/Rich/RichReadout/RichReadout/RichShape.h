
#ifndef RichShape_h
#define RichShape_h 1

//! The signal shape function after the amplifier/shaper
class RichShape {

public:

  RichShape (double , double );

  double operator() (double time) const;

  double getTpeak () const {return tpeak_;}
  void setTpeak (double value) {tpeak_=value;}

private:

      double alpha_;
      double tpeak_;
};


#endif
