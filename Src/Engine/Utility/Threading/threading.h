//
// Created by Kelvin Macartney on 24/03/2020.
//

#ifndef VENUS_THREADING_H
#define VENUS_THREADING_H

#include <thread>
#include <queue>
#include <map>

/** Wrapper for the C++ std::thread::id. */
using ThreadId = std::thread::id;

/** Wrapper for the C++ std::mutex. */
using Mutex = std::mutex;

/** Wrapper for the C++ std::unique_lock<std::mutex>. */
using Lock = std::unique_lock<Mutex>;

/** Wrapper for the C++ std::condition_variable. */
using Signal = std::condition_variable;

/** First-in, first-out data structure. */
template<typename T>
using Queue = std::queue<T, std::deque<T>>;

/** Double ended queue */
template<typename T>
using DeQueue = std::deque<T>;

/** Key value dictionary */
template<typename Key, typename Value>
using Map = std::map<Key, Value>;

/** Returns the ThreadId of the current thread. */
#define THREAD_CURRENT_ID std::this_thread::get_id()

/** The name of the engine's core thread*/
#define CORE_THREAD_NAME "Venus::CoreThread"

/** The engine's application thread name */
#define APPLICATION_THREAD_NAME "Venus::AppThread"

/** The engine's worker thread name prefix */
#define WORKER_THREAD_NAME_PREFIX "Venus::WORKERTHREAD::"

/** Returns the number of logical CPU cores. */
#define THREAD_HARDWARE_CONCURRENCY std::thread::hardware_concurrency()


#if defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__))
#define SUPPORTS_PTHREAD (unix) || __unix__ || __unix
#endif

#endif //VENUS_THREADING_H
