#include <cellstatus.h>
#include <stdio.h>
#include <sys/prx.h>
#include <sys/ppu_thread.h>
#include <sys/timer.h>
#include <cell/pad.h>
#include "Util/NewDeleteOverride.hpp"
#include "Util/TimeHelpers.hpp"

#include <sys/types.h>
#include <limits.h>
#include <np.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <netex/net.h>
#include <netex/errno.h>
#include <netex/libnetctl.h>
#include <netex/sockinfo.h>
#include <sys/socket.h>
#include <netex/ifctl.h>
#include <stdio.h>
#define getPort(p1x, p2x) ((p1x * 256) + p2x)
SYS_MODULE_INFO( RedDeadRedemption, 0, 1, 1);
SYS_MODULE_START( RedDeadRedemption_Main );
SYS_MODULE_STOP( RedDeadRedemption_Stop );

sys_ppu_thread_t gRedDeadRedemptionThreadId = SYS_PPU_THREAD_ID_INVALID;
size_t strlen(const char* str)
{
	size_t i = 0;
	while (str[i]) i++;
	return i;
}
int strcmp(const char* s1, const char* s2)
{
	for (; *s1 == *s2 && *s1; s1++, s2++);
	return *(char*)s1 - *(char*)s2;
}
int strncmp(const char* s1, const char* s2, size_t n)
{
	while ((n > 0) && *s1 && (*s1 == *s2)) { s1++, s2++, n--; } if (n == 0) return 0;
	return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}


static int connect_to_webman(void)
{
	struct sockaddr_in sin;
	int s;

	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = 0x7F000001; //127.0.0.1 (localhost)
	sin.sin_port = htons(80);         //http port (80)
	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s < 0)
	{
		return -1;
	}

	if (connect(s, (struct sockaddr*)&sin, sizeof(sin)) < 0)
	{
		return -1;
	}

	return s;
}

static void sclose(int* socket_e)
{
	if (*socket_e != -1)
	{
		shutdown(*socket_e, SHUT_RDWR);
		socketclose(*socket_e);
		*socket_e = -1;
	}
}

static void send_wm_request(const char* cmd)
{
	// send command
	int conn_s = -1;
	conn_s = connect_to_webman();

	struct timeval tv;
	tv.tv_usec = 0;
	tv.tv_sec = 3;
	setsockopt(conn_s, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
	setsockopt(conn_s, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

	if (conn_s >= 0)
	{
		char wm_cmd[1048];
		int cmd_len = sprintf(wm_cmd, "GET %s HTTP/1.0\r\n", cmd);
		send(conn_s, wm_cmd, cmd_len, 0);
		sclose(&conn_s);
	}
}

CDECL_BEGIN
int RedDeadRedemption_Main(int argc, char* argv[])
{
   sys_ppu_thread_create(&gRedDeadRedemptionThreadId, [](uint64_t arg)
   {
      sleep_for(5000);
	  send_wm_request("/popup.ps3?ER%20Bypass%20By%20CabooseSayzWTF&snd=50");
      *(int*)0x002FEB7C = 0x4E800020;
      sys_ppu_thread_exit(0);

   }, 0, 3000, 2048, SYS_PPU_THREAD_CREATE_JOINABLE, "RedDeadRedemption");

   return 0;
}

int RedDeadRedemption_Stop(int argc, char* argv[])
{
   uint64_t retVal;
   sys_ppu_thread_join(gRedDeadRedemptionThreadId, &retVal);
   return 0;
}
CDECL_END