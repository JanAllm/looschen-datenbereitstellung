
#ifndef IC4CORE_API_H
#define IC4CORE_API_H

#if defined _WIN32

#ifdef IC4CORE_STATIC_DEFINE
#  define IC4CORE_API
#  define IC4CORE_NO_EXPORT
#else
#  ifndef IC4CORE_API
#    ifdef ic4core_EXPORTS
        /* We are building this library */
#      define IC4CORE_API __declspec(dllexport)
#    else
        /* We are using this library */
#      define IC4CORE_API __declspec(dllimport)
#    endif
#  endif

#  ifndef IC4CORE_NO_EXPORT
#    define IC4CORE_NO_EXPORT 
#  endif
#endif

#ifndef IC4CORE_DEPRECATED
#  define IC4CORE_DEPRECATED __declspec(deprecated)
#endif

#ifndef IC4CORE_DEPRECATED_EXPORT
#  define IC4CORE_DEPRECATED_EXPORT IC4CORE_API IC4CORE_DEPRECATED
#endif

#ifndef IC4CORE_DEPRECATED_NO_EXPORT
#  define IC4CORE_DEPRECATED_NO_EXPORT IC4CORE_NO_EXPORT IC4CORE_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef IC4CORE_NO_DEPRECATED
#    define IC4CORE_NO_DEPRECATED
#  endif
#endif

#elif defined __linux__

#ifdef IC4CORE_STATIC_DEFINE
#  define IC4CORE_API
#  define IC4CORE_NO_EXPORT
#else
#  ifndef IC4CORE_API
#    ifdef ic4core_EXPORTS
/* We are building this library */
#      define IC4CORE_API __attribute__((visibility("default")))
#    else
/* We are using this library */
#      define IC4CORE_API __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef IC4CORE_NO_EXPORT
#    define IC4CORE_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef IC4CORE_DEPRECATED
#  define IC4CORE_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef IC4CORE_DEPRECATED_EXPORT
#  define IC4CORE_DEPRECATED_EXPORT IC4CORE_API IC4CORE_DEPRECATED
#endif

#ifndef IC4CORE_DEPRECATED_NO_EXPORT
#  define IC4CORE_DEPRECATED_NO_EXPORT IC4CORE_NO_EXPORT IC4CORE_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef IC4CORE_NO_DEPRECATED
#    define IC4CORE_NO_DEPRECATED
#  endif
#endif

#else

#error "Unknown environment to build"

#endif

#endif /* IC4CORE_API_H */
