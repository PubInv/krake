#include "gateway.h"
#include "gateway_core.h"
#include "gateway_dashboard.h"

static GatewayCore s_core;
static DashboardServer s_dashboard(s_core);

void gateway_init() {
  s_core.begin();
  s_dashboard.begin(80);
}

void gateway_poll() {
  s_core.poll();
}