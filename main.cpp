/**
 * Assignment: synchronization
 * Operating Systems
 */

#include <algorithm>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

using namespace std;

template <typename T> class Buffer {
public:
  Buffer() : bounded(false) {}

  Buffer(size_t bound) : bounded(true), max_size(bound) {
    if (bound == 0)
      throw invalid_argument("Bound should be greater than zero!");
    buffer.reserve(bound);
  }

  /**
   * @brief Pushes an element to the front of the buffer
   *
   * @param element - The element fo be pushed to the front of the buffer
   *
   * @return size_t - The new size of the buffer
   */
  size_t push_front(T element) {
    lock_guard<mutex> guard(buffer_lock);
    if (bounded && buffer.size() == max_size) {
      throw runtime_error("Buffer is full!");
    }
    buffer.insert(buffer.begin(), element);
    return buffer.size();
  }

  /**
   * @brief Pops an element from the end of the buffer
   *
   * @return T - The element popped from the end of the buffer
   */
  T pop_back() {
    lock_guard<mutex> guard(buffer_lock);
    if (buffer.size() == 0) {
      throw runtime_error("Buffer is empty!");
    }
    T element = buffer.back();
    buffer.pop_back();
    return element;
  }

  /**
   * @brief Read n elements from the buffer (without removing them from the
   * buffer)
   *
   * @return vector<T> - A vector containing the read elements
   */
  vector<T> read_n(size_t n) {
    lock_guard<mutex> guard(buffer_lock);
    if (n > buffer.size()) {
      throw runtime_error("Not enough elements to read!");
    }

    vector<T> read_buffer;
    read_buffer.reserve(n);

    for (int i = buffer.size() - n; i < buffer.size(); ++i) {
      read_buffer.push_back(buffer[i]);
    }

    return read_buffer;
  }

  /**
   * @brief Read all the elements from the buffer (without removing them from
   * the buffer)
   *
   * @return vector<T> - A vector containing the read elements
   */
  vector<T> read_all() {
    lock_guard<mutex> guard(buffer_lock);
    if (buffer.empty()) {
      throw runtime_error("Buffer is empty!");
    }

    vector<T> read_buffer;
    read_buffer.reserve(buffer.size());

    for (int i = 0; i < buffer.size(); ++i) {
      read_buffer.push_back(buffer[i]);
    }

    return read_buffer;
  }

  /**
   * @brief Get the upper bound of the buffer
   *
   * @return size_t - Current upper bound of the buffer
   */
  size_t get_bound() {
    lock_guard<mutex> guard(buffer_lock);
    return max_size;
  }

  /**
   * @brief Check if the buffer is set to be bounded
   *
   * @return bool - true if the buffer is bounded and false otherwise
   */
  bool is_bounded() {
    lock_guard<mutex> guard(buffer_lock);
    return bounded;
  }

  /**
   * @brief Set the upper bound of the buffer. In case the new upper bound is
   * smaller than the current size of the buffer, the excess elements are
   * discarded.
   *
   * @param bound The new upper bound for the buffer
   *
   */
  void set_bound(size_t bound) {
    lock_guard<mutex> guard(buffer_lock);
    if (!bounded) {
      bounded = true;
    }
    if (bound >= buffer.capacity()) {
      buffer.reserve(buffer.capacity() - bound);
    } else {
      buffer.resize(bound);
    }
    max_size = bound;
  }

  /**
   * @brief Remove the upper bound of the buffer
   *
   */
  void remove_bound() {
    lock_guard<mutex> guard(buffer_lock);
    if (bounded) {
      bounded = false;
    }
  }

private:
  mutex buffer_lock;
  vector<T> buffer;
  bool bounded;
  size_t max_size;
};

class Logger {
public:
  enum LogLevel {
    INFO,
    ERROR,
  };

  struct LogEntry {
    string message;
    chrono::time_point<chrono::system_clock> timestamp;

    bool operator<(const LogEntry &other) const {
      return timestamp > other.timestamp;
    }
  };

  void write(string message, LogLevel level) {
    auto precise_now = chrono::system_clock::now();

    time_t now = time(0);
    tm *t = localtime(&now);

    char time_buffer[80];
    strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", t);
    string date_time(time_buffer);

    // Prepend the log with the current date and time
    message = log_level_to_str(level) + " [" + date_time + " " +
              to_string(precise_now.time_since_epoch().count()) + "] " +
              message;
    LogEntry log = LogEntry{message, precise_now};

    buffer.push_front(log);
  }

  vector<string> read_n(size_t n) {
    vector<LogEntry> read_buffer = buffer.read_n(n);
    return log_level_to_sorted_string(read_buffer);
  }

  vector<string> read_all() {
    vector<LogEntry> read_buffer = buffer.read_all();
    return log_level_to_sorted_string(read_buffer);
  }

private:
  Buffer<LogEntry> buffer;

  vector<string> log_level_to_sorted_string(vector<LogEntry> logs) {
    vector<string> return_buffer;
    sort(logs.begin(), logs.end());
    for (int i = 0; i < logs.size(); i++) {
      return_buffer.push_back(logs[i].message);
    }
    return return_buffer;
  }

  string log_level_to_str(LogLevel level) {
    switch (level) {
    case INFO:
      return "INFO";
    case ERROR:
      return "ERROR";
    default:
      return "UNKNOWN";
    }
  }
};

class IntBuffer {
public:
  IntBuffer() {}

  IntBuffer(size_t bound) : buffer(bound) {}

  // The logger that all the operations performed on the buffer will be logged
  // to
  Logger logger;

  void push_front(int element) {
    std::lock_guard<std::mutex> lock(int_buffer_lock);
    size_t current_size;
    try {
      current_size = buffer.push_front(element);
    } catch (const runtime_error &e) {
      string log = "Failed to push " + to_string(element) +
                   " to the front of the buffer. Reason: " + string(e.what());
      logger.write(log, Logger::LogLevel::ERROR);
      throw e;
    }
    string log =
        "Pushed " + to_string(element) +
        " to the front of the buffer. Buffer size: " + to_string(current_size);
    logger.write(log, Logger::LogLevel::INFO);
  }

  int pop_back() {
    std::lock_guard<std::mutex> lock(int_buffer_lock);
    int element;
    try {
      element = buffer.pop_back();
    } catch (const runtime_error &e) {
      string log = "Failed to pop from the back of the buffer. Reason: " +
                   string(e.what());
      logger.write(log, Logger::LogLevel::ERROR);
      throw e;
    }
    string log =
        "Popped " + to_string(element) + " from the back of the buffer.";
    logger.write(log, Logger::LogLevel::INFO);
    return element;
  }

  void set_bound(size_t bound) {
    std::lock_guard<std::mutex> lock(int_buffer_lock);
    buffer.set_bound(bound);
    string log = "Set the bound of the buffer to " + to_string(bound) + ".";
    logger.write(log, Logger::LogLevel::INFO);
  }

  void remove_bound() {
    std::lock_guard<std::mutex> lock(int_buffer_lock);
    buffer.remove_bound();
    string log = "Removed bound of the buffer.";
    logger.write(log, Logger::LogLevel::INFO);
  }

private:
  Buffer<int> buffer;
  mutex int_buffer_lock;
};

void performPushOperations(IntBuffer &buffer, int start, int count) {
  for (int i = start; i < start + count; ++i) {
    bool success = false;
    while (!success) {
      try {
        buffer.push_front(i);
        success = true;
      } catch (const runtime_error &e) {
        this_thread::sleep_for(chrono::milliseconds(100));
      }
    }
  }
}

void performPopOperations(IntBuffer &buffer, int count) {
  for (int i = 0; i < count; ++i) {
    bool success = false;
    while (!success) {
      try {
        buffer.pop_back();
        success = true;
      } catch (const runtime_error &e) {
        this_thread::sleep_for(chrono::milliseconds(100));
      }
    }
  }
}

int main(int argc, char *argv[]) {
  IntBuffer buffer(10);

  vector<thread> threads;

  for (int i = 0; i < 5; ++i) {
    threads.push_back(thread(performPushOperations, ref(buffer), i * 5, 5));
  }

  for (int i = 0; i < 3; ++i) {
    threads.push_back(thread(performPopOperations, ref(buffer), 5));
  }

  for (thread &t : threads) {
    t.join();
  }

  vector<string> read_buffer = buffer.logger.read_all();
  for (size_t i = 0; i < read_buffer.size(); ++i) {
    cout << read_buffer[i] << "\n";
  }
  cout << endl;

  return 0;
}
