#include "app_state.h"

namespace shoots::host {

AppState::AppState() = default;

const AppConfig& AppState::Config() const {
    return config_;
}

} // namespace shoots::host
