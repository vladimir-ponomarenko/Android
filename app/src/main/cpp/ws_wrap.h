#pragma once

void initialize_epan(void);
std::string process_message( unsigned char* indata, unsigned int datasize);
std::string decode_msg(std::string msgType, std::string msgData);
void stop_epan(void);

