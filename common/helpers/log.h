#ifndef JM_BUS_LOGGING_H
#define JM_BUS_LOGGING_H

#include <sys/cdefs.h>

__BEGIN_DECLS

/**
 * Log messages as INFO.
 *
 * @param[in] str The string to print.
 * @param[in] ... Options for the string.
 */
void info_log(const char *str, ...) __nonnull((1));
/**
 * Log messages as DEBUG.
 *
 * @param[in] str The string to print.
 * @param[in] ... Options for the string.
 */
void debug_log(const char *str, ...) __nonnull((1));
/**
 * Log messages as ERROR.
 *
 * @param[in] str The string to print.
 * @param[in] ... Options for the string.
 */
void error_log(const char *str, ...) __nonnull((1));

/**
 * Log messages as INFO with debugging info.
 *
 * @param[in] str The string to print.
 * @param[in] ... Options for the string.
 */
#define info_detailed(str, ...) info_log("file:%s func:%s line:%d -- " str, __FILE__, __func__, __LINE__, __VA_ARGS__);
/**
 * Log messages as DEBUG with debugging info.
 *
 * @param[in] str The string to print.
 * @param[in] ... Options for the string.
 */
#define debug_detailed(str, ...) debug_log("file:%s func:%s line:%d -- " str, __FILE__, __func__, __LINE__, __VA_ARGS__);
/**
 * Log messages as ERROR with debugging info.
 *
 * @param[in] str The string to print.
 * @param[in] ... Options for the string.
 */
#define error_detailed(str, ...) error_log("file:%s func:%s line:%d -- " str, __FILE__, __func__, __LINE__, __VA_ARGS__);

__END_DECLS

#endif
