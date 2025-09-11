
//
// Created by DanaNatov on 2025-09-10.
//

#ifndef COOLINGBLOCK_LOOKUPTABLE_H
#define COOLINGBLOCK_LOOKUPTABLE_H

#include <array>
#include <algorithm>
#include <stdexcept>

namespace LowLevelEmbedded::Utility {

template <typename T, typename Y, size_t Size>
class LookupTable
{
private:
  bool _allowExtrapolation;

  // Add the struct for lookup table data
  struct DataPoint
  {
    T x;
    Y y;

    // Constructor for easy initialization
    constexpr DataPoint() : x(T()), y(Y()) {}
    constexpr DataPoint(T xValue, Y yValue) : x(xValue), y(yValue) {}
  };

  // Fixed-size array for data points
  std::array<DataPoint, Size> _data;

public:
  // Constructor
  constexpr LookupTable(const std::array<DataPoint, Size>& data, bool allowExtrapolation = false)
      : _data(data), _allowExtrapolation(allowExtrapolation) {}

  // Constructor that takes separate arrays for x and y values
  template <size_t InputSize>
  constexpr LookupTable(const std::array<T, InputSize>& xValues,
                        const std::array<Y, InputSize>& yValues,
                        bool allowExtrapolation = false)
      : _allowExtrapolation(allowExtrapolation)
  {
    static_assert(InputSize <= Size, "Input arrays must not be larger than the table size");

    // Copy the input values
    for (size_t i = 0; i < InputSize; ++i) {
      _data[i] = DataPoint(xValues[i], yValues[i]);
    }

    // If InputSize < Size, we need to generate additional points through interpolation
    if (InputSize < Size && InputSize > 1) {
      // Calculate how many additional points we need between each pair of input points
      size_t totalGaps = InputSize - 1;
      size_t additionalPointsPerGap = (Size - InputSize) / totalGaps;
      size_t remainingPoints = (Size - InputSize) % totalGaps;

      // Track inserting new points
      size_t currentDataIndex = InputSize;

      // For each gap between input points
      for (size_t i = 0; i < totalGaps; ++i) {
        size_t extraPointsInThisGap = additionalPointsPerGap;
        if (i < remainingPoints) extraPointsInThisGap++;

        // Original points defining this gap
        T x1 = xValues[i];
        T x2 = xValues[i+1];
        Y y1 = yValues[i];
        Y y2 = yValues[i+1];

        // Step size for this gap
        T step = (x2 - x1) / static_cast<T>(extraPointsInThisGap + 1);

        // Generate interpolated points
        for (size_t j = 0; j < extraPointsInThisGap; ++j) {
          T x = x1 + step * static_cast<T>(j + 1);
          // Calculate the normalized position (t) between 0 and 1
          T t = static_cast<T>(j + 1) / static_cast<T>(extraPointsInThisGap + 1);
          // Use t to interpolate directly between y1 and y2
          Y y = (1 - t) * y1 + t * y2;
          _data[currentDataIndex++] = DataPoint(x, y);
        }
      }
    }

    // Sort the data by x value
    std::sort(_data.begin(), _data.end(),
             [](const DataPoint& a, const DataPoint& b) { return a.x < b.x; });
  }

  // Lookup Y value for a given X using linear interpolation
  constexpr Y LookupY(T x) const
  {
    // Check if x is outside the range, considering both ascending and descending x values
    T minX = std::min(_data.front().x, _data.back().x);
    T maxX = std::max(_data.front().x, _data.back().x);

    if (x < minX || x > maxX) {
      if (!_allowExtrapolation) {
        throw std::out_of_range("Value outside lookup table range and extrapolation not allowed");
      }

      // Extrapolate using the closest edge
      if (x < minX) {
        return (_data.front().x < _data.back().x) ? _data.front().y : _data.back().y;
      } else {
        return (_data.front().x < _data.back().x) ? _data.back().y : _data.front().y;
      }
    }

    // Binary search to find the closest points for interpolation
    size_t left = 0;
    size_t right = Size - 1;

    // Handle exact match with first or last element
    if (x == _data[left].x) return _data[left].y;
    if (x == _data[right].x) return _data[right].y;

    // Binary search for position
    while (right - left > 1) {
      size_t mid = left + (right - left) / 2;
      if (_data[mid].x <= x) {
        left = mid;
      } else {
        right = mid;
      }
    }

    // Linear interpolation formula: y = y1 + (x - x1) * (y2 - y1) / (x2 - x1)
    T x1 = _data[left].x;
    T x2 = _data[right].x;
    Y y1 = _data[left].y;
    Y y2 = _data[right].y;

    // Calculate interpolated value
    return y1 + (x - x1) * (y2 - y1) / (x2 - x1);
  }

  // Lookup X value for a given Y using binary search and linear interpolation
  constexpr T LookupX(Y y) const
  {
    // Check if y is outside the range, considering both ascending and descending y values
    Y minY = std::min(_data.front().y, _data.back().y);
    Y maxY = std::max(_data.front().y, _data.back().y);

    if (y < minY || y > maxY) {
      if (!_allowExtrapolation) {
        throw std::out_of_range("Value outside lookup table range and extrapolation not allowed");
      }

      // Extrapolate using the closest edge
      if (y < minY) {
        return (_data.front().y < _data.back().y) ? _data.front().x : _data.back().x;
      } else {
        return (_data.front().y < _data.back().y) ? _data.back().x : _data.front().x;
      }
    }


    // Binary search to find the closest points for interpolation
    size_t left = 0;
    size_t right = Size - 1;

    // Handle exact match with first or last element
    if (y == _data[left].y) return _data[left].x;
    if (y == _data[right].y) return _data[right].x;

    // Find the two nearest points for interpolation
    for (size_t i = 0; i < Size - 1; ++i) {
      if ((_data[i].y <= y && y <= _data[i+1].y) ||
          (_data[i].y >= y && y >= _data[i+1].y)) {
        left = i;
        right = i + 1;
        break;
          }
    }

    // Linear interpolation formula: x = x1 + (y - y1) * (x2 - x1) / (y2 - y1)
    T x1 = _data[left].x;
    T x2 = _data[right].x;
    Y y1 = _data[left].y;
    Y y2 = _data[right].y;

    // Calculate interpolated value
    return x1 + (y - y1) * (x2 - x1) / (y2 - y1);
  }

  // Get the number of data points
  constexpr size_t GetSize() const {
    return Size;
  }
};

/**
 * Generates a lookup table with the specified input and output sizes.
 *
 * This function creates a lookup table by taking x and y values as input
 * arrays and determining whether extrapolation is allowed for values falling
 * outside the provided range. The resulting lookup table will interpolate or
 * extrapolate based on the given flags and constraints.
 *
 * @tparam T The data type of x values.
 * @tparam Y The data type of y values.
 * @tparam InputSize The size of the input xValues and yValues arrays.
 * @tparam OutputSize The size of the output lookup table.
 * @param xValues An array containing the x values for the lookup table.
 * @param yValues An array containing the y values corresponding to the x values.
 * @param allowExtrapolation A boolean specifying whether the lookup table
 * should allow extrapolation for inputs outside the range of the provided x values.
 * @return A `LookupTable` object containing the specified x and y values with
 * the desired output size.
 */
template <typename T, typename Y, size_t InputSize, size_t OutputSize = InputSize>
constexpr auto MakeLookupTable(const std::array<T, InputSize>& xValues,
                         const std::array<Y, InputSize>& yValues,
                         bool allowExtrapolation = false) {
  return new LookupTable<T, Y, OutputSize>(xValues, yValues, allowExtrapolation);
}

} // namespace LowLevelEmbedded::Utility

#endif // COOLINGBLOCK_LOOKUPTABLE_H