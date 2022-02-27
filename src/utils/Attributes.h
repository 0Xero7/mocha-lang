#pragma once

#include <unordered_map>
#include "Token.h"

namespace MochaLang {

	enum class AttrType {
		PRIVATE,
		PUBLIC,
		CONST,
		STATIC,

		__MOCHA__CLASS_CONTRUCTOR
	};

	class Attribute {
	private:
		AttrType type;
	public:
		Attribute(AttrType type) : type(type) { }

		AttrType getType() { return type; }
	};


	class PrivateAttr : public Attribute {
	public:
		PrivateAttr() : Attribute(AttrType::PRIVATE) { }
	};
	
	class PublicAttr : public Attribute {
	public:
		PublicAttr() : Attribute(AttrType::PUBLIC) { }
	};

	class ConstAttr : public Attribute {
	public:
		ConstAttr() : Attribute(AttrType::CONST) { }
	};

	class StaticAttr : public Attribute {
	public:
		StaticAttr() : Attribute(AttrType::STATIC) { }
	};

	class MochaLangClassConstructorAttr : public Attribute {
	public:
		MochaLangClassConstructorAttr() : Attribute(AttrType::__MOCHA__CLASS_CONTRUCTOR) { }
	};
};