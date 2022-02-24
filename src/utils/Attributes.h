#pragma once

namespace MochaLang {

	enum class AttrType {
		PRIVATE,
		PUBLIC,
		CONST
	};

	class Attribute {
	private:
		AttrType type;
	public:
		Attribute(AttrType type) : type(type) { }

		AttrType getType() { return type; }
	};


	class PrivateAttr : Attribute {
	public:
		PrivateAttr() : Attribute(AttrType::PRIVATE) { }
	};
	
	class PublicAttr : Attribute {
	public:
		PublicAttr() : Attribute(AttrType::PUBLIC) { }
	};

	class ConstAttr : Attribute {
	public:
		ConstAttr() : Attribute(AttrType::CONST) { }
	};

};