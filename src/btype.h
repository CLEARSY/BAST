/*
   This file is part of BAST.
   Copyright © CLEARSY 2022-2025
   BAST is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef BType_H
#define BType_H

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

#include "hash.h"

/**
 * @brief Abstract class to represent B data types
 *
 * @todo Add data types for deferred sets and enumerations.
 */
class BType {
 public:
  enum class Kind {
    INTEGER,
    BOOLEAN,
    FLOAT,
    REAL,
    STRING,
    ProductType,
    PowerType,
    Struct,
    AbstractSet,
    EnumeratedSet
  };
  Kind getKind() const { return kind; };

  class ProductType;
  class PowerType;
  class RecordType;
  class AbstractSet;
  class EnumeratedSet;

  const ProductType &toProductType() const;
  const PowerType &toPowerType() const;
  const RecordType &toRecordType() const;
  const AbstractSet &toAbstractSetType() const;
  const EnumeratedSet &toEnumeratedSetType() const;

  class Visitor {
   public:
    virtual void visitINTEGER() = 0;
    virtual void visitBOOLEAN() = 0;
    virtual void visitFLOAT() = 0;
    virtual void visitREAL() = 0;
    virtual void visitSTRING() = 0;
    virtual void visitProductType(const BType &lhs, const BType &rhs) = 0;
    virtual void visitPowerType(const BType &ty) = 0;
    virtual void visitRecordType(
        const std::vector<std::pair<std::string, BType>> &fields) = 0;
    virtual void visitAbstractSet(const BType::AbstractSet &type) = 0;
    virtual void visitEnumeratedSet(const BType::EnumeratedSet &type) = 0;
  };
  void accept(Visitor &v) const;

  static const BType INT;
  static const BType BOOL;
  static const BType FLOAT;
  static const BType REAL;
  static const BType STRING;

  static const BType POW_INT;
  static const BType POW_BOOL;
  static const BType POW_FLOAT;
  static const BType POW_STRING;
  static const BType POW_REAL;

  /** @brief the type for succ and pred */
  static const BType RELATION_INTEGER;

  static BType PROD(const BType &lhs, const BType &rhs);
  static BType POW(const BType &content);

  /* either an abstract set or an enumerated set (from TypeInfos in
   * bxml/ibxml/pog files) */
  static BType SET(const std::string &name);

  static BType ABSTRACT_SET(const std::string &name);
  static BType ENUMERATED_SET(
      const std::pair<std::string, std::vector<std::string>> &values);
  static BType STRUCT(const std::vector<std::pair<std::string, BType>> &fields);

  BType() : kind{Kind::INTEGER}, ptr{nullptr} {}

  // Comparisons
  static int compare(const BType &v1, const BType &v2);
  static int vec_compare(const std::vector<BType> &v1,
                         const std::vector<BType> &v2);
  inline bool operator==(const BType &other) const {
    return compare(*this, other) == 0;
  }
  inline bool operator!=(const BType &other) const {
    return compare(*this, other) != 0;
  }
  inline bool operator<(const BType &other) const {
    return compare(*this, other) < 0;
  }
  inline bool operator>(const BType &other) const {
    return compare(*this, other) > 0;
  }
  inline bool operator<=(const BType &other) const {
    return compare(*this, other) <= 0;
  }
  inline bool operator>=(const BType &other) const {
    return compare(*this, other) >= 0;
  }

  size_t hash_combine(size_t seed) const;
  std::string to_string() const;

 private:
  class AbstractBType {
   public:
    virtual void accept(Visitor &v) const = 0;
    virtual size_t hash_combine(size_t seed) const = 0;
    virtual ~AbstractBType() = default;
  };
  Kind kind;
  std::shared_ptr<AbstractBType> ptr;

  BType(Kind kind, const std::shared_ptr<AbstractBType> &ptr)
      : kind{kind}, ptr{ptr} {};
};

class BType::ProductType : public AbstractBType {
 public:
  ProductType(const BType &lhs, const BType &rhs) : lhs{lhs}, rhs{rhs} {};
  virtual ~ProductType() = default;

  void accept(Visitor &v) const override { v.visitProductType(lhs, rhs); }
  size_t hash_combine(size_t seed) const override {
    return lhs.hash_combine(rhs.hash_combine(seed));
  }

  const BType lhs;
  const BType rhs;
};

class BType::PowerType : public AbstractBType {
 public:
  PowerType(const BType &content) : content{content} {};
  virtual ~PowerType() = default;

  void accept(Visitor &v) const override { v.visitPowerType(content); }
  size_t hash_combine(size_t seed) const override {
    return content.hash_combine(seed);
  }

  const BType content;
};
class BType::AbstractSet : public AbstractBType {
 public:
  AbstractSet(const std::string &name) : name{name} {};

  size_t hash_combine(size_t seed) const {
    return (BType::INT.hash_combine(seed));
  }
  void accept(Visitor &v) const { v.visitAbstractSet(*this); }
  const std::string &getName() const { return name; }
  const std::string name;
};
class BType::EnumeratedSet : public AbstractSet {
 public:
  EnumeratedSet(const std::pair<std::string, std::vector<std::string>> &values)
      : AbstractSet(values.first), values{values} {};

  size_t hash_combine(size_t seed) const {
    return (BType::INT.hash_combine(seed));
  }
  void accept(Visitor &v) const { v.visitEnumeratedSet(*this); }

  const std::string &getName() const { return values.first; }
  const std::vector<std::string> &getContent() const { return values.second; }
  const std::pair<std::string, std::vector<std::string>> values;
};

class BType::RecordType : public AbstractBType {
 public:
  RecordType(const std::vector<std::pair<std::string, BType>> &fields)
      : m_fields{sort(fields)} {};
  virtual ~RecordType() = default;

  void accept(Visitor &v) const override { v.visitRecordType(m_fields); }
  size_t hash_combine(size_t seed) const override {
    size_t res = seed;
    for (auto &p : m_fields)
      res = hashUtil::hash_combine_string(p.first, p.second.hash_combine(res));
    return res;
  }

  const std::vector<std::pair<std::string, BType>>
      m_fields;  // invariant: fields are sorted alphabetically
 private:
  std::vector<std::pair<std::string, BType>> sort(
      const std::vector<std::pair<std::string, BType>> &fields);
};

#endif
