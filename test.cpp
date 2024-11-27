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


    std::vector<std::pair<power, coeff>> input1 = {{2,2}, {1,2}, {0,2}};
    std::vector<std::pair<power, coeff>> input2 = {{4,6}, {2,4}, {1,2}, {0,2}};
    std::vector<std::pair<power, coeff>> input3 = {{9,10}, {6,6}, {4,4}, {0,2}};

    std::vector<std::pair<power, coeff>> inputSized1; //polynomial of [size1] terms of form  1+2x+3x^2+4^x3...
    std::vector<std::pair<power, coeff>> inputSized2; //polynomial of [size2] terms of form  3+6x+9x^2+12^x3...

    int size1 = 500000;
    int size2 = 500000;

    for(int i = 0; i< size1; i++){
        inputSized1.push_back(std::make_pair(i,i+1));
    }
    for(int i = 0; i< size2; i++){
        inputSized1.push_back(std::make_pair(i,3*(i+1)));
    }

    polynomial p1(input1.begin(), input1.end());
    polynomial p2(input2.begin(), input2.end());
    polynomial p3(input3.begin(), input3.end());

    polynomial ps1(inputSized1.begin(), inputSized1.end());
    polynomial ps2(inputSized2.begin(), inputSized2.end());

    // //TEST SUMS:
    // //===================================================
     std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();//start clock to time adding 2 polys
     polynomial sum1 = ps1 + ps2;
     std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now(); //end clock
     auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
     std::cout << "SUM 1 TIME: " << duration.count() << " ms\n";
     std::cout << "SUM 1: \n";
     //sum1.print();

     begin = std::chrono::steady_clock::now();//start clock to time adding   
     polynomial sum2 = ps1 + (int)5;
     end = std::chrono::steady_clock::now(); //end clock
     //TAKES MUCH MUCH LESS THAN REVERSED int poly ORDER, 
     //porbably has to do with the method favoring small polynomials second
     duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
     std::cout << "SUM 2 TIME: " << duration.count() << " ms\n";
     std::cout << "SUM 2: \n";
     //sum2.print();


     polynomial sum3 = (int)6 + ps1;//TAKES TOO LONG
     //porbably has to do with the method favoring small polynomials second
     std::cout << "SUM 3: \n";
     //sum3.print();


    // //TEST MULTIPLY:
    // //===================================================
    // polynomial mul1 = p1 * p2;
    // std::cout << "MUL 1: \n";
    // mul1.print();

    // polynomial mul2 = p1 * 5;
    // std::cout << "MUL 2: \n";
    // mul2.print();

    // polynomial mul3 = 6 * p1;
    // std::cout << "MUL 3: \n";
    // mul3.print();

    //TEST MODULUS:
    //===================================================
    //polynomial mod1 = p1 % p2;
    //std::cout << "MOD 1: \n";
    //mod1.print();

    // polynomial mod2 = p1 % 2;
    // std::cout << "MOD 2: \n";
    // mod2.print();

    //polynomial mod3 = p1 % p1;
    //std::cout << "MOD 3: \n";
    //mod3.print();

    //polynomial mod4 = p3 % p1;
    //std::cout << "MOD 4: \n";
    //mod4.print();

    // polynomial mod5 = p3 % p1;
    // std::cout << "MOD 5: \n";
    // mod5.print();





}