#ifndef _NETWORK_H_
#define _NETWORK_H_

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/_endian.h>
#include <sys/socket.h>

#define WOL_SIZE 102
#define MAC_SIZE 6

uint8_t ctoi(const char c);
void send_packet(char *mac);
uint8_t *parse_mac(char *mac, uint8_t *mac_buf);
uint8_t *make_packet(char *mac, uint8_t *packet);
char *get_broadcast(int socketfd);

#endif // !_NETWORK_H_
