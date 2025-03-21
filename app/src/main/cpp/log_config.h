#ifndef LOG_CONFIG_H_
#define LOG_CONFIG_H_

#include "consts.h"

int get_item_id (int type_id);
int get_equip_id (int type_id);
BinaryBuffer encode_log_config (LogConfigOp op, const std::vector<int>& type_ids);


#endif /* LOG_CONFIG_H_ */
