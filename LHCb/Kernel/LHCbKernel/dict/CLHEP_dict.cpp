// $Id: CLHEP_dict.cpp,v 1.1 2002-05-03 11:09:14 mato Exp $
#include <iostream>
#include <string>

#define private public
#define protected public
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Matrix/SymMatrix.h"
#undef protected
#undef private

#include "GaudiIntrospection/Introspection.h"

class C_dict {
public: C_dict();
};

static void* Meta1_theta(void* v)          
{
  static double ret;
  ret = ((HepLorentzVector*)v)->theta();
  return &ret;
}

static void* Meta1_cosTheta(void* v)       
{ 
  static double ret;
  ret = ((HepLorentzVector*)v)->cosTheta();
  return &ret;
}

static void* Meta1_phi(void* v)            
{ 
  static double ret;
  ret = ((HepLorentzVector*)v)->phi(); 
  return &ret;
}


static void* Meta1_rho(void* v)            
{ 
  static double ret;
  ret = ((HepLorentzVector*)v)->rho(); 
  return &ret;
}


static void* Meta1_perp2(void* v)          
{ 
  static double ret;
  ret = ((HepLorentzVector*)v)->perp2(); 
  return &ret;
}


static void* Meta1_perp(void* v)           
{ 
  static double ret;
  ret = ((HepLorentzVector*)v)->perp(); 
  return &ret;
}


//static void* Meta1_mag2(void* v)
//{
//  static double ret;
//  ret = ((HepLorentzVector*)v)->mag2(); 
//  return &ret;
//}


static void* Meta1_m2(void* v)             
{ 
  static double ret;
  ret = ((HepLorentzVector*)v)->m2(); 
  return &ret;
}


static void* Meta1_mag(void* v)            
{ 
  static double ret;
  ret = ((HepLorentzVector*)v)->mag(); 
  return &ret;
}


static void* Meta1_m(void* v)              
{ 
  static double ret;
  ret = ((HepLorentzVector*)v)->m(); 
  return &ret;
}


static void* Meta1_mt2(void* v)            
{ 
  static double ret;
  ret = ((HepLorentzVector*)v)->mt2(); 
  return &ret;
}


static void* Meta1_mt(void* v)             
{ 
  static double ret;
  ret = ((HepLorentzVector*)v)->mt(); 
  return &ret;
}


static void* Meta1_plus(void* v)           
{ 
  static double ret;
  ret = ((HepLorentzVector*)v)->plus(); 
  return &ret;
}


static void* Meta1_minus(void* v)          
{ 
  static double ret;
  ret = ((HepLorentzVector*)v)->minus(); 
  return &ret;
}


static void* Meta1_rapidity(void* v)       
{ 
  static double ret;
  ret = ((HepLorentzVector*)v)->rapidity(); 
  return &ret;
}


static void* Meta1_pseudoRapidity(void* v) 
{ 
  static double ret;
  ret = ((HepLorentzVector*)v)->pseudoRapidity(); 
  return &ret;
}


static void* Meta2_x(void* v)              
{ 
  static double ret;
  ret = ((HepPoint3D*)v)->x(); 
  return &ret;
}


static void* Meta2_y(void* v)              
{ 
  static double ret;
  ret = ((HepPoint3D*)v)->y(); 
  return &ret;
}


static void* Meta2_z(void* v)              
{ 
  static double ret;
  ret = ((HepPoint3D*)v)->z(); 
  return &ret;
}


static void* Meta4_x(void* v)              
{ 
  static double ret;
  ret = ((Hep3Vector*)v)->x(); 
  return &ret;
}


static void* Meta4_y(void* v)              
{ 
  static double ret;
  ret = ((Hep3Vector*)v)->y(); 
  return &ret;
}


static void* Meta4_z(void* v)              
{ 
  static double ret;
  ret = ((Hep3Vector*)v)->z(); 
  return &ret;
}


static void* Meta4_phi(void* v)            
{ 
  static double ret;
  ret = ((Hep3Vector*)v)->phi(); 
  return &ret;
}


static void* Meta4_theta(void* v)          
{ 
  static double ret;
  ret = ((Hep3Vector*)v)->theta(); 
  return &ret;
}


static void* Meta4_cosTheta(void* v)       
{ 
  static double ret;
  ret = ((Hep3Vector*)v)->cosTheta(); 
  return &ret;
}


static void* Meta4_mag2(void* v)           
{ 
  static double ret;
  ret = ((Hep3Vector*)v)->mag2(); 
  return &ret;
}


static void* Meta4_mag(void* v)            
{ 
  static double ret;
  ret = ((Hep3Vector*)v)->mag(); 
  return &ret;
}


static void* Meta4_perp2(void* v)          
{ 
  static double ret;
  ret = ((Hep3Vector*)v)->perp2(); 
  return &ret;
}


static void* Meta4_perp(void* v)           
{ 
  static double ret;
  ret = ((Hep3Vector*)v)->perp();
  return &ret;
}


static void* Meta4_pseudoRapidity(void* v) 
{ 
  static double ret;
  ret = ((Hep3Vector*)v)->pseudoRapidity(); 
  return &ret;
}


static C_dict instance;

C_dict::C_dict()
{
  std::vector<std::string> argTypes;
  std::string retType = "double";
  
  MetaClass* meta1 = new MetaClass("HepLorentzVector", "HEP Lozentz Vector");
  meta1->addField("px", "double", "Momentum X",&((HepLorentzVector*)0)->pp.dx);   
  meta1->addField("py", "double", "Momentum Y",&((HepLorentzVector*)0)->pp.dy);   
  meta1->addField("pz", "double", "Momentum Z",&((HepLorentzVector*)0)->pp.dz); 
  meta1->addField("pp", "Hep3Vector", "Hep3Vector", &((HepLorentzVector*)0)->pp);  
  meta1->addField("e",  "double", "Energy",    &((HepLorentzVector*)0)->ee);   
  meta1->addMethod("theta", "Get special vector components in spherical coordinate system", retType,  Meta1_theta);
  meta1->addMethod("cosTheta", "Get special vector components in spherical coordinate system", retType,  Meta1_cosTheta);
  meta1->addMethod("phi", "Get special vector components in spherical coordinate system", retType,  Meta1_phi);
  meta1->addMethod("rho", "Get special vector components in spherical coordinate system", retType,  Meta1_rho);
  meta1->addMethod("perp2", "Transverse component of the spatial vector squared", retType,  Meta1_perp2);
  meta1->addMethod("perp", "Transverse component of the spatial vector (R in cylindrical system)", retType,  Meta1_perp);
//  meta1->addMethod("mag2", "Invariant mass squared", retType,  Meta1_mag2);
  meta1->addMethod("m2", "Invariant mass squared", retType,  Meta1_m2);
  meta1->addMethod("mag", "Invariant mass. If mag2() is negative then -squrt(-mag2()) is returned", retType,  Meta1_mag);
  meta1->addMethod("m", "Invariant mass. If mag2() is negative then -squrt(-mag2()) is returned", retType,  Meta1_m);
  meta1->addMethod("mt2", "Transverse mass squared", retType,  Meta1_mt2);
  meta1->addMethod("mt", "Transverse mass", retType,  Meta1_mt);
  meta1->addMethod("plus", "Returns the positive light-cone component t + z", retType,  Meta1_plus);
  meta1->addMethod("minus", "Returns the negative light-cone component t - z", retType,  Meta1_minus);
  meta1->addMethod("rapidity", "Returns the rapidity, i.e. 0.5*ln((E+pz)/(E-pz))", retType,  Meta1_rapidity);
  meta1->addMethod("pseudoRapidity", "Returns the pseudo-rapidity, i.e. -ln(tan(theta/2))", retType,  Meta1_pseudoRapidity);

  MetaClass* meta2 = new MetaClass("HepPoint3D", "Hep Point 3D");
  meta2->addMethod("x", "X-Component in cartesian coordinate system", retType,  Meta2_x);
  meta2->addMethod("y", "Y-Component in cartesian coordinate system", retType,  Meta2_y);
  meta2->addMethod("z", "Z-Component in cartesian coordinate system", retType,  Meta2_z);
  
  MetaClass* meta3 = new MetaClass("HepSymMatrix", "Hep Symetric Matrix");
  meta3->addField("nrow", "int", "Number of rows",&((HepSymMatrix*)0)->nrow);   
  meta3->addField("size", "int", "Total size",&((HepSymMatrix*)0)->size);

  MetaClass* meta4 = new MetaClass("Hep3Vector", "general 3 vector class");
  meta4->addField("dx", "double", "X", &((Hep3Vector*)0)->dx);
  meta4->addField("dy", "double", "Y", &((Hep3Vector*)0)->dy);
  meta4->addField("dz", "double", "Z", &((Hep3Vector*)0)->dz);
  meta4->addMethod("x", "X-Component in cartesian coordinate system", retType,  Meta4_x);
  meta4->addMethod("y", "Y-Component in cartesian coordinate system", retType,  Meta4_y);
  meta4->addMethod("z", "Z-Component in cartesian coordinate system", retType,  Meta4_z);
  meta4->addMethod("phi", "The azimuth angle", retType,  Meta4_phi);
  meta4->addMethod("theta", "The polar angle", retType,  Meta4_theta);
  meta4->addMethod("cosTheta", "Cosine of the polar angle", retType,  Meta4_cosTheta);
  meta4->addMethod("mag2", "The magnitude squared (rho^2 in spherical coordinate system", retType,  Meta4_mag2);
  meta4->addMethod("mag", "The magnitude (rho in sphreical coordinate system", retType,  Meta4_mag);
  meta4->addMethod("perp2", "The transverse component squared (R^2 in cylindical coordinate system)", retType,  Meta4_perp2);
  meta4->addMethod("perp", "The transverse component (R in cylindrical coordinate system", retType,  Meta4_perp);
  meta4->addMethod("pseudoRapidity", "Returns the pseudo-rapidity, i.e. -ln(tan(theta/s))", retType,  Meta4_pseudoRapidity);

  
}

