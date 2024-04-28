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
		void MainThread() {
			std::cout << "Reader - MainThread - called" << std::endl;
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
		FrameCallback frame_callback_;
		std::thread main_thread_;

	public:
		Reader() :
			frame_callback_{nullptr},
			main_thread_{}
		{
			std::cout << "Reader - Constructor - called" << std::endl;
		};
		~Reader() {
			std::cout << "Reader - Destructor - called" << std::endl;
			if (main_thread_.joinable()) {
				main_thread_.join();
			}
		}
		void RegisterCallback(std::string const & frame_id, FrameCallback callback) {
		}
		void StartLoop() {
			std::cout << "Reader - StartLoop - called" << std::endl;
			main_thread_ = std::thread([this] {this->MainThread();});
			//main_thread_.detach();
		}
		void StopLoop() {

		}
	
};
