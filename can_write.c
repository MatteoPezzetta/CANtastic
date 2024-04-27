#include <stdio.h>
#include <string.h>
#include <unistd.h> /* write() */

#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <linux/can.h>
#include <linux/can/raw.h>

int can_write() {

	/* Create a socket */
	int s;
	if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		perror("Socker");
		return 1;
	}

	/* Get interface index */
	struct ifreq ifr; /* interface */
	strcpy(ifr.ifr_name, "vcan0");
	ioctl(s, SIOCGIFINDEX, &ifr);

	/* Bind interface to socket */
	struct sockaddr_can addr;
	memset(&addr, 0, sizeof(addr));
	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("Bind");
		return 1;
	}

	/* Sending the frame */
	struct can_frame frame;
	
	frame.can_id = 0x555; /* Personalize with target frame id */
	frame.can_dlc = 5;
	sprintf(frame.data, "Hello");
	
	if (write(s, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
		perror("Write");
		return 1;
	}
}

int main() {
	can_write();
	return 0;
}
