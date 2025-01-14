/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-11-06 15:33:36
 * @Description:
 */
#ifndef __AVZ_CONNECTOR_H__
#define __AVZ_CONNECTOR_H__

#include "avz_coroutine.h"
#include "avz_tick_runner.h"

class __AConnectVec : public AOrderedStateHook<-1> {
public:
    std::vector<ATickRunnerWithNoStart*> tickVec;

protected:
    virtual void _ExitFight() override;
};

class ATimeConnectHandle {
public:
    using TimeIter = std::optional<__ATimeIter>;
    ATimeConnectHandle(TimeIter iter, const ATime& time)
        : _iter(iter)
        , _time(time)
    {
    }
    ATimeConnectHandle() = default;
    ATimeConnectHandle(const ATimeConnectHandle& rhs)
        : _iter(rhs._iter)
        , _time(rhs._time)
    {
    }

    ATimeConnectHandle& operator=(const ATimeConnectHandle& rhs)
    {
        _iter = rhs._iter;
        _time = rhs._time;
        return *this;
    }

    void Stop();

protected:
    TimeIter _iter;
    ATime _time;
};

class AConnectHandle {
public:
    AConnectHandle() = default;
    AConnectHandle(const AConnectHandle& rhs)
        : _connectPtr(rhs._connectPtr)
    {
    }

    AConnectHandle& operator=(const AConnectHandle& rhs)
    {
        _connectPtr = rhs._connectPtr;
        return *this;
    }
    AConnectHandle(ATickRunnerWithNoStart* connectPtr)
        : _connectPtr(connectPtr)
    {
    }
    void Pause()
    {
        _connectPtr->Pause();
    }
    void GoOn()
    {
        _connectPtr->GoOn();
    }
    void Stop()
    {
        _connectPtr->Stop();
    }
    bool IsPaused()
    {
        return _connectPtr->IsPaused();
    }
    bool IsStopped()
    {
        return _connectPtr->IsStopped();
    }
    __ADeprecated bool isStopped()
    {
        return IsStopped();
    }
    __ADeprecated bool isPaused()
    {
        return IsPaused();
    }

protected:
    ATickRunnerWithNoStart* _connectPtr;
};

#define __ANodiscardRelOp [[nodiscard("ARelOp 需要配合 AConnect 使用")]]

struct ARelOp {
    struct ReOp {
        int relativeTime;
        AOperation operation;

        template <typename Op>
            requires __AIsOperation<Op>
        ReOp(int relativeTime, Op&& op)
            : relativeTime(relativeTime)
            , operation(std::forward<Op>(op))
        {
        }

        template <typename Op>
            requires __AIsCoroutineOp<Op>
        ReOp(int relativeTime, Op&& op)
            : relativeTime(relativeTime)
            , operation(ACoFunctor(std::forward<Op>(op)))
        {
        }
    };

    std::vector<ReOp> OpVec;

    __ANodiscardRelOp ARelOp(ARelOp&& rhs)
        : OpVec(std::move(rhs.OpVec))
    {
    }

    __ANodiscardRelOp ARelOp(const ARelOp& rhs)
        : OpVec(rhs.OpVec)
    {
    }

    template <typename Op>
        requires __AIsCoOpOrOp<Op>
    __ANodiscardRelOp explicit ARelOp(int relativeTime, Op&& op)
    {
        OpVec.emplace_back(relativeTime, std::forward<Op>(op));
    }

    template <typename Op>
        requires __AIsCoOpOrOp<Op>
    __ANodiscardRelOp explicit ARelOp(Op&& op)
        : ARelOp(0, std::forward<Op>(op))
    {
    }

    __ANodiscardRelOp explicit ARelOp(int relativeTime, ARelOp&& reOp)
    {
        for (auto&& op : reOp.OpVec) {
            OpVec.emplace_back(relativeTime + op.relativeTime, std::move(op.operation));
        }
    }
    __ANodiscardRelOp explicit ARelOp(int relativeTime, const ARelOp& reOp)
    {
        for (auto&& op : reOp.OpVec) {
            OpVec.emplace_back(relativeTime + op.relativeTime, op.operation);
        }
    }

    // ARelativeOp + ARelativeOp
    template <typename Lhs, typename Rhs>
        requires std::is_convertible_v<Lhs, ARelOp> && std::is_convertible_v<Rhs, ARelOp>
    __ANodiscard friend ARelOp operator+(Lhs&& lhs, Rhs&& rhs)
    {
        ARelOp tmpReOp(std::forward<Lhs>(lhs));
        tmpReOp._Add(std::forward<Rhs>(rhs));
        return tmpReOp;
    }

    // ARelativeOp + AOperation
    template <typename Lhs, typename Rhs>
        requires std::is_convertible_v<Lhs, ARelOp> && __AIsCoOpOrOp<Rhs>
    __ANodiscard friend ARelOp operator+(Lhs&& lhs, Rhs&& rhs)
    {
        ARelOp tmpReOp(std::forward<Lhs>(lhs));
        tmpReOp.OpVec.emplace_back(0, std::forward<Rhs>(rhs));
        return tmpReOp;
    }

    // AOperation + ARelativeOp
    template <typename Lhs, typename Rhs>
        requires __AIsCoOpOrOp<Lhs> && std::is_convertible_v<Rhs, ARelOp>
    __ANodiscard friend ARelOp operator+(Lhs&& lhs, Rhs&& rhs)
    {
        return std::forward<Rhs>(rhs) + std::forward<Lhs>(lhs);
    }

    template <typename Rhs>
        requires std::is_convertible_v<Rhs, ARelOp>
    friend ARelOp& operator+=(ARelOp& lhs, Rhs&& rhs)
    {
        lhs._Add(std::forward<Rhs>(rhs));
        return lhs;
    }

    template <typename Rhs>
        requires __AIsCoOpOrOp<Rhs>
    friend ARelOp& operator+=(ARelOp& lhs, Rhs&& rhs)
    {
        lhs.OpVec.emplace_back(0, std::forward<Rhs>(rhs));
        return lhs;
    }

protected:
    void _Add(ARelOp&& rhs)
    {
        for (auto&& op : rhs.OpVec) {
            OpVec.emplace_back(op.relativeTime, std::move(op.operation));
        }
    }

    void _Add(const ARelOp& rhs)
    {
        for (auto&& op : rhs.OpVec) {
            OpVec.emplace_back(op.relativeTime, op.operation);
        }
    }
};

#undef __ANodiscardRelOp

// 创建一条连接, 连接创建成功之后会返回一个类型为 AConnectHandle 的对象作为连接控制器
// *** 特别注意：如果连接创建失败, 连接控制器将被赋值为 nullptr. 并且时间连接没有返回值
// AConnect 最后一个参数为运行方式
// 运行方式为 true 时, AConnect 创建的连接选卡界面和高级暂停时都生效, 反之不生效
// *** 使用示例:
// ALogger<AMsgBox> logger;
// AConnectHandle keyHandle;
//
// void AScript()
// {
//     logger.SetLevel({ALogLevel::INFO});
//
//     // 在时间点 (1, -95) 发两门炮
//     auto timeHandle = AConnect(ATime(1, -95), [=] {
//         aCobManager.Fire({{2, 9}, {5, 9}});
//     });
//     timeHandle.Stop(); // 让上面这个连接失效
//
//
//     // 在当前时间点 100cs 之后开始不断尝试种植小喷菇, 直到小喷菇种植成功
//     // 这里的 ANowDelayTime 会返回一个 ATime 对象
//     AConnect(ANowDelayTime(100), [] {
//         if (AIsSeedUsable(AXPG_8)) {
//             ACard(AXPG_8, 1, 1);
//             return false;
//         }
//         return true;
//     });
//
//
//     // 按下 0 键弹出一个窗口, 显示 hello, 注意 0 是单引号
//     keyHandle = AConnect('0', [] { logger.Info("hello"); });
//
//     // 按下 E 键控制 0 键的是否暂停
//     // 如果 0 键此时暂停生效, 按下 E 键便会继续生效, 反之相反
//     AConnect('E', [] {
//         if (keyHandle.isPaused()) {
//             keyHandle.GoOn();
//         } else {
//             keyHandle.Pause();
//         }
//     });
//
//     // 按下 Q 键控制 0 键行为, 即将 0 键的显示变为 world
//     AConnect('Q', [] {
//         keyHandle.Stop(); // 注意此时 keyHandle 已失效
//         keyHandle = AConnect('0', [] { logger.Info("world"); }); // 此时 keyHandle 重新有效
//     });
//
//     // AConnect 第一个参数还可传入一个 bool Func(), 如果此函数返回 true, 则会执行后面的操作
//     // 这个示例就是游戏每 10 秒钟显示一个 world 的窗口
//     AConnect([] { return AGetMainObject()->GameClock() % 1000 == 0; }, [] { logger.Info("world"); });
//
// }
template <typename Op>
    requires __AIsOperation<Op>
ATimeConnectHandle AConnect(const ATime& time, Op&& op)
{
    auto timeIter = __AOperationQueueManager::Push(time, __ABoolOperation(std::forward<Op>(op)));
    return ATimeConnectHandle(timeIter, time);
}

template <typename Op>
    requires __AIsCoroutineOp<Op>
ATimeConnectHandle AConnect(const ATime& time, Op&& op)
{
    return AConnect(time, ACoFunctor(std::forward<Op>(op)));
}

template <typename Sess>
    requires __AIsPredication<Sess>
ATimeConnectHandle AConnect(const ATime& time, Sess&& func)
{
    return AConnect(time, [func = std::forward<Sess>(func)]() mutable {
        auto tickRunner = std::make_shared<ATickRunner>();
        tickRunner->Start([func = std::move(func), tickRunner] {
            if(!func()) {
                tickRunner->Stop();
            } }, false);
    });
}

std::vector<ATimeConnectHandle> AConnect(const ATime& time, ARelOp&& reOp);
std::vector<ATimeConnectHandle> AConnect(const ATime& time, const ARelOp& reOp);

template <typename Pre, typename Op>
    requires __AIsPredication<Pre> && __AIsOperation<Op>
AConnectHandle AConnect(Pre&& pre, Op&& op, bool isInGlobal = false)
{
    extern __AConnectVec __aConnectVec;
    auto tick = new ATickRunnerWithNoStart([pre = std::forward<Pre>(pre), op = std::forward<Op>(op)]() mutable {
        if (pre()) {
            op();
        }
    },
        isInGlobal);
    __aConnectVec.tickVec.push_back(tick);
    return tick;
}

template <typename Pre, typename Op>
    requires __AIsPredication<Pre> && __AIsCoroutineOp<Op>
AConnectHandle AConnect(Pre&& pre, Op&& op, bool isInGlobal = false)
{
    return AConnect(std::forward<Pre>(pre), ACoFunctor(std::forward<Op>(op)), isInGlobal);
}

class __AKeyManager : public AOrderedStateHook<-1> {
public:
    enum KeyState {
        VALID,
        UNKNOWN,
        REPEAT,
    };
    __AKeyManager();
    static KeyState ToVaildKey(AKey& key);
    static void AddKey(AKey key, AConnectHandle connectHandle)
    {
        _keyMap.emplace(key, connectHandle);
    }
    static const std::string& ToName(AKey key)
    {
        return _keyVec[key];
    }

protected:
    static std::vector<std::string> _keyVec;
    static std::unordered_map<AKey, AConnectHandle> _keyMap;
    virtual void _ExitFight() override
    {
        _keyMap.clear();
    }
};

inline __AKeyManager __akm; // AStateHook

template <typename Op>
    requires __AIsCoOpOrOp<Op>
AConnectHandle AConnect(AKey key, Op&& op)
{
    if (__AKeyManager::ToVaildKey(key) != __AKeyManager::VALID) {
        return nullptr;
    }
    auto keyFunc = [key]() -> bool {
        auto pvzHandle = AGetPvzBase()->MRef<HWND>(0x350);
        return ((GetAsyncKeyState(key) & 0x8001) == 0x8001 && //
            GetForegroundWindow() == pvzHandle);              // 检测 pvz 是否为顶层窗口
    };
    auto connectPtr = AConnect(std::move(keyFunc), std::forward<Op>(op), true);
    __AKeyManager::AddKey(key, connectPtr);
    return connectPtr;
}

#endif