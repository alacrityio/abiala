/*
 * Copyright (C) 2012 William Swanson
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the names of the authors or
 * their institutions shall not be used in advertising or otherwise to
 * promote the sale, use or other dealings in this Software without
 * prior written authorization from the authors.
 */

/*
 * This file has been modified by block.one
 */

#ifndef ALAIO_MAP_MACRO_H_INCLUDED
#define ALAIO_MAP_MACRO_H_INCLUDED

#define ALAIO_EVAL0(...) __VA_ARGS__
#define ALAIO_EVAL1(...) ALAIO_EVAL0(ALAIO_EVAL0(ALAIO_EVAL0(__VA_ARGS__)))
#define ALAIO_EVAL2(...) ALAIO_EVAL1(ALAIO_EVAL1(ALAIO_EVAL1(__VA_ARGS__)))
#define ALAIO_EVAL3(...) ALAIO_EVAL2(ALAIO_EVAL2(ALAIO_EVAL2(__VA_ARGS__)))
#define ALAIO_EVAL4(...) ALAIO_EVAL3(ALAIO_EVAL3(ALAIO_EVAL3(__VA_ARGS__)))
#define ALAIO_EVAL(...) ALAIO_EVAL4(ALAIO_EVAL4(ALAIO_EVAL4(__VA_ARGS__)))

#define ALAIO_MAP_END(...)
#define ALAIO_MAP_OUT

#define ALAIO_MAP_GET_END2() 0, ALAIO_MAP_END
#define ALAIO_MAP_GET_END1(...) ALAIO_MAP_GET_END2
#define ALAIO_MAP_GET_END(...) ALAIO_MAP_GET_END1
#define ALAIO_MAP_NEXT0(test, next, ...) next ALAIO_MAP_OUT
#define ALAIO_MAP_NEXT1(test, next) ALAIO_MAP_NEXT0(test, next, 0)
#define ALAIO_MAP_NEXT(test, next) ALAIO_MAP_NEXT1(ALAIO_MAP_GET_END test, next)

// Macros below this point added by block.one

#define ALAIO_MAP_REUSE_ARG0_0(f, arg0, x, peek, ...)                                                                  \
   f(arg0, x) ALAIO_MAP_NEXT(peek, ALAIO_MAP_REUSE_ARG0_1)(f, arg0, peek, __VA_ARGS__)
#define ALAIO_MAP_REUSE_ARG0_1(f, arg0, x, peek, ...)                                                                  \
   f(arg0, x) ALAIO_MAP_NEXT(peek, ALAIO_MAP_REUSE_ARG0_0)(f, arg0, peek, __VA_ARGS__)
// Handle 0 arguments
#define ALAIO_MAP_REUSE_ARG0_I(f, arg0, peek, ...)                                                                     \
   ALAIO_MAP_NEXT(peek, ALAIO_MAP_REUSE_ARG0_1)(f, arg0, peek, __VA_ARGS__)
#define ALAIO_MAP_REUSE_ARG0(f, ...) ALAIO_EVAL(ALAIO_MAP_REUSE_ARG0_I(f, __VA_ARGS__, ()()(), ()()(), ()()(), 0))

#endif
