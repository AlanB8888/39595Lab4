#include <iostream>
#include <chrono>
#include <optional>
#include <vector>

#include "poly.h"

std::optional<double> poly_test(polynomial& p1,
                                polynomial& p2,
                                std::vector<std::pair<power, coeff>> solution)

{
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    polynomial p3 = p1 * p2;

    auto p3_can_form = p3.canonical_form();

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    p3.print();

    if (p3_can_form != solution)
    {
        return std::nullopt;
    }

    return std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
}

int main()
{
    //TEST MULTIPLICATION:
    //=============================================================================================================
    // /** We're doing (x+1)^2, so solution is x^2 + 2x + 1*/
    // std::vector<std::pair<power, coeff>> solution = {{2,1}, {1,2}, {0,1}};

    // /** This holds (x+1), which we'll pass to each polynomial */
    // std::vector<std::pair<power, coeff>> poly_input = {{1,1}, {0,1}};

    // polynomial p1(poly_input.begin(), poly_input.end());
    // polynomial p2(poly_input.begin(), poly_input.end());

    // std::optional<double> result = poly_test(p1, p2, solution);

    // if (result.has_value())
    // {
    //     std::cout << "Passed test, took " << result.value()/1000 << " seconds" << std::endl;
    // } 
    // else 
    // {
    //     std::cout << "Failed test" << std::endl;
    // }


    std::vector<std::pair<power, coeff>> input1 = {{5,7}, {2,3}, {1,1}, {0,1}};
     std::vector<std::pair<power, coeff>> input2 = {{4,6}, {2,4}, {1,2}, {0,2}};

    polynomial p1(input1.begin(), input1.end());
    polynomial p2(input2.begin(), input2.end());

    //TEST SUMS:
    //===================================================
    polynomial sum1 = p1 + p2;
    std::cout << "SUM 1: \n";
    sum1.print();

    polynomial sum2 = p1 + (int)5;
    std::cout << "SUM 2: \n";
    sum2.print();

    polynomial sum3 = (int)6 + p1;
    std::cout << "SUM 3: \n";
    sum3.print();

    //TEST MULTIPLY:
    //===================================================
    polynomial mul1 = p1 * p2;
    std::cout << "MUL 1: \n";
    mul1.print();

    polynomial mul2 = p1 * 5;
    std::cout << "MUL 2: \n";
    mul2.print();

    polynomial mul3 = 6 * p1;
    std::cout << "MUL 3: \n";
    mul3.print();





}