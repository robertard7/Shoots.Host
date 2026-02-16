#pragma once

namespace httplib {
class Server;
}

namespace shoots::host {

class AppState;

void RegisterRoutes(httplib::Server& server, AppState& app);

} // namespace shoots::host
