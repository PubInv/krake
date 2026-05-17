#ifndef GATEWAY_DASHBOARD_H
#define GATEWAY_DASHBOARD_H

#include <vector>
#include "gateway_core.h"

class DashboardServer {
public:
  DashboardServer(GatewayCore& core);
  void begin(int port = 80);

private:
  GatewayCore& m_core;
  struct mg_connection* m_httpConn;
  std::vector<struct mg_connection*> m_wsClients;

  static void handler(struct mg_connection *c, int ev, void *ev_data);
  void onWsOpen(struct mg_connection *c);
  void onWsMsg(struct mg_connection *c, struct mg_str data);
  void broadcastDeviceUpdate(const String& deviceId, const char* status);
  void sendDeviceList(struct mg_connection *c);
};

#endif