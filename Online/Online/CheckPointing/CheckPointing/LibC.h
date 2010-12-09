#ifndef LIBC_H
#define LIBC_H

struct LibC {
private:
  static void* i_getSymbol(const char* name);
public:
  template<class T> static int getSymbol(const char* name, T& func_ptr) {
    func_ptr = (T)i_getSymbol(name);
    return func_ptr == 0 ? 0 : 1;
  }
};

#endif // LIBC_H
