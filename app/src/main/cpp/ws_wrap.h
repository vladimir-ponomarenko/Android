#pragma once

void initialize_epan(void);
std::string process_message( unsigned char* indata, unsigned int datasize);
void stop_epan(void);

