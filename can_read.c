#include <stdio.h>
#include <string.h>

#include <unistd.h>

#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <linux/can.h>
#include <linux/can/raw.h>

typedef void (*CallbackFunction)(unsigned char*, int);

typedef struct {
	CallbackFunction callback;
} CallbackHandler;

void RegisterCallback(CallbackHandler * handler, CallbackFunction callback) {
	handler->callback = callback;
}

void AsciiCallback(unsigned char* data, int data_length) {
	printf("\tConverting to ascii characters:");
	for (int i = 0; i < data_length; i++) {
		printf("%c", data[i]);
	}
}

int can_read(CallbackHandler* handler) {

	/* Create socket */
	int s;
	if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		perror("Socker");
		return 1;
	}

	/* Retrieve the CAN interface index  */
	struct ifreq ifr;
	
	strcpy(ifr.ifr_name, "vcan0");
	ioctl(s, SIOCGIFINDEX, &ifr);

	/* Bind the socket to the interface */
	struct sockaddr_can addr;

	memset(&addr, 0, sizeof(addr));
	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("Bind");
		return 1;
	}

	/* Read CAN frame */
	while (1) {
		int nbytes;
		struct can_frame frame;

		nbytes = read(s, &frame, sizeof(struct can_frame));

		if (nbytes < 0) {
			perror("Read");
			return 1;
		}

		printf("0x%03X [%d] ", frame.can_id, frame.can_dlc);

		for (int i = 0; i < frame.can_dlc; i++) {
			printf("%02X ", frame.data[i]);
		}

		if (NULL != handler->callback) {
			handler->callback(frame.data, frame.can_dlc);
		}

		printf("\r\n");
	}
}

int main() {
	CallbackHandler handler_;
	RegisterCallback(&handler_, AsciiCallback);
	can_read(&handler_);
	return 0;
}
