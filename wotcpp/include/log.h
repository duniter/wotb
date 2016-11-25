#ifndef LOG_H
#define LOG_H

#include <iostream>

namespace libwot {

  class Log {

  public:

    static void setEnabled(bool enabled) {Log::enabled = enabled;};

    Log() {};
    ~Log() { if(enabled) std::cout << std::endl;};

    template <class T> 
    Log& operator<<(const T& v) {
      if(enabled) std::cout << v;
      return *this;
    }

    Log& operator<<(std::ostream& (*p)(std::ostream&)) {
      if(enabled) std::cout << p;
      return *this;
    }


  private :

      static bool enabled;

  };

  class Log2 {

  public:

    static void setEnabled(bool enabled) {Log2::enabled = enabled;};

    Log2() {};
    ~Log2() { if(enabled) std::cout << std::endl;};

    template <class T>
    Log2& operator<<(const T& v) {
      if(enabled) std::cout << v;
      return *this;
    }

      Log2& operator<<(std::ostream& (*p)(std::ostream&)) {
      if(enabled) std::cout << p;
      return *this;
    }


  private :

      static bool enabled;

  };

}

#endif
