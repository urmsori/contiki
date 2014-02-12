#ifndef NTP_H
#define NTP_H
#include "sclock.h"

/* Protocol */
/* Request/Reply
 * header : value
 * 
 * ex)
 * Sync_Request\n
 *
 * ex)
 * Sync_Reply\n
 * t2:1233\n
 * t3:1256\n
 */
#define REQUEST_LEN 50
#define REPLY_LEN 100

struct ntp{
  struct sclock *sc;
  struct timestamp t1, t2, t3, t4;
};

/* ntp functions */
void ntp_init(struct ntp *ntp, struct sclock *clock);
int ntp_make_request(struct ntp *ntp, uint8_t *packet);
/*char* ntp_make_reply(struct ntp *ntp, int* len_p);
void ntp_handle(char* handle_str, struct ntp *ntp);
void ntp_handle_request(struct ntp *ntp);
void ntp_handle_reply(struct ntp *ntp);
void ntp_send_packet(char* packet, int len);
void print_ntp(struct ntp* n);*/


#endif /* NTP_H */
