Overview
========

This is a simple, single-file, header-only, C/C++/C++11 library to read/write
files in the Numpy format. The main purpose is to avoid the complexity of
pulling Python and Numpy into your build just to be able to read or write .npy
files. There are no external library dependencies, but we do assume several
POSIX interfaces like open and mmap. Just place this header in your project or
in your include path and you're good to go.

Advanced usage: The library works with memory buffers, sockets and pipes in
addition to regular disk files. Since the library can work with memory buffers,
you can use your favorite non-blocking library to transfer serialized numpy
files in or out of memory as you like. Consequently there is no direct support
for non-blocking I/O.


Installation
------------

Just copy the file to your own project or place it in your include path. To
install system-wide, you can do

    make [PREFIX=/where/you/like] install

The default PREFIX is /usr.


C Input Example
---------------

    /* Simple example to print the shape of an array in npy format. */

    #include <stdio.h>
    #include "npio.h"

    int main()
    {
      int i;
      npio_Array array;

      npio_init_array(&array);
      npio_load("test1.npy", &array);

      printf("shape: ");
      for (i = 0; i < array.dim; ++i)
        printf("%ld ", array.shape[i]);
      printf("\n");

      npio_free_array(&array);
    }


C++11 Input Example
-------------------

    /* Simple example to print the elements of an array in npy format. */

    #include <iostream>
    #include "npio.h"

    int main()
    {
      auto a = npio::Array("test.npy");
      for (auto v: a.values<float>())
        std::cout << v << "\n";
    }


C Output Example
----------------

    /* Simple example to save a random array to npy format */

    #include <stdlib.h>
    #include "npio.h"

    int main()
    {
      float shape[] = {10, 100};
      float v[1000];
      for (size_t i = 0; i < 1000; ++i)
        v[i] = random() * 1.0f / RAND_MAX;

      npio_Array array;
      npio_init_array(&array);
      array.dim = 2;
      array.shape = shape;
      array.floating_point = true;
      array.is_signed = true;
      array.bit_width = 32;
      array.data = v;
      return npio_save("out.npy", &array);
    }


C++ Output Example
--------------------

    // Simple example to save a random array to npy format

    #include <random>
    #include <vector>
    #include "npio.h"

    int main()
    {
      std::vector<float> v(1000);
      std::mt19937 rng;
      rng.generate(v.begin(), v.end());
      return npio::save("out.npy", {10, 100}, &v[0]);
    }


C++11 Input/Output Example
--------------------------

    
    // Simple example to load an array, modify it and save it.
    
    #define NPIO_CXX_ENABLE_EXCEPTIONS
    #include "npio.h"

    int main()
    {
      auto a = npio::Array("test2.npy");
      for (auto &v : a.values<float>())
        v += 10.0f;
      a.save("example3-out.npy");
    }
    


Known Limitations
-----------------

Structured arrays and Object arrays are not currently supported. If you need to
handle such files, you are probably better off using the numpy C API instead.



Documentation
=============


Error Handling
--------------

Most functions return zero on success and an error number on failure. We reuse
POSIX errno constants rather than introducing our own custom error codes. You
can use `strerror` etc. to get a string representation of the error as usual.
See the documentation of each function for the interpretation of specific error
codes in each context.


C API
-----

### npio_Array

The npio_Array structure is used to represent the array data loaded from a
numpy file and also to describe data to be saved in numpy format. You must
always call `npio_init_array()` to initialize before using this structure.

The following public members are read/write. i.e. you read these when you load
a file and you can/must set these when you save a file.

* `dim`: the number of dimensions to the array (must set).
* `fortran_order`: whether or not data is in fortan-ordering (def: false).
* `little_endian`: whether or not elements are little-endian (def: host).
* `floating_point`: whether data is integral or floating point (must set).
* `is_signed`: whether data is signed or unsigned.
* `bit_width`: the number of bits per element of the array.
* `shape`:  array of sizes, one for each dimension.
* `data`: untyped pointer to array data.
* `major_version`: the major version of the numpy file format
* `minor_version`: the minor version of the numpy file format

The following public members are read-only, i.e. they are only valid if you
are loading a numpy file.

* `header_len`: the length of the dict header of the file being loaded.
* `dtype`: the dtype string in the header of the file being loaded.

You should never examine or modify any of the private members (marked with a
trailing underscore).



### npio_init_array

#### Synopsis

    void npio_init_array(npio_Array* array);

Initializes an array structure. You should always call this before passing an
array structure to a library function.



### npio_free_array

#### Synopsis

    void npio_free_array(npio_Array* array);

Frees all resources associated with the array. You _must_ call this on any
array that you passed into a `npio_load_*` function, whether or not the
function was successful.  You must _not_ call this on an array that was
manually populated.



### npio_load

#### Synopsis

    int npio_load(const char* filename, npio_Array* array);
    int npio_load3(const char* filename, npio_Array* array, size_t max_dim);
    int npio_load4(const char* filename, npio_Array* array, size_t max_dim
      , size_t max_size);

    int npio_load_fd(int fd, npio_Array* array);
    int npio_load_fd3(int fd, npio_Array* array, size_t max_dim);
    int npio_load_fd4(int fd, npio_Array* array, size_t max_dim
      , size_t max_size);

    int npio_load_mem(void* ptr, size_t sz, npio_Array* array);
    int npio_load_mem4(void *ptr, size_t sz, npio_Array* array, size_t max_dim);

Loads a numpy array from the specified filename, file descriptor, or memory
buffer and places the description of the loaded data in `array`. The filename
and file descriptor variants will use `mmap` and fall back on `read` if
necessary. The extended variants (suffixed with the argument count) take
additional parameters to protect against memory-DOS attacks from untrusted
files. `max_dim` specifies the maximum dimensionality of the array you are
willing to load and `max_size` specifies the maximum number of elements.

When loading from a memory buffer, the library only allocates space for the
shape of the array. The array elements are not copied and `array.data` will
point into the source buffer. So you must keep the memory buffer around as long
as you want to keep the array around. Furthermore, if an endianness conversion
must be done, it will be done in-place and will modify the original buffer.
Freeing the array with `npio_free_array` does not free the original buffer.

If you want to load data without doing an endianness conversion, you must use
the lower level `npio_load_header` and `npio_load_data2` functions.

#### Return

Zero upon success. In addition to OS-generated errors, the following errors are
possible:

- `EINVAL`: the file is not a valid numpy file
- `ERANGE`: the header is too large, or the array has too many dimensions, or
     the array has too many elements.
- `ENOTSUP`: the library does not support this numpy file
- `ENOMEM`: memory allocation failed



### npio_save

#### Synopsis

    int npio_save(const char* filename, const npio_Array* array)

Saves a numpy array to the specified filename. `array` should either be a
previously loaded array, or a manually populated structure describing the data
to be saved. At a minimum you must provide the following fields: `dim`, `shape`
and `data`. Unless overridden, we assume that the data is single-precision
float in host-endian order.

#### Return

Zero upon success. In addition to OS-generated errors, the following errors are
possible:

- `ERANGE`: the array has too many dimensions or too many elements for the
     current implementation to handle. It is a fixable problem, but it is
     not likely that you will encounter this in practice on a 64-bit system.



### npio_load_header

#### Synopsis

    int npio_load_header(const char* filename, npio_Array*);
    int npio_load_header3(const char* filename, npio_Array*, size_t max_dim);
    
    int npio_load_header_fd(int fd, npio_Array*);
    int npio_load_header_fd3(int fd, npio_Array*, size_t max_dim);

    int npio_load_header_mem(void*, size_t, npio_Array*);
    int npio_load_header_mem4(void*, size_t, npio_Array*, size_t max_dim);

These functions load just the header from the specified file, file descriptor
or memory buffer. You can use this function to first examine a header before
deciding whether to continue loading it or not using the `npio_load_data`
function decribed below.



### npio_load_data

#### Synopsis

    int npio_load_data(npio_Array* array);
    int npio_load_data2(npio_Array* array, int swap_bytes);

This loads the content into an array that was partially loaded with one of the
`npio_load_header` functions. It is up to the caller to ensure that any
resources that were passed into the load_header function are still valid, such
as the file descriptor or memory buffer. If swap_bytes is non-zero, or if the
single-argument version is used, then the loaded data is automatically
converted to the endianness of the host and the field `little_endian` in
`array` is updated to reflect this.


### npio_save_fd

#### Synopsis

    int npio_save_fd(int fd, const npio_Array* array);

Saves the specified array to the previously opened file descriptor, which
should have been opened for writing. Only write() calls are used, so the
descriptor can be socket or pipe.


### npio_save_header_fd

#### Synopsis

    int npio_save_header_fd(int fd, const npio_Array*);

Writes a header corresponding to the provided array into a writable file
descriptor. The utility of this function is questionable, but it is provided
for completeness anyway.


### npio_save_header_mem

#### Synopsis

    int npio_save_header_mem(void* p, size_t sz, const npio_Array* array, size_t *out_size);

Prepares a numpy header corresponding to the given array in the designated
memory buffer, including any padding spaces as required by the numpy spec.
`out_size` contains the number of bytes written into the buffer on successful
return. If the destination buffer `p` is too small, or if `p` is null, then
`out_size` contains the total size of the header that would have been written
had the buffer been sufficiently large. You should check that `out_size` is
less than or equal to the size of the destination buffer to ensure that the
header was not truncated.

#### Return

- 0 on success.
- `EINVAL`: if the array is not valid.

NOTE: Current implementation returns ERANGE if the buffer is not large enough
to hold the header.  To be fixed in future.



C++ API
-------

The C++-specific functions and classes are available in namespace `npio`. If
you define NPIO_CXX_ENABLE_EXCEPTIONS, then the functions below will throw
instead of returning an error code. This macro has no impact on the C API,
which is also always available in C++ code. Some of the functions defined below
use C++11 language features. These are automatically disabled if you have not
enabled C++11 in your compiler.


### npio::save

#### Synopsis

    template <class T>
    int save(const char* filename, std::initializer_list<size_t> shape, const T* data);
    
    template <class T>
    int save(const char* filename, size_t nDim, const size_t* shape, const T* data);

    template <class T>
    int save(int fd, std::initializer_list<size_t> shape, const T* data);

    template <class T>
    int save(int fd, size_t nDim, const size_t* shape, const T* data);



### npio::Array

#### Synopsis

    Array(const char* filename, size_t max_dim = 32);
    Array(int fd, size_t max_dim = 32);
    Array(void *p, size_t sz, size_t max_dim = 32);

Loads an array from file or memory. The data is not copied if loading from
memory.  If NPIO_CXX_ENABLE_EXCEPTIONS is defined, this will throw an exception
of type `std::system_error` on failure.  Otherwise you should call the `error()`
function to determine if the loading was successful.


### npio::Array::~Array

#### Synopsis

    ~Array()

Releases all resources associated with the loaded array.
    

### Array accessors

#### Synopsis

    size_t dim() const;
    const size_t *shape() const;    
    size_t size() const;
    bool fortran_order() const;
    bool floating_point() const;
    bool is_signed() const;
    bool bit_width() const;
    const void* data() const;
    char major_version() const;
    char minor_version() const;

These are 1-1 readonly accessors for the equivalent fields in `npio_Array`.

#### Synopsis

    size_t shape(size_t i) const

This returns the shape of the array along the i-th dimension.  If i exceeds
the dimensionality of the array, 1 is returned.

#### Synopsis

    template <class T> T* get() const

Returns a typed pointer to the array data.  If the type T does not match the
loaded data, `std::bad_cast` is thrown if exceptions are enabled, otherwise
the return value is NULL.

#### Synopsis

    template <class T> /*unspecified*/ values() const

Returns a range-expression so you can do a range-based for loop such as

    for (auto x: a.values<float>())
      /* do something with value x */

If the type specified is not compatible with the underlying data, throws a
`bad_cast` exception if exceptions are enabled, otherwise returns an empty
range.

