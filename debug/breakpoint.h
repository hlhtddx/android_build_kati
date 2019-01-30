//
// Created by Frank Zhang on 2019-01-26.
//

#ifndef DEBUG_BREAKPOINT_H
#define DEBUG_BREAKPOINT_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include "strutil.h"

using namespace std;

namespace Debug {
  class BreakPointFile {
   private:
    unordered_set<int> lineno_list_;
   public:
    void AddLine(int lineno) {
      lineno_list_.insert(lineno);
    }

    void RemoveLine(int lineno) {
      lineno_list_.erase(lineno);
    }

    size_t GetLineCount() {
      return lineno_list_.size();
    }

    bool ShouldBreak(int lineno) {
      return lineno_list_.find(lineno) != lineno_list_.end();
    }
  };

  class BreakPointMap : public unordered_map<string, BreakPointFile> {
   public:

    void AddBreakPointer(const char* path, int lineno);

    void RemoveBreakPointer(const char* path, int lineno);

    void RemoveAllBreakPoints();
  };

}

#endif // DEBUG_BREAKPOINT_H
