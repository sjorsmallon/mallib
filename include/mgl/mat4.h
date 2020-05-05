#ifndef INCLUDED_mat4_
#define INCLUDED_mat4_

// COLUMN MAJOR

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


// operator*
inline mgl::mat4 operator*(mgl::mat4& lhs, const mgl::mat4& rhs)
{

    mgl::mat4 result{};
    float* result_ptr = reinterpret_cast<float *>(&result);

    // row major
    // for (int col_idx = 0; col_idx < 4; ++col_idx )
    // {
    //     for (int row_idx = 0; row_idx < 4; ++row_idx)
    //     {
    //         *result_ptr = lhs_ptr[0] * rhs_ptr[ 0 * 4 + row_idx]
    //         + lhs_ptr[1] * rhs_ptr[ 1 * 4 + j ]
    //         + lhs_ptr[2] * rhs_ptr[ 2 * 4 + j ]
    //         + lhs_ptr[3] * rhs_ptr[ 3 * 4 + j ];
    //         result_ptr++;
    //     }
    //     lhs_ptr += 4;
    // }

    // column major
    for (int rhs_col_idx = 0; rhs_col_idx != 4; ++rhs_col_idx)
    {
        for (int row_idx = 0; row_idx != 4; ++row_idx)
        {
            *result_ptr = lhs[0][row_idx] * rhs[rhs_col_idx][0]
            + lhs[1][row_idx] * rhs[rhs_col_idx][1]
            + lhs[2][row_idx] * rhs[rhs_col_idx][2]
            + lhs[3][row_idx] * rhs[rhs_col_idx][3];
            result_ptr++;
        }
    }

    return result;

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