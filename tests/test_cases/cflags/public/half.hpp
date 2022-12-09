#ifndef HALF_HPP
#define HALF_HPP

#ifdef HALF_PRECISION_DOUBLE
#define HALF_RESULT_TYPE double
#else
#define HALF_RESULT_TYPE int
#endif

HALF_RESULT_TYPE half(int val);

#endif
