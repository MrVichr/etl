///******************************************************************************
//The MIT License(MIT)
//
//Embedded Template Library.
//https://github.com/ETLCPP/etl
//https://www.etlcpp.com
//
//Copyright(c) 2021 jwellbelove
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files(the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions :
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.
//******************************************************************************/
//
//#include "unit_test_framework.h"
//
//#include "etl/vector.h"
//#include "etl/multi_vector.h"
//
//#include <array>
//#include <vector>
//#include <algorithm>
//#include <iterator>
//
//#include "etl/integral_limits.h"
//
//namespace
//{
//  static const size_t SIZE1 = 4UL;
//  static const size_t SIZE2 = 3UL;
//
//  using Data = etl::multi_vector<int, SIZE1, SIZE2>;
//  using Compare_Data = std::vector<std::vector<int>>;
//  using Array = std::array<std::array<int, SIZE2>, SIZE1>;
//
//  void Copy(const Array& from, Data& to)
//  {
//    to.resize(SIZE1);
//
//    for (size_t i = 0U; i < SIZE1; ++i)
//    {
//      to[i].resize(SIZE2);
//
//      for (size_t j = 0U; j < SIZE2; ++j)
//      {
//        to[i][j] = from[i][j];
//      }
//    }
//  }
//
//  Compare_Data compare_data = { { { 0, 1, 2 }, { 3, 4, 5 }, { 6, 7, 8 }, { 9, 10, 11 } } };
//
//  SUITE(test_vector)
//  {
//    //*************************************************************************
//    TEST(test_constructor)
//    {
//      Array initial = { { { 0, 1, 2 }, { 3, 4, 5 }, { 6, 7, 8 }, { 9, 10, 11 } } };
//      Data data;
//      Copy(initial, data);
//
//      CHECK_EQUAL(data.size(),     SIZE1);
//      CHECK_EQUAL(data.max_size(), SIZE1);
//
//      CHECK_EQUAL(data[0].size(),     SIZE2);
//      CHECK_EQUAL(data[0].max_size(), SIZE2);
//    }
//
//    //*************************************************************************
//    TEST(test_assignment)
//    {
//      Array initial = { { { 0, 1, 2 }, { 3, 4, 5 }, { 6, 7, 8 }, { 9, 10, 11 } } };
//      Data data;
//      Data other_data;
//      Copy(initial, data);
//
//      other_data = data;
//
//      bool isEqual = std::equal(data.begin(), data.end(), other_data.begin());
//
//      CHECK(isEqual);
//    }
//
//    //*************************************************************************
//    TEST(test_at)
//    {
//      Array initial = { { { 0, 1, 2 }, { 3, 4, 5 }, { 6, 7, 8 }, { 9, 10, 11 } } };
//      Data data;
//      Copy(initial, data);
//
//      for (size_t i = 0UL; i < data.size(); ++i)
//      {
//        for (size_t j = 0UL; j < data[0].size(); ++j)
//        {
//          CHECK_EQUAL(data[i].at(j), compare_data[i].at(j));
//        }
//      }
//
//      CHECK_THROW(data.at(data.size()), etl::vector_out_of_bounds);
//      CHECK_THROW(data[0].at(data[0].size()), etl::vector_out_of_bounds);
//    }
//
//    //*************************************************************************
//    TEST(test_at_const)
//    {
//      const Array initial = { { { 0, 1, 2 }, { 3, 4, 5 }, { 6, 7, 8 }, { 9, 10, 11 } } };
//      Data data;
//      Copy(initial, data);
//
//      for (size_t i = 0UL; i < data.size(); ++i)
//      {
//        for (size_t j = 0UL; j < data[0].size(); ++j)
//        {
//          CHECK_EQUAL(data[i].at(j), compare_data[i].at(j));
//        }
//
//        CHECK_THROW(data.at(data.size()), etl::vector_out_of_bounds);
//      }
//    }
//
//    //*************************************************************************
//    TEST(test_index_operator)
//    {
//      Array initial = { { { 0, 1, 2 }, { 3, 4, 5 }, { 6, 7, 8 }, { 9, 10, 11 } } };
//      Data data;
//      Copy(initial, data);
//
//      for (size_t i = 0UL; i < data.size(); ++i)
//      {
//        for (size_t j = 0UL; j < data[0].size(); ++j)
//        {
//          CHECK_EQUAL(data[i][j], compare_data[i][j]);
//        }
//      }
//    }
//
//    //*************************************************************************
//    TEST(test_index_operator_const)
//    {
//      const Array initial = { { { 0, 1, 2 }, { 3, 4, 5 }, { 6, 7, 8 }, { 9, 10, 11 } } };
//      Data data;
//      Copy(initial, data);
//
//      for (size_t i = 0UL; i < data.size(); ++i)
//      {
//        for (size_t j = 0UL; j < data[0].size(); ++j)
//        {
//          CHECK_EQUAL(data[i][j], compare_data[i][j]);
//        }
//      }
//    }
//
//    //*************************************************************************
//    TEST(test_front)
//    {
//      Array initial = { { { 0, 1, 2 }, { 3, 4, 5 }, { 6, 7, 8 }, { 9, 10, 11 } } };
//      Data data;
//      Copy(initial, data);
//
//      int& ref = data.front().front();
//      CHECK(ref == compare_data.front().front());
//    }
//
//    //*************************************************************************
//    TEST(test_front_const)
//    {
//      const Array initial = { { { 0, 1, 2 }, { 3, 4, 5 }, { 6, 7, 8 }, { 9, 10, 11 } } };
//      Data data;
//      Copy(initial, data);
//
//      const int& ref = data.front().front();
//      CHECK(ref == compare_data.front().front());
//    }
//
//    //*************************************************************************
//    TEST(test_back)
//    {
//      Array initial = { { { 0, 1, 2 }, { 3, 4, 5 }, { 6, 7, 8 }, { 9, 10, 11 } } };
//      Data data;
//      Copy(initial, data);
//
//      int& ref = data.back().back();
//      CHECK(ref == compare_data.back().back());
//    }
//
//    //*************************************************************************
//    TEST(test_back_const)
//    {
//      const Array initial = { { { 0, 1, 2 }, { 3, 4, 5 }, { 6, 7, 8 }, { 9, 10, 11 } } };
//      Data data;
//      Copy(initial, data);
//
//      const int& ref = data.back().back();
//      CHECK(ref == compare_data.back().back());
//    }
//
//    //*************************************************************************
//    TEST(test_data)
//    {
//      Array initial = { { { 0, 1, 2 }, { 3, 4, 5 }, { 6, 7, 8 }, { 9, 10, 11 } } };
//      Data data;
//      Copy(initial, data);
//
//      bool isEqual = std::equal(data.begin(), data.end(), data.data());
//
//      CHECK(isEqual);
//    }
//
//    //*************************************************************************
//    TEST(test_data_const)
//    {
//      const Array initial = { { { 0, 1, 2 }, { 3, 4, 5 }, { 6, 7, 8 }, { 9, 10, 11 } } };
//      Data data;
//      Copy(initial, data);
//
//      bool isEqual = std::equal(data.begin(), data.end(), data.data());
//
//      CHECK(isEqual);
//    }
//
//    //*************************************************************************
//    TEST(test_begin)
//    {
//      Array initial = { { { 0, 1, 2 }, { 3, 4, 5 }, { 6, 7, 8 }, { 9, 10, 11 } } };
//      Data data;
//      Copy(initial, data);
//
//      CHECK(data.begin() == &data[0]);
//      CHECK(data[0].begin() == &data[0][0]);
//      CHECK(data[1].begin() == &data[1][0]);
//      CHECK(data[2].begin() == &data[2][0]);
//      CHECK(data[3].begin() == &data[3][0]);
//    }
//
//    //*************************************************************************
//    TEST(test_end)
//    {
//      Array initial = { { { 0, 1, 2 }, { 3, 4, 5 }, { 6, 7, 8 }, { 9, 10, 11 } } };
//      Data data;
//      Copy(initial, data);
//
//      CHECK(data.end() == &data[SIZE1]);
//      CHECK(data[0].end() == &data[0][SIZE2]);
//      CHECK(data[1].end() == &data[1][SIZE2]);
//      CHECK(data[2].end() == &data[2][SIZE2]);
//      CHECK(data[3].end() == &data[3][SIZE2]);
//    }
//
//    //*************************************************************************
//    TEST(test_cbegin)
//    {
//      const Array initial = { { { 0, 1, 2 }, { 3, 4, 5 }, { 6, 7, 8 }, { 9, 10, 11 } } };
//      Data data;
//      Copy(initial, data);
//
//      CHECK(data.cbegin() == &data[0]);
//      CHECK(data[0].cbegin() == &data[0][0]);
//      CHECK(data[1].cbegin() == &data[1][0]);
//      CHECK(data[2].cbegin() == &data[2][0]);
//      CHECK(data[3].cbegin() == &data[3][0]);
//    }
//
//    //*************************************************************************
//    TEST(test_cend)
//    {
//      const Array initial = { { { 0, 1, 2 }, { 3, 4, 5 }, { 6, 7, 8 }, { 9, 10, 11 } } };
//      Data data;
//      Copy(initial, data);
//
//      CHECK(data.cend() == &data[SIZE1]);
//      CHECK(data[0].cend() == &data[0][SIZE2]);
//      CHECK(data[1].cend() == &data[1][SIZE2]);
//      CHECK(data[2].cend() == &data[2][SIZE2]);
//      CHECK(data[3].cend() == &data[3][SIZE2]);
//    }
//
//    //*************************************************************************
//    TEST(test_rbegin)
//    {
//      Array initial = { { { 0, 1, 2 }, { 3, 4, 5 }, { 6, 7, 8 }, { 9, 10, 11 } } };
//      Data data;
//      Copy(initial, data);
//
//      typedef etl::multi_vector<int, SIZE2> Inner;
//
//      CHECK(data.rbegin() == Data::reverse_iterator(&data[SIZE1]));
//      CHECK(data[0].rbegin() == Inner::reverse_iterator(&data[0][SIZE2]));
//      CHECK(data[1].rbegin() == Inner::reverse_iterator(&data[1][SIZE2]));
//      CHECK(data[2].rbegin() == Inner::reverse_iterator(&data[2][SIZE2]));
//      CHECK(data[3].rbegin() == Inner::reverse_iterator(&data[3][SIZE2]));
//    }
//
//    //*************************************************************************
//    TEST(test_rend)
//    {
//      Array initial = { { { 0, 1, 2 }, { 3, 4, 5 }, { 6, 7, 8 }, { 9, 10, 11 } } };
//      Data data;
//      Copy(initial, data);
//
//      typedef etl::multi_vector<int, SIZE2> Inner;
//
//      CHECK(data.rend() == Data::reverse_iterator(&data[0]));
//      CHECK(data[0].rend() == Inner::reverse_iterator(&data[0][0]));
//      CHECK(data[1].rend() == Inner::reverse_iterator(&data[1][0]));
//      CHECK(data[2].rend() == Inner::reverse_iterator(&data[2][0]));
//      CHECK(data[3].rend() == Inner::reverse_iterator(&data[3][0]));
//    }
//
//    //*************************************************************************
//    TEST(test_crbegin)
//    {
//      const Array initial = { { { 0, 1, 2 }, { 3, 4, 5 }, { 6, 7, 8 }, { 9, 10, 11 } } };
//      Data data;
//      Copy(initial, data);
//
//      typedef etl::multi_vector<int, SIZE2> Inner;
//
//      CHECK(data.rbegin() == Data::const_reverse_iterator(&data[SIZE1]));
//      CHECK(data[0].rbegin() == Inner::const_reverse_iterator(&data[0][SIZE2]));
//      CHECK(data[1].rbegin() == Inner::const_reverse_iterator(&data[1][SIZE2]));
//      CHECK(data[2].rbegin() == Inner::const_reverse_iterator(&data[2][SIZE2]));
//      CHECK(data[3].rbegin() == Inner::const_reverse_iterator(&data[3][SIZE2]));
//    }
//
//    //*************************************************************************
//    TEST(test_crend)
//    {
//      const Array initial = { { { 0, 1, 2 }, { 3, 4, 5 }, { 6, 7, 8 }, { 9, 10, 11 } } };
//      Data data;
//      Copy(initial, data);
//
//      typedef etl::multi_vector<int, SIZE2> Inner;
//
//      CHECK(data.rend() == Data::const_reverse_iterator(&data[0]));
//      CHECK(data[0].rend() == Inner::const_reverse_iterator(&data[0][0]));
//      CHECK(data[1].rend() == Inner::const_reverse_iterator(&data[1][0]));
//      CHECK(data[2].rend() == Inner::const_reverse_iterator(&data[2][0]));
//      CHECK(data[3].rend() == Inner::const_reverse_iterator(&data[3][0]));
//    }
//
//    //*************************************************************************
//    TEST(test_empty)
//    {
//      Data data;
//
//      CHECK(data.empty());
//      CHECK(data[0].empty());
//    }
//
//    //*************************************************************************
//    TEST(test_size)
//    {
//      Data data;
//
//      CHECK_EQUAL(0, data.size());
//      CHECK_EQUAL(0, data[0].size());
//    }
//
//    //*************************************************************************
//    TEST(test_max_size)
//    {
//      Data data;
//
//      data.resize(1);
//
//      CHECK_EQUAL(SIZE1, data.max_size());
//      CHECK_EQUAL(SIZE2, data[0].max_size());
//    }
//
//    //*************************************************************************
//    TEST(test_swap)
//    {
//      Array initial1 = { { { 0, 1, 2 }, { 3, 4, 5 }, { 6, 7, 8 }, { 9, 10, 11 } } };
//      Array initial2 = { { { 12, 13, 14 }, { 15, 16, 17 }, { 18, 19, 20 }, { 21, 22, 23 } } };
//
//      Array initial1b = { { { 12, 13, 14 }, { 15, 16, 17 }, { 18, 19, 20 }, { 21, 22, 23 } } };
//      Array initial2b = { { { 0, 1, 2 }, { 3, 4, 5 }, { 6, 7, 8 }, { 9, 10, 11 } } };
//
//      using ETL_OR_STD::swap;
//      swap(data1, data2);
//
//      CHECK(std::equal(data1b.begin(), data1b.end(), data1.begin()));
//      CHECK(std::equal(data2b.begin(), data2b.end(), data2.begin()));
//    }
//
//    //*************************************************************************
//    TEST(test_assign)
//    {
//      Data initial = { { { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } } };
//      Data source  = { { { 0, 1, 2 }, { 3, 4, 5 }, { 6, 7, 8 }, { 9, 10, 11 } } };
//      Data check1  = { { { 0, 1, 2 }, { 3, 4, 5 }, { -1, -1, -1 }, { -1, -1, -1 } } };
//      Data check2  = { { { 0, 1, 2 }, { 3, 4, 5 }, { 99, 99, 99 }, { 99, 99, 99 } } };
//
//      typedef etl::multi_vector<int, SIZE2> Inner;
//      const Inner ninetynine = { { 99, 99, 99 } };
//
//      Data data;
//
//      // Initial data.
//      data.assign(std::begin(initial), std::end(initial));
//      bool isEqual = std::equal(data.begin(), data.end(), std::begin(initial));
//      CHECK(isEqual);
//
//      // Assign smaller.
//      data.assign(std::begin(initial), std::end(initial));
//      data.assign(&source[0], &source[2]);
//      isEqual = std::equal(data.begin(), data.end(), std::begin(check1));
//      CHECK(isEqual);
//    }
//
//    //*************************************************************************
//    TEST(test_insert_value)
//    {
//      Data initial = { { { 0, 1, 2 }, { 3, 4, 5 }, { 6, 7, 8 } } };
//      Data check1  = { { { 9, 10, 11 }, { 0, 1, 2 }, { 3, 4, 5 }, { 6, 7, 8 } } };
//      Data check2  = { { { 0, 1, 2 }, { 9, 10, 11 }, { 3, 4, 5 }, { 6, 7, 8 } } };
//      Data check3  = { { { 0, 1, 2 }, { 3, 4, 5 }, { 6, 7, 8 }, { 9, 10, 11 } } };
//
//      typedef etl::multi_vector<int, SIZE2> Inner;
//      const Inner inserted = { { 9, 10, 11 } };
//
//      Data data;
//      Data::iterator result;
//
//      // Insert beginning.
//      data.assign(std::begin(initial), std::end(initial));
//      result = data.insert(data.begin(), inserted);
//      CHECK(data[0] == *result);
//      bool isEqual = std::equal(data.begin(), data.end(), check1.begin());
//      CHECK(isEqual);
//
//      // Insert middle.
//      data.assign(std::begin(initial), std::end(initial));
//      result = data.insert(data.begin() + 1U, inserted);
//      CHECK(data[1] == *result);
//      isEqual = std::equal(data.begin(), data.end(), check2.begin());
//      CHECK(isEqual);
//
//      // Insert end.
//      data.assign(std::begin(initial), std::end(initial));
//      result = data.insert(data.begin() + 3U, inserted);
//      CHECK(data[3] == *result);
//      isEqual = std::equal(data.begin(), data.end(), check3.begin());
//      CHECK(isEqual);
//    }
//
//    //*************************************************************************
//    TEST(test_equal)
//    {
//      Array initial1 = { { { 0, 1, 2 }, { 3, 4, 5 }, { 6, 7, 8 }, { 9, 10, 11 } } };
//      Array initial2 = { { { 0, 1, 2 }, { 3, 4, 5 }, { 6, 7, 8 }, { 9, 10, 11 } } };
//
//      CHECK(data1 == data2);
//    }
//
//    //*************************************************************************
//    TEST(test_not_equal)
//    {
//      Array initial1 = { { { 0, 1, 2 }, { 3, 4, 5 }, { 6, 7, 8 }, { 9, 10, 11 } } };
//      Array initial2 = { { { 0, 1, 2 }, { 3, 4, 6 }, { 6, 7, 8 }, { 9, 10, 11 } } };
//
//      CHECK(data1 != data2);
//    }
//
//    //*************************************************************************
//    TEST(test_less_than)
//    {
//      Data data    = { { { 0, 1, 2 }, { 3, 4, 5 }, { 6, 7, 8 }, { 9, 10, 11 } } };
//      Data lesser  = { { { 0, 1, 2 }, { 3, 4, 4 }, { 6, 7, 8 }, { 9, 10, 11 } } };
//      Data greater = { { { 0, 1, 2 }, { 3, 4, 6 }, { 6, 7, 8 }, { 9, 10, 11 } } };
//
//      CHECK(lesser    < data);
//      CHECK(!(data    < data));
//      CHECK(!(greater < data));
//    }
//
//    //*************************************************************************
//    TEST(test_less_than_equal)
//    {
//      Data data    = { { { 0, 1, 2 }, { 3, 4, 5 }, { 6, 7, 8 }, { 9, 10, 11 } } };
//      Data lesser  = { { { 0, 1, 2 }, { 3, 4, 4 }, { 6, 7, 8 }, { 9, 10, 11 } } };
//      Data greater = { { { 0, 1, 2 }, { 3, 4, 6 }, { 6, 7, 8 }, { 9, 10, 11 } } };
//
//      CHECK(lesser    <= data);
//      CHECK(data      <= data);
//      CHECK(!(greater <= data));
//    }
//
//    //*************************************************************************
//    TEST(test_greater_than)
//    {
//      Data data    = { { { 0, 1, 2 }, { 3, 4, 5 }, { 6, 7, 8 }, { 9, 10, 11 } } };
//      Data lesser  = { { { 0, 1, 2 }, { 3, 4, 4 }, { 6, 7, 8 }, { 9, 10, 11 } } };
//      Data greater = { { { 0, 1, 2 }, { 3, 4, 6 }, { 6, 7, 8 }, { 9, 10, 11 } } };
//
//      CHECK(greater  > data);
//      CHECK(!(data   > data));
//      CHECK(!(lesser > data));
//    }
//
//    //*************************************************************************
//    TEST(test_greater_than_equal)
//    {
//      Data data    = { { { 0, 1, 2 }, { 3, 4, 5 }, { 6, 7, 8 }, { 9, 10, 11 } } };
//      Data lesser  = { { { 0, 1, 2 }, { 3, 4, 4 }, { 6, 7, 8 }, { 9, 10, 11 } } };
//      Data greater = { { { 0, 1, 2 }, { 3, 4, 6 }, { 6, 7, 8 }, { 9, 10, 11 } } };
//
//      CHECK(greater  >= data);
//      CHECK(data     >= data);
//      CHECK(!(lesser >= data));
//    }
//  };
//}
