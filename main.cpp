#include <iostream>
#include <opencv2/opencv.hpp>
#include <thread>

using namespace std;
using namespace cv;

int findMedianOfVector(vector<int> input) {
    sort(input.begin(), input.end());
    auto midVal = input.size() / 2;
    return input[midVal];
}

vector<int> multiRes;
vector<int> singleRes;

void test(
        vector<int> &result
) {
    result.push_back(1);
    result.push_back(2);
    result.push_back(3);
    result.push_back(4);
}

void findMedianOfKernel(
        Mat &image,
        vector<int> &result,
        int threadId,
        int rowStartIdx = 0,
        int rowEndIdx = -1
) {
    if (rowEndIdx == -1) {
        rowEndIdx = image.rows;
    }

    vector<int> kernel;

    int iter = 0;
    for (int i = rowStartIdx; i < rowEndIdx; i++) {
        for (int j = 0; j < image.cols; j++) {
            if (i == 0 or j == 0 or i == image.rows - 1 or j == image.cols - 1) {
                continue;
            }
            kernel.clear();
            for (int k_i = i - 1; k_i <= i + 1; k_i++) {
                for (int k_j = j - 1; k_j <= j + 1; k_j++) {
                    kernel.push_back(image.at<uchar>(k_i, k_j));
                }
            }
            iter++;
            int median = findMedianOfVector(kernel);
            result.push_back(median);
        }
    }
}

int main() {
    // Read Image
    String fileName = "/Users/tilaksharma/CLionProjects/ImageParallel/image.png";
    Mat image = imread(fileName, IMREAD_GRAYSCALE);
    cout << "Image size: " << image.rows << "x" << image.cols << endl;
//    imshow("window", image);
//    waitKey(0);

    vector<int> medRes;

    // Serially run a median calculator for a kernel size of 3x3 and check elapsed time
    chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    findMedianOfKernel(
            ref(image),
            ref(singleRes),
            -1,
            0,
            image.rows
    );

    chrono::steady_clock::time_point end;
    end = std::chrono::steady_clock::now();
    cout << "Time taken without multi threading = "
         << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << " µs"
         << std::endl << endl;

    auto num_cores = thread::hardware_concurrency();

    cout << "Number of rows: " << image.rows << endl;

    cout << "Number of cores: " << num_cores << endl;

    //Num of cores and exp are the same in the case
    vector<vector<int>> thread_i_res(num_cores);

    begin = std::chrono::steady_clock::now();

    int numThreads = num_cores;
    int task_split = image.rows / numThreads;

    vector<thread> threads;

    int upper;
    int lower;

    for (int x = 0; x < numThreads; x++) {
        if (x == numThreads - 1) {
            lower = x * task_split;
            upper = image.rows;
            threads.emplace_back(
                    findMedianOfKernel,
                    ref(image),
                    ref(thread_i_res[x]),
                    x,
                    lower,
                    upper
            );
        } else {
            lower = x * task_split;
            upper = (x + 1) * task_split;
            threads.emplace_back(
                    findMedianOfKernel,
                    ref(image),
                    ref(thread_i_res[x]),
                    x,
                    lower,
                    upper
            );
        }
        cout << lower << "-" << upper << endl;
    }

    for (thread &t: threads) {
        if (t.joinable()) {
            t.join();
        }
    }

    end = std::chrono::steady_clock::now();
    cout << "Time taken using " << numThreads << " threads = "
         << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << " µs"
         << std::endl;
//        cout << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << endl;

    vector<int> multiRes;

    for (vector<int> &tr: thread_i_res) {
        multiRes.insert(multiRes.end(), tr.begin(), tr.end());
    }

    sort(multiRes.begin(), multiRes.end());
    sort(singleRes.begin(), singleRes.end());

    bool isSame = true;

    for (int i = 0; i < singleRes.size(); i++) {
        if(singleRes[i]!=multiRes[i]){
            isSame = false;
            break;
        }
    }

    cout<<isSame<<endl;

    return 0;
    end = std::chrono::steady_clock::now();
    cout << "Time taken without multi threading = "
         << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << " µs"
         << std::endl;

    // Delegate work to multiple threads and check elapsed time

    begin = std::chrono::steady_clock::now();

}
