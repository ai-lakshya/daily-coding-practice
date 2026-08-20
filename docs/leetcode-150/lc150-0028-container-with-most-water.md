# Container With Most Water
**Source:** leetcode-150 | **Difficulty:** Medium | **Topic:** Two Pointers
**Link:** https://leetcode.com/problems/container-with-most-water/ | **Date:** 2026-08-04 | **Self-rating:** 4/5

## Problem
Given an integer array `height` representing vertical lines, find two lines that together with the x-axis form a container that holds the most water.

## My Approach (as attempted, solve-first)
```cpp
class Solution {
public:
    int maxArea(vector<int>& height) {
        int left = 0;
        int right = height.size() - 1;
        int maxArea = 0;

        while (left < right) {
            int currentArea = min(height[left], height[right]) * (right - left);
            maxArea = max(maxArea, currentArea);

            if (height[left] < height[right]) {
                left++;
            } else {
                right--;
            }
        }

        return maxArea;
    }
};
```

## Flaws Identified
no flaws, optimal on first attempt

## Optimal Approach
Use two pointers starting at the outermost edges of the array to maximize initial width. At each step, calculate the area. Then, move the pointer that points to the shorter line inward, as keeping the shorter line while decreasing width would only yield a smaller or equal area. Continue this until the pointers meet.

## Complexity Comparison
| | Time | Space |
|---|---|---|
| Mine | O(N) | O(1) |
| Optimal | O(N) | O(1) |

## Pattern / Category
Two Pointers (opposite directional / inward-moving)

## Language Notes
- C++: `std::min` and `std::max` are used to find the bottleneck height and track the maximum area.
- JS/TS:
```typescript
function maxArea(height: number[]): number {
    let left = 0;
    let right = height.length - 1;
    let maxArea = 0;

    while (left < right) {
        const currentArea = Math.min(height[left], height[right]) * (right - left);
        maxArea = Math.max(maxArea, currentArea);

        if (height[left] < height[right]) {
            left++;
        } else {
            right--;
        }
    }

    return maxArea;
}
```

## Mistakes to Remember
(none, flawlessly executed)
