#include <stdio.h>
#include <stdlib.h> /* free() */
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

int can_read(unsigned char** data_array, int* data_array_length) {

	/* Create socket */
	int s;
	if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		perror("Socker");
		return 1;
	}

	/* Retrieve the CAN interface index  */
	struct ifreq ifr;
	
	strcpy(ifr.ifr_name, "vcan0"); //TODO use variable for interface name: to allow logging and flexiblity
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

	*data_array = (unsigned char*)malloc(frame.can_dlc * sizeof(unsigned char));
    if (*data_array == NULL) {
        // Handle memory allocation failure
        *data_array_length = 0;
        return 1;
    }

	for (int i = 0; i < frame.can_dlc; i++) {
		(*data_array)[i] = frame.data[i];
	}
	//*data_array = frame.data;
	*data_array_length = frame.can_dlc;

	printf("\r\n");
}

int can_fd_read(unsigned char** data_array, int* data_array_length) {

	/* Create socket */
	int s;
	if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		perror("Socker");
		return 1;
	}
	int enable_can_fd = 1;
    if (setsockopt(s, SOL_CAN_RAW, CAN_RAW_FD_FRAMES, &enable_can_fd, sizeof(enable_can_fd)) < 0) {
        perror("Error setting CAN_RAW_FD_FRAMES option");
        close(s);
        return 1;
    }

	/* Retrieve the CAN interface index  */
	struct ifreq ifr;
	
	strcpy(ifr.ifr_name, "vcan1");
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
	int nbytes;
	struct canfd_frame frame;

	nbytes = read(s, &frame, CANFD_MTU);

	if (nbytes < 0) {
		perror("Read");
		return 1;
	}

	printf("0x%03X [%d] ", frame.can_id, frame.len);

	for (int i = 0; i < frame.len; i++) {
		printf("%02X ", frame.data[i]);
	}

	*data_array = (unsigned char*)malloc(frame.len * sizeof(unsigned char));
    if (*data_array == NULL) {
        // Handle memory allocation failure
        *data_array_length = 0;
        return 1;
    }

	for (int i = 0; i < frame.len; i++) {
		(*data_array)[i] = frame.data[i];
	}
	//*data_array = frame.data;
	*data_array_length = frame.len;

	printf("\r\n");
}

/*
int main() {
	CallbackHandler handler_;
	RegisterCallback(&handler_, AsciiCallback);
	unsigned char* data;
	int data_length;
	can_read(&data, &data_length);
	printf("Data from main: ");
	for (int i = 0; i < data_length; i++) {
		printf("%02X ", data[i]);
	}
	return 0;
}
*/
