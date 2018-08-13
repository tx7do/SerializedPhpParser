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

#ifndef __SerializedPhpParser_H__
#define __SerializedPhpParser_H__


//////////////////////////////////////////////////////////////////////////
/// 值类型
//////////////////////////////////////////////////////////////////////////
enum SerializedValueType
{
	nullValue = 0,	///< 'null' value
	intValue,		///< signed integer value
	realValue,		///< double value
	stringValue,	///< UTF-8 string value
	booleanValue,	///< bool value
	arrayValue,		///< array value (ordered list)
	objectValue,	///< object value (collection of name/value pairs).
	referenceValue	///< reference value
};

//////////////////////////////////////////////////////////////////////////
/// 值
//////////////////////////////////////////////////////////////////////////
class SerializedValue
{
public:
	static const SerializedValue null;

public:
	SerializedValue(SerializedValueType type = nullValue);
	SerializedValue(int32 value);
	SerializedValue(int64 value);
	SerializedValue(uint32 value);
	SerializedValue(uint64 value);
	SerializedValue(float value);
	SerializedValue(double value);
	SerializedValue(const char *value);
	SerializedValue(const char *beginValue, const char *endValue);
	SerializedValue(const std::string& value);
	SerializedValue(bool value);
	SerializedValue(const SerializedValue& other);
	~SerializedValue();

	SerializedValue &operator=(const SerializedValue &other);

	void swap(SerializedValue &other);

	SerializedValueType type() const;

	bool operator <(const SerializedValue &other) const;
	bool operator <=(const SerializedValue &other) const;
	bool operator >=(const SerializedValue &other) const;
	bool operator >(const SerializedValue &other) const;

	bool operator ==(const SerializedValue &other) const;
	bool operator !=(const SerializedValue &other) const;

	int compare(const SerializedValue &other) const;

	SerializedValue& operator[](int32 index);
	const SerializedValue& operator[](int32 index) const;

	SerializedValue& operator[](const char *key);
	const SerializedValue& operator[](const char *key) const;
	SerializedValue& operator[](const std::string &key);
	const SerializedValue& operator[](const std::string &key) const;

public:
	bool isNull() const;
	bool isBool() const;
	bool isInt() const;
	bool isIntegral() const;
	bool isDouble() const;
	bool isNumeric() const;
	bool isString() const;
	bool isArray() const;
	bool isObject() const;

	const char *asCString() const;
	std::string asString() const;

	int32 asInt() const;
	uint32 asUInt() const;
	int64 asInt64() const;
	uint64 asUInt64() const;
	float asFloat() const;
	double asDouble() const;
	bool asBool() const;

	int32 size() const;

	bool empty() const;

	bool operator!() const;

	void clear();

	SerializedValue key() const;

	uint32 index() const;

private:
	SerializedValue& resolveReference(const char* key);

public:
	SerializedValueType type_;

	typedef std::map<String, SerializedValue> ObjectValues;

	union ValueHolder
	{
		int64 int_;
		double real_;
		bool bool_;
		char* string_;
		ObjectValues *map_;
	} value_;
};
typedef std::vector<SerializedValue> SerializedValueArray;


//////////////////////////////////////////////////////////////////////////
/// PHP序列化解析器
//////////////////////////////////////////////////////////////////////////
class SerializedPhpParser
{
public:
	SerializedPhpParser(const String& strInput);
	SerializedPhpParser(const String& strInput, bool bAssumeUTF8);
	virtual ~SerializedPhpParser();

public:
	SerializedValue parse();

private:
	void cleanup();

	SerializedValue parseInternal(bool isKey);

	SerializedValue parseInt(bool isKey);
	SerializedValue parseFloat(bool isKey);
	SerializedValue parseBoolean();
	SerializedValue parseString(bool isKey);
	SerializedValue parseArray();
	SerializedValue parseObject();
	SerializedValue parseReference();

	int32 readLength();

	bool isAcceptedAttribute(const SerializedValue& key)
	{
		return true;
	}

private:
	bool checkUnexpectedLength(int32 newIndex);

private:
	String		m_strInput;
	int32		m_nInputLenght;
	bool		m_bAssumeUTF8;
	int32		m_nIndex;
	SerializedValueArray m_arrRef;
};


#endif
