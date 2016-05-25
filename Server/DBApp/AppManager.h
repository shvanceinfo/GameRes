#ifndef __APP_MANAGER_H__
#define __APP_MANAGER_H__

#include <stdint.h>

class AppManager
{
public:
	bool init();
	void RecivedMessage(uint32_t conn, char *buf, uint32_t len);
private:

};

#endif