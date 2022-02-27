#pragma once

#include <chrono>

namespace Ciao
{
    class Timer
    {
    public:
        // 创建一个新的 timer，并且重置
        Timer() { reset(); }

        // 重置 timer 到当前时间
        void reset() { start = std::chrono::system_clock::now(); }

        // 返回到上一次 Reset 之间的毫秒数
        double elapsed() const {
            auto now = std::chrono::system_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
            return (double) duration.count();
        }

        // 返回时间间隔的字符串
        std::string elapsedString(bool precise = false) const {
            double time = elapsed();
            
            if (std::isnan(time) || std::isinf(time))
                return "inf";

            std::string suffix = "ms";
            if (time > 1000) {
                time /= 1000; suffix = "s";
                if (time > 60) {
                    time /= 60; suffix = "m";
                    if (time > 60) {
                        time /= 60; suffix = "h";
                        if (time > 12) {
                            time /= 12; suffix = "d";
                        }
                    }
                }
            }
            std::ostringstream os;
            os << std::setprecision(precise ? 4 : 1)
               << std::fixed << time << suffix;

            return os.str();
        }

    private:
        std::chrono::system_clock::time_point start;
    };
}
