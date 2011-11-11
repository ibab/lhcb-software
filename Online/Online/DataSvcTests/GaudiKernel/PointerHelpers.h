#ifndef GAUDIKERNEL_POINTERHELPERS_H
#define GAUDIKERNEL_POINTERHELPERS_H
template <class T> void releasePtr(T*& p) {
  if ( p ) p->release(); 
  p = 0;
}
template <class T> void swapRefCounted(T*& from, T*& to) {
  if ( from == to ) return;
  T* tmp = to;
  to = from;
  if ( to  ) to->addRef();
  if ( tmp ) tmp->release();
}

template <class T> void deletePtr(T*& p) {
  if ( p ) delete p; 
  p = 0;
}
template <class T> void deleteArray(T*& p) {
  if ( p ) delete [] p; 
  p = 0;
}
#endif  // GAUDIKERNEL_POINTERHELPERS_H
