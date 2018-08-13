/**********************************************************************
* Copyright (C) 2018 - tx7do - All Rights Reserved
*
* 文件名称:		SerializedPhpParser.h
* 摘    要:		PHP序列化解析器
*
* 作    者:		yanglinbo,
* 修    改:		查看文件最下方.
*
***********************************************************************/

#include "SerializedPhpParser.h"

static inline char* duplicateStringValue(const char* value, unsigned int length = -1)
{
	if (length == -1)
	{
		length = (unsigned int)strlen(value);
	}
	char* newString = static_cast<char*>(::malloc(length + 1));
	memcpy(newString, value, length);
	newString[length] = 0;
	return newString;
}
static inline void releaseStringValue(char* value)
{
	if (value) ::free(value);
}

const SerializedValue SerializedValue::null;

SerializedValue::SerializedValue(SerializedValueType type /*= nullValue*/)
{
	type_ = type;

	switch (type)
	{
	case nullValue:
		break;
	case intValue:
		value_.int_ = 0;
		break;
	case realValue:
		value_.real_ = 0.0;
		break;
	case stringValue:
		value_.string_ = 0;
		break;
	case arrayValue:
	case objectValue:
		value_.map_ = new ObjectValues();
		break;
	case booleanValue:
		value_.bool_ = false;
		break;
	default:
		assert(false);
	}
}

SerializedValue::SerializedValue(int32 value)
{
	type_ = intValue;
	value_.int_ = value;
}

SerializedValue::SerializedValue(int64 value)
{
	type_ = intValue;
	value_.int_ = value;
}

SerializedValue::SerializedValue(uint32 value)
{
	type_ = intValue;
	value_.int_ = value;
}

SerializedValue::SerializedValue(uint64 value)
{
	type_ = intValue;
	value_.int_ = value;
}

SerializedValue::SerializedValue(float value)
{
	type_ = realValue;
	value_.real_ = value;
}

SerializedValue::SerializedValue(double value)
{
	type_ = realValue;
	value_.real_ = value;
}

SerializedValue::SerializedValue(const char *value)
{
	type_ = stringValue;
	value_.string_ = duplicateStringValue(value);
}

SerializedValue::SerializedValue(const char *beginValue, const char *endValue)
{
	type_ = stringValue;
	value_.string_ = duplicateStringValue(beginValue, (unsigned int)(endValue - beginValue));
}

SerializedValue::SerializedValue(const std::string& value)
{
	type_ = stringValue;
	value_.string_ = duplicateStringValue(value.c_str(), (unsigned int)value.length());
}

SerializedValue::SerializedValue(bool value)
{
	type_ = booleanValue;
	value_.bool_ = value;
}

SerializedValue::SerializedValue(const SerializedValue& other)
{
	type_ = other.type_;
	switch (type_)
	{
	case nullValue:
	case intValue:
	case realValue:
	case booleanValue:
		value_ = other.value_;
		break;
	case stringValue:
		if (other.value_.string_)
		{
			value_.string_ = duplicateStringValue(other.value_.string_);
			//allocated_ = true;
		}
		else
			value_.string_ = 0;
		break;
	case arrayValue:
	case objectValue:
		value_.map_ = new ObjectValues(*other.value_.map_);
		break;
	default:
		assert(false);
	}
}

SerializedValue::~SerializedValue()
{
	switch (type_)
	{
	case nullValue:
	case intValue:
	case realValue:
	case booleanValue:
		break;
	case stringValue:
		releaseStringValue(value_.string_);
		break;
	case arrayValue:
	case objectValue:
		delete value_.map_;
		break;
	default:
		assert(false);
	}
}

void SerializedValue::swap(SerializedValue &other)
{
	SerializedValueType temp = type_;
	type_ = other.type_;
	other.type_ = temp;
	std::swap(value_, other.value_);
}

SerializedValueType SerializedValue::type() const
{
	return type_;
}

bool SerializedValue::operator>=(const SerializedValue &other) const
{
	return !(*this < other);
}

bool SerializedValue::operator<=(const SerializedValue &other) const
{
	return !(other < *this);
}

bool SerializedValue::operator<(const SerializedValue &other) const
{
	int typeDelta = type_ - other.type_;
	if (typeDelta)
		return typeDelta < 0 ? true : false;
	switch (type_)
	{
	case nullValue:
		return false;
	case intValue:
		return value_.int_ < other.value_.int_;
	case realValue:
		return value_.real_ < other.value_.real_;
	case booleanValue:
		return value_.bool_ < other.value_.bool_;
	case stringValue:
		return (value_.string_ == 0 && other.value_.string_)
			|| (other.value_.string_
				&&  value_.string_
				&& strcmp(value_.string_, other.value_.string_) < 0);
	case arrayValue:
	case objectValue:
	{
		int delta = int(value_.map_->size() - other.value_.map_->size());
		if (delta)
			return delta < 0;
		return (*value_.map_) < (*other.value_.map_);
	}
	default:
		assert(false);
	}
	return false;  // unreachable
}

bool SerializedValue::operator>(const SerializedValue &other) const
{
	return other < *this;
}

bool SerializedValue::operator==(const SerializedValue &other) const
{
	int temp = other.type_;
	if (type_ != temp)
		return false;
	switch (type_)
	{
	case nullValue:
		return true;
	case intValue:
		return value_.int_ == other.value_.int_;
	case realValue:
		return value_.real_ == other.value_.real_;
	case booleanValue:
		return value_.bool_ == other.value_.bool_;
	case stringValue:
		return (value_.string_ == other.value_.string_)
			|| (other.value_.string_
				&&  value_.string_
				&& strcmp(value_.string_, other.value_.string_) == 0);
	case arrayValue:
	case objectValue:
		return value_.map_->size() == other.value_.map_->size()
			&& (*value_.map_) == (*other.value_.map_);
	default:
		assert(false);
	}
	return false;  // unreachable
}

bool SerializedValue::operator!=(const SerializedValue &other) const
{
	return !(*this == other);
}

bool SerializedValue::operator!() const
{
	return isNull();
}

void SerializedValue::clear()
{
	assert(type_ == nullValue || type_ == arrayValue || type_ == objectValue);

	switch (type_)
	{
	case arrayValue:
	case objectValue:
		value_.map_->clear();
		break;
	default:
		break;
	}
}

SerializedValue SerializedValue::key() const
{
	return SerializedValue(0);
}

uint32 SerializedValue::index() const
{
	return uint32(-1);
}

int SerializedValue::compare(const SerializedValue &other) const
{
	if (*this < other)
		return -1;
	if (*this > other)
		return 1;
	return 0;
}

SerializedValue& SerializedValue::operator[](int32 index)
{
	assert(type_ == nullValue || type_ == arrayValue);
	if (type_ == nullValue)
	{
		*this = SerializedValue(arrayValue);
	}
	String key(StringUtility::toStringA(index));
	ObjectValues::iterator it = value_.map_->lower_bound(key);
	if (it != value_.map_->end() && (*it).first == key)
	{
		return (*it).second;
	}

	ObjectValues::value_type defaultValue(key, null);
	it = value_.map_->insert(it, defaultValue);
	return (*it).second;
}

const SerializedValue& SerializedValue::operator[](int32 index) const
{
	assert(type_ == nullValue || type_ == arrayValue);
	if (type_ == nullValue)
	{
		return null;
	}
	String key(StringUtility::toStringA(index));
	ObjectValues::const_iterator it = value_.map_->find(key);
	if (it == value_.map_->end())
	{
		return null;
	}
	return (*it).second;
}

SerializedValue& SerializedValue::operator[](const char* key)
{
	return resolveReference(key);
}

const SerializedValue& SerializedValue::operator[](const char* key) const
{
	assert(type_ == nullValue || type_ == objectValue || type_ == arrayValue);
	if (type_ == nullValue)
	{
		return null;
	}
	ObjectValues::const_iterator it = value_.map_->find(key);
	if (it == value_.map_->end())
	{
		return null;
	}
	return (*it).second;
}

SerializedValue& SerializedValue::operator[](const std::string& key)
{
	return (*this)[key.c_str()];
}

const SerializedValue& SerializedValue::operator[](const std::string& key) const
{
	return (*this)[key.c_str()];
}

bool SerializedValue::isNull() const
{
	return type_ == nullValue;
}

bool SerializedValue::isBool() const
{
	return type_ == booleanValue;
}

bool SerializedValue::isInt() const
{
	return type_ == intValue;
}

bool SerializedValue::isIntegral() const
{
	return type_ == intValue
		|| type_ == booleanValue;
}

bool SerializedValue::isDouble() const
{
	return type_ == realValue;
}

bool SerializedValue::isNumeric() const
{
	return isIntegral() || isDouble();
}

bool SerializedValue::isString() const
{
	return type_ == stringValue;
}

bool SerializedValue::isArray() const
{
	return type_ == nullValue || type_ == arrayValue;
}

bool SerializedValue::isObject() const
{
	return type_ == nullValue || type_ == objectValue;
}

const char * SerializedValue::asCString() const
{
	assert(type_ == stringValue);
	return value_.string_;
}

std::string SerializedValue::asString() const
{
	switch (type_)
	{
	case nullValue:
		return "";
	case stringValue:
		return value_.string_ ? value_.string_ : "";
	case booleanValue:
		return value_.bool_ ? "true" : "false";
	case intValue:
	case realValue:
	case arrayValue:
	case objectValue:
	default:
		break;
	}
	return ""; // unreachable
}

int32 SerializedValue::asInt() const
{
	switch (type_)
	{
	case nullValue:
		return 0;
	case intValue:
		return int32(value_.int_);
	case realValue:
		return int32(value_.real_);
	case booleanValue:
		return value_.bool_ ? 1 : 0;
	case stringValue:
	case arrayValue:
	case objectValue:
		assert(false);
	default:
		assert(false);
	}
	return 0; // unreachable;
}

uint32 SerializedValue::asUInt() const
{
	switch (type_)
	{
	case nullValue:
		return 0;
	case intValue:
		return uint32(value_.int_);
	case realValue:
		return uint32(value_.real_);
	case booleanValue:
		return value_.bool_ ? 1 : 0;
	case stringValue:
	case arrayValue:
	case objectValue:
		assert(false);
	default:
		assert(false);
	}
	return 0; // unreachable;
}

int64 SerializedValue::asInt64() const
{
	switch (type_)
	{
	case nullValue:
		return 0;
	case intValue:
		return int64(value_.int_);
	case realValue:
		return int64(value_.real_);
	case booleanValue:
		return value_.bool_ ? 1 : 0;
	case stringValue:
	case arrayValue:
	case objectValue:
		assert(false);
	default:
		assert(false);
	}
	return 0; // unreachable;
}

uint64 SerializedValue::asUInt64() const
{
	switch (type_)
	{
	case nullValue:
		return 0;
	case intValue:
		return uint64(value_.int_);
	case realValue:
		return uint64(value_.real_);
	case booleanValue:
		return value_.bool_ ? 1 : 0;
	case stringValue:
	case arrayValue:
	case objectValue:
		assert(false);
	default:
		assert(false);
	}
	return 0; // unreachable;
}

float SerializedValue::asFloat() const
{
	switch (type_)
	{
	case nullValue:
		return 0.0f;
	case intValue:
		return static_cast<float>(value_.int_);
	case realValue:
		return float(value_.real_);
	case booleanValue:
		return value_.bool_ ? 1.0f : 0.0f;
	case stringValue:
	case arrayValue:
	case objectValue:
		assert(false);
	default:
		assert(false);
	}
	return 0; // unreachable;
}

double SerializedValue::asDouble() const
{
	switch (type_)
	{
	case nullValue:
		return 0.0;
	case intValue:
		return static_cast<double>(value_.int_);
	case realValue:
		return value_.real_;
	case booleanValue:
		return value_.bool_ ? 1.0 : 0.0;
	case stringValue:
	case arrayValue:
	case objectValue:
		assert(false);
	default:
		assert(false);
	}
	return 0; // unreachable;
}

bool SerializedValue::asBool() const
{
	switch (type_)
	{
	case nullValue:
		return false;
	case intValue:
	case realValue:
		return value_.real_ != 0.0;
	case booleanValue:
		return value_.bool_;
	case stringValue:
		return value_.string_  &&  value_.string_[0] != 0;
	case arrayValue:
	case objectValue:
		return value_.map_->size() != 0;
	default:
		assert(false);
	}
	return false; // unreachable;
}

int32 SerializedValue::size() const
{
	switch (type_)
	{
	case nullValue:
	case intValue:
	case realValue:
	case booleanValue:
	case stringValue:
		return 0;
	case arrayValue:  // size of the array is highest index + 1
	case objectValue:
		return int32(value_.map_->size());
	default:
		assert(false);
	}
	return 0; // unreachable;
}

bool SerializedValue::empty() const
{
	if (isNull() || isArray() || isObject())
		return size() == 0u;
	else
		return false;
}

SerializedValue& SerializedValue::resolveReference(const char* key)
{
	assert(type_ == nullValue || type_ == objectValue || type_ == arrayValue);
	if (type_ == nullValue)
	{
		*this = SerializedValue(arrayValue);
	}
	ObjectValues::iterator it = value_.map_->lower_bound(key);
	if (it != value_.map_->end() && (*it).first == key)
	{
		return (*it).second;
	}

	ObjectValues::value_type defaultValue(key, null);
	it = value_.map_->insert(it, defaultValue);
	SerializedValue& value = (*it).second;
	return value;
}

SerializedValue& SerializedValue::operator=(const SerializedValue &other)
{
	SerializedValue temp(other);
	swap(temp);
	return *this;
}

//////////////////////////////////////////////////////////////////////////


SerializedPhpParser::SerializedPhpParser(const String& strInput)
{
	m_strInput = strInput;
	m_nInputLenght = m_strInput.length();
	m_nIndex = 0;
}

SerializedPhpParser::SerializedPhpParser(const String& strInput, bool bAssumeUTF8)
{
	m_strInput = strInput;
	m_nInputLenght = m_strInput.length();
	m_bAssumeUTF8 = bAssumeUTF8;
	m_nIndex = 0;
}

SerializedPhpParser::~SerializedPhpParser()
{

}

SerializedValue SerializedPhpParser::parse()
{
	SerializedValue result = parseInternal(false);
	cleanup();
	return result;
}

void SerializedPhpParser::cleanup()
{
	m_arrRef.clear();
}

SerializedValue SerializedPhpParser::parseInternal(bool isKey)
{
	checkUnexpectedLength(m_nIndex + 2);

	char type = m_strInput[m_nIndex];
	switch (type)
	{
	case 'i':
	{
		m_nIndex += 2;
		return parseInt(isKey);
	}
	break;

	case 'd':
	{
		m_nIndex += 2;
		return parseFloat(isKey);
	}
	break;

	case 'b':
	{
		m_nIndex += 2;
		return parseBoolean();
	}
	break;

	case 's':
	{
		m_nIndex += 2;
		return parseString(isKey);
	}
	break;

	case 'a':
	{
		m_nIndex += 2;
		return parseArray();
	}
	break;

	case 'O':
	{
		m_nIndex += 2;
		return parseObject();
	}
	break;

	case 'N':
	{
		m_nIndex += 2;
		return nullptr;
	}
	break;

	case 'R':
	{
		m_nIndex += 2;
		return parseReference();
	}
	break;

	default:
	{
		return SerializedValue();
	}
	break;
	}
}

SerializedValue SerializedPhpParser::parseInt(bool isKey)
{
	int32 delimiter = m_strInput.find_first_of(';', m_nIndex);
	if (delimiter == -1)
	{
		return SerializedValue(0);
	}
	checkUnexpectedLength(delimiter + 1);
	String strValue = m_strInput.substr(m_nIndex, delimiter - m_nIndex);
	int32 nValue = StringUtility::parseInt32(strValue);
	SerializedValue value(nValue);
	m_nIndex = delimiter + 1;
	if (!isKey)
	{
		m_arrRef.push_back(value);
	}
	return value;
}

SerializedValue SerializedPhpParser::parseFloat(bool isKey)
{
	int32 delimiter = m_strInput.find_first_of(';', m_nIndex);
	if (delimiter == -1)
	{
		return SerializedValue(0.0f);
	}
	checkUnexpectedLength(delimiter + 1);
	String strValue = m_strInput.substr(m_nIndex, delimiter - m_nIndex);
	double nValue = StringUtility::parseDouble(strValue);
	SerializedValue value(nValue);
	m_nIndex = delimiter + 1;
	if (!isKey)
	{
		m_arrRef.push_back(value);
	}
	return value;
}

SerializedValue SerializedPhpParser::parseBoolean()
{
	int32 delimiter = m_strInput.find_first_of(';', m_nIndex);
	if (delimiter == -1)
	{
		return SerializedValue(false);
	}
	checkUnexpectedLength(delimiter + 1);
	String strValue = m_strInput.substr(m_nIndex, delimiter - m_nIndex);
	bool nValue = StringUtility::parseBool(strValue);
	SerializedValue value(nValue);
	m_nIndex = delimiter + 1;
	m_arrRef.push_back(value);
	return value;
}

int32 SerializedPhpParser::readLength()
{
	int32 delimiter = m_strInput.find_first_of(':', m_nIndex);
	if (delimiter == -1)
	{
		return 0;
	}
	checkUnexpectedLength(delimiter + 2);
	int32 arrayLen = StringUtility::parseInt32(m_strInput.substr(m_nIndex, delimiter - m_nIndex));
	m_nIndex = delimiter + 2;
	return arrayLen;
}

SerializedValue SerializedPhpParser::parseString(bool isKey)
{
	int32 strLen = readLength();
	checkUnexpectedLength(strLen);

	int32 utfStrLen = 0;
	int32 byteCount = 0;
	int32 nextCharIndex = 0;
	while (byteCount != strLen)
	{
		nextCharIndex = m_nIndex + utfStrLen++;
		if (nextCharIndex >= m_nInputLenght)
		{
			return SerializedValue("");
		}
		char ch = m_strInput[nextCharIndex];
		if (m_bAssumeUTF8)
		{
			if ((ch >= 0x0000) && (ch <= 0x007F))
			{
				byteCount++;
			}
			else if (ch > 0x07FF)
			{
				byteCount += 3;
			}
			else
			{
				byteCount += 2;
			}
		}
		else
		{
			byteCount++;
		}
	}
	String strValue = m_strInput.substr(m_nIndex, utfStrLen);
	SerializedValue value(strValue);
	if ((m_nIndex + utfStrLen + 2) > m_nInputLenght
		|| (m_nIndex + utfStrLen) > m_nInputLenght)
	{
		return SerializedValue("");
	}
	String endString = m_strInput.substr(m_nIndex + utfStrLen, 2);
	if (!endString.compare("\";") == 0)
	{
		return SerializedValue("");
	}
	m_nIndex = m_nIndex + utfStrLen + 2;
	if (!isKey)
	{
		m_arrRef.push_back(value);
	}
	return value;
}

SerializedValue SerializedPhpParser::parseArray()
{
	int32 arrayLen = readLength();
	checkUnexpectedLength(arrayLen);

	SerializedValue result(arrayValue);
	m_arrRef.push_back(result);

	for (int i = 0; i < arrayLen; i++)
	{
		const auto& key = parseInternal(true);
		const auto& value = parseInternal(false);
		if (isAcceptedAttribute(key))
		{
			if (key.isNumeric())
			{
				result[key.asInt()] = value;
			}
			else
			{
				result[key.asCString()] = value;
			}
		}
	}
	char endChar = m_strInput[m_nIndex];
	if (endChar != '}')
	{
		return result;
	}

	m_nIndex++;
	return result;
}

SerializedValue SerializedPhpParser::parseObject()
{
	SerializedValue result(objectValue);
	m_arrRef.push_back(result);

	int32 strLen = readLength();
	checkUnexpectedLength(strLen);
	String name = m_strInput.substr(m_nIndex, strLen);
	m_nIndex = m_nIndex + strLen + 2;
	int32 attrLen = readLength();
	for (int32 i = 0; i < attrLen; i++)
	{
		const auto& key = parseInternal(true);
		const auto& value = parseInternal(false);
		if (isAcceptedAttribute(key))
		{
			result[key.asCString()] = value;
		}
	}
	m_nIndex++;
	return result;
}

SerializedValue SerializedPhpParser::parseReference()
{
	int32 delimiter = m_strInput.find_first_of(';', m_nIndex);
	if (delimiter == -1)
	{
		return SerializedValue(referenceValue);
	}
	checkUnexpectedLength(delimiter + 1);
	int32 refIndex = StringUtility::parseInt32(m_strInput.substr(m_nIndex, delimiter - m_nIndex)) - 1;
	m_nIndex = delimiter + 1;
	if ((refIndex + 1) > m_arrRef.size())
	{
		return SerializedValue(referenceValue);
	}
	auto value = m_arrRef[refIndex];
	m_arrRef.push_back(value);
	return value;
}

bool SerializedPhpParser::checkUnexpectedLength(int32 newIndex)
{
	if (m_nIndex > m_nInputLenght || newIndex > m_nInputLenght)
	{
		return false;
	}
	return true;
}
