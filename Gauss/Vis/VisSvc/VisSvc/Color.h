// $Id: Color.h,v 1.4 2009-06-03 14:21:06 jonrob Exp $
#ifndef VISSVC_COLOR_H 
#define VISSVC_COLOR_H 1

// Include files

/** @class Color Color.h VisSvc/Color.h
 *  
 *  This defines a simple Color class. This mimics what was done in
 *  the SoFree package but was copied to avoid useless dependencies.
 *
 *  A color is defined by its three components in the Red Green Blue
 *  space. These components are floats and are supposed to be between
 *  0 and 1
 *
 *  @author Sebastien Ponce
 *  @date   18/01/2002
 */
class Color {
public:
  /// Standard constructor
  Color (); 
  Color (float R, float G, float B); 
  Color (float R, float G, float B, float A); 
  Color (const float [4]);

  const float* value() const;
  float red () const;
  float green () const;
  float blue () const;
  float alpha () const;

  bool isValid() const;
  bool operator== (const Color& c) const;
  bool operator!= (const Color& c) const;

protected:
  float vec[4];
};

inline Color::Color () {
  vec[0] = 2;
  vec[1] = 2;
  vec[2] = 2;
  vec[3] = 2;
}

inline Color::Color (float R, float G, float B) {
  vec[0] = R;
  vec[1] = G;
  vec[2] = B;
  vec[3] = 0;
}

inline Color::Color (float R, float G, float B, float A) {
  vec[0] = R;
  vec[1] = G;
  vec[2] = B;
  vec[3] = A;
}

inline const float* Color::value() const {
  return vec;
}

inline float Color::red () const {
  return vec[0];
}

inline float Color::green () const {
  return vec[1];
}

inline float Color::blue () const {
  return vec[2];
}

inline float Color::alpha () const {
  return vec[3];
}

inline bool Color::isValid() const {
  return vec[0] >= 0 && vec[0] <= 1 &&
    vec[1] >= 0 && vec[1] <= 1 &&
    vec[2] >= 0 && vec[2] <= 1 &&
    vec[3] >= 0 && vec[3] <= 1;
}

inline bool Color::operator==(const Color& c) const {
  return vec[0] == c.red() && vec[1] == c.green() &&
    vec[2] == c.blue() && vec[3] == c.alpha();
}

inline bool Color::operator!= (const Color& c) const {
  return !this->operator==(c);
}

#endif // VISSVC_COLOR_H
