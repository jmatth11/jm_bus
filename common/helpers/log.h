#ifndef JM_BUS_LOGGING_H
#define JM_BUS_LOGGING_H

#include <sys/cdefs.h>

__BEGIN_DECLS

void info_log(const char *str, ...) __nonnull((1));
void debug_log(const char *str, ...) __nonnull((1));
void error_log(const char *str, ...) __nonnull((1));

#define info_detailed(str, ...) info_log("file:%s func:%s line:%d -- " str, __FILE__, __func__, __LINE__, __VA_ARGS__);
#define debug_detailed(str, ...) debug_log("file:%s func:%s line:%d -- " str, __FILE__, __func__, __LINE__, __VA_ARGS__);
#define error_detailed(str, ...) error_log("file:%s func:%s line:%d -- " str, __FILE__, __func__, __LINE__, __VA_ARGS__);

__END_DECLS

#endif
