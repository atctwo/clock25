#ifndef CLOCK_NTP_H
#define CLOCK_NTP_H

// NTP client
void get_time_from_ntp();

// NTP server
bool setup_ntp_server();
void check_ntp_server();

#endif /* CLOCK_NTP_H */