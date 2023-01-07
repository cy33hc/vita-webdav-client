#ifndef UTIL_H
#define UTIL_H

#include <vitasdk.h>
#include <string>
#include <vector>
#include <algorithm>
#include "fs.h"

namespace Util
{

    static inline std::string &Ltrim(std::string &str, std::string chars)
    {
        str.erase(0, str.find_first_not_of(chars));
        return str;
    }

    static inline std::string &Rtrim(std::string &str, std::string chars)
    {
        str.erase(str.find_last_not_of(chars) + 1);
        return str;
    }

    // trim from both ends (in place)
    static inline std::string &Trim(std::string &str, std::string chars)
    {
        return Ltrim(Rtrim(str, chars), chars);
    }

    static inline void ReplaceAll(std::string &data, std::string toSearch, std::string replaceStr)
    {
        size_t pos = data.find(toSearch);
        while (pos != std::string::npos)
        {
            data.replace(pos, toSearch.size(), replaceStr);
            pos = data.find(toSearch, pos + replaceStr.size());
        }
    }

    static inline std::string ToLower(std::string s)
    {
        std::transform(s.begin(), s.end(), s.begin(),
                       [](unsigned char c)
                       { return std::tolower(c); });
        return s;
    }

    static inline void convertUtcToLocalTime(const SceDateTime *utc, SceDateTime *local_time)
    {
        SceRtcTick utc_tick;
        SceRtcTick local_tick;
        sceRtcGetTick(utc, &utc_tick);
        sceRtcConvertUtcToLocalTime(&utc_tick, &local_tick);
        sceRtcSetTick(local_time, &local_tick);
    }

    static inline void convertLocalTimeToUtc(const SceDateTime *local_time, SceDateTime *utc)
    {
        SceRtcTick utc_tick;
        SceRtcTick local_tick;
        sceRtcGetTick(local_time, &local_tick);
        sceRtcConvertLocalTimeToUtc(&local_tick, &utc_tick);
        sceRtcSetTick(utc, &utc_tick);
    }

}
#endif
