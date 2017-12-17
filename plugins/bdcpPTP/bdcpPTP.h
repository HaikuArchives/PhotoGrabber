/* bdcpPTP.h */

#ifndef BDCPPTP_H
#define BDCPPTP_H

#include <string>
#include <stdio.h>

#define DL_SIZE 4096


/* Defines for tracing and debugging */
#define APP_NAME "bdcpPTP"
#define APP_SIGNATURE "application/x-vnd.EnhancedBDCP2PTP"

/* Defines for tracing and debugging */
#define TRACING
#define DEBUGGING

#ifdef DEBUGGING
#define DEBUG(x...) fprintf(stderr, APP_NAME " - " x)
#else
#define DEBUG(x...)
#endif
#ifdef TRACING
#define TRACE(x...) fprintf(stderr, APP_NAME " - " x)
#else
#define TRACE(x...)
#endif
#define MSG(x...) printf(x)

#endif
