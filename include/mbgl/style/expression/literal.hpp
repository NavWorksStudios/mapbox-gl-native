#pragma once

#include <mbgl/style/expression/expression.hpp>
#include <mbgl/style/expression/parsing_context.hpp>
#include <mbgl/style/conversion.hpp>

#include <memory>

#include "mbgl/nav/nav.palette.hpp"
#include "mbgl/nav/nav.style.hpp"

namespace mbgl {
namespace style {
namespace expression {

class Literal : public Expression {
public:
    Literal(const Value& value_) : Expression(Kind::Literal, typeOf(value_)), value(value_) {
        if (value.is<Color>()) {
            nav::style::Domain domain("literal");
            nav::palette::bind(domain, value.get<Color>(), this,
            [this](const mbgl::Color& color) {
                value = color;
            });
        }
    }

    Literal(const type::Array& type_, std::vector<Value> value_)
        : Expression(Kind::Literal, type_), value(std::move(value_)) {}
    
    ~Literal() {
        nav::palette::unbind(this);
    }

    EvaluationResult evaluate(const EvaluationContext&) const override {
        return value;
    }
    
    static ParseResult parse(const mbgl::style::conversion::Convertible&, ParsingContext&);

    void eachChild(const std::function<void(const Expression&)>&) const override {}
    
    bool operator==(const Expression& e) const override {
        if (e.getKind() == Kind::Literal) {
            auto rhs = static_cast<const Literal*>(&e);
            return value == rhs->value;
        }
        return false;
    }

    std::vector<optional<Value>> possibleOutputs() const override {
        return {{ value }};
    }
    
    Value getValue() const {
        return value;
    }

    mbgl::Value serialize() const override;
    std::string getOperator() const override { return "literal"; }

private:
    Value value;
};

} // namespace expression
} // namespace style
} // namespace mbgl
