#ifndef BUFFERS_H
#define BUFFERS_H

template<size_t buffer_size, typename buffer_type>
struct CircBuffer {
  const static size_t n = buffer_size;
  buffer_type data[buffer_size];
  int idx = 0;

  CircBuffer() {
    //zero out the buffer;
    for (int i = 0; i < n; i++) {
      data[i] = 0;
    }
  }

  void add(buffer_type t) {
    idx = (idx + 1) % n;
    data[idx] = t;
  }

  buffer_type getNFwd(int i) {
    assert(i < n);
    return data[(i + idx) % n];
  }

  buffer_type getNBck(int i) {
    assert(i < n);
    int shifted = (i > idx) ? (n + idx - i - 1) : (idx - i);
    return data[shifted];
  }

  buffer_type get(int i) {
    assert(i < n);
    return data[i];
  }

  buffer_type getCurr() {
    return data[idx];
  }
};

#define DATA_SIZE 100

struct DataBuffer : CircBuffer<DATA_SIZE, unsigned long> {
  int n_hits_sec_back(unsigned long time) {
    int cnt = 0;
    for (int i = 0;; ++i) {
      if (time - getNBck(i) > 1000000) {
        break;
      }
      cnt++;
    }
    return cnt;
  }
};

#endif