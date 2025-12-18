#include "network.h"
#include <string.h>

inline uint8_t ctoi(const char c) {
  if (c >= 'a')
    return c - 'a' + 10;
  else if (c >= 'A')
    return c - 'A' + 10;
  else
    return c - '0';
}

void send_packet(char *mac) {
  uint8_t *packet = make_packet(mac);

  int socketfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (socketfd == -1) {
    perror("error making socket");
  }
  int opt = 1;
  setsockopt(socketfd, SOL_SOCKET, SO_BROADCAST, &opt, sizeof(&opt));
  struct sockaddr_in addr = {
      .sin_family = AF_INET,
      .sin_port = htons(9),
  };
  // inet_aton(get_broadcast(), &addr.sin_addr);
  inet_aton("10.0.69.255", &addr.sin_addr);

  int sendres = sendto(socketfd, packet, WOL_SIZE, 0,
                       (const struct sockaddr *)&addr, sizeof(addr));
  if (sendres == -1) {
    perror("error sending packet");
  }
}

// creates a wake on lan packet with given mac address returns an array of uint8
// bytes.
inline uint8_t *make_packet(char *mac) {
  // FIXME: exit if unparsable
  // parse colon seperated string of bytes to ints
  uint8_t newMac[MAC_SIZE];
  uint8_t oct1 = (ctoi(mac[0]) << 4) | ctoi(mac[1]);
  uint8_t oct2 = (ctoi(mac[3]) << 4) | ctoi(mac[4]);
  uint8_t oct3 = (ctoi(mac[6]) << 4) | ctoi(mac[7]);
  uint8_t oct4 = (ctoi(mac[9]) << 4) | ctoi(mac[10]);
  uint8_t oct5 = (ctoi(mac[12]) << 4) | ctoi(mac[13]);
  uint8_t oct6 = (ctoi(mac[15]) << 4) | ctoi(mac[16]);

  memcpy(newMac, &oct1, 1);
  memcpy(newMac + 1, &oct2, 1);
  memcpy(newMac + 2, &oct3, 1);
  memcpy(newMac + 3, &oct4, 1);
  memcpy(newMac + 4, &oct5, 1);
  memcpy(newMac + 5, &oct6, 1);

  static uint8_t packet[WOL_SIZE] = {
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  };

  for (size_t i = 0; i < 16; i++) {
    memcpy(packet + (i * MAC_SIZE) + 6, newMac, MAC_SIZE);
  }

  // for (int i = 0; i < WOL_SIZE; i++) {
  //   printf("%-3x ", packet[i]);
  //   if ((i + 1) % 6 == 0 && i != 0) {
  //     printf("\n");
  //   }
  // }
  // printf("\n");

  return packet;
}

char *get_broadcast(int socketfd) { return NULL; }
