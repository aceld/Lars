#include "host_info.h"
#include "main_server.h"

void host_info::set_idle()
{
    vsucc = lb_config.init_succ_cnt;
    verr = 0;
    rsucc = 0;
    rerr = 0;
    contin_succ = 0;
    contin_err = 0;
    overload = false;
    idle_ts = time(NULL);//设置判定为idle状态的时刻,也是重置窗口时间
}

void host_info::set_overload()
{
    vsucc = 0;
    verr = lb_config.init_err_cnt;//overload的初试虚拟err错误次数
    rsucc = 0;
    rerr = 0;
    contin_err = 0;
    contin_succ = 0;
    overload = true;
    overload_ts = time(NULL);//设置判定为overload的时刻
}

//计算整个窗口内的真实失败率，如果达到连续失败窗口值则返回true，代表需要调整状态
bool host_info::check_window() {
    if (rsucc + rerr == 0) {
        return false; //分母不能为0
    }

    if (rerr * 1.0 / (rsucc + rerr) >= lb_config.window_err_rate) {
        return true;
    }

    return false;
}
