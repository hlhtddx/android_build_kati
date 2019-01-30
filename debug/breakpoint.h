//
// Created by Frank Zhang on 2019-01-26.
//

#ifndef DEBUG_BREAKPOINT_H
#define DEBUG_BREAKPOINT_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include "strutil.h"

using std::string;
using std::unordered_map;
using std::unordered_set;

namespace Debug {
  class BreakPointFile {
   private:
    string real_path_;
    unordered_set<int> lineno_list_;
   public:
    BreakPointFile(const char* path) {
      real_path_ = path;
      //AbsPath(path, &real_path_);
    }

    const char* GetRealPath() const {
      return real_path_.c_str();
    }

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

    void AddBreakPointer(const char* path, int lineno) {
      string real_path(path);
//      AbsPath(path, &real_path);
      auto iter = find(real_path);
      if (find(real_path) == end()) {
        emplace(real_path, real_path.c_str());
      }
      iter = find(real_path);
      iter->second.AddLine(lineno);
    }

    void RemoveBreakPointer(const char* path, int lineno) {
      string real_path(path);
//      AbsPath(path, &real_path);
      auto iter = find(real_path);
      if (find(real_path) == end()) {
        return;
      }
      auto &bp_file = iter->second;
      bp_file.RemoveLine(lineno);
      if (bp_file.GetLineCount() == 0) {
        erase(real_path);
      }
    }

    void RemoveAllBreakPoints() {
      clear();
    }
  };

}

#endif // DEBUG_BREAKPOINT_H
