#if __GNUC__ >= 4
  #define PROF_API_PUBLIC __attribute__ ((visibility ("default")))
#else
  #define PROF_API_PUBLIC
#endif

#ifdef __cplusplus
extern "C" {
#endif

PROF_API_PUBLIC void prof_register_foo(const char *name);
PROF_API_PUBLIC void prof_unregister_foo(const char *name);

#ifdef __cplusplus
} // extern "C"
#endif
