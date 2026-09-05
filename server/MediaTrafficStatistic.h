#ifndef ZLMEDIAKIT_MEDIA_TRAFFIC_STATISTIC_H
#define ZLMEDIAKIT_MEDIA_TRAFFIC_STATISTIC_H

#include <cstdint>
#include <mutex>
#include <string>
#include <unordered_set>

namespace mediakit {

struct MediaTrafficSnapshot {
    uint64_t upstream_bytes_per_second = 0;
    uint64_t downstream_bytes_per_second = 0;
    size_t origin_socket_count = 0;
    size_t player_socket_count = 0;
};

class MediaTrafficStatistic {
public:
    void addOrigin(const std::string &identifier, uint64_t bytes_per_second) {
        std::lock_guard<std::mutex> lock(_mutex);
        if (!_origins.emplace(identifier).second) {
            return;
        }
        _snapshot.upstream_bytes_per_second += bytes_per_second;
        _snapshot.origin_socket_count = _origins.size();
    }

    void addPlayer(const std::string &identifier, uint64_t bytes_per_second) {
        std::lock_guard<std::mutex> lock(_mutex);
        if (!_players.emplace(identifier).second) {
            return;
        }
        _snapshot.downstream_bytes_per_second += bytes_per_second;
        _snapshot.player_socket_count = _players.size();
    }

    MediaTrafficSnapshot snapshot() const {
        std::lock_guard<std::mutex> lock(_mutex);
        return _snapshot;
    }

private:
    mutable std::mutex _mutex;
    std::unordered_set<std::string> _origins;
    std::unordered_set<std::string> _players;
    MediaTrafficSnapshot _snapshot;
};

} // namespace mediakit

#endif // ZLMEDIAKIT_MEDIA_TRAFFIC_STATISTIC_H
