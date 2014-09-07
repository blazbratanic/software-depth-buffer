/// @file AnewAllocator.h
/// <summary>
///	Declares AnewAllocator
/// </summary>
///
///	Supervision: Dejan Tomazevic
///
///	Company: Sensum d.o.o.
///
///	@authors Ziga Lenarcic
///
///	@version     3.9.2012    Initial version
///
//-----------------------------------------------------------
// reviewed by: Marko Bukovec (25.10.2012)
//-----------------------------------------------------------

#ifndef _ANEWALLOCATOR_H_
#define _ANEWALLOCATOR_H_

#include <stdexcept>
#include <malloc.h>

#ifdef __GNUC__
#include <stdint.h>
#endif

#ifndef ANEW_ALIGNED_MALLOC
#define ANEW_ALIGNED_MALLOC
#ifdef __MINGW32__
#define anew(atype, asize) \
  (atype*) _aligned_malloc(sizeof(atype) * (asize), 256)
#define adelete(aptr) _aligned_free((void*)(aptr))
#elif defined __GNUC__
#define anew(atype, asize) (atype*) memalign(256, sizeof(atype) * (asize))
#define adelete(aptr) free((void*)(aptr))
#elif defined _MSC_VER
#define anew(atype, asize) \
  (atype*) _aligned_malloc(sizeof(atype) * (asize), 256)
#define adelete(aptr) _aligned_free((void*)(aptr))
#endif
#endif

template <typename T>
class AnewAllocator {
  // made on the basis of
  // http://blogs.msdn.com/b/vcblog/archive/2008/08/28/the-mallocator.aspx
 public:
  // The following will be the same for virtually all allocators.
  typedef T* pointer;
  typedef const T* const_pointer;
  typedef T& reference;
  typedef const T& const_reference;
  typedef T value_type;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;

  T* address(T& r) const { return &r; }

  const T* address(const T& s) const { return &s; }

  size_t max_size() const {
    // The following has been carefully written to be independent of
    // the definition of size_t and to avoid signed/unsigned warnings.
    return (static_cast<size_t>(0) - static_cast<size_t>(1)) / sizeof(T);
  }

  // The following must be the same for all allocators.
  template <typename U>
  struct rebind {
    typedef AnewAllocator<U> other;
  };

  bool operator!=(const AnewAllocator& other) const {
    return !(*this == other);
  }

  void construct(T* const p, const T& t) const {
    void* const pv = static_cast<void*>(p);

    new (pv) T(t);
  }

  void destroy(T* const p) const;  // Defined below.

  // Returns true if and only if storage allocated from *this
  // can be deallocated from other, and vice versa.
  // Always returns true for stateless allocators.
  bool operator==(const AnewAllocator& other) const { return true; }

  // Default constructor, copy constructor, rebinding constructor, and
  // destructor.
  // Empty for stateless allocators.
  AnewAllocator() {}

  AnewAllocator(const AnewAllocator&) {}

  template <typename U>
  AnewAllocator(const AnewAllocator<U>&) {}

  ~AnewAllocator() {}

  // The following will be different for each allocator.
  T* allocate(const size_t n) const {
// AnewAllocator prints a diagnostic message to demonstrate
// what it's doing. Real allocators won't do this.
#ifdef MEX_DEBUG_PRINT
// mexPrintf("Allocating %d objects of size %d.\n", n, sizeof(T));
// std::cout << "Allocating " << n << (n == 1 ? " object" : "objects") << " of
// size " << sizeof(T) << "." << std::endl;
#endif

    // The return value of allocate(0) is unspecified.
    // AnewAllocator returns NULL in order to avoid depending
    // on malloc(0)'s implementation-defined behavior
    // (the implementation can define malloc(0) to return NULL,
    // in which case the bad_alloc check below would fire).
    // All allocators can return NULL in this case.
    if (n == 0) {
      return NULL;
    }

    // All allocators should contain an integer overflow check.
    // The Standardization Committee recommends that std::length_error
    // be thrown in the case of integer overflow.
    if (n > max_size()) {
      throw std::length_error(
          "AnewAllocator<T>::allocate() - Integer overflow.");
    }

    // Allocate using fftw_malloc
    // AnewAllocator wraps malloc().
    // void * const pv = malloc(n * sizeof(T));
    // void * const pv = fftw_malloc(n * sizeof(T));

    size_t n_alloc = n;

    if (n_alloc % 16 != 0) n_alloc += 16 - (n_alloc % 16);

    pointer pv = anew(T, n_alloc);
    // pointer pv = reinterpret_cast<pointer>(myAlignedNew(n * sizeof(T)));

    // Allocators should throw std::bad_alloc in the case of memory allocation
    // failure.
    if (pv == NULL) {
      throw std::bad_alloc();
    }

    // return static_cast<T *>(pv);
    return pv;
  }

  void deallocate(T* const p, const size_t n) const {
// AnewAllocator prints a diagnostic message to demonstrate
// what it's doing. Real allocators won't do this.
#ifdef MEX_DEBUG_PRINT
// mexPrintf("Deallocating %d objects of size %d.\n", n, sizeof(T));
// std::cout << "Deallocating " << n << (n == 1 ? " object" : "objects") << " of
// size " << sizeof(T) << "." << std::endl;
#endif

    // AnewAllocator wraps free().
    // free(p);
    adelete(p);
    // myAlignedDelete((void *) p);
  }

  // The following will be the same for all allocators that ignore hints.
  template <typename U>
  T* allocate(const size_t n, const U* /* const hint */) const {
    return allocate(n);
  }

  // Allocators are not required to be assignable, so
  // all allocators should have a private unimplemented
  // assignment operator. Note that this will trigger the
  // off-by-default (enabled under /Wall) warning C4626
  // "assignment operator could not be generated because a
  // base class assignment operator is inaccessible" within
  // the STL headers, but that warning is useless.
  // private:
  // AnewAllocator& operator=(const AnewAllocator&);
};

// A compiler bug causes it to believe that p->~T() doesn't reference p.
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4100)  // unreferenced formal parameter
#endif

// The definition of destroy() must be the same for all allocators.
template <typename T>
void AnewAllocator<T>::destroy(T* const p) const {
  p->~T();
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif  // _FFTWALLOCATOR_H_
