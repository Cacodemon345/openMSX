#include "XMLElement.hh"
#include "XMLEscape.hh"
#include "StringOp.hh"
#include "FileContext.hh" // for bwcompat
#include "ConfigException.hh"
#include "ranges.hh"
#include "serialize.hh"
#include "serialize_stl.hh"
#include "stl.hh"
#include "strCat.hh"
#include "xrange.hh"
#include <cassert>

using std::string;
using std::string_view;

namespace openmsx {

XMLElement::Attributes::iterator XMLElement::getAttributeIter(string_view attrName)
{
	return ranges::find(attributes, attrName,
	                    [](auto& a) { return a.first; });
}
XMLElement::Attributes::const_iterator XMLElement::getAttributeIter(string_view attrName) const
{
	return ranges::find(attributes, attrName,
	                    [](auto& a) { return a.first; });
}

const string* XMLElement::findAttribute(string_view attrName) const
{
	auto it = getAttributeIter(attrName);
	return (it != end(attributes)) ? &it->second : nullptr;
}

void XMLElement::removeAttribute(string_view attrName)
{
	if (auto it = getAttributeIter(attrName); it != end(attributes)) {
		attributes.erase(it);
	}
}

std::vector<const XMLElement*> XMLElement::getChildren(string_view childName) const
{
	std::vector<const XMLElement*> result;
	for (const auto& c : children) {
		if (c.getName() == childName) {
			result.push_back(&c);
		}
	}
	return result;
}

XMLElement* XMLElement::findChild(string_view childName)
{
	auto it = ranges::find(children, childName, &XMLElement::getName);
	return (it != end(children)) ? &*it : nullptr;
}
const XMLElement* XMLElement::findChild(string_view childName) const
{
	return const_cast<XMLElement*>(this)->findChild(childName);
}

const XMLElement* XMLElement::findNextChild(string_view childName,
	                                    size_t& fromIndex) const
{
	for (auto i : xrange(fromIndex, children.size())) {
		if (children[i].getName() == childName) {
			fromIndex = i + 1;
			return &children[i];
		}
	}
	for (auto i : xrange(fromIndex)) {
		if (children[i].getName() == childName) {
			fromIndex = i + 1;
			return &children[i];
		}
	}
	return nullptr;
}

XMLElement& XMLElement::getChild(string_view childName)
{
	if (auto* elem = findChild(childName)) {
		return *elem;
	}
	throw ConfigException("Missing tag \"", childName, "\".");
}
const XMLElement& XMLElement::getChild(string_view childName) const
{
	return const_cast<XMLElement*>(this)->getChild(childName);
}

string_view XMLElement::getChildData(string_view childName) const
{
	return getChild(childName).getData();
}

string_view XMLElement::getChildData(string_view childName,
                                     string_view defaultValue) const
{
	const auto* child = findChild(childName);
	return child ? child->getData() : defaultValue;
}

bool XMLElement::getChildDataAsBool(string_view childName, bool defaultValue) const
{
	const auto* child = findChild(childName);
	return child ? StringOp::stringToBool(child->getData()) : defaultValue;
}

int XMLElement::getChildDataAsInt(string_view childName, int defaultValue) const
{
	const auto* child = findChild(childName);
	if (!child) return defaultValue;
	auto r = StringOp::stringTo<int>(child->getData());
	return r ? *r : defaultValue;
}

bool XMLElement::hasAttribute(string_view attrName) const
{
	return findAttribute(attrName);
}

string_view XMLElement::getAttributeValue(string_view attrName) const
{
	if (const auto* value = findAttribute(attrName)) {
		return *value;
	}
	throw ConfigException("Missing attribute \"", attrName, "\".");
}

string_view XMLElement::getAttributeValue(string_view attrName,
                                          string_view defaultValue) const
{
	const auto* value = findAttribute(attrName);
	return value ? *value : defaultValue;
}

bool XMLElement::getAttributeValueAsBool(string_view attrName,
                                    bool defaultValue) const
{
	const auto* value = findAttribute(attrName);
	return value ? StringOp::stringToBool(*value) : defaultValue;
}

int XMLElement::getAttributeValueAsInt(string_view attrName,
                                       int defaultValue) const
{
	const auto* value = findAttribute(attrName);
	if (!value) return defaultValue;
	auto r = StringOp::stringTo<int>(*value);
	return r ? *r : defaultValue;
}

bool XMLElement::findAttributeInt(string_view attrName,
                                  unsigned& result) const
{
	if (const auto* value = findAttribute(attrName)) {
		if (auto r = StringOp::stringTo<int>(*value)) {
			result = *r;
			return true;
		}
	}
	return false;
}

static std::unique_ptr<FileContext> lastSerializedFileContext;
std::unique_ptr<FileContext> XMLElement::getLastSerializedFileContext()
{
	return std::move(lastSerializedFileContext); // this also sets value to nullptr;
}
// version 1: initial version
// version 2: removed 'context' tag
//            also removed 'parent', but that was never serialized
//        2b: (no need to increase version) name and data members are
//            serialized as normal members instead of constructor parameters
//        2c: (no need to increase version) attributes were initially stored as
//            map<string, string>, later this was changed to
//            vector<pair<string, string>>. To keep bw-compat the serialize()
//            method converted between these two formats. Though (by luck) in
//            the XML output both datastructures are serialized to the same
//            format, so we can drop this conversion step without breaking
//            bw-compat.
template<typename Archive>
void XMLElement::serialize(Archive& ar, unsigned version)
{
	ar.serialize("name",       name,
	             "data",       data,
	             "attributes", attributes,
	             "children",   children);

	if (ar.versionBelow(version, 2)) {
		assert(Archive::IS_LOADER);
		std::unique_ptr<FileContext> context;
		ar.serialize("context", context);
		if (context) {
			assert(!lastSerializedFileContext);
			lastSerializedFileContext = std::move(context);
		}
	}
}
INSTANTIATE_SERIALIZE_METHODS(XMLElement);

} // namespace openmsx
