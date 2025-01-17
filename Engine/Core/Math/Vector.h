#pragma once

namespace z {
namespace math {

template<typename T>
class TVector4;

// Vector2
template <typename T>
class TVector2 {
public:
	// ctor
	TVector2() : x(0), y(0){}
	TVector2(T v) : TVector2(v, v) {}
	TVector2(T _x, T _y) : x(_x), y(_y) {}
	TVector2(const TVector2& v) { *this = v; }

	// operator
	TVector2 operator- () const { return TVector2(-x, -y); }
	TVector2 operator+ (const TVector2& v2) const { return TVector2(x + v2.x, y + v2.y); }
	TVector2 operator- (const TVector2& v2) const { return TVector2(x - v2.x, y - v2.y); }
	TVector2 operator* (const TVector2& v2) const { return TVector2(x * v2.x, y * v2.y); }
	TVector2 operator/ (const TVector2& v2) const { return TVector2(x / v2.x, y / v2.y); }
	TVector2 operator* (T v2) const { return *this * TVector2(v2); }
	TVector2 operator/ (T v2) const { return *this / TVector2(v2); }

	TVector2& operator += (TVector2 v2) { *this = *this + v2; return *this; }
	TVector2& operator -= (TVector2 v2) { *this = *this - v2; return *this; }
	TVector2& operator *= (TVector2 v2) { *this = *this * v2; return *this; }
	TVector2& operator /= (TVector2 v2) { *this = *this / v2; return *this; }

	friend TVector2 operator* (T v1, TVector2 v2) { return TVector2(v1) * v2; }
	friend TVector2 operator/ (T v1, TVector2 v2) { return TVector2(v1) / v2; }

	T& operator [](int idx) { return value[idx]; }
	const T operator [](int idx) const { return value[idx]; }

	// data
	union {
		struct { T x, y; };
		T value[2];
	};

	const static TVector2 Zero;
	const static TVector2 Identity;
};

template<typename T> const TVector2<T> TVector2<T>::Zero(0, 0);
template<typename T> const TVector2<T> TVector2<T>::Identity(1, 1);

typedef TVector2<float> Vector2;
typedef TVector2<float> Vector2F;
typedef TVector2<int> Vector2I;


// Vector3
template <typename T>
class TVector3 {
public:
	// ctor
	TVector3() : x{0}, y{0}, z{0} {}
	TVector3(T v) : TVector3(v, v, v) {}
	TVector3(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}
	TVector3(const TVector3& v) { *this = v; }

	// operator
	TVector3 operator- () const { return TVector3(-x, -y, -z); }
	TVector3 operator+ (const TVector3 &v2) const { return TVector3(x + v2.x, y + v2.y, z + v2.z); }
	TVector3 operator- (const TVector3 &v2) const { return TVector3(x - v2.x, y - v2.y, z - v2.z); }
	TVector3 operator* (const TVector3 &v2) const { return TVector3(x * v2.x, y * v2.y, z * v2.z); }
	TVector3 operator/ (const TVector3 &v2) const { return TVector3(x / v2.x, y / v2.y, z / v2.z); }
	TVector3 operator* (T v2) const { return *this * TVector3(v2); }
	TVector3 operator/ (T v2) const { return *this / TVector3(v2); }

	TVector3& operator += (TVector3 v2) { *this = *this + v2; return *this; }
	TVector3& operator -= (TVector3 v2) { *this = *this - v2; return *this; }
	TVector3& operator *= (TVector3 v2) { *this = *this * v2; return *this; }
	TVector3& operator /= (TVector3 v2) { *this = *this / v2; return *this; }

	friend TVector3 operator* (T v1, TVector3 v2) { return TVector3(v1) * v2; }
	friend TVector3 operator/ (T v1, TVector3 v2) { return TVector3(v1) / v2; }

	T& operator [](int idx) { return value[idx]; }
	const T operator [](int idx) const { return value[idx]; }

	friend std::ostream& operator<<(std::ostream& out, const TVector3& v) {
		out << "(" << v.x << ", " << v.y << ", " << v.z << ")";
		return out;
	}
	 
	// data
	union {
		struct { T x, y, z; };
		T value[3];
	};

	const static TVector3 Zero;
	const static TVector3 Identity;
};

template<typename T> const TVector3<T> TVector3<T>::Zero(0, 0, 0);
template<typename T> const TVector3<T> TVector3<T>::Identity(1, 1, 1);

typedef TVector3<float> Vector3;
typedef TVector3<float> Vector3F;
typedef TVector3<int> Vector3I;


// Vector4
template <typename T>
class TVector4 {
public:
	// ctor
	TVector4() {}
	TVector4(T _x, T _y, T _z, T _w) : x(_x), y(_y), z(_z), w(_w) {}
	TVector4(T v) : TVector4(v, v, v, v) {}
	TVector4(const TVector4& v) { *this = v; }
	TVector4(const TVector3<T>& v, T _w) : TVector4(v.x, v.y, v.z, _w) {}

	// operator
	TVector4 operator- () const { return TVector4(-x, -y, -z. - w); }
	TVector4 operator+ (const TVector4& v2) const { return Vector4(x + v2.x, y + v2.y, z + v2.z, w + v2.w); }
	TVector4 operator- (const TVector4& v2) const { return Vector4(x - v2.x, y - v2.y, z - v2.z, w - v2.w); }
	TVector4 operator* (const TVector4& v2) const { return Vector4(x * v2.x, y * v2.y, z * v2.z, w * v2.w); }
	TVector4 operator/ (const TVector4& v2) const { return Vector4(x / v2.x, y / v2.y, z / v2.z, w / v2.w); }
	TVector4 operator* (T v2) const { return *this * Vector4(v2); }
	TVector4 operator/ (T v2) const { return *this / Vector4(v2); }

	TVector4& operator += (const TVector4& v2) { *this = *this + v2; return *this; }
	TVector4& operator -= (const TVector4& v2) { *this = *this - v2; return *this; }
	TVector4& operator *= (const TVector4& v2) { *this = *this * v2; return *this; }
	TVector4& operator /= (const TVector4& v2) { *this = *this / v2; return *this; }

	friend TVector4 operator* (T v1, const TVector4& v2) { return TVector4(v1) * v2; }
	friend TVector4 operator/ (T v1, const TVector4& v2) { return TVector4(v1) / v2; }

	T& operator [](int idx) { return value[idx]; }
	const T operator [](int idx) const { return value[idx]; }
	
	operator TVector3<T>() { return TVector3<T>{x, y, z}; }

	friend std::ostream& operator<<(std::ostream& out, const TVector4& v) {
		out << "(" << v.x << ", " << v.y << ", " <<  v.z << ", " << v.w << ")";
		return out;
	}

	// data
	union {
		struct { T x, y, z, w; };
		T value[4];
	};

	const static TVector4 Zero;
	const static TVector4 Identity;
};

template<typename T>
const TVector4<T> TVector4<T>::Zero(0, 0, 0, 0);

template<typename T>
const TVector4<T> TVector4<T>::Identity(1, 1, 1, 1);

typedef TVector4<float> Vector4;
typedef TVector4<float> Vector4F;
typedef TVector4<int> Vector4I;

}
}