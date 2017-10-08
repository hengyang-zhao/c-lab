#ifndef HOOK_H
#define HOOK_H

#include <type_traits>


template<class Entity, class Peer=Entity>
class Hook {
public:
    Entity *Left() { return left_; }
    Entity *Right() { return right_; }

    void HookToLeft(Entity *rhs)
    {
        right_ = rhs;
        if (right_) right_->Peer::left_ = static_cast<Entity *>(this);
    };

    void HookToRight(Entity *lhs)
    {
        left_ = lhs;
        if (left_) left_->Peer::right_ = static_cast<Entity *>(this);
    };

    Entity *BreakLeft()
    {
        Entity *ret = left_;
        left_ = nullptr;
        if (ret) ret->Peer::right_ = nullptr;
        return ret;
    }

    Entity *BreakRight()
    {
        Entity *ret = right_;
        right_ = nullptr;
        if (ret) ret->Peer::left_ = nullptr;
        return ret;
    }

    std::pair<Entity *, Entity *> Unhook()
    {
        Entity *ret_l(left_), *ret_r(right_);
        left_ = right_ = nullptr;
        if (ret_l) ret_l.Peer::right_ = ret_r;
        if (ret_r) ret_r.Peer::left_ = ret_l;
        return std::make_pair(ret_l, ret_r);
    }

    virtual ~Hook() = default;

private:
    Entity *left_;
    Entity *right_;
};


#endif // HOOK_H
