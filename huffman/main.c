#include <iostream>
#include <string>
#include <queue>
#include <map>
#include <fstream>
#include <bitset>
#include <sstream>
using namespace std;



// A Tree node
struct Node
{
    char ch;
    int freq;
    Node *left, *right;
};

Node* getNode(char ch, int freq, Node* left, Node* right)
{
    Node* node = new Node();
    node->ch = ch;
    node->freq = freq;
    node->left = left;
    node->right = right;

    return node;
}

string readFile(const string& filename)
{
    string input;

       std::ifstream t(filename);

       t.seekg(0, std::ios::end);
       input.reserve(t.tellg());
       t.seekg(0, std::ios::beg);

       input.assign((std::istreambuf_iterator<char>(t)),
                    std::istreambuf_iterator<char>());
       return input;


}

void readFile(const string& filename, int& zero, map<char, int>& freqMap, string& str)
{

        std::ifstream myfile(filename.c_str(), std::ifstream::binary);
        if (!myfile.good())
        {
            cout << "File not found!!";
            exit(1);
        }

        string line;

        // to read extra zeros
        getline(myfile, line);
        zero = stoi(line);

        // to read frequencies of characters for tree construction
        while (getline(myfile, line))
        {
            if (line == "-1")
                break;
            stringstream ss(line);
            string ch;
            string code;
            ss >> ch;
            ss >> code;
            if (ch == "nl")
                freqMap['\n'] = stoi(code);
            else if (ch == "sp")
                freqMap[' '] = stoi(code);
            else if (ch == "tb")
                freqMap['\t'] = stoi(code);
            else
                freqMap[ch[0]] = stoi(code);
        }
        // to read encoded message
        stringstream buffer;
        buffer << myfile.rdbuf();
        str = buffer.str();

}

struct comp
{
    bool operator()(Node* l, Node* r)
    {
        return(l->freq == r->freq)? l->ch < r->ch : l->freq > r->freq;
    }
};

string Char_to_binary(const string& encodedInput, int zeros)
{
    string binaryInput = "";
    for (char ch : encodedInput)
    {
        bitset<8> temp(ch);
        binaryInput += temp.to_string();
    }
    if (zeros && binaryInput.size() > 8)
        binaryInput.erase(binaryInput.end() - 8, binaryInput.end() - 8 + zeros);
    else if (zeros)
            binaryInput.erase(binaryInput.begin(), binaryInput.begin() + zeros);
    return binaryInput;
}

string binForm(char ch)
{
    bitset<8> temp(ch);
    return temp.to_string();
}

string Binary_to_char(const string& binaryInput)
{
    int left = 0, right = 8;
    string encodedString = "", currentBits = "";
    bool check = false;
    while (true)
    {
        if (check)
            break;
        if (right >= int(binaryInput.length()))
        {
            right = binaryInput.length();
            check = true;
        }
        while (left < right)
            currentBits += binaryInput[left++];
        right += 8;

        stringstream sstream(currentBits);
        bitset<8> bits;
        sstream >> bits;
        char character = bits.to_ulong();

        encodedString += character;
        currentBits = "";
    }
    return encodedString;
}

void printCodes(const map<char, string>& codes)
{
    cout << "Huffman Codes are:\n" << '\n';
    for (auto pair : codes)
    {
        if (pair.first == '\n'){
            cout << "newLine" << " " << binForm(pair.first)<<" "<< pair.second << endl;}
        else if (pair.first == ' '){
            cout << "Space" << " " <<binForm(pair.first)<< " " <<pair.second << endl;}
        else{
            cout << pair.first << " " << binForm(pair.first)<<" " <<pair.second << endl;}
    }
}

void leafCodes(Node* root, string str, map<char, string> &huffmanCode)
{
    if (root == nullptr) {
        return;
    }

    // found a leaf node
    if (root->left == nullptr && root->right == nullptr) {
        huffmanCode[root->ch] = (str != "") ? str : "1";
    }

    leafCodes(root->left, str + "0", huffmanCode);
    leafCodes(root->right, str + "1", huffmanCode);
}

string binaryIn(map<char, string>& codes, const string& input)
{
    string temp = "";
    for (char ch : input)
    {
        temp += codes[ch];
    }
    return temp;
}

string createCompressedFile(map<char, int>& freqMap, const string& encodedInput, int extraZeros)
{

    cout << "Enter compressed file name without extension:" << endl;
    string compressed_filename;
    cin >> compressed_filename;
    compressed_filename += ".ziptxt";
    std::ofstream myfile(compressed_filename.c_str(), std::ofstream::binary | std::ofstream::trunc);
    myfile << extraZeros << endl;

    for (auto pair : freqMap)
    {
        if (pair.first == '\n')
            myfile << "nl" << " " << pair.second << endl;
        else if (pair.first == ' ')
            myfile << "sp" << " " << pair.second << endl;
        else if (pair.first == '\t')
            myfile << "tb" << " " << pair.second << endl;
        else
            myfile << pair.first << " " << pair.second << endl;
    }

    myfile << "-1" << endl << encodedInput;
    myfile.close();
    return compressed_filename;
}

void createUncompressedFile(const string& uncompressedInput)
{

    ofstream myfile;

    cout << "Enter uncompressed file name without extension:" << endl;
    string uncompressed_filename;
    cin >> uncompressed_filename;
    uncompressed_filename += ".txt";

    myfile.open(uncompressed_filename);

    myfile << uncompressedInput;

    myfile.close();
}

priority_queue<Node*, vector<Node*>, comp> buildHuffmanTree(map<char, int>& freqMap)
{

    priority_queue<Node*, vector<Node*>, comp> q;
    for (auto it : freqMap)

        q.push(getNode(it.first, it.second, nullptr, nullptr));
    while (q.size() > 1)
    {
        Node* l = q.top();
        q.pop();
        Node* r = q.top();
        q.pop();
        q.push(getNode('\0', l->freq + r->freq, l, r));
    }
    return q;
}

void decode(Node* root, int& index, string str, string& uncompressedInput)
{
    if (root == nullptr)
    {
        return;
    }

    // found a leaf node
    if (root->left == nullptr && root->right == nullptr)
    {
        uncompressedInput += root->ch;
        return;
    }

    index++;

    if (str[index] == '0')
    {
        decode(root->left, index, str, uncompressedInput);
    }
    else
    {
        decode(root->right, index, str, uncompressedInput);
    }
}

void Delete(Node* root)
{
    if (!root)
        return;
    Delete(root->left);
    Delete(root->right);
    delete root;
}

void decompression(const string& filename)
{
    // read file
    // read first line [number]
    // read tree frequency map to construct priority queue


    int zero;
    map<char, int> freqMap;
    string compressedInput;

    readFile(filename, zero, freqMap, compressedInput);
    cout << "read file" << endl;
    priority_queue<Node*, vector<Node*>, comp> q = buildHuffmanTree(freqMap);
    cout << "build tree" << endl;
    Node* root = q.top();

    // convert encoded to binary

    string binaryInput = Char_to_binary(compressedInput, zero);
    cout << "input decoded to binary" << endl;
    string uncompressedInput = "";

    if (root->left == nullptr && root->right == nullptr)
    {
        // Special case: For input like a, aa, aaa, etc
        while (root->freq--)
        {
            uncompressedInput += root->ch;
        }
    }
    else
    {
        // Traverse the Huffman Tree again and this time
        // decode the encoded string
        int index = -1;
        while (index < (int)binaryInput.size() - 1)
        {
            decode(root, index, binaryInput, uncompressedInput);
        }
    }

    createUncompressedFile(uncompressedInput);


    Delete(root);

}

void compression(const string& filename)
{
    string input = readFile(filename);

    map<char, int> freqMap;
    for (char ch : input)
        freqMap[ch]++;

    priority_queue<Node*, vector<Node*>, comp> q = buildHuffmanTree(freqMap);

    map<char, string> codes;
    Node* root = q.top();

    leafCodes(root, "", codes);

    string binaryInput = binaryIn(codes, input); // input after replacing chars with 01 codes
    string encodedInput = Binary_to_char(binaryInput); // input after combining each 8 bits into 1 single byte char

    // store file
    int extraZeros = (8 - (binaryInput.size() % 8)) % 8;
    string compFile = createCompressedFile(freqMap, encodedInput, extraZeros);

    ifstream in_file(filename, ios::binary);
    in_file.seekg(0, ios::end);
    long long int file_size = in_file.tellg();

    ifstream comp_file(compFile, ios::binary);
    comp_file.seekg(0, ios::end);
    long long int comp_file_size = comp_file.tellg();

    long double compression_ratio = (long double)comp_file_size / (long double)file_size * 100;

    cout << "compression ratio is : " << compression_ratio << endl;
    printCodes(codes);
    Delete(root);

}

// Huffman coding algorithm
int main()
{
    string filename;
    cout << "Enter file name:" << endl;
    cin >> filename;

    string input;
    cout << "\nChoose compress or decompress:\n" << endl << flush;
    cin >> input;
    if(input == "compress")
        compression(filename);
    else if(input == "decompress")
        decompression(filename);
    else cout << "input is invalid";

    return 0;
}
Aa

21:59
