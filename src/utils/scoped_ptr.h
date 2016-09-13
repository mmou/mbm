/* from mbm code*/

#ifndef SCOPED_PTR_H
#define SCOPED_PTR_H

template<typename T>
class scoped_ptr {
 public:
  explicit scoped_ptr(T* ptr) : ptr_(ptr) { }
  ~scoped_ptr() { delete ptr_; }

  T* operator->() { return ptr_; }
  const T* operator->() const { return ptr_; }

  const T* get() const { return ptr_; }

 private:
  T* ptr_;
};

#endif  // SCOPED_PTR_H
