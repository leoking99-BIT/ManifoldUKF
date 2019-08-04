#pragma once

#include <iostream>

#include <yaml-cpp/yaml.h>
#include <Eigen/Dense>

namespace YAML
{
template <typename _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows, int _MaxCols>
struct convert<Eigen::Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols>>
{
    static Node encode(
        const Eigen::Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols>& matrix)
    {
        Node node(NodeType::Sequence);

        int nValues = matrix.rows() * matrix.cols();

        for (int i = 0; i < nValues; ++i)
        {
            node.push_back(matrix(i / matrix.cols(), i % matrix.cols()));
        }

        return node;
    }

    static bool decode(const Node& node,
        Eigen::Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols>& matrix)
    {
        // Totally Dynamic sized arrays are not supported
        if (_Rows == Eigen::Dynamic && _Cols == Eigen::Dynamic)
        {
            std::cerr << "Double Dynamic Matrices are not supported! Matrices may only be dynamic "
                         "in one dimension!"
                      << std::endl;
            return false;
        }

        int nSize = node.size();  // Sequence check is implicit

        // If one dimension is dynamic -> calculate and resize

        // Rows Dynamic
        if (_Rows == Eigen::Dynamic)
        {
            // Check
            if (nSize % _Cols != 0)
            {
                std::cerr << "Inputsize " << nSize
                          << " of dynamic row matrix is not a multiple of fixed column size "
                          << _Cols << std::endl;
                return false;
            }

            int nDynRows = nSize / _Cols;
            matrix.resize(nDynRows, Eigen::NoChange);
        }

        // Cols Dynamic
        if (_Cols == Eigen::Dynamic)
        {
            // Check
            if (nSize % _Rows != 0)
            {
                std::cerr << "Inputsize " << nSize
                          << " of dynamic column matrix is not a multiple of fixed row size "
                          << _Rows << std::endl;
                return false;
            }

            int nDynCols = nSize / _Rows;
            matrix.resize(Eigen::NoChange, nDynCols);
        }

        if (nSize != matrix.rows() * matrix.cols())
        {
            std::cerr << "Unable to create Eigen::Matrix. Input is of wrong size! Matrix("
                      << matrix.rows() << "x" << matrix.cols() << ") != Tokens(" << nSize << ")"
                      << std::endl;
            ;
            return false;
        }
        else
        {
            // fill
            for (int i = 0; i < matrix.rows(); i++)
            {
                for (int j = 0; j < matrix.cols(); j++)
                {
                    // ROS_INFO_STREAM("Node pos: " << i*matrix.rows() + j << "Matrix: " << i << ","
                    // << j);
                    //                                      YAML::Node field = node[i +
                    //                                      j*matrix.cols()];
                    //                                      matrix(i,j) = field.as<_Scalar>();

                    matrix(i, j) = node[(int)(i * matrix.cols() + j)].as<_Scalar>();
                }
            }
        }
        return true;
    }
};
}