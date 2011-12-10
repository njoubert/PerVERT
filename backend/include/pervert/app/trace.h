#ifndef PERVERT_APP_TRACE_H_
#define PERVERT_APP_TRACE_H_

#include <stdint.h>

#include <iostream>
#include <list>
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

    Trace(const char* linefile, const char* tracefile, uint64_t cacheLine = 1024);

    bool okay() const;

    // TODO:
    // Don't modify these arrays!
    // They should be initialized during construction, and thereafter read-only!
    // Hide this behind an interface eventually.

    // Trace File
    std::vector<Event> events_;

    // Trace index by type
    std::map<Event::Type, std::vector<Event*> > byType_;
    // Trace index by context
    std::map<Context*, std::vector<Event*> > byContext_;
    // Valid regions by timestamp
    std::vector<std::list<Event*> > validRegions_;

    // This will generate A LOT of output
    std::string debugPrint() const;

  private:
    // Internal status
    bool okay_;
    // Cache line size
    uint64_t cacheLine_;
    // Log file
    Utils::Log& log_;

    // Location pool
    std::vector<Location> locations_;
    // Filename pool
    std::set<std::string> filenames_;
    // Context pool
    std::map<std::string, Context> contexts_;

    // Helper methods used by constructor
    bool parseLineFile(const char* file);
    bool parseTraceFile(const char* file);
    void parseContext(const std::string& s);
    void indexTraceFile();
    void remapAddresses();
};

}
}

#endif
