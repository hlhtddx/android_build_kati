#include <unordered_map>
#include <unordered_set>
#include <algorithm>
using namespace std;

#include "strutil.h"

#include "breakpoint.h"
#include "log.h"

namespace Debug {
  class BrokenFile : public unordered_set<int> {
   public:
    bool Find(int lineno) const {
      return find(lineno) != end();
    }
  };

  class BrokenFileMap : public unordered_map<string, BrokenFile> {
   public:
    //Insert a new break point. Do nothing if exists
    void Insert(const char *path, int lineno) {
      string real_path(path);
      auto iter = find(real_path);
      if (iter == end()) {
        emplace(real_path, BrokenFile());
      }
      iter = find(real_path);
      iter->second.insert(lineno);
    }

    //Remove a break point. Do nothing if not exist
    void Remove(const string &path, int lineno) {
      auto iter = find(path);
      if (iter == end()) {
        return;
      }
      auto &bp_file = iter->second;
      bp_file.erase(lineno);
      if (bp_file.empty()) {
        erase(path);
      }
    }

    //Remove all break points. Do nothing if not exist
    void RemoveAll() {
      clear();
    }

    //Return true if exists, or false otherwise
    bool Find(const string &path, int lineno) const {
      auto iter = find(path);
      if (iter == end()) {
        return false;
      }
      auto &bp_file = iter->second;
      return bp_file.Find(lineno);
    }

    //Return true if exists, or false otherwise
    const BrokenFile *Find(const string &path) const {
      if (path.empty()) {
        return nullptr;
      }
      auto iter = find(path);
      if (iter == end()) {
        return nullptr;
      }
      return &(iter->second);
    }
  };

  class BreakPointList : public vector<BreakPoint> {
   public:
    //Insert a new break point. Do nothing if exists
    void Insert(const char *path, int lineno) {
      emplace_back(path, lineno);
      sort(begin(), end(), BreakPoint::Compare);
    }

    //Remove a break point. Do nothing if not exist
    void Remove(int index) {
      erase(begin() +index);
    }

    //Remove all break points. Do nothing if not exist
    void RemoveAll() {
      clear();
    }
  };

  class BreakPointsImpl : public BreakPoints {
   public:
    BreakPointsImpl() : current_bf_(nullptr) {
    }
    //Insert a new break point. Do nothing if exists
    void Insert(const char *path, int lineno) override {
      if (bf_map_.Find(path, lineno)) {
        WARN("Break point at %s:%d already exists!\n", path, lineno);
        return;
      }
      bf_map_.Insert(path, lineno);
      bp_list_.Insert(path, lineno);
    }

    //Remove a break point. Do nothing if not exist
    bool Remove(int index) override {
      if (index < 0 || index >= bp_list_.size()) {
        WARN("Break point index %d does not exist!\n", index + 1);
        return false;
      }
      auto& bp = bp_list_[index];
      bf_map_.Remove(bp.filename, bp.lineno);
      bp_list_.Remove(index);
      return true;
    }


    //Remove all break points. Do nothing if not exist
    void RemoveAll() override {
      bf_map_.RemoveAll();
      bp_list_.RemoveAll();
    }

    //Remove a vector of all break points.
    const vector<BreakPoint>& GetList() override {
      return bp_list_;
    }

    //Set current file that is being parsed, to check the break point
    void SetCurrentFile(const string &path) override {
      current_bf_ = bf_map_.Find(path);
    }

    //Return true if it should be broken at current line
    bool ShouldBreak(int lineno) override {
      if (current_bf_ == nullptr) {
        return false;
      }
      return current_bf_->Find(lineno);
    }

   private:
    const BrokenFile *current_bf_;
    BreakPointList bp_list_;
    BrokenFileMap bf_map_;
  };

  unique_ptr<BreakPoints> GetBreakPoints() {
    return make_unique<BreakPointsImpl>();
  }
}
