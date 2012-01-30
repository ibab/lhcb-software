#ifndef DETDESC_ELEMENTS_H
#define DETDESC_ELEMENTS_H

#include <string>
#include <typeinfo>
#include <stdexcept>
#include <TNamed.h>

class TObject;
class TObjArray;
class TGeoManager;

/*
 *   Gaudi namespace declaration
 */
namespace DetDesc {

  namespace Geometry  {
    struct  LCDD;
    struct  Element;
    struct  RefElement;
    struct  Collection_t;

    typedef TObject     Element_t;
    typedef TObjArray   NodeList_t;
    typedef TGeoManager Document_t;

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

    template<class T> inline void deletePtr(T*& p) { if(p) delete p; p=0; }

    template <typename Q, typename P> struct Value : public Q, public P  {
      typedef  Q first_base;
      typedef  P second_base;
      virtual ~Value() {}
    };

    struct Element  {
      Element_t* m_element;
      Element(Element_t* e) : m_element(e)    {                         }
      Element(const Element& e) : m_element(e.m_element) {              }
      Element_t* ptr() const                  {  return m_element;      }
      template <typename T> T* _ptr() const   {  return (T*)m_element;  }
      bool isValid() const                    {  return 0 != m_element; }
      bool operator!() const                  {  return 0 == m_element; }
      template <typename T, typename Q> Q* data() const  {
	return (Value<T,Q>*)m_element;
      }
      template<class T> void verifyObject() const {
	if ( dynamic_cast<T*>(ptr()) == 0 )  {
	  bad_assignment(ptr() ? typeid(*ptr()) : typeid(void),typeid(T));
	}
      }
      static void bad_assignment(const std::type_info& from, const std::type_info& to);
    };

    struct RefElement : public Element  {
      RefElement() : Element(0) {}
      RefElement(const Element& e) : Element(e) {}
      const char* name() const;
      const char* refName() const;
      void setName(const std::string& new_name);
      void assign(TNamed* n, const std::string& nam, const std::string& title);
    };

  }       /* End namespace Geometry  */
}         /* End namespace DetDesc   */
#endif    /* DETDESC_ELEMENTS_H      */
