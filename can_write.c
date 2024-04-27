#include <stdio.h>
#include <string.h>

#include <unistd.h> /* write() */

#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <linux/can.h>
#include <linux/can/raw.h>

int can_write(char* input_data, int input_data_length) {

	/* Create a socket */
	int s;
	if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		perror("Socket");
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
	if (input_data_length <= 8)
		frame.can_dlc = input_data_length;
	else
		frame.can_dlc = 8;
	/*sprintf(frame.data, "Hello");*/
	strcpy(frame.data, input_data);
	
	if (write(s, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
		perror("Write");
		return 1;
	}
}

int main() {
	char* str = "Hello Forever"; /* Max 8 bytes will be sent, unless ISO-TP is implemented */
	int str_len = sizeof(str);
	can_write(str, str_len);
	return 0;
}
