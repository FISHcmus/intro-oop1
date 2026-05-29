#include "ServerConfig.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cstdio>

#ifndef CARO_ENABLE_ENDPOINT_OVERRIDE
#define CARO_ENABLE_ENDPOINT_OVERRIDE 0
#endif
#ifndef CARO_DEFAULT_PRODUCTION_HOST
#define CARO_DEFAULT_PRODUCTION_HOST ""
#endif
#ifndef CARO_DEFAULT_PRODUCTION_PORT
#define CARO_DEFAULT_PRODUCTION_PORT 34567
#endif

namespace {

constexpr const char* kProductionHost = CARO_DEFAULT_PRODUCTION_HOST;
constexpr int kProductionPort = CARO_DEFAULT_PRODUCTION_PORT;
constexpr const char* kLocalHost = "127.0.0.1";
constexpr int kLocalPort = 34567;

bool envTruthy(const char* name) {
    const char* value = std::getenv(name);
    if (!value || !*value) return false;
    return value[0] == '1' || value[0] == 'y' || value[0] == 'Y' ||
           value[0] == 't' || value[0] == 'T';
}

std::string lowerCopy(const char* value) {
    std::string out = value ? value : "";
    for (char& ch : out) {
        ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    }
    return out;
}

bool parsePort(const char* value, int& out) {
    if (!value || !*value) return false;
    char* end = nullptr;
    long parsed = std::strtol(value, &end, 10);
    if (!end || *end != '\0') return false;
    if (parsed < 1 || parsed > 65535) return false;
    out = static_cast<int>(parsed);
    return true;
}

}  // namespace

namespace ServerConfig {

bool advancedSelectorEnabled() {
#if CARO_ENABLE_ENDPOINT_OVERRIDE
    return true;
#else
    return envTruthy("CARO_ENABLE_ENDPOINT_OVERRIDE");
#endif
}

OnlineEndpointPreset defaultPreset() {
    std::string env = lowerCopy(std::getenv("CARO_SERVER_ENV"));
    if (env == "local" || env == "localhost" || env == "dev") {
        return OnlineEndpointPreset::Local;
    }
    return OnlineEndpointPreset::Production;
}

Endpoint endpointForPreset(OnlineEndpointPreset preset) {
    switch (preset) {
        case OnlineEndpointPreset::Local:
            return {kLocalHost, kLocalPort};
        case OnlineEndpointPreset::Production:
        default:
            return {kProductionHost, kProductionPort};
    }
}

Endpoint resolveOnlineEndpoint(OnlineEndpointPreset preset) {
    Endpoint endpoint = endpointForPreset(preset);

    const char* host = std::getenv("CARO_SERVER_HOST");
    if (host && *host) endpoint.host = host;

    int port = endpoint.port;
    if (parsePort(std::getenv("CARO_SERVER_PORT"), port)) {
        endpoint.port = port;
    }
    return endpoint;
}

bool endpointConfigured(const Endpoint& endpoint) {
    return !endpoint.host.empty() && endpoint.port > 0;
}

const char* presetLabel(OnlineEndpointPreset preset) {
    switch (preset) {
        case OnlineEndpointPreset::Local: return "Localhost";
        case OnlineEndpointPreset::Production:
        default: return "Production";
    }
}

std::string presetSummary(OnlineEndpointPreset preset) {
    Endpoint endpoint = resolveOnlineEndpoint(preset);
    if (!endpointConfigured(endpoint)) {
        return std::string(presetLabel(preset)) +
               " · set CARO_SERVER_HOST or build with CARO_PRODUCTION_SERVER_HOST";
    }
    char buf[192];
    std::snprintf(buf, sizeof(buf), "%s · %s:%d",
                  presetLabel(preset), endpoint.host.c_str(), endpoint.port);
    return buf;
}

int defaultLanPort() {
    return kLocalPort;
}

}  // namespace ServerConfig
