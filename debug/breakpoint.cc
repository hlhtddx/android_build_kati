#include "breakpoint.h"

namespace Debug {
  void BreakPointMap::AddBreakPointer(const char* path, int lineno) {
    string real_path(path);
//      AbsPath(path, &real_path);
    auto iter = find(real_path);
    if (iter == end()) {
      emplace(real_path, BreakPointFile());
    }
    iter = find(real_path);
    iter->second.AddLine(lineno);
  }

  void BreakPointMap::RemoveBreakPointer(const char* path, int lineno) {
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

  void BreakPointMap::RemoveAllBreakPoints() {
    clear();
  }

}
