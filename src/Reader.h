#include <iostream>
#include <functional>
#include <string>
#include <vector>
#include <thread>
#include <iomanip>

#include "can_read.h"

using FrameCallback = std::function<void(std::vector<char> data)>;

class Reader {
	private:
		//TODO add filter in order to filter on non-FD frames only
		void CanReadThread() {
			std::cout << "Reader - ReadCanThread - called" << std::endl;
			while(true) {
				unsigned char* data;
				int data_length;
				can_read(&data, &data_length);//This function blocks waiting for data
				std::cout << "Data of length: " << data_length <<" from Reader: ";
				for (int i {0}; i < data_length; i++) {
					std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(data[i]) << " ";
				}
				std::cout << std::endl;
			}

		}
		//TODO add filter in order to filter on FD frames only
		void CanFdReadThread() {
			std::cout << "Reader - ReadCanFdThread - called" << std::endl;
			while(true) {
				unsigned char* data;
				int data_length;
				can_fd_read(&data, &data_length);//This function blocks waiting for data
				std::cout << "Data of length: " << data_length <<" from Reader: ";
				for (int i {0}; i < data_length; i++) {
					std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(data[i]) << " ";
				}
				std::cout << std::endl;
			}

		}

		FrameCallback frame_callback_;
		std::thread can_read_thread_;
		std::thread can_fd_read_thread_;

	public:
		Reader() :
			frame_callback_{nullptr},
			can_read_thread_{},
			can_fd_read_thread_{}
		{
			std::cout << "Reader - Constructor - called" << std::endl;
		};
		~Reader() {
			std::cout << "Reader - Destructor - called" << std::endl;
			//TODO make a function to stop all threads.
			if (can_read_thread_.joinable()) {
				can_read_thread_.join();
			}
			if (can_fd_read_thread_.joinable()) {
				can_fd_read_thread_.join();
			}
		}
		void RegisterCallback(std::string const & frame_id, FrameCallback callback) {
		}
		void StartLoop() {
			std::cout << "Reader - StartLoop - called" << std::endl;

			can_fd_read_thread_ = std::thread([this] {this->CanFdReadThread();});
			

			//can_read_thread_ = std::thread([this] {this->CanReadThread();}); //TODO uncomment -> make this thread work at the same time
//main_thread_.detach();
		}
		void StopLoop() {

		}
	
};
