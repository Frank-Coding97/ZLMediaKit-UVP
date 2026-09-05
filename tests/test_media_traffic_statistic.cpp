#include <cassert>

#include "../server/MediaTrafficStatistic.h"

using mediakit::MediaTrafficStatistic;

int main() {
    MediaTrafficStatistic statistic;

    statistic.addOrigin("origin-1", 1024);
    statistic.addOrigin("origin-1", 1024);
    statistic.addOrigin("origin-2", 2048);
    statistic.addPlayer("player-1", 512);
    statistic.addPlayer("player-1", 512);
    statistic.addPlayer("player-2", 256);

    auto snapshot = statistic.snapshot();
    assert(snapshot.upstream_bytes_per_second == 3072);
    assert(snapshot.downstream_bytes_per_second == 768);
    assert(snapshot.origin_socket_count == 2);
    assert(snapshot.player_socket_count == 2);
    return 0;
}
