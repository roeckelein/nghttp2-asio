/// Modified for compatibility with Boost 1.87+
///
/// Copyright (c) 2025 Ashley Roeckelein
/// (same license as Tatsuhiro Tsujikawa and Christopher M. Kohlhoff)
/*
 * nghttp2 - HTTP/2 C Library
 *
 * Copyright (c) 2014 Tatsuhiro Tsujikawa
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
// We wrote this code based on the original code which has the
// following license:
//
// io_service_pool.hpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ASIO_IO_CONTEXT_POOL_H
#define ASIO_IO_CONTEXT_POOL_H

#include "nghttp2_config.h"

#include <vector>
#include <memory>
#include <future>

#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>

#include <nghttp2/asio_http2.h>

namespace nghttp2 {

namespace asio_http2 {

/// A pool of io_context objects.
class io_context_pool : private boost::noncopyable {
public:
  using io_context = boost::asio::io_context;
  using io_context_shptr = std::shared_ptr<io_context>;
  using io_context_shptr_vector = std::vector<io_context_shptr>;

  /// Construct the io_context pool.
  explicit io_context_pool(std::size_t pool_size);

  /// Run all io_context objects in the pool.
  void run(bool asynchronous = false);

  /// Stop all io_context objects in the pool.
  void force_stop();

  /// Destroy all work objects to signals end of work
  void stop();

  /// Join on all io_context objects in the pool.
  void join();

  /// Get an io_context to use.
  io_context &get_io_context();

  /// Get access to all io_context objects.
  const io_context_shptr_vector& io_contexts() const;

private:
  using work_guard = boost::asio::executor_work_guard<io_context::executor_type>;
  using work_guard_shptr = std::shared_ptr<work_guard>;
  using work_guard_shptr_vector = std::vector<work_guard_shptr>;

  /// The pool of io_contexts.
  io_context_shptr_vector io_contexts_;

  /// The work that keeps the io_contexts running.
  work_guard_shptr_vector work_;

  /// The next io_context to use for a connection.
  std::size_t next_io_context_;

  /// Futures to all the io_context objects
  std::vector<std::future<std::size_t>> futures_;
};

} // namespace asio_http2

} // namespace nghttp2

#endif // ASIO_IO_CONTEXT_POOL_H
