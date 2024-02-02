#if DONT_USE_ALLOCA
/* heap versions */
# define ALLOCA(type, array) ((array) = (type *)getbytes((nmemb) * sizeof(type)))
# define FREEA(type, array) (free((array)))

#else /* !DONT_USE_ALLOCA */
/* stack version (unless <nmemb> exceeds <maxnmemb>) */

# ifdef HAVE_ALLOCA_H
#  include <alloca.h> /* linux, mac, mingw, cygwin,... */
# elif defined _WIN32
#  include <malloc.h> /* MSVC or mingw on windows */
# else
#  include <stdlib.h> /* BSDs for example */
# endif

# define ALLOCA(type, nmemb) (type *)((nmemb) < (100) ? \
            alloca((nmemb) * sizeof(type)) : malloc((nmemb) * sizeof(type)))
# define FREEA(array) (                                 \
        ((nmemb) < (100) || (free((array)), 0)))
#endif /* !DONT_USE_ALLOCA */

#define CALLOCA()