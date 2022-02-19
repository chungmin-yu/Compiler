#ifndef AST_P_TYPE_H
#define AST_P_TYPE_H

#include <memory>
#include <cstring>
#include <vector>

class PType;

using PTypeSharedPtr = std::shared_ptr<PType>;

class PType {
  public:
    enum class PrimitiveTypeEnum : uint8_t {
        kVoidType,
        kIntegerType,
        kRealType,
        kBoolType,
        kStringType
    };

  private:
    PrimitiveTypeEnum m_type;
    std::vector<uint64_t> m_dimensions;
    mutable std::string m_type_string;
    mutable bool m_type_string_is_valid = false;

  public:
    ~PType() = default;
    PType(const PrimitiveTypeEnum type) : m_type(type) {}

    bool operator==(PType b) { return !( strcmp(getPTypeCString(), b.getPTypeCString()) ); }
    bool operator!=(PType b) { return !(*this == b); }
    void setDimensions(std::vector<uint64_t> &p_dims) {
        m_dimensions = std::move(p_dims);
    }
    std::vector<uint64_t> getDimensions() {return m_dimensions;};

    PrimitiveTypeEnum getPrimitiveType() const { return m_type; }
    const char *getPTypeCString() const;
};

#endif
