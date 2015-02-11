#ifndef _PL_PLANTFORM_H_
#define _PL_PLANTFORM_H_

#ifdef _WIN32
# define FMT_TYPE_SIZE_T "%u"
# define FMT_TYPE_INT "%ld"
# define FMT_TYPE_FLOAT "%lf"
#else
# define FMT_TYPE_SIZE_T "%zu"
# define FMT_TYPE_INT "%ld"
# define FMT_TYPE_FLOAT "%lf"
#endif

#endif
