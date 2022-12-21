/**
 * \author Wentai Ye
 */

#ifndef _LOGGER_H_
#define _LOGGER_H_

extern int seq;     // log sequence number

/**
 * create a log file
 */
void create_log_file(void);

/**
 * append a log message to the log file
 * param msg:  the pointer of log message
 */
void append_log(char* msg);

#endif /* _LOGGER_H_ */

