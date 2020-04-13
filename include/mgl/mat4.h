#ifndef INCLUDED_mat4_
#define INCLUDED_mat4_

// ROW MAJOR. otherwise my brain breaks w.r.t. initialization.

namespace mgl 
{
    struct mat4
    {
        float data[4][4]; // we can always change this. 

        float *operator[](size_t idx)
        {
        	//assert(idx < 4)
        	return data[idx];
        }
        float const *operator[](size_t idx) const
        {
            return data[idx];
        }
    };
};


// free mat4 functions.
//@Todo: is this matmul row major?
inline mgl::mat4 operator*(mgl::mat4& lhs, const mgl::mat4& rhs)
{
    const float *m1Ptr, *m2Ptr;
    float *dstPtr;
    mgl::mat4 dst;

    m1Ptr = reinterpret_cast<const float *>(&lhs);
    m2Ptr = reinterpret_cast<const float *>(&rhs);
    dstPtr = reinterpret_cast<float *>(&dst);

    for (int i = 0; i < 4; i++ ) {
        for (int j = 0; j < 4; j++ ) {
            *dstPtr = m1Ptr[0] * m2Ptr[ 0 * 4 + j ]
            + m1Ptr[1] * m2Ptr[ 1 * 4 + j ]
            + m1Ptr[2] * m2Ptr[ 2 * 4 + j ]
            + m1Ptr[3] * m2Ptr[ 3 * 4 + j ];
            dstPtr++;
        }
        m1Ptr += 4;
    }
    return dst;

    // mat4 result = {};
    // for (int row = 0; row != 4; ++row)
    // {
    //     for (int col = 0; col != 4; ++col)
    //     {
            
    //         result[row][col] = lhs[row][col] * rhs[col][row]
    //     }
    // }
    // result[0][0] = lhs[0][0] * lhs[0][0] + lhs[0][1] * rhs[1][0]
    // result[1][0] = lhs[1][0] * rhs[1][0]

}

inline mgl::mat4& operator*=(mgl::mat4& lhs, const float factor)
{
    const size_t matrix_dim = 4;

    for (size_t row = 0; row != matrix_dim; ++row)
    {
        for (size_t col = 0; col != matrix_dim; ++row)
        {
            lhs[row][col] *= factor;
        }
    }

    
    return lhs;
}

inline mgl::mat4& operator*=(mgl::mat4& lhs, mgl:: mat4& rhs)
{
    lhs = lhs * rhs;
    return lhs;
}


#endif