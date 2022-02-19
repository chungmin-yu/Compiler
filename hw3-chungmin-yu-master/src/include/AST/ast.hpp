#ifndef __AST_H
#define __AST_H

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

#include "visitor/AstNodeVisitor.hpp"
using namespace std;

struct Location {
    uint32_t line;
    uint32_t col;

    ~Location() = default;
    Location(const uint32_t line, const uint32_t col) : line(line), col(col) {}
};

struct constValue{
  string type;
  int integer_value;
  double real_value;
  string string_value;
  bool boolean_value;  
  int col;
};

class AstNode {
  protected:
    const Location location;

  public:
    AstNode(const uint32_t line, const uint32_t col);
    virtual ~AstNode() = 0;

    const Location &getLocation() const;
    virtual void print() = 0;
    virtual void accept(AstNodeVisitor &p_visitor) = 0;
    virtual void visitChildNodes(AstNodeVisitor &p_visitor) = 0;   
};

#endif
