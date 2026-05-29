#pragma once

#include <string>

namespace ServerConfig {

enum class OnlineEndpointPreset {
    Production,
    Local,
};

struct Endpoint {
    std::string host;
    int port;
};

bool advancedSelectorEnabled();
OnlineEndpointPreset defaultPreset();
Endpoint endpointForPreset(OnlineEndpointPreset preset);
Endpoint resolveOnlineEndpoint(OnlineEndpointPreset preset);
bool endpointConfigured(const Endpoint& endpoint);
const char* presetLabel(OnlineEndpointPreset preset);
std::string presetSummary(OnlineEndpointPreset preset);
int defaultLanPort();

}  // namespace ServerConfig
