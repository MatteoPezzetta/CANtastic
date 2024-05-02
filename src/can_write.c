#include <stdio.h>
#include <string.h>

#include <unistd.h> /* write() */

#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <linux/can.h>
#include <linux/can/raw.h>
#include <errno.h>

int can_write(char* input_data, int input_data_length) {
	printf("debug: can_write called\n");
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

int can_fd_write(char* input_data, int input_data_length) {
	printf("debug: can_fd_write called\n");
	/* Create a socket */
	int s;
	if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		perror("Socket");
		return 1;
	}

	int enable_can_fd = 1;
	if (setsockopt(s, SOL_CAN_RAW, CAN_RAW_FD_FRAMES, &enable_can_fd, sizeof(enable_can_fd)) < 0) {
        perror("Error setting CAN_RAW_FD_FRAMES option");
        close(s);
        return 1;
    }

	/* Get interface index */
	struct ifreq ifr; /* interface */
	strcpy(ifr.ifr_name, "vcan1");
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
	struct canfd_frame frame;
	
	frame.can_id = 0x123; /* Personalize with target frame id */
	frame.flags = CANFD_BRS | CANFD_ESI;
	printf("Data length=%d\n", input_data_length);
	//if (input_data_length <= 64)
		frame.len = input_data_length;
	//else
	//	frame.len = 64;
	/*sprintf(frame.data, "Hello");*/
	strcpy(frame.data, input_data);
	//memcpy(frame.data, "\x11\x22\x33\x44\x55\x66\x77\x88", 8);
	printf("frame.data=%s\n", frame.data);
	printf("frame.len=%d\n", frame.len);
	printf("Size of canfd_frame=%ld\n", sizeof(struct canfd_frame));
	int nbytes = (write(s, &frame, sizeof(struct canfd_frame)));
	if (nbytes < 0) {
		perror("Write error:");
		return 1;
	}

	if (nbytes < sizeof(frame)) {
		perror("write: incomplete CAN FD frame");
	}
	close(s);
	printf("End of write\n");
}


int main() {
	char* str = "Hello Forever"; /* Max 8 bytes will be sent, unless ISO-TP is implemented */	
	int str_len = sizeof(str);
	can_write(str, str_len);

	can_fd_write(str, 13);
	return 0;
}
