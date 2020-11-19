#include <random>
#include <iostream>
#include <chrono>
#include <omp.h>

using namespace std::chrono;

void printMat(double *mat,int n)
{
    for (int i = 0; i < n;++i)
    {
        for (int j = 0; j < n;++j)
            std::cout << mat[i*n+j]  << " ";
        std::cout << std::endl;
    }
    return;
}

int main(int argc, char **argv)
{
    int n;
    double threshold;
    n = atoi(argv[1]);
    threshold = atof(argv[2]);

    // For Edge elements at each side taking extra elements to escape from checking if condition
    // Taking single array and giving continous memory with malloc
    int total = (n + 2) * (n + 2), i, j,count=0;
    int changed_len = n + 2;
    double *mat, *mat1; // For removing dependency between iteration
    mat = (double *)malloc(sizeof(double) * total);
    mat1 = (double *)malloc(sizeof(double) * total);

    // Value Generation
    double lbound = 0;
    double ubound = 10;
    std::uniform_real_distribution<double> urd(lbound, ubound);
    std::default_random_engine re;
    for (i = 0; i < changed_len; ++i)
    {
        for (j = 0; j < changed_len; ++j)
        {
            if (i == 0 || j == 0 || i == changed_len-1 || j == changed_len-1)
            {
                mat[i * changed_len + j] = 0;
                mat1[i * changed_len + j] = 0;
            }
            else
                mat[i * changed_len + j] = urd(re);
        }
    }
    int th = n / 50 > 9 ? 10 : (n / 50) + 1, tmp;
    double sum, total_d, avg;
    auto start = high_resolution_clock::now();
    // Main Logic
    omp_set_num_threads(th);
    bool th_flag = true;
    while (th_flag)
    {
        th_flag = false;
        #pragma omp parallel
        {
            #pragma omp for collapse(2) private(i,j,tmp,sum,total_d,avg)
                for (i = 1; i <= n; ++i)
                {
                    for (j = 1; j <= n; ++j)
                    {
                        tmp = i * changed_len + j;
                        sum = mat[tmp] + mat[tmp - changed_len] + mat[tmp + changed_len] + mat[tmp + 1] + mat[tmp - 1];
                        total_d = 5;
                        if (i == 1 || i == n)
                            total_d--;
                        if (j == 1 || j == n)
                            total_d--;
                        // std::cout << sum << " : " << total_d << std::endl;
                        avg = (sum / (double)total_d);
                        // std::cout << avg << " : " << threshold << std::endl;
                        if (fabs(avg - mat[tmp]) >= threshold)
                            th_flag = true;
                        mat1[tmp] = avg;
                    }
                }
            if(th_flag)
            {
                #pragma omp for collapse(2) private(i,j)
                for (i = 1; i <= n;++i)
                {
                    for (j = 1; j <= n;++j)
                        mat[i * changed_len + j] = mat1[i * changed_len + j];
                }
            }
            ++count;
        }
        //printMat(mat,changed_len);
        //printMat(mat1,changed_len);
        //std::cin.get();
    }
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    std::cout << "Time: " << duration.count() << std::endl;
    std::cout << "Number of iterations: " << count << std::endl;
    std::cout << "Number of threads launched: " << th << std::endl;
    return 0;
}