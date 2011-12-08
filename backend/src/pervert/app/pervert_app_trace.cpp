#include "pervert/app/trace.h"

#include <cassert>

#include <fstream>
#include <sstream>

using namespace Utils;
using namespace std;

namespace PerVERT
{
namespace App
{

Trace::Trace(const char* linefile, const char* tracefile) :
  okay_(true),
  log_(GETLOG("TRACE"))
{
  log_.log(LOG_STATUS, "Initializing trace\n");
  log_.log(LOG_STATUS, "  linefile=%s\n", linefile);
  log_.log(LOG_STATUS, "  tracefile=%s\n", tracefile);

  okay_ = okay_ && parseLineFile(linefile);
  log_.log(LOG_STATUS, "Done parsing line file!\n");
  okay_ = okay_ && parseTraceFile(tracefile);
  log_.log(LOG_STATUS, "Done parsing trace file!\n");
  indexTraceFile();
  log_.log(LOG_STATUS, "Done indexing trace file!\n");

  log_.log(LOG_STATUS, "Trace:\n%s", debugPrint().c_str());
}

bool Trace::okay() const
{
  return okay_;
}

string Trace::Location::debugPrint() const
{
  ostringstream oss;
  oss << *file << ":" << line << " (" << hex << address << dec << ")";

  return oss.str();
}

string Trace::Event::debugPrint() const
{
  ostringstream oss;

  switch ( type )
  {
    case Trace::Event::MALLOC:
      oss << "MALLOC" << endl;
      oss << "  begin: " << hex << arg1 << dec << endl;
      oss << "  end:   " << hex << arg2 << dec << endl;
      break;
    case Trace::Event::FREE:
      oss << "FREE" << endl;
      oss << "  begin: " << hex << arg1 << dec << endl;
      break;
    case Trace::Event::READ:
      oss << "READ" << endl;
      oss << "  addr: " << hex << arg1 << dec << endl;
      break;
    case Trace::Event::WRITE:
      oss << "WRITE" << endl;
      oss << "  addr: " << hex << arg1 << dec << endl;
      break;

    default:
      assert(false && "Control should never reach here!");
      break;
  }

  oss << "  context:" << endl;
  for ( Context::const_iterator i = context->begin(), ie = context->end(); i != ie; ++i )
    oss << "    " << (*i)->debugPrint() << endl;

  return oss.str();
}

string Trace::debugPrint() const
{
  ostringstream oss;

  oss << "Line File:" << endl;
  for ( vector<Location>::const_iterator i = locations_.begin(), ie = locations_.end(); i != ie; ++i )
    oss << (*i).debugPrint() << endl;

  oss << "Trace File:" << endl;
  for ( vector<Event>::const_iterator i = events_.begin(), ie = events_.end(); i != ie; ++i )
    oss << (*i).debugPrint();

  return oss.str();
}

bool Trace::parseLineFile(const char* file)
{
  ifstream ifs(file);
  if ( ifs.fail() )
    return false;

  // Insert the unknown location
  Location unknown;
  unknown.file = &(*(filenames_.insert("UNKNOWN").first));
  unknown.line = 0;
  unknown.address = 0;

  locations_.push_back(unknown);

  // Always discard the first seven lines (but record the file on line 6)
  string ignore, currentfile;
  getline(ifs, ignore);      // 
  getline(ifs, ignore);      // <exec>:     file format elf64-x86-64
  getline(ifs, ignore);      // 
  getline(ifs, ignore);      // Decoded dump of debug contents of section .debug_line:
  getline(ifs, ignore);      //
  getline(ifs, currentfile); // CU: <filename>:
  getline(ifs, ignore);      // File name [...] Line number [...] Starting address

  // Discard leading and trailing characters from currentfile
  currentfile = currentfile.substr(4, currentfile.rfind(':')-4);

  Location location;
  while ( true )
  {
    string line;
    getline(ifs, line);

    if ( ifs.eof() )
      break;

    // Ignore empty lines
    if ( line.length() == 0 )
      continue;

    // Ignore lines that begin UNKNOWN
    if ( line.length() >= 7 && line.substr(0,7) == "UNKNOWN" ) 
      continue;

    // Update the currentfile for lines that begin with path delimiters
    if ( line[0] == '.' || line[0] == '/' )
    {
      currentfile = line.substr(0, line.rfind(':'));
      continue;
    }

    // Everything else is a location (ignore the short filename, use currentfile)
    istringstream iss(line);
    iss >> ignore;

    location.file = &(*(filenames_.insert(currentfile).first));
    iss >> location.line;
    iss >> hex >> location.address >> dec;

    locations_.push_back(location);
  }

  return true;
}

bool Trace::parseTraceFile(const char* file)
{
  ifstream ifs(file);
  if ( ifs.fail() )
    return false;

  Event event;
  while ( true )
  {
    char c;
    ifs >> c;

    switch ( c )
    {
      case 'M':
        event.type = Trace::Event::MALLOC;
        ifs >> hex >> event.arg1 >> dec;
        ifs >> hex >> event.arg2 >> dec;
        break;

      case 'F':
        event.type = Trace::Event::FREE;
        ifs >> hex >> event.arg1 >> dec;
        break;

      case 'R':
        event.type = Trace::Event::READ;
        ifs >> hex >> event.arg1 >> dec;
        break;

      case 'W':
        event.type = Trace::Event::WRITE;
        ifs >> hex >> event.arg1 >> dec;
        break;

      case 'E':
        event.type = Trace::Event::END;
        break;

      default:
        assert(false && "Control should never reach here!");
        break;
    }

    if ( event.type == Trace::Event::END )
      break;

    string context;
    getline(ifs, context);
    parseContext(context);
    event.context = &contexts_[context]; 

    events_.push_back(event);
  }

  return true;
}

void Trace::parseContext(const string& s)
{
  if ( contexts_.find(s) != contexts_.end() )
    return;

  istringstream iss(s);

  Context context;
  while ( true )
  {
    uint64_t address;
    iss >> hex >> address >> dec;
    
    if ( iss.eof() )
      break;

    unsigned int i = 1;
    for ( unsigned int ie = locations_.size() - 1; i < ie; ++i )
      if ( address >= locations_[i].address && address < locations_[i+1].address ) 
      {
        context.push_back(&locations_[i]);
        break;
      }
 
    if ( i == locations_.size() - 1 ) 
      context.push_back(&locations_[0]);
  }

  contexts_[s] = context;
}

void Trace::indexTraceFile()
{
  for ( vector<Event>::iterator i = events_.begin(), ie = events_.end(); i != ie; ++i )
  {
    Event& event = *i;

    byContext_[event.context].push_back(&event);
    switch ( event.type )
    {
      case Trace::Event::MALLOC:
        byType_[Event::MALLOC].push_back(&event);
        break;
      case Trace::Event::FREE:
        byType_[Event::FREE].push_back(&event);
        break;
      case Trace::Event::READ:
        byType_[Event::READ].push_back(&event);
        break;
      case Trace::Event::WRITE:
        byType_[Event::WRITE].push_back(&event);
        break;
        
      default:
        assert(false && "Control should never reach here!");
        break;
    }
  }
}

}
}
