#include "Cloud\HologramCloud.h"
#include <iostream>

int main(int argc, char* argv[])
{
	std::map<std::string, std::string> credentials;
	credentials.emplace("devicekey", argv[1]); //1 - 404 2 - 201
	HologramCloud cloud = HologramCloud(credentials, false, NetworkType::CELLULAR);
	std::vector<std::wstring> topics;
	topics.push_back(L"WINDOWS");
	topics.push_back(L"CPPSDK");
	cloud.sendMessage(L"Windows Desktop", topics);
	//cloud.sendSMS(L"Just the CPP SDK sending SMSs eith the 404", argv[3]);
	return 0;
}