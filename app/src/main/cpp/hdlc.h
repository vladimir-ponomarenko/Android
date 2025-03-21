#ifndef SIGNAL_HDLC_H
#define SIGNAL_HDLC_H

#include <string>

std::string encode_hdlc_frame (const char *payld, int length);
void  feed_binary (const char *b, int length);
void reset_binary();
bool get_next_frame (std::string& output_frame, bool& crc_correct);

#endif //SIGNAL_HDLC_H
