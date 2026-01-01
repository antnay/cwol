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

void process_mac(char *mac) {
  uint8_t packet[WOL_SIZE];
  make_packet(mac, packet);
  send_packet(packet);
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

void send_packet(uint8_t *mac) {
  int socketfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (socketfd == -1) {
    perror("error making socket");
  }
  int opt = 1;
  setsockopt(socketfd, SOL_SOCKET, SO_BROADCAST, &opt, sizeof(opt));
  struct sockaddr_in addr = {
      .sin_family = AF_INET,
      .sin_port = htons(9),
  };
  char *broadcast_ip = get_broadcast();
  if (broadcast_ip == NULL) {
    fprintf(stderr, "Error: Could not get broadcast address\n");
    return;
  }
  inet_aton(broadcast_ip, &addr.sin_addr);

  int sendres = sendto(socketfd, mac, WOL_SIZE, 0,
                       (const struct sockaddr *)&addr, sizeof(addr));
  if (sendres == -1) {
    perror("error sending packet");
  }
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

char *get_broadcast() {
  static char broadcast_ip[INET_ADDRSTRLEN];
  struct ifaddrs *ifap, *ifa;

  if (getifaddrs(&ifap) == -1) {
    perror("getifaddrs");
    return NULL;
  }

  for (ifa = ifap; ifa != NULL; ifa = ifa->ifa_next) {
    if (ifa->ifa_addr == NULL)
      continue;

    if (ifa->ifa_addr->sa_family == AF_INET &&
        (ifa->ifa_flags & IFF_BROADCAST) && (ifa->ifa_flags & IFF_UP)) {
      struct sockaddr_in *sa = (struct sockaddr_in *)ifa->ifa_broadaddr;
      strlcpy(broadcast_ip, inet_ntoa(sa->sin_addr), INET_ADDRSTRLEN);
      freeifaddrs(ifap);

#ifdef DEBUG
      printf("broadcast: %s (interface: %s)\n", broadcast_ip, ifa->ifa_name);
#endif
      return broadcast_ip;
    }
  }

  freeifaddrs(ifap);
  return NULL;
}
