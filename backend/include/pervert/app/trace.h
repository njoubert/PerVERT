#ifndef PERVERT_APP_TRACE_H_
#define PERVERT_APP_TRACE_H_

#include <stdint.h>

#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "utils/log.h"

namespace PerVERT
{
namespace App
{

class Trace
{
  public:
    
    struct Location
    {
      const std::string* file; // Filename
      unsigned int line;       // Line number
      uint64_t address;        // Starting address

      std::string debugPrint() const;
    };

    typedef std::vector<Location*> Context;

    struct Event
    {
      enum Type { MALLOC = 0, FREE, READ, WRITE, END };
      
      Type type;        // What type of event was this?
      uint64_t arg1;    // MALLOC: begin, FREE: begin, READ/WRITE: addr
      uint64_t arg2;    // MALLOC: end
      Context* context; // Context

      std::string debugPrint() const;
    };

    Trace(const char* linefile, const char* tracefile);

    bool okay() const;

    std::string debugPrint() const;

  private:
    // Internal status
    bool okay_;

    // Log file
    Utils::Log& log_;

    // Location File
    std::vector<Location> locations_;
    // Trace File
    std::vector<Event> events_;

    // Filename pool
    std::set<std::string> filenames_;
    // Context pool
    std::map<std::string, Context> contexts_;

    // Trace index by type
    std::map<Event::Type, std::vector<Event*> > byType_;
    // Trace index by context
    std::map<Context*, std::vector<Event*> > byContext_;

    // Helper methods used by constructor
    bool parseLineFile(const char* file);
    bool parseTraceFile(const char* file);
    void parseContext(const std::string& s);
    void indexTraceFile();
};

}
}

#endif
