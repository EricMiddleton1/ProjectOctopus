template<typename T, size_t MaxSize>
class StaticQueue {
public:
  StaticQueue()
    : len  {0} {
  }

  bool push(const T& element) {
    if(len >= MaxSize) {
      return false;
    }
    else {
      data[len++] = element;
      return true;
    }
  }
  
  bool front(T& element) {
    if(len == 0) {
      return false;
    }
    else {
      element = data[0];
      return true;
    }
  }
  
  bool pop() {
    if(len == 0) {
      return false;
    }
    else {
      memmove(data, data + 1, (len - 1) * sizeof(T));
      --len;
      
      return true;
    }
  }

  size_t size() const {
    return len;
  }
  
  size_t maxSize() const {
    return MaxSize;
  }
  
private:
  T data[MaxSize];
  size_t len;
};
