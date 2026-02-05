#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include "can.h"
#include "shared.h"

static int sock = -1;

int CanInit(){
  printf("CAN Reader starting\n");

  int sock = socket(PF_CAN, SOCK_RAW, CAN_RAW);
  if (sock < 0) {
      perror("Socket");
      return -1;
  }

  struct ifreq ifr;

  strcpy(ifr.ifr_name, "can0");
  ioctl(sock, SIOCGIFINDEX, &ifr);

  struct sockaddr_can addr;

  memset(&addr, 0, sizeof(addr));
  addr.can_family = AF_CAN;
  addr.can_ifindex = ifr.ifr_ifindex;

  if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0){
      perror("Bind");
      return -1;
  }

  return 1;
}

int CanRead(Telemetry* telemetry){
  // switch case on can frame id... todo
  return 0;
}

int CanClose(){
  printf("CAN Reader closing\n");

  if (close(sock) < 0){
      perror("Close");
      return -1;
  }
  return 1;
}