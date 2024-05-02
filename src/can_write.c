#include <stdio.h>
#include <string.h>

#include <unistd.h> /* write() */

#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <linux/can.h>
#include <linux/can/raw.h>
#include <errno.h>

/**
  * \brief Struct for caller functions to allow for efficient one-time socket initialization.
  */
typedef struct {
	int s;
} CanSocket;

/**
  * \brief Connect socket to CAN interface
  * \param can_socket Pointer to CanSocket struct holding the socket value
  * \return 0 if successful, 1 otherwise
  */
int can_connect(CanSocket* can_socket) { //TODO accept as input the interface name
	printf("can_write - can_connect");
	if ((can_socket->s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		perror("can_connect - Socket");
		return 1;
	}

	/* Get interface index */
	struct ifreq ifr; /* interface */
	strcpy(ifr.ifr_name, "vcan0");
	ioctl(can_socket->s, SIOCGIFINDEX, &ifr);

	/* Bind interface to socket */
	struct sockaddr_can addr;
	memset(&addr, 0, sizeof(addr));
	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	if (bind(can_socket->s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("can_connect - Bind");
		return 1;
	}
	return 0;
}

/**
  * \brief Connect socket to CAN FD interface
  * \param can_socket Pointer to CanSocket struct holding the socket value
  * \return 0 if successful, 1 otherwise
  */
int can_fd_connect(CanSocket* can_socket) {
	printf("can_write - can_fd_connect");
	/* Create a socket */
	if ((can_socket->s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		perror("Socket");
		return 1;
	}

	/* Enable can-fd */
	int enable_can_fd = 1;
	if (setsockopt(can_socket->s, SOL_CAN_RAW, CAN_RAW_FD_FRAMES, &enable_can_fd, sizeof(enable_can_fd)) < 0) {
        perror("Error setting CAN_RAW_FD_FRAMES option");
        close(can_socket->s);
        return 1;
    }

	/* Get interface index */
	struct ifreq ifr; /* interface */
	strcpy(ifr.ifr_name, "vcan1");
	ioctl(can_socket->s, SIOCGIFINDEX, &ifr);

	/* Bind interface to socket */
	struct sockaddr_can addr;
	memset(&addr, 0, sizeof(addr));
	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	if (bind(can_socket->s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("Bind");
		return 1;
	}
	return 0;
}

int can_disconnect() {

}

int can_fd_disconnect() {

}

//TODO: add as input the frame ID
int can_write(CanSocket* can_socket, char* input_data, int input_data_length) {
	printf("debug: can_write called\n");

	/* Sending the frame */
	struct can_frame frame;
	
	frame.can_id = 0x555; /* Personalize with target frame id */
	if (input_data_length <= 8)
		frame.can_dlc = input_data_length;
	else
		frame.can_dlc = 8;
	strcpy(frame.data, input_data);
	
	if (write(can_socket->s, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
		perror("Write");
		return 1;
	}
}

int can_fd_write(CanSocket* can_socket, char* input_data, int input_data_length) {
	printf("debug: can_fd_write called\n");

	/* Sending the frame */
	struct canfd_frame frame;
	
	frame.can_id = 0x123; /* Personalize with target frame id */
	frame.flags = CANFD_BRS | CANFD_ESI;
	printf("Data length=%d\n", input_data_length);
	if (input_data_length <= 64)
		frame.len = input_data_length;
	else
		frame.len = 64;
	strcpy(frame.data, input_data);
	printf("frame.data=%s\n", frame.data);
	printf("frame.len=%d\n", frame.len);
	printf("Size of canfd_frame=%ld\n", sizeof(struct canfd_frame));
	int nbytes = (write(can_socket->s, &frame, sizeof(struct canfd_frame)));
	if (nbytes < 0) {
		perror("Write error:");
		return 1;
	}

	if (nbytes < sizeof(frame)) {
		perror("write: incomplete CAN FD frame");
	}
	printf("End of write\n");
}


int main() {
	char* str = "Hello Forever"; /* Max 8 bytes will be sent, unless ISO-TP is implemented */	
	int str_len = sizeof(str);
	//can_write(str, str_len);
	CanSocket socket;
	if (0 != can_connect(&socket)) {
		perror("Socket CAN connect error");
		return 1;
	}
	can_write(&socket, str, str_len);

	CanSocket socket_fd;
	if (0 != can_fd_connect(&socket_fd)) {
		perror("Socket CAN FD connect error");
		return 1;
	}
	can_fd_write(&socket_fd, str, 13);
	return 0;
}
