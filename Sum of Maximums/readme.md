This is my solution for the Sum of the Maximums problem.
https://www.hackerrank.com/challenges/little-alexey-and-sum-of-maximums/problem

I am the 68th person to solve this challenge.

This was a very challenging problem and took about 15 hours, 5 sheets of scratch paper, and several iterations to solve completely. 

This problem is concerned with finding the sum of the maximums of all the subranges for a given subrange.

The naive brute force way produces a runtime of O(n^2) for each query, however since there are many queries, it is worth it to preprocess the data. The first method of preprocessing that I used was to calculate what I call the left and right "dominances" for each index of the array. The dominance value represents how many sequential values to the left or right a given value is greater than. In the sequence 2 5 3 3 7, 5 has a left dominance of 1 and a right dominance of 2. 7 has a left dominance of 4 and a right dominance of 0 because it reaches the end of the sequence. I also defined that if two values are equal, then the one to the left is considered greater. Therefore, in the sequence 10 10 10, the second 10 has a left dominance of 0 and a right dominance of 1. This drops the runtime for a single query to O(n), but this is still too much for this challenge. 

I created a system of child nodes to fix this issue. In the sequence 2 5 3 3 7 4, both 3s are considered children of 5 because they are less than it and to the right of it. 4 is not 5's child because it is closest to 7 and is therefore 7's child. 5's next node is considered 7 because 7 is greater than 5 and ends the sequence of children that follow 5. This system creates "valleys" that are independent of the rest of the sequence. The 3s in 2 5 3 3 7 4 are in a valley between 5 and 7. They will never affect a sum of a range that is to the left of 5 or to the right of 7 because 5 and 7 would dominate them in each direction. This also means that for any range that they are in, their contribution is finite and can be stored as 5's "childSum." For larger child sequences, this becomes particularly helpful because a lot of computation can be avoided.

The solution in main_simple.cpp uses everything described above in its' solution, however that solution was only able to solve 1/3 of the test cases and timed out on the rest.

It was relatively easy to figure out the types of test cases that is method failed on - structred data. In the sequence 1 2 3 4 5, none of the nodes have children thereby forcing each partial sum to be computed rather than looked up. My initial reaction was to also compute children in the other direction, making 1, 2, 3 and 4 all left children of 5, however this did not solve the problem. It was clear that I needed to keep track of the sequences somehow, and I eventually worked out the formula (index - left + 1) * (rightDominance + 1) * A[index] for the partial contribution of each element in an increasing sequence and  (right - index + 1) * (leftDominance + 1) * A[index] for the partial contribution of each element in an reversed increasing sequence. The next node in a sequence is defined the same way as a node's next node in the system of children that I was using before. In each of those equations, all terms are known beforehand except for left and right, so separating the formulae result in "right * (leftDominance + 1) * A[index] + (1 - index) * (leftDominance + 1) * A[index]" and "-left * (rightDominance + 1) * A[index] + (index + 1) * (rightDominance + 1) * A[index]". Left and right can be factored out of the sum of the partial sums to leave the sum other terms. Each of these terms can be stored as the "sum so far" in each of the sequences. Subtracting the sum so far of one element from another gives the sum of the terms in the given range, which can then be used to easily compute the contribution of the sums in the sequence. The functions ascendingSum and rAscendingSum make use of this math.

This was very convenient for sequences that were contiguous and did not have breaks, but it was unclear what to do when the elements in a given sequence weren't contiguous, so that addition still left me with half of the test cases unsolved.

The simplest thing to do was to add a list of breakpoints to each of the sequences for each time the continuity of a sequence was broken. This was an ideal solution because each of the breakpoints also marked the start of a valley and that breakpoint's childSum contained the sum of that valley. Each breakpoint could then be iterated over to have their childSum added to the total, erasing the need to compute any values in the range of that sequence. Luckily std::sets are ordered and can easily be iterated over within a given range using std::lower_bound and std::upper_bound, so I put the breakpoints and elements in the sequence each into their respective sets. I could have also used vectors or simply arrays, however that would have involved sorting the breakpoints and reversing the elements in the reverse sequences, but sets are automatically sorted and are easier to use because of it.

The bigger issue was determining which sequence to iterate over. I created a notion of a "superior" sequence. In the example 6 7 1 2 3 8 9, the sequence 6 7 8 9 is considered the direct superior of 1 2 3. I used this to find the sequence that spanned the largest distance in the given range, however it turned out that in certain cases, this method did not do as I initially expected, but in the process, I realized that I could simply find the largest element in the range and use one of the sequences associated with that. The maximum element, like all elements, is part of both a forward sequence and a reverse sequence. I find the first element of the sequence in the range and use that to compute the sum for the range and then split the remaining unaccounted for elements into recursive problems. Using this technique, I was able to get all but 2 of the test cases. Since I was computing the max every time query was called and it was relatively expensive, I added an if statement to redirect to the query to use the simple child-only method whenever the range gets smaller than a certain point, which I somewhat arbitrarily set to 1000. This sped things up enough to pass the final 2 test cases.