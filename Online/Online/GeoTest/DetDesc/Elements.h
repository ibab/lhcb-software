#ifndef DETDESC_ELEMENTS_H
#define DETDESC_ELEMENTS_H

#include <string>
#include <typeinfo>
#include <stdexcept>
#include <TNamed.h>

class TObject;
class TObjArray;
class TGeoManager;

// Conversion factor from radians to degree: 360/(2*PI)
#define RAD_2_DEGREE 57.295779513082320876798154814105

/*
 *   Gaudi namespace declaration
 */
namespace DetDesc {

  namespace Geometry  {
    struct  LCDD;

    std::string    _toString(bool value);
    std::string    _toString(int value);
    std::string    _toString(float value);
    std::string    _toString(double value);

    bool           _toBool  (const std::string& value);
    int            _toInt   (const std::string& value);
    float          _toFloat (const std::string& value);
    double         _toDouble(const std::string& value);

    inline bool    _toBool(bool value)       {  return value; }
    inline int     _toInt(int value)         {  return value; }
    inline float   _toFloat(float value)     {  return value; }
    inline double  _toDouble(double value)   {  return value; }

    void _toDictionary(const std::string& name, const std::string& value);

    template<typename T> inline void deletePtr(T*& p) { if(p) delete p; p=0; }

    template <typename Q, typename P> struct Value : public Q, public P  {
      typedef  Q first_base;
      typedef  P second_base;
      virtual ~Value() {}
    };

    long num_object_validations();
    void increment_object_validations();

    template <typename T> struct Element_type  {
      typedef T Implementation;

      Implementation* m_element;
      Element_type() : m_element(0) {}
      Element_type(Implementation* e) : m_element(e) {}
      Element_type(const Element_type<Implementation>& e) : m_element(e.m_element) {}

      template<typename Q> Element_type(Q* e)
	: m_element((T*)e)           { verifyObject();                  }

      template<typename Q> Element_type(const Element_type<Q>& e) 
	: m_element((T*)e.m_element) { verifyObject();                  }

      T* ptr() const                          {  return m_element;      }
      template <typename Q> Q* _ptr() const   {  return (Q*)m_element;  }
      bool isValid() const                    {  return 0 != m_element; }
      bool operator!() const                  {  return 0 == m_element; }
      template <typename Q> Q* data() const  {
	return (Value<Implementation,Q>*)m_element;
      }
      void verifyObject() const {
	increment_object_validations();
	if ( m_element && dynamic_cast<T*>(m_element) == 0 )  {
	  bad_assignment(typeid(*m_element),typeid(T));
	}
      }
      static void bad_assignment(const std::type_info& from, const std::type_info& to);
    };

    template <typename T> struct RefElement_type : public Element_type<T>  {
      typedef T Implementation;
      RefElement_type() : Element_type<T>() {}
      RefElement_type(const Element_type<T>& e) : Element_type<T>(e) {}

      template<typename Q> 
      RefElement_type(const Element_type<Q>& e) : Element_type<T>(e) {}

      const char* name() const;
      const char* refName() const;
      void setName(const std::string& new_name);
      void assign(Implementation* n, const std::string& nam, const std::string& title);
      Implementation* operator->() const { return this->m_element;    }
      operator Implementation& ()  const { return *(this->m_element); }
    };

    template <> struct RefElement_type<TNamed> : public Element_type<TNamed>  {
      typedef TNamed Implementation;
      RefElement_type() : Element_type<TNamed>() {}
      RefElement_type(const Element_type<TNamed>& e) : Element_type<TNamed>(e) {}

      template<typename Q> 
      RefElement_type(const Element_type<Q>& e) : Element_type<TNamed>(e) {}
      
      TNamed*  operator->() const { return this->m_element; }
      operator TNamed*() const    { return this->m_element; }
      
      const char* name() const;
      const char* refName() const;
      void setName(const std::string& new_name);
      void assign(Implementation* n, const std::string& nam, const std::string& title);
    };

  }       /* End namespace Geometry  */
}         /* End namespace DetDesc   */
#endif    /* DETDESC_ELEMENTS_H      */
