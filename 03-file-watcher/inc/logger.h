#ifndef LOGGER_H 
#define LOGGER_H

void log_event(const char *event_desc);
void *handle_single_event(void *arg);

#endif //LOGGER_H