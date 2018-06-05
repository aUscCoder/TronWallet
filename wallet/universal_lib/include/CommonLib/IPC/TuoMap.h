// Author: TuotuoXP
// [USED BY WEBKIT] 
// NOLINT (all)
#pragma once


/*
[TuotuoXP] 使用tuomap，需要配合tuomap编译器使用，源文件test.tuom如下：

package tuomap_test;

struct data_1
{
	char char_data;
	bool bool_data;
	string str_data;
	wstring wstr_data;
};

struct data_2
{
	int int_data;
	short short_data;
};

struct data_3
{
	binary bin_data;
	array<data_2> another;
};

编译器编译之后，会生成对应的test.tuom.h文件，所有struct都会生成一份in和out类，
可以直接调用里面的接口做序列化和反序列化，使用方便，性能高（zero-copy）

注意：在序列化中，字符串只保存指针，不做深拷贝

*/


//////////////////////////////////////////////////////////////////////////

#ifndef TuoAssert
#define TuoAssert
#endif

class tuomap {
public:

class serialize_context;


template<typename char_type>
class string
{
public:
	string() : _string(NULL), _string_len(0) {}
	string(const char_type *s) : _string(s), _string_len(se::strlenTemplate(s)) {}
	string(const char_type *s, int l) : _string(s), _string_len(l) {}
	string(const std::basic_string<char_type> &s) : _string(s.c_str()), _string_len(s.length()) {}
	void operator =(const char_type *s) { *this = string(s); }
	void operator =(const std::basic_string<char_type> &s) { *this = string(s); }

#ifdef USE_SE_STRINGBUF
	string(const se::basic_stringBuffer<char_type> &s) : _string(s.str()), _string_len(s.length()) {}
	void operator =(const se::basic_stringBuffer<char_type> &s) { *this = string(s); }
#endif

	// for in
	const char_type* data_in() const { return _string; }
	int get_needed_memory_size() const { return (_string_len + 1) * sizeof(char_type); }

	// for out
	const char_type* c_str() const { return (const char_type*)((char*)&_string + (int)_string); }
	char_type operator[](int i) const { TuoAssert(i < _string_len); return ((const char_type*)((char*)&_string + (int)_string))[i]; }

	// for both
	int length() const { return _string_len; }
private:
	const char_type *_string;
	int _string_len;
	friend class ::tuomap::serialize_context;
};


class binary
{
public:
	binary() : _data(NULL), _len(0) {}
	binary(const void *p, int l) : _data(p), _len(l) {}

	// for in
	void set(const void *p, int l) { _data = p; _len = l; }
	const void* data_in() const { return _data; }

	// for out
	const void* data() const { return _data == NULL ? NULL : (const void*)((char*)&_data + (int)_data); }

	// for both
	int size() const { return _len; }
private:
	const void *_data;
	int _len;
	friend class ::tuomap::serialize_context;
};


template<class T> class uni;
template<class T> class vec;
template<class TKeyIn, class TKeyOut, class TKeySize, class TVal> class map;


#define ALIGN(s)			((s) + ((4 - (int)(s)) & 3))
#define CHECK_ALIGN(s)		TuoAssert((s) % 4 == 0 && "检测是否4字节对齐")


class serialize_context
{

public:

	template<class T> static inline HANDLE serialize_to_mapping(const T &tuomObj)
	{
		serialize_context context;
		int iDataLen = context.calc_root(tuomObj);
		HANDLE hMappingHandle = ::CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, iDataLen, NULL);
		TuoAssert(hMappingHandle);
		if (hMappingHandle)
		{
			BYTE *pMappingData = (BYTE*)::MapViewOfFile(hMappingHandle, FILE_MAP_WRITE, 0, 0, 0);
			TuoAssert(pMappingData);

			context.serialize_root(tuomObj, pMappingData);

			::UnmapViewOfFile(pMappingData);
		}
		return hMappingHandle;
	}

	serialize_context(bool check_validation = false) : _forward_pos(0), _check_validation(check_validation) {}

	template<class T> int calc_root(const T &t)
	{
		_backward_pos = ALIGN(sizeof(T)) + t.calc_size();
		return (_check_validation ? sizeof(int) * 2 : 0) + _backward_pos;
	}
	template<class T> void serialize_root(const T &t, void *buffer)
	{
		_buffer = (char*)buffer;
		if (_check_validation)
		{
			((int*)_buffer)[0] = T::hash_value;
			((int*)_buffer)[1] = _backward_pos;
			_buffer += sizeof(int) * 2;
		}
		memcpy(_buffer, &t, sizeof(T));
		_forward_pos += ALIGN(sizeof(T));
		TuoAssert(_forward_pos <= _backward_pos);
		t.serialize(this, 0);
	}


	int prealloc_for_container(int first_param, int buf_len, int var_pos)
	{
		CHECK_ALIGN(var_pos);
		CHECK_ALIGN(buf_len);
		// 按照uni/vec/map输出数据的格式赋值，即第2个参数是数据的offset
		*(unsigned int*)(_buffer + var_pos) = first_param;
		*(unsigned int*)(_buffer + var_pos + sizeof(int)) = _forward_pos - var_pos;
		int old_forward_pos = _forward_pos;
		_forward_pos += buf_len;
		TuoAssert(var_pos + (int)sizeof(int) * 2 <= _backward_pos);
		TuoAssert(_forward_pos <= _backward_pos);
		return old_forward_pos;
	}


	// 用于序列化位于用户自定义结构体中的子项（对于string/bin，无需设置size）
	void serialize_extra_buffer(const void *buf, int len, int var_pos, int for_string_null = 0)
	{
		CHECK_ALIGN(var_pos);
		_backward_pos -= len;
		if (buf)
		{
			memcpy(_buffer + _backward_pos, buf, len - for_string_null);
			memset(_buffer + _backward_pos + len - for_string_null, 0, for_string_null);
			*(unsigned int*)(_buffer + var_pos) = _backward_pos - var_pos;
		}
		else
			*(unsigned int*)(_buffer + var_pos) = 0;
		TuoAssert(var_pos + (int)sizeof(int) <= _backward_pos);
		TuoAssert(_forward_pos <= _backward_pos);
	}
	template<typename c> void serialize(const tuomap::string<c> &s, int var_pos) { serialize_extra_buffer(s._string, s.get_needed_memory_size(), var_pos, sizeof(c)); }
	void serialize(const tuomap::binary &bin, int var_pos) { serialize_extra_buffer(bin._data, bin._len, var_pos); }
	template<class T> void serialize(const uni<T> &t, int var_pos) { t.serialize(this, var_pos); }
	template<class T> void serialize(const vec<T> &t, int var_pos) { t.serialize(this, var_pos); }
	template<class TKeyIn, class TKeyOut, class TKeySize, class TVal> void serialize(const map<TKeyIn, TKeyOut, TKeySize, TVal> &t, int var_pos) { t.serialize(this, var_pos); }
	template<class T> void serialize(const T &t, int var_pos) { CHECK_ALIGN(var_pos); t.serialize(this, var_pos); }	// 对于自定义结构体无需序列化header


	// 用于序列化vec/map中的项（对于string/bin，需要重新设置size）
	void serialize_extra_buffer_for_container(const void *buf, int len, int var_pos, int for_string_null = 0)
	{
		serialize_extra_buffer(buf, len, var_pos, for_string_null);
		*(unsigned int*)(_buffer + var_pos + sizeof(int)) = len;
		TuoAssert(var_pos + (int)sizeof(int) * 2 <= _backward_pos);
	}
	template<typename c> void serialize_for_container(const tuomap::string<c> &s, int var_pos) { serialize_extra_buffer_for_container(s._string, s.get_needed_memory_size(), var_pos, sizeof(c)); }
	void serialize_for_container(const tuomap::binary &bin, int var_pos) { serialize_extra_buffer_for_container(bin._data, bin._len, var_pos); }
	template<class T> void serialize_for_container(const uni<T> &t, int var_pos) { t.serialize(this, var_pos); }
	template<class T> void serialize_for_container(const vec<T> &t, int var_pos) { t.serialize(this, var_pos); }
	template<class TKeyIn, class TKeyOut, class TKeySize, class TVal> void serialize_for_container(const map<TKeyIn, TKeyOut, TKeySize, TVal> &t, int var_pos) { t.serialize(this, var_pos); }
	template<class U> inline void serialize_struct_or_pod_for_container(int var_pos, const U &u, void(U::*)())
	{
		// for user struct, 位于vec/map里面的自定义结构体需要序列化header
		memcpy(_buffer + var_pos, &u, sizeof(U));
		TuoAssert(var_pos + (int)sizeof(U) <= _backward_pos);
		u.serialize(this, var_pos);
	}
	template<class U> inline void serialize_struct_or_pod_for_container(int var_pos, const U &u, ...)
	{
		// for pod
		*(U*)(_buffer + var_pos) = u;
		TuoAssert(var_pos + (int)sizeof(U) <= _backward_pos);
	}
	template<class T> void serialize_for_container(const T &t, int var_pos) { serialize_struct_or_pod_for_container<T>(var_pos, t, NULL); }


	// 用于序列化map的key
	template<typename c> void serialize_for_keymap(const std::basic_string<c> *s, int var_pos) { serialize_extra_buffer(s->c_str(), (s->length() + 1) * sizeof(c), var_pos); }
	void serialize_for_keymap(const int *i, int var_pos) { CHECK_ALIGN(var_pos); *(int*)(_buffer + var_pos) = *i; TuoAssert(var_pos + (int)sizeof(int) <= _backward_pos); }
	void serialize_for_keymap(const unsigned int *i, int var_pos) { CHECK_ALIGN(var_pos); *(int*)(_buffer + var_pos) = *i; TuoAssert(var_pos + (int)sizeof(int) <= _backward_pos); }
	void serialize_for_keymap(const __int64 *i, int var_pos) { CHECK_ALIGN(var_pos); *(__int64*)(_buffer + var_pos) = *i; TuoAssert(var_pos + (int)sizeof(__int64) <= _backward_pos); }
	void serialize_for_keymap(const unsigned __int64 *i, int var_pos) { CHECK_ALIGN(var_pos); *(__int64*)(_buffer + var_pos) = *i; TuoAssert(var_pos + (int)sizeof(__int64) <= _backward_pos); }

private:

	char *_buffer;
	int _forward_pos;
	int _backward_pos;

	// 是否在序列化过程中加入合法性检查（即在头部加入hash value和size）
	bool _check_validation;
};


class serialize_helper
{
public:

	// 判断某个对象是否需要额外的存储空间
	template<typename c> static inline bool need_extra_space(const tuomap::string<c>*) { return true; }
	static inline bool need_extra_space(const tuomap::binary*) { return true; }
	template<class T> static inline bool need_extra_space(const uni<T>*) { return true; }
	template<class T> static inline bool need_extra_space(const vec<T>*) { return true; }
	template<class TKeyIn, class TKeyOut, class TKeySize, class TVal> static inline bool need_extra_space(const map<TKeyIn, TKeyOut, TKeySize, TVal>*) { return true; }
	template<class T> static inline bool need_extra_space_struct_or_pod(void(T::*)()) { return T::need_extra_space(); }
	template<class T> static inline bool need_extra_space_struct_or_pod(...) { return false; }
	template<class T> static bool need_extra_space(const T*) { return need_extra_space_struct_or_pod<T>(NULL); }


	// 计算某个对象的额外存储空间
	template<typename c> static inline int calc_size(const tuomap::string<c> &s) { return s.get_needed_memory_size(); }
	static inline int calc_size(const tuomap::binary &bin) { return bin.size(); }
	template<class T> static inline int calc_size(const uni<T> &t) { return t.calc_size(); }
	template<class T> static inline int calc_size(const vec<T> &t) { return t.calc_size(); }
	template<class TKeyIn, class TKeyOut, class TKeySize, class TVal> static inline int calc_size(const map<TKeyIn, TKeyOut, TKeySize, TVal> &t) { return t.calc_size(); }
	template<class T> static inline int calc_size_struct_or_pod(const T &t, void(T::*)()) { return t.calc_size(); }
	template<class T> static inline int calc_size_struct_or_pod(const T &, ...) { TuoAssert(0 && "cannot reach here!"); return -1; }
	template<class T> static inline int calc_size(const T &t) { return calc_size_struct_or_pod<T>(t, NULL); }


	// 判断map的key是否需要额外的存储空间
	static inline bool need_extra_space_for_mapkey(const int*) { return false; }
	static inline bool need_extra_space_for_mapkey(const unsigned int*) { return false; }
	static inline bool need_extra_space_for_mapkey(const __int64*) { return false; }
	static inline bool need_extra_space_for_mapkey(const unsigned __int64*) { return false; }
	static inline bool need_extra_space_for_mapkey(const std::string*) { return true; }
	static inline bool need_extra_space_for_mapkey(const std::wstring*) { return true; }


	// 计算map中key所需要的额外存储空间的大小
	static inline int calc_size_for_mapkey(const int*) { TuoAssert(0 && "cannot reach here!"); return 0; }
	static inline int calc_size_for_mapkey(const unsigned int*) { TuoAssert(0 && "cannot reach here!"); return 0; }
	static inline int calc_size_for_mapkey(const __int64*) { TuoAssert(0 && "cannot reach here!"); return 0; }
	static inline int calc_size_for_mapkey(const unsigned __int64*) { TuoAssert(0 && "cannot reach here!"); return 0; }
	static inline int calc_size_for_mapkey(const std::string *s) { return s->length() + 1; }
	static inline int calc_size_for_mapkey(const std::wstring *s) { return (s->length() + 1) * sizeof(wchar_t); }
};


template<class T> static const T* check_and_get(const void *data, int data_len = -1)
{
	if (data_len != -1 && data_len < sizeof(int) * 2)
		return NULL;
	if (((const int*)data)[0] != T::hash_value)
		return NULL;
	if (data_len != -1 && ((const int*)data)[1] != data_len - (int)sizeof(int) * 2)
		return NULL;
	return (const T*)((char*)data + sizeof(int) * 2);
}

//////////////////////////////////////////////////////////////////////////


#define DUMMY				0x12345678
#define ENSURE_DATA_OUT()	TuoAssert(_dummy != DUMMY && "这个函数只能在读取时使用")

#ifdef _DEBUG
#define FILL_DUMMY()		, _dummy(DUMMY)
#else
#define FILL_DUMMY()
#endif


template<class T> class ref_wrapper : public T
{
public:
	ref_wrapper() : _ref(1) {}
	void add() { _ref++; }
	void release()
	{
		_ref--;
		if (_ref == 0)
			delete this;
	}

private:
	int _ref;
};

template<class T> class reffed_uni : public ref_wrapper<T> {};
template<class T> class reffed_vec : public ref_wrapper<std::vector<T>> {};
template<class TKey, class TVal> class reffed_map : public ref_wrapper<std::map<TKey, TVal>> {};


//////////////////////////////////////////////////////////////////////////


template<class U> class uni
{
public:
	uni() : _datain(NULL) FILL_DUMMY() {}
	uni(const uni &that) : _datain(NULL) FILL_DUMMY()
	{
		if (that._datain)
		{
			_datain = that._datain;
			_datain->add();
		}
	}
	~uni()
	{
		if (_datain)
			_datain->release();
	}

	U* operator->()
	{
		if (_datain == NULL)
			_datain = new reffed_uni<U>;
		return _datain;
	}

	int calc_size() const
	{
		if (_datain == NULL)
		{
			return 0;
		}
		else
		{
			int header_len = _datain->calc_header_size();
			return _datain->calc_size() + ALIGN(header_len);
		}
	}

	void serialize(serialize_context *context, int var_pos) const
	{
		CHECK_ALIGN(var_pos);
		if (_datain == NULL)
		{
			context->prealloc_for_container(-1, 0, var_pos);
		}
		else
		{
			int header_len = _datain->calc_header_size();
			int p = context->prealloc_for_container(_datain->type, ALIGN(header_len), var_pos);
			_datain->serialize(context, p);
		}
	}

	// for out
	int union_type() const { ENSURE_DATA_OUT(); return _union_type; }
	template<class T> const T& get() const { ENSURE_DATA_OUT(); return *(const T*)((char*)this + _dataout); }

private:
	union
	{
		reffed_uni<U> *_datain;	// for in
		int _union_type;		// for out
	};
	union
	{
		int _dummy;				// for in, 用于检测是输入还是输出状态
		int _dataout;			// for out
	};
};


//////////////////////////////////////////////////////////////////////////


template<class T> class vec
{
public:
	typedef reffed_vec<T> refvec;

	vec() : _datain(NULL) FILL_DUMMY() {}
	vec(const vec &that) : _datain(NULL) FILL_DUMMY()
	{
		if (that._datain)
		{
			_datain = that._datain;
			_datain->add();
		}
	}
	~vec()
	{
		if (_datain)
			_datain->release();
	}

	refvec& operator()()
	{
		if (_datain == NULL)
			_datain = new refvec;
		return *_datain;
	}
	refvec* operator->()
	{
		if (_datain == NULL)
			_datain = new refvec;
		return _datain;
	}

	int calc_size() const
	{
		if (_datain == NULL || _datain->empty())
			return 0;
		int header_size = _datain->size() * sizeof(T), body_size = 0;
		if (serialize_helper::need_extra_space((T*)NULL))
		{
			for (size_t i = 0; i < _datain->size(); i++)
				body_size += serialize_helper::calc_size((*_datain)[i]);
		}
		return ALIGN(header_size) + body_size;
	}

	void serialize(serialize_context *context, int var_pos) const
	{
		CHECK_ALIGN(var_pos);
		if (_datain == NULL || _datain->empty())
		{
			context->prealloc_for_container(0, 0, var_pos);
		}
		else
		{
			int header_size = _datain->size() * sizeof(T);
			int p = context->prealloc_for_container(_datain->size(), ALIGN(header_size), var_pos);
			for (size_t i = 0; i < _datain->size(); i++)
				context->serialize_for_container((*_datain)[i], p + i * sizeof(T));
		}
	}

	// for out
	size_t size() const { ENSURE_DATA_OUT(); return _count; }
	const T& operator[](int i) const { ENSURE_DATA_OUT(); return ((const T*)((char*)this + _dataout))[i]; }

private:
	union
	{
		refvec *_datain;	// for in
		int _count;			// for out
	};

	union
	{
		int _dummy;			// for in, 用于检测是输入还是输出状态
		int _dataout;		// for out
	};
};


//////////////////////////////////////////////////////////////////////////


// map序列化的结构，_data指向的内容为：
//  n * sizeof(int) : 存储n个key，如果key是int，直接保存，
//                    如果是string，则保存的是string的offset（相对string自身，而非相对map）
//  n * sizeof(TVal) : 存储n个value，和vec一样，顺序保存
template<class TKeyIn, class TKeyOut, class TKeySize, class TVal> class map
{
public:
	typedef reffed_map<TKeyIn, TVal> refmap;

	map() : _datain(NULL) FILL_DUMMY() {}
	map(const map &that) : _datain(NULL) FILL_DUMMY()
	{
		if (that._datain)
		{
			_datain = that._datain;
			_datain->add();
		}
	}
	~map()
	{
		if (_datain)
			_datain->release();
	}

	refmap& operator()()
	{
		if (_datain == NULL)
			_datain = new refmap;
		return *_datain;
	}
	refmap* operator->()
	{
		if (_datain == NULL)
			_datain = new refmap;
		return _datain;
	}

	int calc_size() const
	{
		if (_datain == NULL)
			return 0;
		int key_header_size = _datain->size() * sizeof(TKeySize);
		int val_header_size = _datain->size() * sizeof(TVal);
		int body_size = 0;
		if (serialize_helper::need_extra_space_for_mapkey((TKeyIn*)NULL) && serialize_helper::need_extra_space((TVal*)NULL))
		{
			for (refmap::const_iterator it = _datain->begin(); it != _datain->end(); it++)
				body_size += serialize_helper::calc_size_for_mapkey(&it->first) + serialize_helper::calc_size(it->second);
		}
		else if (serialize_helper::need_extra_space_for_mapkey((TKeyIn*)NULL))
		{
			for (refmap::const_iterator it = _datain->begin(); it != _datain->end(); it++)
				body_size += serialize_helper::calc_size_for_mapkey(&it->first);
		}
		else if (serialize_helper::need_extra_space((TVal*)NULL))
		{
			for (refmap::const_iterator it = _datain->begin(); it != _datain->end(); it++)
				body_size += serialize_helper::calc_size(it->second);
		}
		return ALIGN(key_header_size) + ALIGN(val_header_size) + body_size;
	}

	void serialize(serialize_context *context, int var_pos) const
	{
		CHECK_ALIGN(var_pos);
		if (_datain == NULL || _datain->empty())
		{
			context->prealloc_for_container(0, 0, var_pos);
		}
		else
		{
			int key_header_size = _datain->size() * sizeof(TKeySize);
			int val_header_size = _datain->size() * sizeof(TVal);
			int p = context->prealloc_for_container(_datain->size(), ALIGN(key_header_size) + ALIGN(val_header_size), var_pos);
			int i = 0;
			for (refmap::iterator it = _datain->begin(); it != _datain->end(); it++, i++)
			{
				context->serialize_for_keymap(&it->first, p + i * sizeof(TKeySize));
				context->serialize_for_container(it->second, p + key_header_size + i * sizeof(TVal));
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// for out

	size_t size() const { ENSURE_DATA_OUT(); return _count; }
	const TVal& get_by_index(int index) const
	{
		ENSURE_DATA_OUT();
		return *((const TVal*)((char*)this + _dataout + _count * sizeof(TKeySize)) + index);
	}
	TKeyOut get_key_by_index(int index) const
	{
		ENSURE_DATA_OUT();
		return get_key_by_index_stub<TKeyOut>(index, NULL);
	}

	const TVal* operator[](TKeyOut k) const
	{
		ENSURE_DATA_OUT();
		int index = get_index_by_key(k);
		if (index < 0)
			return NULL;
		return &get_by_index(index);
	}

private:

	template<class T> TKeyOut get_key_by_index_stub(int index, T deduction) const { return get_int_key_value<TKeyOut>(index); }
	template<> TKeyOut get_key_by_index_stub(int index, const char* deduction) const { return (TKeyOut)get_string_key_value(index); }
	template<> TKeyOut get_key_by_index_stub(int index, const wchar_t* deduction) const { return (TKeyOut)get_string_key_value(index); }

	template<class T> T get_int_key_value(int index) const
	{
		return *(T*)((char*)this + _dataout + index * sizeof(TKeySize));
	}
	void* get_string_key_value(int index) const
	{
		// 字符串的存储位置是相对于字符串位置offset自身的offset，
		// 所以获取字符串地址时要做两次间接寻址
		// 例如map的地址是20，里面有一个字符串偏移量位于map中的8，其值为40
		// 则这个字符串地址是20 + 8 + 40，即20 + 8 + *(20 + 8)
		char *off = (char*)this + _dataout + index * sizeof(TKeySize);
		return off + *(int*)off;
	}

	template<class T> int compare_key(const T &src, int target) const
	{
		if (src < get_int_key_value<T>(target))
			return -1;
		if (src > get_int_key_value<T>(target))
			return 1;
		return 0;
	}
	int compare_key(const char* src, int target) const
	{
		return strcmp(src, (char*)get_string_key_value(target));
	}
	int compare_key(const wchar_t* src, int target) const
	{
		return wcscmp(src, (wchar_t*)get_string_key_value(target));
	}

	int get_index_by_key(TKeyOut k) const
	{
		int num = _count, result;
		int lo = 0, hi = _count - 1;
		while (lo <= hi)
		{
			int half = num / 2;
			if (half != 0)
			{
				int mid = lo + (num & 1 ? half : (half - 1));
				result = compare_key(k, mid);
				if (result == 0)
				{
					return mid;
				}
				else if (result < 0)
				{
					hi = mid - 1;
					num = num & 1 ? half : half - 1;
				}
				else
				{
					lo = mid + 1;
					num = half;
				}
			}
			else if (num)
			{
				return (compare_key(k, lo) ? -1 : lo);
			}
			else
			{
				break;
			}
		}
		return -1;
	}

private:
	union
	{
		refmap *_datain;	// for in
		int _count;			// for out
	};

	union
	{
		int _dummy;			// for in, 用于检测是输入还是输出状态
		int _dataout;		// for out
	};
};


//////////////////////////////////////////////////////////////////////////


class temp_str_buffer
{
public:
	temp_str_buffer() {}
	~temp_str_buffer()
	{
		for (size_t i = 0; i < _data.size(); i++)
			delete _data[i];
	}

	tuomap::string<char> from_string(const tuomap::string<char> &s) { return from_tstring<char>(s); }
	tuomap::string<wchar_t> from_string(const tuomap::string<wchar_t> &s) { return from_tstring<wchar_t>(s); }
	tuomap::binary from_buffer(void* p, int iLen)
	{
		_data.push_back(p);
		return tuomap::binary(p, iLen);
	}

private:

	template<class char_type>
	const tuomap::string<char_type> from_tstring(const tuomap::string<char_type> &s)
	{
		char_type *p = new char_type[s.length() + 1];
		memcpy(p, s.data_in(), s.get_needed_memory_size());
		_data.push_back(p);
		return tuomap::string<char_type>(p, s.length());
	}

	std::vector<void*> _data;
};


};


#define tuomap_offset(var)		((char*)&var - (char*)this)
