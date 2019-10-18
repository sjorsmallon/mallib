


struct Bivector3
{
	float b01 = 0;
	float b02 = 0;
	float b12 = 0;
	
	Bivector3( float b01, float b02, float b12 );
};

Bivector3::Bivector3( float b01, float b02, float b12 )
: b01(b01), b02(b02), b12(b12) {}

// Wedge product
inline Bivector3 Wedge( const Vector3& u, const Vector3& v )
{
	Bivector3 ret(
		u[0]*v[1] - u[1]*v[0], // XY
		u[0]*v[2] - u[2]*v[0], // XZ
		u[1]*v[2] - u[2]*v[1]  // YZ
	);
	return ret;
}


// ---------------------------------------------------------------------
struct Rotor3
{
	// scalar part
	float a = 1;
	// bivector part
	float b01 = 0;
	float b02 = 0;
	float b12 = 0;

	// default ctor
	Rotor3() {}
	Rotor3( float a, float b01, float b02, float b12 );
	Rotor3( float a, const Bivector3& bv );
	
	// construct the rotor that rotates one vector to another
	Rotor3( const Vector3& vFrom, const Vector3& vTo ); 
	// angle+axis, or rather angle+plane
	Rotor3( float angleRadian, const Bivector3& bvPlane );

	// rotate a vector by the rotor
	Vector3 rotate( const Vector3& v ) const;

	// multiply
	Rotor3 operator*( const Rotor3& r ) const;
	Rotor3 operator*=( const Rotor3& r );
	Rotor3 rotate( const Rotor3& r ) const;

	// length utility
	Rotor3 reverse() const; // equivalent to conjugate
	float lengthsqrd() const;
	float length() const;
	void normalize();
	Rotor3 normal() const;
	
	// convert to matrix
	Matrix3 toMatrix3() const;
};

// default ctor
Rotor3::Rotor3( float a, float b01, float b02, float b12 )
: a(a), b01(b01), b02(b02), b12(b12) {}

Rotor3::Rotor3( float a, const Bivector3& bv )
: a(a), b01(bv.b01), b02(bv.b02), b12(bv.b12) {}

// construct the rotor that rotates one vector to another
// uses the usual trick to get the half angle
Rotor3::Rotor3( const Vector3& vFrom, const Vector3& vTo )
{
	a = 1 + Dot( vTo, vFrom );
	// the left side of the products have b a, not a b, so flip
	Bivector3 minusb = Wedge( vTo, vFrom );
	b01 = minusb.b01;
	b02 = minusb.b02;
	b12 = minusb.b12;
	normalize();
}

// angle+plane, plane must be normalized
Rotor3::Rotor3( float angleRadian, const Bivector3& bvPlane )
{
	float sina = sin(angleRadian / 2.f);
	a = cos(angleRadian / 2.f);
	// the left side of the products have b a, not a b
	b01 = -sina * bvPlane.b01;
	b02 = -sina * bvPlane.b02;
	b12 = -sina * bvPlane.b12;
}

// Rotor3-Rotor3 product
// non-optimized
inline Rotor3 Rotor3::operator*( const Rotor3& q ) const
{
	const Rotor3& p = *this;
	Rotor3 r;
	
	// here we just expanded the geometric product rules
	
	
	
	r.a = p.a * q.a 
		- p.b01 * q.b01 - p.b02 * q.b02 - p.b12 * q.b12;

	r.b01 = p.b01 * q.a   + p.a   * q.b01
		  + p.b12 * q.b02 - p.b02 * q.b12;

	r.b02 = p.b02 * q.a   + p.a   * q.b02
		  - p.b12 * q.b01 + p.b01 * q.b12;

	r.b12 = p.b12 * q.a   + p.a   * q.b12
		  + p.b02 * q.b01 - p.b01 * q.b02;

	return r;
}

/// R x R*
// non-optimized
Vector3 Rotor3::rotate( const Vector3& x ) const
{
	const Rotor3& p = *this;
	
	// q = P x
	Vector3 q;
	q[0] = p.a * x[0] + x[1] * p.b01 + x[2] * p.b02;
	q[1] = p.a * x[1] - x[0] * p.b01 + x[2] * p.b12;
	q[2] = p.a * x[2] - x[0] * p.b02 - x[1] * p.b12;

	float q012 = - x[0] * p.b12 + x[1] * p.b02 - x[2] * p.b01; // trivector

	// r = q P*
	Vector3 r;
	r[0] = p.a * q[0] + q[1] * p.b01 + q[2] * p.b02    - q012 * p.b12;
	r[1] = p.a * q[1] - q[0] * p.b01 + q012 * p.b02    + q[2] * p.b12;
	r[2] = p.a * q[2] - q012 * p.b01 - q[0] * p.b02    - q[1] * p.b12;

	return r;
}

// Rotor3-Rotor3 product
inline Rotor3 Rotor3::operator*=( const Rotor3& r )
{
	(*this) = (*this) * r;
	return *this;
}

// rotate a rotor by another
inline Rotor3 Rotor3::rotate( const Rotor3& r ) const
{
	// should unwrap this for efficiency
	return (*this) * r * (*this).reverse();
}


// Equivalent to conjugate
inline Rotor3 Rotor3::reverse() const
{
	return Rotor3( a, -b01, -b02, -b12 );
}
// Length Squared
inline float Rotor3::lengthsqrd() const
{
	return Sqr(a) + Sqr(b01) + Sqr(b02) + Sqr(b12);
}
// Length
inline float Rotor3::length() const
{
	return sqrt( lengthsqrd() );
}
// Normalize
inline void Rotor3::normalize()
{
	float l = length();
	a /= l; b01 /= l; b02 /= l; b12 /= l;
}
// Normalized rotor
inline Rotor3 Rotor3::normal() const
{
	Rotor3 r = *this; 
	r.normalize(); 
	return r;
}

// convert to matrix
// non-optimized
inline Matrix3 Rotor3::toMatrix3() const
{
	Vector3 v0 = rotate( Vector3(1,0,0) );
	Vector3 v1 = rotate( Vector3(0,1,0) );
	Vector3 v2 = rotate( Vector3(0,0,1) );
	return Matrix3( v0, v1, v2 );
}

// geometric product (for reference), produces twice the angle, negative direction
inline Rotor3 Geo( const Vector3 & a, const Vector3 & b )
{
	return Rotor3( Dot(a,b), Wedge(a,b) );
}


 
 
 
 
 
 
 





// Cross product
inline Vector3 Cross( const Vector3& u, const Vector3& v )
{
	Vector3 ret(
		u[1]*v[2] - u[2]*v[1], 
		u[2]*v[0] - u[0]*v[2], 
		u[0]*v[1] - u[1]*v[0]  
	);
	return ret;
}


// ---------------------------------------------------------------------
struct Quaternion
{
	// real part
	float a = 1;
	// imaginary part
	float v0 = 0;
	float v1 = 0;
	float v2 = 0;

	// default ctor
	Quaternion() {}
	Quaternion( float a, float v0, float v1, float v2 );
	
	
	// construct the quaternion that rotates one vector to another
	Quaternion( const Vector3& vFrom, const Vector3& vTo ); 
	// angle+axis, axis must be normalized
	Quaternion( float angleRadian, const Vector3& axis );

	// rotate a vector by the quaternion
	Vector3 rotate( const Vector3& v ) const;

	// multiply
	Quaternion operator*( const Quaternion& q ) const;
	Quaternion operator*=( const Quaternion& q );
	Quaternion rotate( const Quaternion& q ) const;

	// length utility
	Quaternion conjugate() const;
	float lengthsqrd() const;
	float length() const;
	void normalize();
	Quaternion normal() const;
	
	// convert to matrix
	Matrix3 toMatrix3() const;
};

// default ctor
Quaternion::Quaternion( float a, float v0, float v1, float v2 )
: a(a), v0(v0), v1(v1), v2(v2) {}




// construct the quaternion that rotates one vector to another
// uses the usual trick to get the half angle
Quaternion::Quaternion( const Vector3& vFrom, const Vector3& vTo )
{
	a = 1 + Dot( vTo, vFrom );
	
	Vector3 vec = cross( vFrom, vTo );
	v0 = vec[0];
	v1 = vec[1];
	v2 = vec[2];
	normalize();
}

// angle+axis, axis must be normalized
Quaternion::Quaternion( float angleRadian, const Vector3& axis )
{
	float sina = sin(angleRadian / 2.f);
	a = cos(angleRadian / 2.f);
	
	v0 = sina * axis[0];
	v1 = sina * axis[1];
	v2 = sina * axis[2];
}

// Quaternion-Quaternion product
// non-optimized
inline Quaternion Quaternion::operator*( const Quaternion& q ) const
{
	const Quaternion& p = *this;
	Quaternion r;
	
	// Quaternion formulas:
	// r.a = p.a * q.a - Dot(p.v,q.v);
	// r.v = p.a * q.v + q.a * p.v + Cross(p.v,q.v);
	
	r.a = p.a * q.a 
		- p.v0 * q.v0 - p.v1 * q.v1 - p.v2 * q.v2;

	r.v0 = p.a * q.v0 + q.a * p.v0  
		+ p.v1 * q.v2 - p.v2 * q.v1;
	
	r.v1 = p.a * q.v1 + q.a * p.v1  
		+ p.v2 * q.v0 - p.v0 * q.v2;
	
	r.v2 = p.a * q.v2 + q.a * p.v2  
		+ p.v0 * q.v1 - p.v1 * q.v0;

	return r;
}

/// Q (x,0) Q*
// non-optimized
Vector3 Quaternion::rotate( const Vector3& x ) const
{
	const Quaternion& p = *this;
	
	// q = P (x,0)
	Quaternion q;
	q.v0 = p.a * x[0] + p.v1 * x[2] - p.v2 * x[1];
	q.v1 = p.a * x[1] + p.v2 * x[0] - p.v0 * x[2];
	q.v2 = p.a * x[2] + p.v0 * x[1] - p.v1 * x[0];
	
	q.a = - p.v0 * x[0] - p.v1 * x[1] - p.v2 * x[2];

	// r = q P*
	Vector3 r;
	r[0] = q.a * -p.v0 + p.a * q.v0  -  q.v1 * p.v2 + q.v2 * p.v1;
	r[1] = q.a * -p.v1 + p.a * q.v1  -  q.v2 * p.v0 + q.v0 * p.v2;
	r[2] = q.a * -p.v2 + p.a * q.v2  -  q.v0 * p.v1 + q.v1 * p.v0;

	return r;
}

// Quaternion-Quaternion product
inline Quaternion Quaternion::operator*=( const Quaternion& r )
{
	(*this) = (*this) * r;
	return *this;
}

// rotate a rotor by another
inline Quaternion Quaternion::rotate( const Quaternion& r ) const
{
	// should unwrap this for efficiency
	return (*this) * r * (*this).conjugate();
}


// C
inline Quaternion Quaternion::conjugate() const
{
	return Quaternion( a, -v0, -v1, -v2 );
}
// Length Squared
inline float Quaternion::lengthsqrd() const
{
	return Sqr(a) + Sqr(v0) + Sqr(v1) + Sqr(v2);
}
// Length
inline float Quaternion::length() const
{
	return sqrt( lengthsqrd() );
}
// Normalize
inline void Quaternion::normalize()
{
	float l = length();
	a /= l; v0 /= l; v1 /= l; v2 /= l;
}
// Normalized rotor
inline Quaternion Quaternion::normal() const
{
	Quaternion r = *this; 
	r.normalize(); 
	return r;
}

// convert to matrix
// non-optimized
inline Matrix3 Quaternion::toMatrix3() const
{
	Vector3 v0 = rotate( Vector3(1,0,0) );
	Vector3 v1 = rotate( Vector3(0,1,0) );
	Vector3 v2 = rotate( Vector3(0,0,1) );
	return Matrix3( v0, v1, v2 );
}


