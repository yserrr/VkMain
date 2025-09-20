//
// Created by ljh on 25. 9. 18..
//
#include "log_sink.hpp"
#include "formater.hpp"

UILogSink::UILogSink()
{
  set_formatter(nullptr);
}

void UILogSink::log(const spdlog::details::log_msg &msg)
{
  spdlog::memory_buf_t formatted;
  {
    std::lock_guard<std::mutex> lk(mutex_);
    formatter_->format(msg, formatted);
  }
  std::string s(formatted.data(), formatted.size());
  {
    std::lock_guard<std::mutex> lk(mutex_);
    if (buffer_.size() >= max_items_)
    {
      buffer_.pop_front();
    }
    buffer_.push_back(std::move(s));
  }
}

void UILogSink::flush()
{
  // 필요하면 파일 flush 등 처리
}

void UILogSink::set_pattern(const std::string &pattern) {}

void UILogSink::set_formatter(std::unique_ptr<spdlog::formatter> sink_formatter)
{
  formatter_ = std::make_unique<UIFormatter>();
}

std::deque<std::string> UILogSink::snapshot()
{
  std::lock_guard<std::mutex> lk(mutex_);
  return buffer_;
}

void UILogSink::clear()
{
  std::lock_guard<std::mutex> lk(mutex_);
  buffer_.clear();
}

inline void UILogSink::set_max_items(size_t m)
{
  max_items_ = m;
}