#include <ossia/editor/scenario/scenario.hpp>
#include <ossia/editor/scenario/time_sync.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <ossia/editor/scenario/time_interval.hpp>

#include <iostream>
namespace ossia
{
scenario::sync_status scenario::trigger_sync(
    time_sync& sync, small_event_vec& pending, small_event_vec& maxReachedEv,
    interval_set& started, interval_set& stopped,
    ossia::time_value tick_offset, const ossia::token_request& tk, bool maximalDurationReached)
{
  if (!sync.m_evaluating)
  {
    sync.m_evaluating = true;
    sync.trigger_request = false;
    sync.entered_evaluation.send();
  }

  // update the expression one time
  // then observe and evaluate TimeSync's expression before to trig
  // only if no maximal duration have been reached
  if (*sync.m_expression != expressions::expression_true()
      && !maximalDurationReached)
  {
    if (!sync.has_trigger_date() && !sync.m_is_being_triggered)
    {
      if (!sync.is_observing_expression())
        expressions::update(*sync.m_expression);

      sync.observe_expression(true);

      if (sync.trigger_request)
        sync.trigger_request = false;
      else if (!expressions::evaluate(*sync.m_expression))
        return sync_status::NOT_READY;
    }

    // at this point we can assume we are going to trigger.
    sync.m_is_being_triggered = true;

    if (sync.has_trigger_date())
    {
      bool ok = true;
      for (const auto& ev : sync.get_time_events())
      {
        for (const auto& cst : ev->previous_time_intervals())
        {
          if (cst->get_start_event().get_status()
              == ossia::time_event::status::HAPPENED)
          {
            ok &= (cst->get_date() == m_itv_end_map.find(cst.get())->second);
          }
          if (!ok)
            break;
        }
        if (!ok)
          break;
      }

      if (!ok)
        return sync_status::NOT_READY;
      else
      {
        maximalDurationReached = true;

        for (const auto& ev : sync.get_time_events())
        {
          for (const auto& cst : ev->previous_time_intervals())
          {
            m_itv_end_map.erase(cst.get());
          }
        }
      }
    }
    else if (sync.has_sync_rate())
    {
      const auto rate = sync.get_sync_rate();
      // we are asked to execute, now we must quantize to the next step

      optional<time_value> quantification_date{};
      if(rate <= 1.)
      {
        // Quantize relative to bars
        if(tk.musical_end_last_bar != tk.musical_start_last_bar)
        {
          // There is a bar change in this tick
          double musical_tick_duration = tk.musical_end_position - tk.musical_start_position;
          double musical_bar_start = tk.musical_end_last_bar - tk.musical_start_position;

          double ratio = musical_bar_start / musical_tick_duration;
          time_value dt = tk.date - tk.prev_date; // TODO should be tick_offset

          quantification_date = tk.prev_date + dt * ratio;
        }
      }
      else
      {
        // Quantize relative to quarter divisions
        // note ! if there is a bar change,
        // and no prior quantization date before that, we have to quantize to the bar change

        double start_quarter = (tk.musical_start_position - tk.musical_start_last_bar);
        double end_quarter = (tk.musical_end_position - tk.musical_start_last_bar);

        // rate = 2 -> half
        // rate = 4 -> quarter
        // rate = 8 -> 8th..

        // duration of what we quantify in terms of quarters
        double musical_quant_dur = rate / 4.;
        double start_quant = std::floor(start_quarter * musical_quant_dur);
        double end_quant = std::floor(end_quarter * musical_quant_dur);

        if(start_quant != end_quant)
        {
          // Date to quantify is the next one :
          auto quant_date = tk.musical_start_last_bar + (start_quant + 1) * 4. / rate;
          std::cerr << start_quant << " " << end_quant <<  " => " << quant_date << std::endl;


          double musical_tick_duration = tk.musical_end_position - tk.musical_start_position;
          double ratio = quant_date / musical_tick_duration;
          time_value dt = tk.date - tk.prev_date; // TODO should be tick_offset
          quantification_date = tk.prev_date + dt * ratio;
        }
      }

      if(quantification_date)
      {
        std::cerr << "next bar start: " << *quantification_date << std::endl;
        sync.set_trigger_date(*quantification_date);
        auto diff_date = *quantification_date - tk.prev_date;
        // compute the "fake max" date at which intervals must end for this to
        // work
        for (auto& ev : sync.get_time_events())
        {
          //maxReachedEv.push_back(ev.get());
          for (const auto& cst : ev->previous_time_intervals())
          {
            m_itv_end_map.insert(
                std::make_pair(cst.get(), cst->get_date() + diff_date));
          }
        }

        sync.observe_expression(false);
        return sync_status::RETRY;
      }

      return sync_status::NOT_READY;
    }
  }

  // trigger the time sync
  sync.m_is_being_triggered = false;

  // now TimeEvents will happen or be disposed.
  // the last added events are necessarily the ones of this node.
  for (time_event* ev : pending)
  {
    auto& expr = ev->get_expression();
    // update any Destination value into the expression
    expressions::update(expr);

    if (expressions::evaluate(expr))
    {
      make_happen(*ev, started, stopped, tick_offset, tk);
      if (maximalDurationReached)
        maxReachedEv.push_back(ev);
    }
    else
    {
      make_dispose(*ev, stopped);
    }
  }

  // stop expression observation now the TimeSync has been processed
  sync.observe_expression(false);

  // notify observers
  sync.triggered.send();

  sync.m_evaluating = false;
  sync.finished_evaluation.send(maximalDurationReached);
  if (maximalDurationReached)
    sync.m_status = time_sync::status::DONE_MAX_REACHED;
  else
    sync.m_status = time_sync::status::DONE_TRIGGERED;

  return sync_status::DONE;
}

scenario::sync_status scenario::process_this(
    time_sync& sync, small_event_vec& pendingEvents,
    small_event_vec& maxReachedEvents, interval_set& started,
    interval_set& stopped, ossia::time_value tick_offset,
    const ossia::token_request& req)
{
  // prepare to remember which event changed its status to PENDING
  // because it is needed in time_sync::trigger
  pendingEvents.clear();
  auto activeCount = sync.get_time_events().size();
  std::size_t pendingCount = 0;

  bool maximalDurationReached = false;
  auto on_pending = [&](ossia::time_event* timeEvent) {
    if (!ossia::contains(pendingEvents, timeEvent))
    {
      pendingCount++;
      pendingEvents.push_back(timeEvent);
    }

    for (const std::shared_ptr<ossia::time_interval>& timeInterval :
         timeEvent->previous_time_intervals())
    {
      if (timeInterval->get_date() >= timeInterval->get_max_duration())
      {
        maximalDurationReached = true;
        break;
      }
    }
  };

  for (const std::shared_ptr<time_event>& timeEvent : sync.get_time_events())
  {
    switch (timeEvent->get_status())
    {
      // check if NONE TimeEvent is ready to become PENDING
      case time_event::status::NONE:
      {
        bool minimalDurationReached = true;

        for (const std::shared_ptr<ossia::time_interval>& timeInterval :
             timeEvent->previous_time_intervals())
        {
          const auto& ev = timeInterval->get_start_event();
          // previous TimeIntervals with a DISPOSED start event are ignored
          if (ev.get_status() == time_event::status::DISPOSED)
          {
            continue;
          }

          // previous TimeInterval with a none HAPPENED start event
          // can't have reached its minimal duration
          if (ev.get_status() != time_event::status::HAPPENED)
          {
            minimalDurationReached = false;
            break;
          }

          // previous TimeInterval which doesn't reached its minimal duration
          // force to quit
          if (timeInterval->get_date() < timeInterval->get_min_duration())
          {
            minimalDurationReached = false;
            break;
          }
        }

        // access to PENDING status once all previous TimeIntervals allow it
        if (minimalDurationReached)
        {
          timeEvent->set_status(time_event::status::PENDING);
          on_pending(timeEvent.get());
        }
        break;
      }

      // PENDING TimeEvent is ready for evaluation
      case time_event::status::PENDING:
        on_pending(timeEvent.get());
        break;

      case time_event::status::HAPPENED:
        break;

      case time_event::status::DISPOSED:
        activeCount--;
        break;
    }
  }

  // if all TimeEvents are not PENDING
  if (pendingCount != activeCount)
  {
    if (sync.m_evaluating)
    {
      sync.m_evaluating = false;
      sync.trigger_request = false;
      sync.left_evaluation.send();
    }

    return scenario::sync_status::NOT_READY;
  }

  return trigger_sync(
      sync, pendingEvents, maxReachedEvents, started, stopped, tick_offset, req,
      maximalDurationReached);
}
}
