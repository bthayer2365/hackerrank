#include <iostream>
#include <algorithm>
#include <string>
#include <vector>

using namespace std;

class Sequence {
    // A sequence is one character repeated end - start times
    public:
    char c;
    int start;
    int end; // end points to character after last
    Sequence(char letter, int s, int e):c(letter),start(s),end(e) {}
    
    int length() {
        return end - start;
    }

    int centerx2() {

        return start + end - 1;
    }

};

class PString {
    public:
    vector<Sequence> sequences;
    string s;
    PString(string s) {
        // Converts string into a list of sequences
        this->s = s;
        char c = s[0];
        int start = s.length() - 1;
        for ( ; s[start] == c && start > 0; start--) ;
        start++;
        if (start == 1) { 
            // The entire string is one character
            sequences.push_back(Sequence(c, 0, s.length()));
            return;
        }
        int end = 1;
        // Gets us to the first character that is different
        for ( ; s[end] == c; end++) ;
        sequences.push_back(Sequence(c, start - s.length(), end));
        // first one locked in
        
        int first_start = start;
        while (end < first_start) {
            start = end;
            c = s[start];
            for ( ; s[end] == c; end++) ;
            sequences.push_back(Sequence(c, start, end));
        }
    }
    
    int length() {
        return s.length();
    }
};

class Palindrome {
    public:
    int start;
    int end;
    Palindrome(int s, int e):start(s),end(e) {}

    int length() {
        return end - start;
    }

    int centerx2() {
        return start + end;
    }
    
};

Palindrome makePalindrome(int centerx2, int length) {
    int start = (centerx2 - length + 1)/2;
    int end = start + length;
    return Palindrome(start, end);
}

Palindrome getPalindrome(PString& p, int seqNum) {
    // Finds the longest possible palindrome from a given sequence
    // The palindrome found automatically loops past the ends
    auto& seqs = p.sequences;
    int centerx2 = seqs[seqNum].centerx2();
    int start = seqNum + 2 * seqs.size();
    int end = start;

    int length = seqs[start % seqs.size()].length(); // Possibility is not an endcap
    start--;
    end++;
    
    while (end - start <= seqs.size()) {
        Sequence& s1 = seqs[start % seqs.size()];
        Sequence& s2 = seqs[end % seqs.size()];

        // extend if possible
        // same char, if not end
        if (s1.c != s2.c) {
            return makePalindrome(centerx2, length);
        }
        if (&s1 == &s2) {
            length += s1.length();
            if (s1.length() % 2 == 1) {
                length -= 1;
            }
            return makePalindrome(centerx2, length);
        }
        // same length, if not add, then end
        if (s1.length() != s2.length()) {
            length += min(s1.length(), s2.length()) * 2; // shorter one limits
            return makePalindrome(centerx2, length);
        } else {
            length += 2 * s1.length();
        }
        start--;
        end++;
    }
    return makePalindrome(centerx2, length);
}

int testPalindrome(Palindrome& p, int r, int strLen) {
    // Finds the length of the palindrome after being cut off by the end of the string's rotation
    int rCenterx2 = (p.centerx2() - 2 * r + 4 * strLen) % (2 * strLen);
    int length = min(rCenterx2, strLen * 2 - rCenterx2);
    length = min(length, p.length());

    // cout << "Testing: " << p.start << "," << p.length() << " | " << p.centerx2() << "," << rCenterx2 << "," << length << endl;
    return length;
}

int longestPalindrome(PString& p, vector<Palindrome>& pals, int r, int strLen, Sequence& endcap) {
    // returns the longest palindrome for a given rotation

    // endcap is a special case since the center of the palindrome moves relative to the center of the sequence
    int rStart = (endcap.start - r + 2*strLen) % strLen;
    int rEnd = (endcap.end - r + 2*strLen) % strLen;
    int longest = max(rEnd, strLen - rStart);
    // cout << "Endcap: " << rStart << "," << rEnd << " - " << longest << endl;

    // check the largest possibility first
    for (auto it = pals.begin(); it != pals.end(); it++) {
        if (it->length() < longest) {
            // None of the next possible palindromes will beat the endcap
            return longest;
        }
        int length = testPalindrome(*it, r, strLen);
        longest = max(length, longest);
    }
    return longest;
}

vector<int> circularPalindromes(string s) {
    // Returns a vector with the length of the longest palindrome for each rotation

    cout << "Starting" << endl;
    PString p(s);

    cout << "Sequences found:" << endl;
    for (int i = 0; i < p.sequences.size(); i++) {
        Sequence seq = p.sequences[i];
        cout << "(" << seq.c << "," << seq.start << "," << seq.length() << ")" << endl;
    }

    vector<Palindrome> pals;

    for (int i = 0; i < p.sequences.size(); i++) {
        pals.push_back(getPalindrome(p, i));
    }

    // sort from longest to shortest
    sort(pals.rbegin(), pals.rend(), 
         [] (Palindrome a, Palindrome b) {
             return a.length() < b.length();
         });
    
    cout << "Palindromes:" << endl;
    for (int i = 0; i < pals.size(); i++) {
        cout << pals[i].start << "," << pals[i].end << " - " << pals[i].length() << endl;
    }

    int endcapNum = -1;
    Sequence endcap = p.sequences[p.sequences.size() - 1];
    int nextEndcap = endcap.end - s.length();

    vector<int> v;
    for (int r = 0; r < s.length(); r++) {
        // cout << "r=" << r << endl;

        // keeps track of which sequence is at the end ie is the endcap
        // center of palindrome for endcap can change
        if (nextEndcap < r) {
            ++endcapNum;
            // cout << "Endcap increased to " << endcapNum << endl;
            if (endcapNum == p.sequences.size()) {
                endcapNum = 0;
                nextEndcap = s.length();
                endcap = p.sequences[endcapNum];
            } else {
                endcap = p.sequences[endcapNum];
                nextEndcap = endcap.end;
            }
        }
        v.push_back(longestPalindrome(p, pals, r, s.length(), endcap));
    }
    return v;
}


int testCase(string s, vector<int> answer) {
    if (s.length() < 100) {
        cout << "Testing string: " << s << endl;
    }
        
    vector<int> result = circularPalindromes(s);
    if (s.length() < 100) {
        cout << "Result: " << endl;
        for (int i = 0; i < result.size(); i++) {
            cout << result[i] << ", ";
        }
        cout << endl;
        cout << "Actual: " << endl;
        for (int i = 0; i < answer.size(); i++) {
            cout << answer[i] << ", ";
        }
        cout << endl;
    }

    bool correct = true;
    for (int i = 0; i < result.size(); i++) {
        if (result[i] != answer[i]) {
            correct = false;
            break;
        }
    }
    if (correct) {
        cout << "Answer was correct" << endl;
    } else {
        cout << "Answer was incorrect" << endl;
    }

}

int main()
{
    // testCase("aaaaabbbbaaaa", {12, 12, 10, 8, 8, 9, 11, 13, 11, 9, 8, 8, 10});
    // testCase("cacbbba", {3, 3, 3, 3, 3, 3, 3});
    // testCase("eededdeedede", {5, 7, 7, 7, 7, 9, 9, 9, 9, 7, 5, 4});

    return 0;
}