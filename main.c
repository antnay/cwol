#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/_endian.h>
#include <sys/socket.h>

#define WOL_SIZE 102
#define MAC_SIZE 8 * 6

// todo: db
// todo: tui mode

char *help();
uint8_t *makePacket(char *mac);

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    help();
    return EXIT_FAILURE;
  }

  printf("mac: %s\n", argv[1]);

  uint8_t *packet = makePacket(argv[1]);

  printf("packet: %s", packet);

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
  inet_aton("10.0.69.255", &addr.sin_addr);

  // int sendres = sendto(socketfd, packet, sizeof(packet), 0,
  //                      (const struct sockaddr *)&addr, sizeof(addr));
  // if (sendres == -1) {
  //   perror("error sending packet");
  // }

  return EXIT_SUCCESS;
}

char *help() {}

// FIXME: exit if unparsable
inline uint8_t *makePacket(char *mac) {
  // TODO: convert char to hex or something better idk
  static uint8_t macBuffer[MAC_SIZE * 6];
  uint8_t oct1 = (mac[0] << 4) ^ mac[1];
  uint8_t oct2 = (mac[3] << 4) ^ mac[4];
  uint8_t oct3 = (mac[6] << 4) ^ mac[7];
  uint8_t oct4 = (mac[9] << 4) ^ mac[10];
  uint8_t oct5 = (mac[12] << 4) ^ mac[13];
  uint8_t oct6 = (mac[14] << 4) ^ mac[15];

  printf("1: %x\n", (uint8_t)2);
  printf("2: %x\n", (uint8_t)2 << 4);
  printf("3: %x\n", ((uint8_t)2 << 4) ^ 0);
  printf("mac[0] mac[1]: %x and %x | %d and %d | %c and %c \n", mac[0], mac[1],
         mac[0], mac[1], mac[0], mac[1]);
  printf("oct1: %x | %d\n", oct1, oct1);

  uint8_t newMac[MAC_SIZE];
  memcpy(newMac, &oct1, 8);
  memcpy(newMac + 1, &oct2, 8);
  memcpy(newMac + 2, &oct3, 8);

  memcpy(newMac + 3, &oct4, 8);
  memcpy(newMac + 4, &oct5, 8);
  memcpy(newMac + 5, &oct6, 8);

  static uint8_t pack[WOL_SIZE] = {
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  };

  for (size_t i = 0; i < 6; i++) {
    memcpy(macBuffer + 6 + i, newMac, MAC_SIZE);
  }

  return pack;
}
