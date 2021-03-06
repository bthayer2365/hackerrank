#include <iostream>
#include <algorithm>
#include <string>
#include <vector>

using namespace std;

class PossiblePalindrome {
    public:
    int maxLength;
    int sequenceNum;
    PossiblePalindrome(int l, int s):maxLength(l),sequenceNum(s) {}
};

class Sequence {
    public:
    char c;
    int start;
    int end; // end points to character after last
    int maxPalindrome = -1;
    Sequence(char letter, int s, int e):c(letter),start(s),end(e) {}
    
    int length() {
        return end - start;
    }

};

class PString {
    public:
    vector<Sequence> sequences;
    string s;
    PString(string s) {
        this->s = s;
        char c = s[0];
        int start = s.length() - 1;
        for ( ; s[start] == c && start > 0; start--) ;
        start++;
        if (start == 1) { // every char == c
            sequences.push_back(Sequence(c, 0, s.length()));
            return;
        }
        int end = 1;
        for ( ; s[end] == c; end++) ;// not every char == c, so this will stop
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


int expandPossibility(PossiblePalindrome& p, PString& s, int r, int endcap) {
    // cout << "Expanding possibility " << p.sequenceNum;
    
    vector<Sequence>& seqs = s.sequences;
    if (seqs[p.sequenceNum].maxPalindrome != -1) {
        return seqs[p.sequenceNum].maxPalindrome;
    }
    int start = p.sequenceNum + 2 * seqs.size();
    int end = start;

    // cout << " (" << seqs[end % seqs.size()].c << ", " << p.maxLength << ")" << endl;

    int length = seqs[start % seqs.size()].length(); // Possibility is not an endcap
    start--;
    end++;
    
    while (true) {
        Sequence& s1 = seqs[start % seqs.size()];
        Sequence& s2 = seqs[end % seqs.size()];

        // extend if possible
        // same char, if not end
        if (s1.c != s2.c) {
            seqs[p.sequenceNum].maxPalindrome = length;
            return length;
        }
        // hits end, add, end
        if (start % seqs.size() == endcap || end % seqs.size() == endcap) { // assumes we'll hit the endcap
            int s1Length = s1.length();
            int s2Length = s2.length();
            
            if (start % seqs.size() == endcap) {
                // update s1Length
                s1Length = (s1.end - r + 2 * s.length()) % s.length();
            }
            if (end % seqs.size() == endcap) {
                // update s2Length;
                s2Length = (r - s2.start + 2 * s.length()) % s.length();
            }
            return length + min(s1Length, s2Length) * 2;
        }
        // same length, if not add, then end
        // return min * 2
        if (s1.length() != s2.length()) {
            length += min(s1.length(), s2.length()) * 2;
            seqs[p.sequenceNum].maxPalindrome = length;
            return length;
        } else {
            length += 2 * s1.length();
        }
        start--;
        end++;
    }
    return length;
}

int longestPalindrome(PString& s, int r) {
    
    // cout << "r=" << r << endl;

    int longest = 0;
    vector<PossiblePalindrome> v;
    
    int endcap = 0;
    
    for (int seqNum = 0; seqNum < s.sequences.size(); seqNum++) {
        Sequence& seq = s.sequences[seqNum];
        
        int rStart = (seq.start - r + 2*s.length()) % s.length();
        int rEnd = (seq.end - r + 2*s.length()) % s.length();
        // cout << "Start, end " << rStart << ", " << rEnd << endl;
        if (rEnd < rStart) {
            // endcaps
            endcap = seqNum;
            longest = max(rEnd, s.length() - rStart);
            // cout << "Sequence " << seqNum << " is endcap (" << longest << ")" << endl;
        } else {
            // not endcap
            // cout << "Sequence " << seqNum << " added as middle" << endl;
            // check if palindrome has already been checked
            int rCenterx2 = rStart + rEnd - 1;
            int maxLength;
            if (rCenterx2 < s.length() * 2) {
                maxLength = rCenterx2;
            } else {
                maxLength = s.length() * 2 - rCenterx2;
            }
            if (seq.maxPalindrome != -1) {
                maxLength = seq.maxPalindrome;
            }
            v.push_back(PossiblePalindrome(maxLength, seqNum));
        }
    }

    
    sort(v.rbegin(), v.rend(), 
         [] (PossiblePalindrome a, PossiblePalindrome b) {
             return a.maxLength < b.maxLength;
         });
    // v contains possiblepalindrome locations sorted in descending order
    
    for (auto it = v.begin(); it != v.end(); it++) {
        if (it->maxLength < longest) {
            return longest;
        }
        int l = expandPossibility(*it, s, r, endcap);
        longest = max(l, longest);
    }
    return longest;
}

vector<int> circularPalindromes(string s) {
    // only need to calculate for middle of each of the sequences, not every character
    // find all palindromes
    cout << "Starting" << endl;
    PString p(s);

    cout << "Sequences found:" << endl;
    for (int i = 0; i < p.sequences.size(); i++) {
        Sequence seq = p.sequences[i];
        cout << "(" << seq.c << "," << seq.start << "," << seq.length() << ")" << endl;
    }
    
    vector<int> v;
    if (p.sequences.size() == 1) {
        for (int i = 0; i < s.length(); i++) {
            v.push_back(s.length());
        }
        return v;
    }
    for (int r = 0; r < s.length(); r++) {
        v.push_back(longestPalindrome(p, r));
    }
    return v;
}


int testCase(string s, vector<int> answer) {
    if (s.length() < 100) {
        cout << "Testing string: " << s << endl;
    }
        
    vector<int> result = circularPalindromes(s);
    if (s.length() < 100) {
        cout << "Testing string: " << s << endl;
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