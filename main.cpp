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

mutex cout_mutex;

template <typename T> class Buffer {
public:
  Buffer() : bounded(false) {}

  Buffer(size_t bound) : bounded(true), bound(bound) {
    if (bound == 0)
      throw invalid_argument("Bound should be greater than zero!");
    buffer.reserve(bound);
  }

  /**
   * @brief Pushes an element to the front of the buffer
   *
   * @param element The element to be pushed
   * @return true - The element has been pushed successfully
   * @return false - Pushing the element has failed
   */
  bool push_front(T element) {

    lock_guard<mutex> guard(buffer_lock);
    if (bounded && buffer.size() == max_size) {
      return false;
    }
    buffer.insert(buffer.begin(), element);
    return true;
  }

  /**
   * @brief Pops an element from the end of the buffer
   *
   * @param element Reference to a placeholder where the popped element will be
   * saved
   * @return true - The element has been popped successfully
   * @return false - Popping the element has failed
   */
  bool pop_back(T &element) {
    lock_guard<mutex> guard(buffer_lock);
    if (buffer.size() == 0) {
      // Buffer empty
      return false;
    }
    element = buffer.back();
    buffer.pop_back();
    return true;
  }

  /**
   * @brief Read the n last elements from the buffer and append them to the
   * provided read buffer
   *
   * @param n Number of elements to be read from the buffer
   * @param b Reference to a buffer where the read elements should be appended
   * to
   * @return true - The elements have been read successfully
   * @return false - Reading has failed
   */
  bool read_n(size_t n, vector<T> &read_buffer) {
    lock_guard<mutex> guard(buffer_lock);
    if (n > buffer.size()) {
      // Not enough elements to read
      return false;
    }

    read_buffer.resize(read_buffer.size() + n);

    for (int i = buffer.size() - n; i < buffer.size(); ++i) {
      read_buffer.push_back(buffer[i]);
    }

    return true;
  }

  /**
   * @brief Read all the elements from the buffer and append them to the
   * provided read buffer.
   *
   * @param read_buffer Reference to a buffer where the read elements should be
   * appended to
   * @return true - The elements have been read successfully
   * @return false - Reading has failed
   */
  bool read(vector<T> &read_buffer) {
    lock_guard<mutex> guard(buffer_lock);
    if (buffer.empty()) {
      return false;
    }

    read_buffer.resize(read_buffer.size() + buffer.size());

    for (int i = 0; i < buffer.size(); ++i) {
      read_buffer.push_back(buffer[i]);
    }

    return true;
  }

  /**
   * @brief Get the upper bound of the buffer
   *
   * @return size_t - Current upper bound of the buffer
   */
  size_t get_bound() {
    lock_guard<mutex> guard(buffer_lock);
    return bound;
  }

  /**
   * @brief Set the upper bound of the buffer
   *
   * @param bound The new upper bound for the buffer
   */
  void set_bound(size_t bound) {
    lock_guard<mutex> guard(buffer_lock);
    if (bound >= buffer.size()) {
      buffer.reserve(this->bound - bound);
    } else {
      buffer.resize(bound);
    }
    return
  }

  void print_buffer() {
    lock_guard<mutex> guard(buffer_lock);
    lock_guard<mutex> guard(cout_mutex);
    for (size_t i = 0; i < buffer.size(); ++i) {
      std::cout << buffer[i] << " "; // Access elements using index
    }
    std::cout << std::endl; // Print a newline at the end
  }

private:
  mutex buffer_lock;
  vector<T> buffer;
  bool bounded;
  size_t max_size;
};

class Logger {
public:
  void write(string log) { buffer.push_front(log); }

  string read_one() {
    vector<string> read_buffer;
    buffer.read_n(1, read_buffer);

    return read_buffer.at(0);
  }

  vector<string> read_n(size_t n) {
    vector<string> read_buffer;
    buffer.read_n(n, read_buffer);

    return read_buffer;
  }

  vector<string> read_all() {
    vector<string> read_buffer;
    buffer.read(read_buffer);

    return read_buffer;
  }

  void print_logs() { buffer.print_buffer(); }

private:
  Buffer<string> buffer;
};

class IntBuffer {
public:
  Logger logger;

  void push_front(int element) { buffer.push_front(element); }

private:
  Buffer<int> buffer;
};

void performBufferOperationsOne(Buffer<int> &buffer) {
  // buffer.push_front(10);
  // this_thread::sleep_for(chrono::seconds(2));
  // int element;
  // buffer.pop_back(element);
  // cout_mutex.lock();
  // cout << "Thread 1: " << element << "\n";
  // cout_mutex.unlock();
  buffer.push_front(1);
  buffer.push_front(2);
  buffer.push_front(3);
  buffer.push_front(4);
}

void performBufferOperationsTwo(Buffer<int> &buffer) {
  // this_thread::sleep_for(chrono::seconds(1));
  // int element;
  // buffer.pop_back(element);
  // buffer.push_front(20);
  // cout_mutex.lock();
  // cout << "Thread 2: " << element << "\n";
  // cout_mutex.unlock();
  buffer.push_front(5);
  buffer.push_front(6);
  buffer.push_front(7);
  buffer.push_front(8);
}

int main(int argc, char *argv[]) {
  Buffer<int> buffer;

  thread t1(performBufferOperationsOne, ref(buffer));
  thread t2(performBufferOperationsTwo, ref(buffer));

  t1.join();
  t2.join();

  vector<int> read_buffer;

  buffer.read_n(4, ref(read_buffer));

  for (size_t i = 0; i < read_buffer.size(); ++i) {
    std::cout << read_buffer[i] << " "; // Access elements using index
  }
  std::cout << std::endl; // Print a newline at the end

  return 0;
}
