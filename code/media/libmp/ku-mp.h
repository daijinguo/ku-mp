#ifndef KU_MP_API_H_
#define KU_MP_API_H_

#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Return the ku-mp source has been compiled with(mean version).
 */
const char *ku_mp_api_version(void);

/**
 * ku-mp context used by API. Every player has its own private context;
 */
typedef struct ku_mp_context_t ku_mp_context;

typedef enum ku_mp_error {
	/**
     * No error happened (used to signal successful operation).
     * Keep in mind that many API functions returning error codes can also
     * return positive values, which also indicate success. API users can
     * hardcode the fact that ">= 0" means success.
     */
	KU_MP_SUCCESS = 0,

	/**
     * Memory allocation failed.
     */
	KU_MP_ERROR_NOMEM = -1,

	/**
     * Generic catch-all error if a parameter is set to an invalid or
     * unsupported value. This is used if there is no better error code.
     */
	KU_MP_ERROR_INVALID_PARAMETER = -2,
} ku_mp_error;

/**
 * Return a string describing the error. For unknown errors, the string
 * "unknown error" is returned.
 *
 * @param error error number, see enum ku_mp_error
 * @return A static string describing the error. The string is completely
 *         static, i.e. doesn't need to be deallocated, and is valid forever.
 */
const char *ku_mp_error_string(int error);

#ifdef __cplusplus
}
#endif

#endif
