#ifndef ZHKEYESIMSERVER_MODEL_USER_H_
#define ZHKEYESIMSERVER_MODEL_USER_H_

#include <cstdint>
#include <string>

struct UserInfo {
	UserInfo() :
		name(""), pwd(""), uid(0), 
		email(""), nick(""), desc(""), 
		sex(0), icon(""), back("") 
	{}


	std::string name;
	std::string pwd;
	uint32_t uid;
	std::string email;
	std::string nick;
	std::string desc;
	uint32_t sex;
	std::string icon;
	std::string back;
};


#endif