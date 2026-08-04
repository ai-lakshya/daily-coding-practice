# Minimum Number of Arrows to Burst Balloons
**Source:** leetcode-150 | **Difficulty:** Medium | **Topic:** Intervals
**Link:** https://leetcode.com/problems/minimum-number-of-arrows-to-burst-balloons/ | **Date:** 2026-08-04 | **Self-rating:** 3/5

## Problem
Given horizontal balloon intervals on the x-axis, find the minimum number of vertical arrows needed so every balloon is pierced by at least one arrow.

## My Approach (as attempted, solve-first)
```cpp
class Solution {
public:
    vector<int> intersection(vector<int>& i1, vector<int>& i2)
    {
        if(i1[1] >= i2[0])
            return {i2[0], i1[1]};
        return {};
    }

    int findMinArrowShots(vector<vector<int>>& points) {
        sort(points.begin(), points.end());
        vector<vector<int>> pointsCommon;
        pointsCommon.push_back(points.back());
        points.pop_back();

        while(points.size() > 0)
        {
            auto intersec = intersection(points.back(), pointsCommon.back());
            if(intersec.size() == 0)
            {
                pointsCommon.push_back(points.back());
            }
            else
            {
                pointsCommon.pop_back();
                pointsCommon.push_back(intersec);
            }
            points.pop_back();
        }

        return pointsCommon.size();
    }
};
```

## Flaws Identified
- No correctness bugs — verified against all 3 official examples plus an induction proof during the session. The algorithm is a valid mirror-image greedy: sort ascending by xstart, process from the back (i.e. descending xstart), and track a group "anchor" (`i2[0]`, the highest xstart in the current group) that never changes once set. A new balloon joins the group iff its xend reaches the anchor. Since every group member is checked against that same fixed anchor, `x = anchor` is always a valid common point for the whole group, and disjoint balloons are correctly rejected.
- **Space:** O(n) extra space for the `pointsCommon` stack (it can hold up to all n balloons, e.g. in the fully-disjoint case), versus O(1) extra space achievable with the standard formulation.
- **Mutates the caller's input:** `points` is sorted in place and then fully emptied via repeated `pop_back` in the loop. Harmless for LeetCode's single-call judge, but a footgun if this function were ever reused elsewhere.
- **`intersection()` is misleadingly named/fragile:** it doesn't compute a true geometric interval intersection (it discards `i2[1]` and never takes `min` of the two ends). It only works because of a non-obvious invariant (`i2[0]` is always <= the group's true min-end) that isn't enforced explicitly — risky if edited later without understanding why it's correct.

## Optimal Approach
Sort balloons by `xend` ascending instead of `xstart`. Track a single running boundary (`currentEnd`) instead of a stack: start with `arrows = 1` and `currentEnd = points[0][1]`. Walk the rest of the sorted array — whenever a balloon's `xstart > currentEnd`, it can't be hit by the current arrow, so increment `arrows` and reset `currentEnd` to that balloon's `xend`. Otherwise it's already covered (since the array is sorted by end, its end is >= `currentEnd`, and its start is <= `currentEnd`, so the existing arrow already pierces it). Same O(n log n) time as the original (dominated by the sort either way), but genuine O(1) extra space, no auxiliary stack, no helper function whose correctness needs a proof, and no mutation side effect beyond the in-place sort.

## Complexity Comparison
| | Time | Space |
|---|---|---|
| Mine | O(n log n) | O(n) |
| Optimal | O(n log n) | O(1) extra |

## Pattern / Category
Interval point cover (minimum points to stab all intervals) — same family as *Non-overlapping Intervals* and *Meeting Rooms*. Generalizable trick: sort by one endpoint, sweep with a single running boundary, count how many times a new "resource" (arrow / room / point) is forced.

## Language Notes
- C++:
```cpp
class Solution {
public:
    int findMinArrowShots(vector<vector<int>>& points) {
        sort(points.begin(), points.end(),
             [](const vector<int>& a, const vector<int>& b) { return a[1] < b[1]; });

        int arrows = 1;
        int currentEnd = points[0][1];
        for (size_t i = 1; i < points.size(); ++i) {
            if (points[i][0] > currentEnd) {
                ++arrows;
                currentEnd = points[i][1];
            }
        }
        return arrows;
    }
};
```
- JS/TS:
```ts
function findMinArrowShots(points: number[][]): number {
  points.sort((a, b) => a[1] - b[1]);

  let arrows = 1;
  let currentEnd = points[0][1];
  for (let i = 1; i < points.length; i++) {
    if (points[i][0] > currentEnd) {
      arrows++;
      currentEnd = points[i][1];
    }
  }
  return arrows;
}
```

## Mistakes to Remember
No implementation mistakes this time. The thing to remember is recognizing "interval point cover" problems (minimum arrows/points/rooms to cover or separate all intervals) and defaulting to sort-by-end + single running boundary — it's simpler to implement and verify than a stack-based alternative, even when the alternative also happens to be correct.
