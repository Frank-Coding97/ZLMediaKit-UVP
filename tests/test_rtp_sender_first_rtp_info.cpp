// 二开回归测试：RtpSender 首个 RTP 包采样（getFirstRtpInfo / resetFirstRtpInfo）
//
// 背景：GB28181 的 RTP-Info 响应需要 seq/rtptime。RtpSender 在发送热路径记录
// 启动或显式重置后观测到的第一个 RTP 包，供 WebApi 输出 rtp_info 字段。
//
// 覆盖策略（重要）：首包「记录」动作发生在 RtpSender::sendRtpUdp() 内部的
// send_func lambda 里（取 rtp_list 第0 个包），它依赖已建立的 UDP socket，
// 单元测试无法在不启动真实网络会话的前提下驱动。
// 因此本测试聚焦「采样状态机」本身—— 即 getFirstRtpInfo/resetFirstRtpInfo
// 这对读写接口的语义契约：
//   1. 初始状态没有采样（返回 false，且不污染出参）
//   2. reset 是幂等的：连续多次 reset 仍处于「无采样」
//   3. 重新构造的对象同样是干净的初始态（采样状态是实例级的，不是全局的）
// 「记录首个包后能读回」这一条由集成测试（真实推流）覆盖，不在本文件断言。
//
// 另：采样逻辑整段包在 #if defined(ENABLE_RTPPROXY) 内，
// 未启用 RTPPROXY 时本测试退化为空壳（return 0），不算失败。
#include <cassert>
#include <cstdint>

#if defined(ENABLE_RTPPROXY)

#include "../src/Rtp/RtpSender.h"

using mediakit::RtpSender;

namespace {

// 未采样时，getFirstRtpInfo 不得修改出参——调用方常依赖这点来判断
// "读到的值是不是自己的初值"。
void assert_reports_no_sample(RtpSender &sender) {
    uint16_t seq = 0xFFFF;
    uint32_t stamp = 0xFFFFFFFF;
    assert(!sender.getFirstRtpInfo(seq, stamp));
    assert(seq == 0xFFFF);
    assert(stamp == 0xFFFFFFFF);
}

} // namespace

int main() {
    // 1. 构造后立即查询：无采样
    {
        RtpSender sender;
        assert_reports_no_sample(sender);
    }

    // 2. reset 是幂等的：连续多次 reset 仍报告无采样
    {
        RtpSender sender;
        sender.resetFirstRtpInfo();
        assert_reports_no_sample(sender);
        sender.resetFirstRtpInfo();
        assert_reports_no_sample(sender);
        sender.resetFirstRtpInfo();
        assert_reports_no_sample(sender);
    }

    // 3. 析构再重建（新对象）依然是干净的初始态，
    //    确认采样状态没有落到类级/静态存储里
    {
        RtpSender sender;
        sender.resetFirstRtpInfo();
        assert_reports_no_sample(sender);
    }
    {
        RtpSender sender;
        assert_reports_no_sample(sender);
    }

    return 0;
}

#else // ENABLE_RTPPROXY 未启用

int main() {
    // 首包采样逻辑整段包在 #if defined(ENABLE_RTPPROXY) 内，
    // 未启用 RTPPROXY 时本测试无覆盖，不做断言。
    return 0;
}

#endif // ENABLE_RTPPROXY