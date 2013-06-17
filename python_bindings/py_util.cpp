/* -----------------------------------------------------------------------
   Utility functions for Halide Python bindings.
   ----------------------------------------------------------------------- */

#include "py_util.h"
#include "../apps/support/image_io.h"
#include <signal.h>
#include <string>
#include "Python.h"
#include "frameobject.h"

void (*signal(int signum, void (*sighandler)(int)))(int);

void assign(Func &f, const Expr &e) {
  f = e;
}

Expr expr_from_int(int a) { return Expr(a); }

Expr add(Expr a, Expr b) { return a+b; }
Expr sub(Expr a, Expr b) { return a-b; }
Expr neg(Expr a) { return -a; }
Expr mul(Expr a, Expr b) { return a*b; }
Expr div(Expr a, Expr b) { return a/b; }
Expr mod(Expr a, Expr b) { return a%b; }

Expr lt(Expr a, Expr b) { return a < b; }
Expr le(Expr a, Expr b) { return a <= b; }
Expr eq(Expr a, Expr b) { return a == b; }
Expr ne(Expr a, Expr b) { return a != b; }
Expr gt(Expr a, Expr b) { return a > b; }
Expr ge(Expr a, Expr b) { return a >= b; }

Expr and_op(Expr a, Expr b) { return a&&b; }
Expr or_op(Expr a, Expr b) { return a||b; }
Expr invert(Expr a) { return !a; }

Expr iadd(Expr &a, Expr b) { a += b; return a; }
Expr isub(Expr &a, Expr b) { a -= b; return a; }
Expr imul(Expr &a, Expr b) { a *= b; return a; }
Expr idiv(Expr &a, Expr b) { a /= b; return a; }

FuncRefExpr call(Func &a, Expr b) { return a(b); }
FuncRefExpr call(Func &a, Expr b, Expr c) { return a(b, c); }
FuncRefExpr call(Func &a, Expr b, Expr c, Expr d) { return a(b, c, d); }
FuncRefExpr call(Func &a, Expr b, Expr c, Expr d, Expr e) { return a(b, c, d, e); }
FuncRefExpr call(Func &a, const std::vector<Expr> &args) { return a(args); }

Expr call(const ImageParam &a, Expr b) { return a(b); }
Expr call(const ImageParam &a, Expr b, Expr c) { return a(b, c); }
Expr call(const ImageParam &a, Expr b, Expr c, Expr d) { return a(b, c, d); }
Expr call(const ImageParam &a, Expr b, Expr c, Expr d, Expr e) { return a(b, c, d, e); }

void assign(FuncRefExpr &a, Expr b) { a = b; }
void assign(FuncRefVar &a, Expr b) { a = b; }
void assign(ImageParam &a, const Buffer &b) { a.set(b); }

#define DEFINE_TYPE(T) void assign(ImageParam &a, Image<T> b) { a = b; }
#include "expand_types.h"
#undef DEFINE_TYPE

#define DEFINE_TYPE(T) void assign(Image<T> &a, Buffer b) { a = b; }
#include "expand_types.h"
#undef DEFINE_TYPE

#define DEFINE_TYPE(T) \
void assign(Param<T> &a, int b) { a = b; } \
void assign(Param<T> &a, double b) { a = b; }
#include "expand_types.h"
#undef DEFINE_TYPE

#define DEFINE_TYPE(T) Image<T> load_png(Image<T> a, std::string b) { return load<T>(b); }
DEFINE_TYPE(uint8_t)
DEFINE_TYPE(uint16_t)
DEFINE_TYPE(uint32_t)
DEFINE_TYPE(float)
DEFINE_TYPE(double)
//#include "expand_types.h"
#undef DEFINE_TYPE

#define DEFINE_TYPE(T) void save_png(Image<T> a, std::string b) { save(a, b); }
DEFINE_TYPE(uint8_t)
DEFINE_TYPE(uint16_t)
DEFINE_TYPE(uint32_t)
DEFINE_TYPE(float)
DEFINE_TYPE(double)
#undef DEFINE_TYPE

void signal_handler(int sig_num) {
    printf("Trapped signal %d in C++ layer, exiting\n", sig_num);
 	//PyErr_SetString(PyExc_ValueError,"Trapped signal in C++ layer, exiting");
    printf("\n");
    PyThreadState *tstate = PyThreadState_GET();
    if (NULL != tstate && NULL != tstate->frame) {
        PyFrameObject *frame = tstate->frame;

        printf("Python stack trace:\n");
        while (NULL != frame) {
            int line = frame->f_lineno;
            const char *filename = PyString_AsString(frame->f_code->co_filename);
            const char *funcname = PyString_AsString(frame->f_code->co_name);
            printf("    %s(%d): %s\n", filename, line, funcname);
            frame = frame->f_back;
        }
    }
    exit(0);
}

void exit_on_signal() {
    signal(SIGINT , signal_handler);
    signal(SIGABRT , signal_handler);
    signal(SIGILL , signal_handler);
    signal(SIGFPE , signal_handler);
    signal(SIGSEGV, signal_handler);
    signal(SIGTERM , signal_handler);
    signal(SIGBUS, signal_handler);
}

#define DEFINE_TYPE(T) \
std::string image_to_string(const Image<T> &a) { \
    int dims = a.dimensions(); \
    Buffer d(a); \
    return std::string((char *) a.data(), (d.type().bits/8)*d.stride(dims-1)*a.size(dims-1)); \
}
DEFINE_TYPE(uint8_t)
DEFINE_TYPE(uint16_t)
DEFINE_TYPE(uint32_t)
DEFINE_TYPE(int8_t)
DEFINE_TYPE(int16_t)
DEFINE_TYPE(int32_t)
DEFINE_TYPE(float)
DEFINE_TYPE(double)
#undef DEFINE_TYPE

#define DEFINE_TYPE(T) Buffer to_buffer(const Image<T> &a) { return Buffer(a); }
DEFINE_TYPE(uint8_t)
DEFINE_TYPE(uint16_t)
DEFINE_TYPE(uint32_t)
DEFINE_TYPE(int8_t)
DEFINE_TYPE(int16_t)
DEFINE_TYPE(int32_t)
DEFINE_TYPE(float)
DEFINE_TYPE(double)
#undef DEFINE_TYPE

#define DEFINE_TYPE(T) \
Expr call(Image<T> &a, Expr b) { return a(b); } \
Expr call(Image<T> &a, Expr b, Expr c) { return a(b,c); } \
Expr call(Image<T> &a, Expr b, Expr c, Expr d) { return a(b,c,d); }                     \
Expr call(Image<T> &a, Expr b, Expr c, Expr d, Expr e) { return a(b,c,d,e); }                   
DEFINE_TYPE(uint8_t)
DEFINE_TYPE(uint16_t)
DEFINE_TYPE(uint32_t)
DEFINE_TYPE(int8_t)
DEFINE_TYPE(int16_t)
DEFINE_TYPE(int32_t)
DEFINE_TYPE(float)
DEFINE_TYPE(double)
#undef DEFINE_TYPE

Expr minimum_func(const Expr &a) { return minimum(a); }
Expr maximum_func(const Expr &a) { return maximum(a); }
Expr product_func(const Expr &a) { return product(a); }
Expr sum_func(const Expr &a) { return sum(a); }

void iadd(FuncRefVar &f, const Expr &e) { f += e; }
void imul(FuncRefVar &f, const Expr &e) { f *= e; }
void iadd(FuncRefExpr &f, const Expr &e) { f += e; }
void imul(FuncRefExpr &f, const Expr &e) { f *= e; }

//void assign(UniformImage &a, Image<uint8_t> b) { a = DynImage(b); }

#define DEFINE_TYPE(T) \
void assign_array(Image<T> &a, size_t base, size_t xstride) { \
    for (int x = 0; x < a.size(0); x++) { \
        a(x) = *(T*)(((uint8_t *) base) + (xstride*x)); \
    } \
} \
void assign_array(Image<T> &a, size_t base, size_t xstride, size_t ystride) { \
    for (int x = 0; x < a.size(0); x++) { \
    for (int y = 0; y < a.size(1); y++) { \
        a(x,y) = *(T*)(((uint8_t *) base) + (xstride*x) + (ystride*y)); \
    } \
    } \
} \
void assign_array(Image<T> &a, size_t base, size_t xstride, size_t ystride, size_t zstride) { \
    for (int x = 0; x < a.size(0); x++) { \
    for (int y = 0; y < a.size(1); y++) { \
    for (int z = 0; z < a.size(2); z++) { \
        a(x,y,z) = *(T*)(((uint8_t *) base) + (xstride*x) + (ystride*y) + (zstride*z)); \
    } \
    } \
    } \
} \
void assign_array(Image<T> &a, size_t base, size_t xstride, size_t ystride, size_t zstride, size_t wstride) { \
    for (int x = 0; x < a.size(0); x++) { \
    for (int y = 0; y < a.size(1); y++) { \
    for (int z = 0; z < a.size(2); z++) { \
    for (int w = 0; w < a.size(3); w++) { \
        a(x,y,z,w) = *(T*)(((uint8_t *) base) + (xstride*x) + (ystride*y) + (zstride*z) + (wstride*w)); \
    } \
    } \
    } \
    } \
}
DEFINE_TYPE(uint8_t)
DEFINE_TYPE(uint16_t)
DEFINE_TYPE(uint32_t)
DEFINE_TYPE(int8_t)
DEFINE_TYPE(int16_t)
DEFINE_TYPE(int32_t)
DEFINE_TYPE(float)
DEFINE_TYPE(double)
#undef DEFINE_TYPE

