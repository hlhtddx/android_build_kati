//
// Created by Frank Zhang on 2019-01-26.
//

#ifndef DEBUG_BREAKPOINT_H
#define DEBUG_BREAKPOINT_H

#include <string>
#include <vector>
#include <memory>
using namespace std;

namespace Debug {

  struct BreakPoint {
    string filename;
    int lineno;

    BreakPoint(const char *path, int l)
        : filename(path), lineno(l) {
    }

    static bool Compare(const BreakPoint &bp1, const BreakPoint &bp2) {
      if (bp1.filename < bp2.filename) {
        return true;
      } else if (bp1.filename == bp2.filename) {
        return bp1.lineno < bp2.lineno;
      }
      return false;
    }
  };

  class BreakPoints {
   public:
    //Insert a new break point. Do nothing if exists
    virtual void Insert(const char *path, int lineno) = 0;

    //Remove a break point. Do nothing if not exist
    virtual bool Remove(int index) = 0;

    //Remove all break points. Do nothing if not exist
    virtual void RemoveAll() = 0;

    //Remove a vector of all break points.
    virtual const vector<BreakPoint>& GetList() = 0;

    //Set current file that is being parsed, to check the break point
    virtual void SetCurrentFile(const string &path) = 0;

    //Return true if it should be broken at current line
    virtual bool ShouldBreak(int lineno) = 0;
  };

  unique_ptr<BreakPoints> GetBreakPoints();
}

#endif // DEBUG_BREAKPOINT_H
