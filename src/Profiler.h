#include "../ThirdParty/remotery/Remotery.h"

#define PROFILE_SCOPED(name) rmt_ScopedCPUSample(name, 0)

#define PROFILE_BEGIN(name) rmt_BeginCPUSample(name, 0)
#define PROFILE_END() rmt_EndCPUSample()


#define rmt_BeginCPUSampleAutoName() \
    RMT_OPTIONAL(RMT_ENABLED, { \
        static rmtU32 rmt_sample_hash_##__LINE__ = 0;   \
        _rmt_BeginCPUSample(__FUNCTION__, 0, &rmt_sample_hash_##__LINE__); \
    })

#define PROFILE_FUNCTION() \
    RMT_OPTIONAL(RMT_ENABLED, rmt_BeginCPUSampleAutoName( )); \
    RMT_OPTIONAL(RMT_ENABLED, rmt_EndCPUSampleOnScopeExit rmt_ScopedCPUSample##__LINE__);