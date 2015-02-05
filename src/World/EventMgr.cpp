/*
 * Copyright (C) 2015 Dehravor <dehravor@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "EventMgr.h"
using namespace std::chrono;

Event::Event(EventId id) : id_(id)
{

}

EventId Event::GetId()
{
    return id_;
}

void Event::SetPeriod(uint32 period)
{
    period_ = period;
    remaining_ = period;
}

void Event::SetCallback(EventCallback callback)
{
    callback_ = callback;
}

void Event::Update(uint32 diff)
{
    if (remaining_ <= 0)
    {
        callback_();
        remaining_ = period_;
    }
    else
        remaining_ -= diff;
}

EventMgr::EventMgr() : isRunning_(false)
{

}

EventMgr::~EventMgr()
{
    isRunning_ = false;

    if (thread_.joinable())
        thread_.join();
}

void EventMgr::AddEvent(std::shared_ptr<Event> event)
{
    events_.push_back(event);
}

void EventMgr::RemoveEvent(EventId id)
{
    events_.remove_if([id](std::shared_ptr<Event> const& event) {
        return event->GetId() == id;
    });
}

void EventMgr::Start()
{
    thread_ = std::thread(&EventMgr::ProcessEvents, this);
}

void EventMgr::ProcessEvents()
{
    isRunning_ = true;
    uint32 diff = 0;

    while (isRunning_)
    {
        system_clock::time_point start = high_resolution_clock::now();

        for (auto event : events_)
            event->Update(diff);

        std::this_thread::sleep_for(milliseconds(5));
        system_clock::time_point end = high_resolution_clock::now();
        diff = uint32(duration_cast<milliseconds>(end - start).count());
    }
}