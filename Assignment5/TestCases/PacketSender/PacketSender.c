#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX_ADDR_LEN 128

#define ADDR_LENGTH_OFFSET 4
#define ADDR_OFFSET 8

typedef unsigned char shsize_t;

typedef struct{
  shsize_t len;
  char addr[MAX_ADDR_LEN];
  char* hwtype;
  char* prototype;
  char* oper;
  char* protolen;
} arp_addr;

void calc_addr(char addrList[], int size) {
  const int offset = 0xA;
  printf("\nProcessing: ");

  for (int i = size-1; i > 0; i--) {
    addrList[i] = (addrList[i] - offset) ^ offset;
  }
}

void print_address(char *packet) {
  arp_addr hwaddr;

  hwaddr.hwtype = malloc(4);

  memset(hwaddr.hwtype, 1, 4);
  memset(hwaddr.addr, 0, MAX_ADDR_LEN);

  char *hwaddLen = packet + ADDR_LENGTH_OFFSET;

  hwaddr.len = (shsize_t) *(hwaddLen);
  memcpy(hwaddr.addr, packet + ADDR_OFFSET, hwaddr.len);
  memcpy(hwaddr.hwtype, packet, 4);

  calc_addr(hwaddr.addr, hwaddr.len);

  printf("Sender hardware address: ");
  for (int i = 0; i < hwaddr.len - 1; i++) {
    printf("%02hhx::", hwaddr.addr[i] );
  }
  printf("%02hhx\n", hwaddr.addr[hwaddr.len - 1]);

  return;
}

int main(int argc, char *argv[]) {
  struct stat sbuf;
  char *packet;
  char *packetCpy;
  int fd;

  if (argc != 2){
    printf("Usage: %s <packet file>\n", argv[0]);
    return 0;
  }

  if ((stat(argv[1], &sbuf)) < 0){
    printf("Error opening packet file\n");
    return 0;
  }

  if ((fd = open(argv[1], O_RDONLY)) < 0){
    printf("Error opening packet file\n");
    return 0;
  }

  packet = (char *)malloc((size_t)(sbuf.st_size * sizeof(char)));

  packetCpy = (char *)calloc(sbuf.st_size, sizeof(char));

  if (read(fd, packet, sbuf.st_size) < 0){
    printf("Error reading packet from file\n");
    return 0;
  }

  close(fd);
  memcpy(packetCpy, packet, sbuf.st_size * sizeof(char));
  print_address(packet);
  free(packet);
  free(packetCpy);
  return 0;
}

