#ifndef __OPERATION_HANDLER_T_HPP__
#define __OPERATION_HANDLER_T_HPP__

//
// operation_id.hpp
//
// Copyright (c) 2013 Orlin Hristov (orlin dot hristov at gmail dot com)
//
// Distributed under the Apache License, Version 2.0
// visit http://www.apache.org/licenses/ for more information.
//

#include <cstddef>
#ifdef CPORT_ENABLE_TASK_STATUS
#include <completion_status.hpp>
#include <detail/event.hpp>
#include <atomic>
#include <memory>
#include <utility>
#endif

namespace mt {

namespace detail {

class operation_id {
public:
    typedef std::size_t value_type;

    operation_id(value_type vt = value_type())
        : value_(vt)
#ifdef CPORT_ENABLE_TASK_STATUS
        , status_(new atomic_status(completion_status::none))
        , completion_event_(new event())
#endif
    {
    }

    operation_id(const operation_id & op) = default;

    operation_id(operation_id&& op)
        : value_(op.value_)
#ifdef CPORT_ENABLE_TASK_STATUS
        , status_(std::move(op.status_))
        , completion_event_(std::move(op.completion_event_))
#endif
    {
    }

    operation_id& operator=(const operation_id & op) = default;

    operation_id& operator=(operation_id&& op)
    {
        if (this != &op)
        {
            value_ = op.value_;
#ifdef CPORT_ENABLE_TASK_STATUS
            status_ = std::move(op.status_);
            completion_event_ = std::move(op.completion_event_);
#endif
        }
        return *this;
    }

    bool operator==(const operation_id &op) const
    {
        return value_ == op.value_;
    }

    bool operator!=(const operation_id &op) const
    {
        return !(*this == op);
    }

    bool operator<(const operation_id &op) const
    {
        return value_ < op.value_;
    }

    bool valid() const
    {
        return value_ != value_type();
    }

    operator value_type() const
    {
        return value_;
    }
#ifdef CPORT_ENABLE_TASK_STATUS
    completion_status get_status() const
    {
        return status_->load();
    }

    void set_status(completion_status s)
    {
        status_->store(s);
        if (completion_status::canceled == s
            || completion_status::complete == s)
        {
            completion_event_->notify_all();
        }
    }

    void wait()
    {
        completion_event_->wait();
    }
#endif

private:
    value_type value_;

#ifdef CPORT_ENABLE_TASK_STATUS
    typedef std::atomic<completion_status> atomic_status;
    typedef std::shared_ptr<atomic_status> shared_atomic_status;
    shared_atomic_status status_;

    typedef std::shared_ptr<event> shared_event;
    shared_event completion_event_;
#endif
};

} // namespace detail

} // namespace mt

#endif // __OPERATION_HANDLER_T_HPP__