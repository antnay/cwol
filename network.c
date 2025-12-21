#include "network.h"
#include <stdint.h>
#include <string.h>
#include <sys/ioctl.h>

inline uint8_t ctoi(const char c) {
  if (c >= 'a')
    return c - 'a' + 10;
  else if (c >= 'A')
    return c - 'A' + 10;
  else
    return c - '0';
}

void send_packet(char *mac) {
  uint8_t packet[WOL_SIZE];
  make_packet(mac, packet);

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
  inet_aton(get_broadcast(socketfd), &addr.sin_addr);
  // inet_aton("10.0.69.255", &addr.sin_addr);

  // int sendres = sendto(socketfd, packet, WOL_SIZE, 0,
  //                      (const struct sockaddr *)&addr, sizeof(addr));
  // if (sendres == -1) {
  //   perror("error sending packet");
  // }
}

inline uint8_t *parse_mac(char *mac, uint8_t *mac_buf) {
#ifdef DEBUG
  printf("mac: %s\n", mac);
#endif /* ifdef DEBUG */
  uint8_t oct1 = (ctoi(mac[0]) << 4) | ctoi(mac[1]);
  uint8_t oct2 = (ctoi(mac[3]) << 4) | ctoi(mac[4]);
  uint8_t oct3 = (ctoi(mac[6]) << 4) | ctoi(mac[7]);
  uint8_t oct4 = (ctoi(mac[9]) << 4) | ctoi(mac[10]);
  uint8_t oct5 = (ctoi(mac[12]) << 4) | ctoi(mac[13]);
  uint8_t oct6 = (ctoi(mac[15]) << 4) | ctoi(mac[16]);

  memcpy(mac_buf, &oct1, 1);
  memcpy(mac_buf + 1, &oct2, 1);
  memcpy(mac_buf + 2, &oct3, 1);
  memcpy(mac_buf + 3, &oct4, 1);
  memcpy(mac_buf + 4, &oct5, 1);
  memcpy(mac_buf + 5, &oct6, 1);

  return mac_buf;
}

// creates a wake on lan packet with given mac address returns an array of uint8
// bytes.
inline uint8_t *make_packet(char *mac, uint8_t *packet) {
  uint8_t mac_buf[MAC_SIZE];
  uint8_t *newMac = parse_mac(mac, mac_buf);
  packet[0] = 0xFF;
  packet[1] = 0xFF;
  packet[2] = 0xFF;
  packet[3] = 0xFF;
  packet[4] = 0xFF;
  packet[5] = 0xFF;

  for (size_t i = 0; i < 16; i++) {
    memcpy(packet + (i * MAC_SIZE) + 6, newMac, MAC_SIZE);
  }

#ifdef DEBUG
  for (int i = 0; i < WOL_SIZE; i++) {
    printf("%-3x ", packet[i]);
    if ((i + 1) % 6 == 0 && i != 0) {
      printf("\n");
    }
  }
  printf("\n");
#endif /* ifdef DEBUG */

  return packet;
}

char *get_broadcast(int socketfd) {
  struct ifreq ifr;
  struct sockaddr_in *addr;
  char *broadcast;

  if (socketfd == -1) {
    perror("error making socket");
    return NULL;
  }

  memset(&ifr, 0, sizeof(ifr));
  ifr.ifr_addr.sa_family = AF_INET;

  if (ioctl(socketfd, SIOCGIFBRDADDR, &ifr) == -1) {
    perror("error getting broadcast address");
    return NULL;
  }

  addr = (struct sockaddr_in *)&ifr.ifr_addr;
  broadcast = (char *)&addr->sin_addr;
#ifdef DEBUG
  printf("broadcast: %s\n", broadcast);
#endif /* ifdef DEBUG */

  return broadcast;
}
